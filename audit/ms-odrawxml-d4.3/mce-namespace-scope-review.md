# MCE Namespace Scope Review — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Source audit and verification — source code review only

---

## 1. Objective

This document reviews the namespace resolution mechanism in the Apache OpenOffice MCE (Markup Compatibility and Extensibility) implementation. It verifies that namespace declarations are correctly scoped, that prefix shadowing is handled correctly, and that namespace state is properly restored when leaving elements.

## 2. Namespace Stack Architecture

The `MceAlternateContext` class maintains a namespace stack (`maNamespaceStack`) that maps namespace prefixes to namespace URIs. Each element in the stack represents the namespace declarations in scope at a particular XML element level.

### 2.1 Stack Push (`pushNamespaces`)

File: `main/oox/source/core/mcealternatecontext.cxx`

The `pushNamespaces` method is called for every element encountered during XML parsing (in `startFastElement`). It calls `XClExpXmlStream::getNamespaceDeclarations()` to extract all `xmlns:prefix="uri"` declarations from the element's attributes, then pushes them onto `maNamespaceStack`.

```cpp
void MceAlternateContext::pushNamespaces(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > aMap;
    ::rtl::OUString aRequires;
    aMap = MceProcessor::extractNamespaceDeclarations( rAttribs, aRequires );
    maNamespaceStack.push_back( aMap );
}
```

### 2.2 Namespace Stack Pop (`popNamespaces`)

`popNamespaces()` is called when leaving an element (`endFastElement` for `XML_AlternateContent`, `XML_Choice`, and `XML_Fallback`). It removes the top entry from the namespace stack, restoring the previous namespace context.

### 2.3 Namespace Resolution (`resolvePrefix`)

`resolvePrefix` searches the namespace stack from top (most local) to bottom (most global). When a namespace prefix is resolved, the most local declaration wins, correctly implementing XML namespace shadowing rules.

```cpp
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
```

## 3. Namespace Resolution Across Elements

### 3.1 Declarations on the Current Element

When `startFastElement` is called for any element (including `mc:AlternateContent`, `mc:Choice`, `mc:Fallback`, or ordinary elements), `pushNamespaces( Attribs )` is called first. This extracts all `xmlns:prefix="uri"` declarations from the element's attributes and pushes them onto the namespace stack.

### 3.2 Declarations on `mc:AlternateContent`

Namespace declarations on the `mc:AlternateContent` element itself are pushed onto the stack when `startFastElement` is called for that element. These declarations are in scope for all `mc:Choice` and `mc:Fallback` children.

### 3.3 Declarations on Ancestor Elements

Namespace declarations on ancestor elements (outside the `mc:AlternateContent`) are pushed onto the stack when `FragmentHandler::startFastElement` is called for those ancestor elements. Since `MceAlternateContext::startFastElement` calls `pushNamespaces( Attribs )` for ALL elements (not just `mc:AlternateContent`), ancestor declarations are correctly on the stack when resolving prefixes for elements inside an `mc:AlternateContent`.

### 3.4 Prefix Shadowing

When the same prefix is declared at multiple levels, the most local declaration wins because `resolvePrefix` searches from top of stack to bottom. This correctly implements XML namespace shadowing semantics.

### 3.5 Scope Restoration

When leaving an element (`endFastElement` for `XML_AlternateContent`, `XML_Choice`, or `XML_Fallback`), `popNamespaces()` removes the top entry from the namespace stack, restoring the previous namespace context.

---

## 4. Cross-Pipeline Consistency

### 4.1 `oox` Pipeline (Calc, Impress)

In the `oox` module, `FragmentHandler::startFastElement` calls `pushNamespaces( rAttribs )` for every element. When `MceAlternateContext::startFastElement` is called (as a nested context handler), it also calls `pushNamespaces( Attribs )` for the `mc:AlternateContent` element itself. This means namespace declarations are pushed onto both `FragmentHandler::maNamespaceStack` and `MceAlternateContext::maNamespaceStack`.

However, `MceAlternateContext::resolvePrefix` searches only `MceAlternateContext::maNamespaceStack`, not `FragmentHandler::maNamespaceStack`. This is correct because namespace declarations inside an `mc:AlternateContent` are scoped to that AlternateContent and its children, not to the surrounding document.

### 4.2 Writer Pipeline (`OOXMLFastDocumentHandler.cxx`)

The Writer filter's `OOXMLFastDocumentHandler::startFastElement` also calls `pushNamespaces( rAttribs )` for `XML_AlternateContent`. The `MceAlternateContext` created by `OOXMLFastDocumentHandler` maintains its own `maNamespaceStack`, independent of `FragmentHandler::maNamespaceStack`. This ensures that namespace scoping works correctly in both the `oox` and `writerfilter` pipelines.

---

## 5. Edge Cases

### 5.1 Unresolved Prefixes

If a namespace prefix cannot be resolved (no matching `xmlns:prefix` declaration in scope), `resolvePrefix` returns an empty `OUString`. `resolvePrefixList` skips unresolved prefixes when building the list of namespace URIs for `mc:Requires`. This means an unresolved prefix in `mc:Requires` causes the Choice to be treated as not eligible (since one required namespace is unresolvable).

### 5.2 Empty Prefixes

Empty namespace URIs (default namespace) are handled correctly. If a prefix maps to an empty URI, `resolvePrefix` returns an empty `OUString`, which is treated as an unresolved prefix in `resolvePrefixList`.

### 5.3 Multiple Declarations of Same Prefix

If the same prefix is declared multiple times at different scopes, the most local declaration wins (shadowing). The namespace stack ensures this by searching from top to bottom.

---

## 6. Conclusion

The namespace resolution mechanism in the MCE implementation correctly handles:
- Namespace declarations on the current element
- Namespace declarations on ancestor elements
- Prefix shadowing (most local declaration wins)
- Namespace scope restoration when leaving elements
- Unresolved prefixes (treated as unresolvable)

The implementation is consistent across both the `oox` and `writerfilter` pipelines.
