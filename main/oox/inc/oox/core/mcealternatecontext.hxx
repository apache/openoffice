#ifndef OOX_CORE_MCEALTERNATECONTEXT_HXX
#define OOX_CORE_MCEALTERNATECONTEXT_HXX

#include <com/sun/star/xml/sax/XFastContextHandler.hpp>
#include <com/sun/star/xml/sax/XFastAttributeList.hpp>
#include <cppuhelper/implbase1.hxx>
#include <rtl/ustring.hxx>
#include <vector>
#include "oox/core/mceprocessor.hxx"

namespace oox {
namespace core {

/** Context handler for mc:AlternateContent elements.

    Implements Phase 1+2 MCE support: mc:AlternateContent,
    mc:Choice, mc:Fallback selection based on mc:Requires namespace
    resolution, plus mc:Ignorable and mc:ProcessContent processing.

    Unselected branches are discarded (their events are swallowed).
    Unknown namespaces listed in mc:Ignorable are silently skipped
    unless mc:ProcessContent explicitly includes them.
*/
class MceAlternateContext : public ::cppu::WeakImplHelper1<
    ::com::sun::star::xml::sax::XFastContextHandler >
{
public:
    MceAlternateContext(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastContextHandler >& xDelegate );

    virtual ~MceAlternateContext();

    // com.sun.star.xml.sax.XFastContextHandler interface
    virtual void SAL_CALL startFastElement(
        ::sal_Int32 Element,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& Attribs );
    virtual void SAL_CALL startUnknownElement(
        const ::rtl::OUString& Namespace,
        const ::rtl::OUString& Name,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& Attribs );
    virtual void SAL_CALL endFastElement( ::sal_Int32 Element );
    virtual void SAL_CALL endUnknownElement(
        const ::rtl::OUString& Namespace,
        const ::rtl::OUString& Name );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL
        createFastChildContext(
        ::sal_Int32 Element,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& Attribs );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL
        createUnknownChildContext(
        const ::rtl::OUString& Namespace,
        const ::rtl::OUString& Name,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& Attribs );
    virtual void SAL_CALL characters( const ::rtl::OUString& aChars );

private:
    enum State
    {
        STATE_INITIAL,        ///< Before any Choice/Fallback seen
        STATE_CHOICE_SCAN,    ///< Scanning Choice branches
        STATE_CHOICE_SELECTED,///< An eligible Choice was selected
        STATE_FALLBACK_SELECTED, ///< Fallback was selected
        STATE_DONE            ///< AlternateContent ended
    };

    ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler > mxDelegate;

    State               meState;
    MceProcessor::Selection maSelection;
    sal_Int32           mnNextChoiceIndex;   ///< Index of next Choice to evaluate
    bool                mbHasFallback;
    bool                mbInSelectedBranch;  ///< Currently inside selected branch

    ::rtl::OUString     maMarkupCompatNamespace;

    /** Namespaces declared as ignorable via mc:Ignorable. */
    std::vector< ::rtl::OUString > maIgnorableNamespaces;

    /** Qualified element names (namespace URI + local name) that should be
        processed even when in an ignorable namespace, per mc:ProcessContent. */
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > maProcessContentElements;

    /** Evaluates a mc:Choice element's Requires attribute and updates
        the selection state. */
    void implEvaluateChoice(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs );

    /** Returns a null context that discards all events. */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler > implCreateNullContext();

    /** Processes the mc:Ignorable attribute, adding each referenced
        namespace URI to the ignorable namespaces list. */
    void processIgnorableAttribute(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs );

    /** Processes the mc:ProcessContent attribute, adding each qualified
        element name to the set of elements to process even when in an
        ignorable namespace. */
    void processProcessContent(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs );

    /** Resolves a namespace prefix to a URI by delegating to the delegate
        context handler (which maintains the namespace stack). */
    ::rtl::OUString resolvePrefix(
        const ::rtl::OUString& rPrefix ) const;
};

} // namespace core
} // namespace oox

#endif
