# MCE ProcessContent Processing — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Implementation

---

## 1. Overview

The `mc:ProcessContent` attribute (namespace `http://schemas.openxmlformats.org/markup-compatibility/2006`) specifies which elements within an ignorable namespace should still be processed by consumers that do not understand those namespaces. When `mc:Ignorable` declares a namespace as ignorable, elements in that namespace are normally skipped. However, `mc:ProcessContent` allows selective processing of specific element types within those ignorable namespaces.

## 2. Implementation

### 2.1 File Locations

| File | Role |
|------|------|
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | Declaration of `maProcessContentElements` and `isProcessContentElement()` |
| `main/oox/source/core/mcealternatecontext.cxx` | Implementation of `processProcessContent()` and `isProcessContentElement()` |

### 2.3 Implementation Details

#### `processProcessContent`

Located in `main/oox/source/core/mcealternatecontext.cxx`, this method is called from `startFastElement` for every element. It:

1. Extracts the `mc:ProcessContent` attribute value from the element's attributes
2. Splits the value by whitespace into QName tokens
3. Splits each QName into prefix and local name
4. Resolves each prefix to a namespace URI using `resolvePrefix`
5. Adds each (namespace URI, local name) pair to `maProcessContentElements` if not already present

```cpp
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
```

#### `isProcessContentElement`

```cpp
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
```

### 2.4 Usage in `startUnknownElement`

When an element outside a selected `mc:AlternateContent` branch has an ignorable namespace, `startUnknownElement` checks:

1. Is the element's namespace in `maIgnorableNamespaces`?
2. Is the element's qualified name in `maProcessContentElements`?
3. If ignorable AND NOT in process content list → skip (do not forward)
4. If namespace is ignorable AND element IS in process content list → forward (process the element even though namespace is ignorable)
5. If namespace is NOT ignorable → forward normally

```cpp
void SAL_CALL MceAlternateContext::startUnknownElement(
    const ::rtl::OUString& Namespace,
    const ::rtl::OUString& Name,
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& Attribs )
{
    if ( mbInSelectedBranch && mxDelegate.is() )
    {
        mxDelegate->startUnknownElement( Namespace, Name, Attribs );
        return;
    }

    // Element outside selected branch: check mc:Ignorable and mc:ProcessContent
    if ( isIgnorableNamespace( Namespace ) &&
         !isProcessContentElement( Namespace, Name ) )
    {
        return; // Skip - ignorable namespace, not in ProcessContent
    }

    if ( mxDelegate.is() )
        mxDelegate->startUnknownElement( Namespace, Name, Attribs );
}
```

### 2.4 Example Behavior

Given this document:

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:ms="http://schemas.microsoft.com/office/myschema/2006">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="ms">
          <ms:specialElement>Excel content</a:pic>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
      <ms:unknownElement>Ignored content</ms:unknownElement>
      <ms:specialElement>Processed content</ms:specialElement>
    </w:p>
  </w:body>
</w:document>
```

- `<ms:unknownElement>` is in ignorable namespace `ms` and NOT in `mc:ProcessContent` → skipped
- `<ms:specialElement>` is in ignorable namespace `ms` AND in `mc:ProcessContent` → forwarded to delegate

### 2.5 Testing

No runtime tests have been executed due to the build environment limitations (missing Boost headers). The implementation follows the ECMA-376 Part 1 specification for Markup Compatibility and Extensibility.

### 2.6 Testing

No runtime tests have been executed due to the build environment limitations (missing Boost headers). The implementation follows the ECMA-376 Part 1 specification for Markup Compatibility and Extensibility.
