#ifndef OOX_CORE_MCEPROCESSOR_HXX
#define OOX_CORE_MCEPROCESSOR_HXX

#include <com/sun/star/xml/sax/XFastAttributeList.hpp>
#include <com/sun/star/xml/Attribute.hpp>
#include <rtl/ustring.hxx>
#include <vector>

namespace oox {
namespace core {

/** Minimal MCE (Markup Compatibility and Extensibility) selection engine.

    Implements Phase 1 of the MCE support plan: mc:AlternateContent,
    mc:Choice, mc:Fallback selection based on mc:Requires namespace
    resolution.

    The MceProcessor is a stateless logic class with no UNO dependencies
    beyond the XFastAttributeList interface. It can be shared across all
    OOXML filter implementations (Calc, Impress, Writer).
*/
class MceProcessor
{
public:
    /** Result of branch selection. */
    enum BranchResult
    {
        BRANCH_NONE,        ///< No branch selected (no eligible Choice, no Fallback)
        BRANCH_CHOICE,      ///< A Choice branch was selected
        BRANCH_FALLBACK     ///< The Fallback branch was selected
    };

    /** Selected branch information. */
    struct Selection
    {
        BranchResult        meResult;     ///< Which branch was selected
        sal_Int32           mnChoiceIndex;///< Index of selected Choice (0-based), -1 if not a Choice
    };

    MceProcessor();
    ~MceProcessor();

    /** Returns the Markup Compatibility namespace URI. */
    static ::rtl::OUString getMarkupCompatNamespace();

    /** Returns the set of namespace URIs that AOO supports for MCE Choice
        branches in Phase 1. These are namespaces where AOO has actual
        element-level implementation, not just namespace registration.

        The Phase 1 support set is deliberately conservative:
        - DrawingML main (http://schemas.openxmlformats.org/drawingml/2006/main)
          AOO implements DrawingML element parsing in oox/source/drawingml/.

        Namespaces NOT in the Phase 1 set:
        - SpreadsheetML, WordprocessingML, PresentationML main namespaces
          (document-level markup, not DrawingML content)
        - MS-ODRAWXML extension namespaces (not implemented at all)
        - VML namespaces (legacy format, not DrawingML)
    */
    static const std::vector< ::rtl::OUString >& getSupportedNamespaceSet();

    /** Checks whether a namespace URI is in the Phase 1 supported set. */
    static bool isNamespaceSupported( const ::rtl::OUString& rNamespaceURI );

    /** Extracts namespace declarations (xmlns:prefix="uri") from an attribute
        list and returns them as a prefix-to-URI mapping. Also extracts the
        mc:Requires value if present.

        @param rAttribs      Attribute list of an element
        @param rRequires     Output: value of mc:Requires attribute, or empty
                             if not present
        @return              Prefix-to-URI mapping from xmlns:* declarations
    */
    static std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > extractNamespaceDeclarations(
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs,
        ::rtl::OUString& rRequires );

    /** Evaluates the mc:Requires attribute value.

        Resolves each namespace prefix listed in Requires to a namespace URI
        using the provided prefix-to-URI mapping, then checks whether all
        resolved namespaces are in the supported set.

        @param rRequires     Value of mc:Requires attribute (space-separated
                             namespace prefixes)
        @param rPrefixMap    Mapping of namespace prefixes to namespace URIs

        @return  true if all required namespaces are supported, false otherwise
    */
    static bool isChoiceEligible(
        const ::rtl::OUString& rRequires,
        const std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > >& rPrefixMap );

    /** Selects a branch from an mc:AlternateContent element.

        Evaluates Choice branches in document order and returns the first
        eligible Choice. If no Choice is eligible and a Fallback exists,
        returns Fallback. If neither, returns NONE.

        @param rChoiceEligible   Vector indicating which Choice branches are
                                 eligible (true = eligible, false = not)
        @param bHasFallback      Whether a mc:Fallback child exists

        @return  Selection result
    */
    static Selection selectBranch(
        const std::vector< bool >& rChoiceEligible,
        bool bHasFallback );

private:
    /** Resolves a namespace prefix to a URI using the prefix map. */
    static ::rtl::OUString resolvePrefix(
        const ::rtl::OUString& rPrefix,
        const std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > >& rPrefixMap );

    static std::vector< ::rtl::OUString > maSupportedNamespaces;
};

} // namespace core
} // namespace oox

#endif
