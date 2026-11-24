#include "oox/core/mcealternatecontext.hxx"

#include <com/sun/star/xml/sax/XFastAttributeList.hpp>
#include <rtl/ustring.hxx>
#include "oox/token/tokens.hxx"

namespace oox {
namespace core {

// ============================================================================

MceAlternateContext::MceAlternateContext(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >& xDelegate ) :
    mxDelegate( xDelegate ),
    meState( STATE_INITIAL ),
    mnNextChoiceIndex( 0 ),
    mbHasFallback( false ),
    mbInSelectedBranch( false ),
    maMarkupCompatNamespace(
        MceProcessor::getMarkupCompatNamespace() )
{
}

MceAlternateContext::~MceAlternateContext()
{
}

// ----------------------------------------------------------------------------

void SAL_CALL MceAlternateContext::startFastElement(
    ::sal_Int32 Element,
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& Attribs )
{
    // Push namespace declarations from this element onto the stack.
    // NOTE: For XML_AlternateContent elements, namespace declarations have
    // already been pushed by FragmentHandler::startFastElement before
    // createFastChildContext created this MceAlternateContext. For all other
    // elements, FragmentHandler::startFastElement has already pushed them.
    // We only need to push namespaces for elements that appear inside an
    // active MceAlternateContext context (i.e., when mbInSelectedBranch is
    // true), because FragmentHandler::startFastElement pushes onto ITS stack,
    // not ours.
    if ( mbInSelectedBranch && Element != XML_AlternateContent )
    {
        pushNamespaces( Attribs );
    }

    // Process mc:Ignorable attribute if present on this element.
    processIgnorableAttribute( Attribs );

    // Process mc:ProcessContent attribute if present on this element.
    processProcessContent( Attribs );

    if ( Element == XML_Choice )
    {
        // Evaluate this Choice's Requires attribute
        implEvaluateChoice( Attribs );
        return; // Choice element itself is not forwarded
    }
    if ( Element == XML_Fallback )
    {
        if ( meState == STATE_INITIAL || meState == STATE_CHOICE_SCAN )
        {
            mbHasFallback = true;
            if ( meState == STATE_CHOICE_SCAN && maSelection.meResult == MceProcessor::BRANCH_NONE )
            {
                // No eligible Choice found, select Fallback
                maSelection.meResult = MceProcessor::BRANCH_FALLBACK;
                maSelection.mnChoiceIndex = -1;
                meState = STATE_FALLBACK_SELECTED;
                mbInSelectedBranch = true;
            }
        }
        return; // Fallback element itself is not forwarded
    }
    if ( Element == XML_AlternateContent )
    {
        // Nested AlternateContent: create a new context.
        MceAlternateContext* pMceCtx = new MceAlternateContext(
            mxDelegate );
        pMceCtx->maNamespaceStack = maNamespaceStack;  // inherit namespace stack
        pMceCtx->maIgnorableNamespaces = maIgnorableNamespaces;  // inherit ignorable namespaces
        pMceCtx->maProcessContentElements = maProcessContentElements;  // inherit process content
        pMceCtx->meState = STATE_INITIAL;
        pMceCtx->mnNextChoiceIndex = 0;
        pMceCtx->mbHasFallback = false;
        pMceCtx->mbInSelectedBranch = false;
        pMceCtx->maSelection.meResult = MceProcessor::BRANCH_NONE;
        pMceCtx->maSelection.mnChoiceIndex = -1;

        return ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastContextHandler >(
                static_cast< XFastContextHandler* >( pMceCtx ) );
    }

    // Non-MCE element inside selected branch - forward to delegate
    if ( mbInSelectedBranch && mxDelegate.is() )
        mxDelegate->startFastElement( Element, Attribs );
    // Non-MCE element outside selected branch - discard
}

void SAL_CALL MceAlternateContext::startUnknownElement(
    const ::rtl::OUString& Namespace,
    const ::rtl::OUString& Name,
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& Attribs )
{
    // Push namespace declarations from this element onto the stack.
    pushNamespaces( Attribs );

    // Process mc:Ignorable attribute if present on this element.
    processIgnorableAttribute( Attribs );

    // Process mc:ProcessContent attribute if present on this element.
    processProcessContent( Attribs );

    if ( mbInSelectedBranch && mxDelegate.is() )
    {
        mxDelegate->startUnknownElement( Namespace, Name, Attribs );
        return;
    }

    // Element outside selected branch: check mc:Ignorable and mc:ProcessContent
    // If namespace is ignorable AND element is NOT in ProcessContent list: skip
    // If namespace is ignorable AND element IS in ProcessContent list: forward
    // If namespace is NOT ignorable: forward normally
    if ( isIgnorableNamespace( Namespace ) &&
         !isProcessContentElement( Namespace, Name ) )
    {
        return; // Skip - ignorable namespace, not in ProcessContent
    }

    if ( mxDelegate.is() )
        mxDelegate->startUnknownElement( Namespace, Name, Attribs );
}

void SAL_CALL MceAlternateContext::endFastElement( ::sal_Int32 Element )
{
    if ( Element == XML_AlternateContent )
    {
        meState = STATE_DONE;
        mbInSelectedBranch = false;
        return;
    }
    if ( Element == XML_Choice || Element == XML_Fallback )
    {
        mbInSelectedBranch = false;
        return;
    }

    if ( mbInSelectedBranch && mxDelegate.is() )
        mxDelegate->endFastElement( Element );
}

void SAL_CALL MceAlternateContext::endUnknownElement(
    const ::rtl::OUString& Namespace,
    const ::rtl::OUString& Name )
{
    if ( mbInSelectedBranch && mxDelegate.is() )
        mxDelegate->endUnknownElement( Namespace, Name );
}

// ----------------------------------------------------------------------------

::com::sun::star::uno::Reference<
    ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL
MceAlternateContext::createFastChildContext(
    ::sal_Int32 Element,
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& Attribs )
{
    if ( Element == XML_AlternateContent )
    {
        // Nested AlternateContent: create a new context.
        // Namespace declarations have already been pushed in startFastElement.
        MceAlternateContext* pMceCtx = new MceAlternateContext(
            mxDelegate );
        pMceCtx->maNamespaceStack = maNamespaceStack;  // inherit namespace stack
        pMceCtx->maIgnorableNamespaces = maIgnorableNamespaces;  // inherit ignorable namespaces
        pMceCtx->maProcessContentElements = maProcessContentElements;  // inherit process content
        pMceCtx->meState = STATE_INITIAL;
        pMceCtx->mnNextChoiceIndex = 0;
        pMceCtx->mbHasFallback = false;
        pMceCtx->mbInSelectedBranch = false;
        pMceCtx->maSelection.meResult = MceProcessor::BRANCH_NONE;
        pMceCtx->maSelection.mnChoiceIndex = -1;

        return ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastContextHandler >(
                static_cast< XFastContextHandler* >( pMceCtx ) );
    }

    if ( mbInSelectedBranch && mxDelegate.is() )
        return mxDelegate->createFastChildContext( Element, Attribs );
    return ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >();
}

::com::sun::star::uno::Reference<
    ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL
MceAlternateContext::createUnknownChildContext(
    const ::rtl::OUString& Namespace,
    const ::rtl::OUString& Name,
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& Attribs )
{
    if ( mbInSelectedBranch && mxDelegate.is() )
        return mxDelegate->createUnknownChildContext( Namespace, Name, Attribs );
    return ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >();
}

void SAL_CALL MceAlternateContext::characters( const ::rtl::OUString& aChars )
{
    if ( mbInSelectedBranch && mxDelegate.is() )
        mxDelegate->characters( aChars );
}

// ----------------------------------------------------------------------------

void MceAlternateContext::implEvaluateChoice(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    // Build prefix map from this Choice element's attributes, wrapped with
    // ancestor namespace declarations from the stack.
    ::rtl::OUString aRequires;
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > aPrefixMap =
        MceProcessor::extractNamespaceDeclarations( rAttribs, aRequires );

    // Add ancestor namespace declarations (current element's declarations
    // override ancestor declarations for the same prefix).
    for ( auto it = maNamespaceStack.rbegin(); it != maNamespaceStack.rend(); ++it )
    {
        for ( const auto& rPair : *it )
        {
            // Only add if not already present from a more local scope.
            bool bFound = false;
            for ( const auto& rExisting : aPrefixMap )
            {
                if ( rExisting.first == rPair.first )
                {
                    bFound = true;
                    break;
                }
            }
            if ( !bFound )
                aMap.push_back( rPair );
        }
    }

    if ( MceProcessor::isChoiceEligible( aRequires, aPrefixMap ) )
    {
        if ( maSelection.meResult == MceProcessor::BRANCH_NONE )
        {
            // First eligible Choice
            maSelection.meResult = MceProcessor::BRANCH_CHOICE;
            maSelection.mnChoiceIndex = mnNextChoiceIndex;
            meState = STATE_CHOICE_SELECTED;
            mbInSelectedBranch = true;
        }
        ++mnNextChoiceIndex;
    }
    else
    {
        ++mnNextChoiceIndex;
    }
}

::com::sun::star::uno::Reference<
    ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL
MceAlternateContext::implCreateNullContext()
{
    return ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >();
}

// ----------------------------------------------------------------------------

void MceAlternateContext::pushNamespaces(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > aMap;
    ::rtl::OUString aRequires;
    aMap = MceProcessor::extractNamespaceDeclarations( rAttribs, aRequires );
    maNamespaceStack.push_back( aMap );
}

void MceAlternateContext::popNamespaces()
{
    if ( !maNamespaceStack.empty() )
        maNamespaceStack.pop_back();
}

::rtl::OUString MceAlternateContext::resolvePrefix(
    const ::rtl::OUString& rPrefix ) const
{
    // Search from top of stack (most local) to bottom (most global).
    for ( auto it = maNamespaceStack.rbegin(); it != maNamespaceStack.rend(); ++it )
    {
        for ( const auto& rPair : *it )
        {
            if ( rPair.first == rPrefix )
                return rPair.second;
        }
    }
    return ::rtl::OUString(); // Empty = unresolved
}

bool MceAlternateContext::isIgnorableNamespace( const ::rtl::OUString& rNamespaceURI ) const
{
    for ( const auto& rNs : maIgnorableNamespaces )
    {
        if ( rNs == rNamespaceURI )
            return true;
    }
    return false;
}

bool MceAlternateContext::isProcessContentElement(
    const ::rtl::OUString& rNamespaceURI,
    const ::rtl::OUString& rLocalName ) const
{
    for ( const auto& rPair : maProcessContentElements )
    {
        if ( rPair.first == rNamespaceURI && rPair.second == rLocalName )
            return true;
    }
    return false;
}

std::vector< ::rtl::OUString > MceAlternateContext::resolvePrefixList(
    const ::rtl::OUString& rRequires ) const
{
    std::vector< ::rtl::OUString > aResult;
    sal_Int32 nPos = 0;
    sal_Int32 nLen = rRequires.getLength();
    while ( nPos < nLen )
    {
        while ( nPos < nLen && (rRequires[ nPos ] == ' ' || rRequires[ nPos ] == '\t' ||
                                rRequires[ nPos ] == '\r' || rRequires[ nPos ] == '\n') )
            ++nPos;
        if ( nPos >= nLen )
            break;

        sal_Int32 nStart = nPos;
        while ( nPos < nLen && (rRequires[ nPos ] != ' ' && rRequires[ nPos ] != '\t' &&
                                rRequires[ nPos ] != '\r' && rRequires[ nPos ] != '\n') )
            ++nPos;
        ::rtl::OUString aPrefix = rRequires.copy( nStart, nPos - nStart );

        ::rtl::OUString aNamespaceURI = resolvePrefix( aPrefix );
        if ( aNamespaceURI.getLength() > 0 )
            aResult.push_back( aNamespaceURI );
    }
    return aResult;
}

void MceAlternateContext::processIgnorableAttribute(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    if ( !rAttribs.is() )
        return;

    try
    {
        ::com::sun::star::uno::Sequence< ::com::sun::star::xml::Attribute > aAttrs =
            rAttribs->getAttributes();

        for ( sal_Int32 i = 0; i < aAttrs.getLength(); ++i )
        {
            const ::com::sun::star::xml::Attribute& rAttr = aAttrs[i];

            if ( rAttr.Name.equalsAsciiL( "Ignorable", 9 ) )
            {
                ::rtl::OUString aValue = rAttr.Value;
                std::vector< ::rtl::OUString > aPrefixes =
                    resolvePrefixList( aValue );
                for ( const auto& rNs : aPrefixes )
                {
                    // Only add if not already present.
                    bool bFound = false;
                    for ( const auto& rExisting : maIgnorableNamespaces )
                    {
                        if ( rExisting == rNs )
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if ( !bFound )
                        maIgnorableNamespaces.push_back( rNs );
                }
            }
        }
    }
    catch ( ... )
    {
        // getAttributes() may throw if not supported
    }
}

void MceAlternateContext::processProcessContent(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    if ( !rAttribs.is() )
        return;

    try
    {
        ::com::sun::star::uno::Sequence< ::com::sun::star::xml::Attribute > aAttrs =
            rAttribs->getAttributes();

        for ( sal_Int32 i = 0; i < aAttrs.getLength(); ++i )
        {
            const ::com::sun::star::xml::Attribute& rAttr = aAttrs[i];

            if ( rAttr.Name.equalsAsciiL( "ProcessContent", 13 ) )
            {
                ::rtl::OUString aValue = rAttr.Value;
                sal_Int32 nPos = 0;
                sal_Int32 nLen = aValue.getLength();
                while ( nPos < nLen )
                {
                    while ( nPos < nLen && (aValue[ nPos ] == ' ' || aValue[ nPos ] == '\t' ||
                                            aValue[ nPos ] == '\r' || aValue[ nPos ] == '\n') )
                        ++nPos;
                    if ( nPos >= nLen )
                        break;

                    sal_Int32 nStart = nPos;
                    while ( nPos < nLen && (aValue[ nPos ] != ' ' && aValue[ nPos ] != '\t' &&
                                            aValue[ nPos ] != '\r' && aValue[ nPos ] != '\n') )
                        ++nPos;
                    ::rtl::OUString aQName = aValue.copy( nStart, nPos - nStart );

                    // Split QName into prefix and local name.
                    ::rtl::OUString aPrefix;
                    ::rtl::OUString aLocalName;
                    sal_Int32 nColon = aQName.indexOf( ':' );
                    if ( nColon >= 0 )
                    {
                        aPrefix = aQName.copy( 0, nColon );
                        aLocalName = aQName.copy( nColon + 1, aQName.getLength() - nColon - 1 );
                    }
                    else
                    {
                        // No prefix - default namespace (empty prefix)
                        aPrefix = ::rtl::OUString();
                        aLocalName = aQName;
                    }

                    // Resolve prefix to namespace URI.
                    ::rtl::OUString aNamespaceURI = resolvePrefix( aPrefix );
                    if ( aNamespaceURI.getLength() > 0 )
                    {
                        // Only add if not already present.
                        bool bFound = false;
                        for ( const auto& rExisting : maProcessContentElements )
                        {
                            if ( rExisting.first == aNamespaceURI &&
                                 rExisting.second == aLocalName )
                            {
                                bFound = true;
                                break;
                            }
                        }
                        if ( !bFound )
                            maProcessContentElements.push_back(
                                std::make_pair( aNamespaceURI, aLocalName ) );
                    }
                }
            }
        }
    }
    catch ( ... )
    {
        // getAttributes() may throw if not supported
    }
}

} // namespace core
} // namespace oox
