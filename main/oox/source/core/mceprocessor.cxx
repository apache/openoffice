#include "oox/core/mceprocessor.hxx"

#include <com/sun/star/xml/sax/XFastAttributeList.hpp>
#include <rtl/ustring.hxx>
#include <algorithm>

namespace oox {
namespace core {

// ============================================================================

std::vector< ::rtl::OUString > MceProcessor::maSupportedNamespaces;

// ============================================================================

MceProcessor::MceProcessor()
{
}

MceProcessor::~MceProcessor()
{
}

::rtl::OUString MceProcessor::getMarkupCompatNamespace()
{
    return ::rtl::OUString::createFromAscii(
        "http://schemas.openxmlformats.org/markup-compatibility/2006" );
}

const std::vector< ::rtl::OUString >& MceProcessor::getSupportedNamespaceSet()
{
    if ( maSupportedNamespaces.empty() )
    {
        // Phase 1: Conservative support set. Only namespaces where AOO has
        // actual element-level implementation in DrawingML import code.
        //
        // DrawingML main namespace - AOO implements DrawingML element parsing
        // in oox/source/drawingml/ (shape.cxx, fillproperties.cxx,
        // lineproperties.cxx, textparagraph.cxx, textrun.cxx, etc.)
        maSupportedNamespaces.push_back(
            ::rtl::OUString::createFromAscii(
                "http://schemas.openxmlformats.org/drawingml/2006/main" ) );
    }
    return maSupportedNamespaces;
}

bool MceProcessor::isNamespaceSupported( const ::rtl::OUString& rNamespaceURI )
{
    const std::vector< ::rtl::OUString >& rSet = getSupportedNamespaceSet();
    return (std::find( rSet.begin(), rSet.end(), rNamespaceURI ) != rSet.end());
}

std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > MceProcessor::extractNamespaceDeclarations(
    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs,
    ::rtl::OUString& rRequires )
{
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > aMap;
    rRequires = ::rtl::OUString();

    if ( !rAttribs.is() )
        return aMap;

    // Get unknown attributes (includes xmlns: declarations and mc:Requires)
    try
    {
        ::com::sun::star::uno::Sequence< ::com::sun::star::xml::Attribute > aUnknownAttrs =
            rAttribs->getUnknownAttributes();

        for ( sal_Int32 i = 0; i < aUnknownAttrs.getLength(); ++i )
        {
            const ::com::sun::star::xml::Attribute& rAttr = aUnknownAttrs[i];

            if ( rAttr.Name.equalsAsciiL( "Requires", 8 ) )
            {
                rRequires = rAttr.Value;
            }
            else if ( rAttr.Name.getLength() > 6 &&
                      rAttr.Name.match( ::rtl::OUString::createFromAscii( "xmlns:" ), 0 ) )
            {
                ::rtl::OUString aPrefix = rAttr.Name.copy( 6 );
                aMap.push_back( std::make_pair( aPrefix, rAttr.Value ) );
            }
        }
    }
    catch ( ... )
    {
        // getUnknownAttributes may throw if not supported
    }

    return aMap;
}

bool MceProcessor::isChoiceEligible(
    const ::rtl::OUString& rRequires,
    const std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > >& rPrefixMap )
{
    if ( rRequires.getLength() == 0 )
        return false; // Empty Requires means no requirements - treat as not eligible

    // Split by whitespace and resolve each prefix
    sal_Int32 nPos = 0;
    sal_Int32 nLen = rRequires.getLength();
    while ( nPos < nLen )
    {
        // Skip whitespace
        while ( nPos < nLen && (rRequires[ nPos ] == ' ' || rRequires[ nPos ] == '\t' ||
                                rRequires[ nPos ] == '\r' || rRequires[ nPos ] == '\n') )
            ++nPos;
        if ( nPos >= nLen )
            break;

        // Extract prefix token
        sal_Int32 nStart = nPos;
        while ( nPos < nLen && (rRequires[ nPos ] != ' ' && rRequires[ nPos ] != '\t' &&
                                rRequires[ nPos ] != '\r' && rRequires[ nPos ] != '\n') )
            ++nPos;
        ::rtl::OUString aPrefix = rRequires.copy( nStart, nPos - nStart );

        // Resolve prefix to namespace URI
        ::rtl::OUString aNamespaceURI = resolvePrefix( aPrefix, rPrefixMap );
        if ( aNamespaceURI.getLength() == 0 )
            return false; // Unresolved prefix - not eligible

        // Check if namespace is supported
        if ( !isNamespaceSupported( aNamespaceURI ) )
            return false; // Unsupported namespace - not eligible
    }

    // All required namespaces are supported
    return true;
}

MceProcessor::Selection MceProcessor::selectBranch(
    const std::vector< bool >& rChoiceEligible,
    bool bHasFallback )
{
    Selection aSelection;
    aSelection.meResult = BRANCH_NONE;
    aSelection.mnChoiceIndex = -1;

    // Evaluate Choice branches in document order
    for ( size_t i = 0; i < rChoiceEligible.size(); ++i )
    {
        if ( rChoiceEligible[ i ] )
        {
            aSelection.meResult = BRANCH_CHOICE;
            aSelection.mnChoiceIndex = static_cast< sal_Int32 >( i );
            return aSelection;
        }
    }

    // No eligible Choice - use Fallback if available
    if ( bHasFallback )
    {
        aSelection.meResult = BRANCH_FALLBACK;
        aSelection.mnChoiceIndex = -1;
    }

    return aSelection;
}

::rtl::OUString MceProcessor::resolvePrefix(
    const ::rtl::OUString& rPrefix,
    const std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > >& rPrefixMap )
{
    for ( const auto& rPair : rPrefixMap )
    {
        if ( rPair.first.equals( rPrefix ) )
            return rPair.second;
    }
    return ::rtl::OUString(); // Empty = unresolved
}

} // namespace core
} // namespace oox
