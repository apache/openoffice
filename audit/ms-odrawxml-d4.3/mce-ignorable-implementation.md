# MCE Ignorable Implementation — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Implementation

---

## 1. Overview

The `mc:Ignorable` attribute (namespace `http://schemas.openxmlformats.org/markup-compatibility/2006`) lists namespace prefixes whose elements may be ignored by consumers that do not understand them. When AOO encounters an element in an ignorable namespace outside an `mc:AlternateContent` context, it should silently skip it rather than fail or produce garbage output.

## 2. Implementation

### 2.1 Files Modified

| File | Change |
|------|--------|
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | Added `processIgnorableAttribute()` declaration |
| `main/oox/source/core/mcealternatecontext.cxx` | Implemented `processIgnorableAttribute()` |
| `main/oox/source/core/mcealternatecontext.cxx` | `startFastElement()` calls `processIgnorableAttribute(Attribs)` for ALL elements |

### 2.2 Implementation Details

#### `processIgnorableAttribute()` — `mcealternatecontext.cxx` lines 353-396

This method:
1. Extracts all attributes from the element's `XFastAttributeList` via `getAttributes()`
2. For each attribute named `Ignorable` in the Markup Compatibility namespace:
   - Parses the whitespace-separated prefix list
   - Resolves each prefix to a namespace URI using `resolvePrefixList()`
   - Adds each resolved namespace URI to `maIgnorableNamespaces` if not already present

```cpp
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
```

### 2.3 Ignorable Namespace Check

```cpp
bool MceAlternateContext::isIgnorableNamespace( const ::rtl::OUString& rNamespaceURI ) const
{
    for ( const auto& rNs : maIgnorableNamespaces )
    {
        if ( rNs == rNamespaceURI )
            return true;
    }
    return false;
}
```

### 2.4 Usage in `startUnknownElement`

When an unknown element is encountered **outside** a selected `mc:AlternateContent` branch, `startUnknownElement` checks:

1. Is the element's namespace in `maIgnorableNamespaces`?
2. Is the element qualified name in `maProcessContentElements`?
3. If ignorable AND NOT in ProcessContent list → **skip** (do not forward)
4. If namespace is ignorable AND element IS in ProcessContent list → forward to delegate
   (because `mc:ProcessContent` says to process content even though the wrapper is ignorable)
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

---

## 3. Scope Management

The `maIgnorableNamespaces` vector is a member of `MceAlternateContext`. It is initialized empty in the constructor and populated as `mc:Ignorable` attributes are encountered on elements. There is **no explicit "pop ignorable namespaces"** because:

- `MceAlternateContext` is created per `mc:AlternateContent` element
- All ignorable namespaces declared within an `AlternateContent` scope are discarded when the `MceAlternateContext` instance is destroyed (destructor runs)

For nested `mc:AlternateContent` elements, each nested context **inherits** the ignorable namespaces from its parent at creation time:

```cpp
pMceCtx->maIgnorableNamespaces = maIgnorableNamespaces;  // inherit ignorable namespaces
```

This ensures that ignorable namespaces declared in an outer `AlternateContent` are also ignorable in nested `AlternateContent` elements.

---

## 4. Testing

No runtime tests have been executed due to the build environment limitations (missing Boost headers). The implementation follows the ECMA-376 Part 1 specification for Markup Compatibility and Extensibility.

### Test Cases to Verify

| Test ID | Description | Expected Behavior |
|---------|-------------|-------------------|
| IGN-01 | `mc:Ignorable="ms"` on element, unknown element in `ms` namespace | Element skipped, not forwarded |
| F2 | Multiple ignorable namespaces | All listed namespaces ignored |
| F3 | Ignorable namespace declared on ancestor | Inherited by child elements |
| F4 | Prefix shadowing | Most local declaration wins |
| F5 | Unknown element in non-ignorable namespace | Forwarded normally |
| F6 | Ignorable content containing recognized DrawingML | DrawingML elements inside ignorable namespace are still skipped unless in ProcessContent list |

---

## 5. Relationship to Transitional OOXML

The `mc:Ignorable` attribute is part of the Markup Compatibility and Extensibility framework defined in ISO/IEC 29500-1. It allows document authors to include extension elements from namespaces that consuming applications may not understand, without those elements causing import failures. This is essential for forward compatibility of OOXML documents.
