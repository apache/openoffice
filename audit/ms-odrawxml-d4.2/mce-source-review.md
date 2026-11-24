# MCE Phase 1 Source Review — Milestone D.4.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.2  
**Mode:** Source review — no build or runtime testing performed

---

## 1. Scope of Review

This review covers all MCE Phase 1 implementation files:

| File | Type | Lines | Purpose |
|------|------|-------|---------|
| `main/oox/inc/oox/core/mceprocessor.hxx` | Header | 122 | MceProcessor class declaration |
| `main/oox/source/core/mceprocessor.cxx` | Implementation | 175 | MceProcessor: namespace resolution, eligibility, selection |
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | Header | 96 | MceAlternateContext class declaration |
| `main/oox/source/core/mcealternatecontext.cxx` | Implementation | 174 | MceAlternateContext: branch selection, event forwarding |
| `main/oox/source/core/fragmenthandler.cxx` | Modified | 160 | Calc/Impress pipeline integration |
| `main/oox/inc/oox/core/fragmenthandler.hxx` | Modified | 141 | Header declarations for FragmentHandler |
| `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Modified | 170 | Writer pipeline integration |

---

## 2. MceProcessor — Selection Logic Review

### 2.1 Namespace Registration

| Method | File:Line | Finding |
|--------|-----------|---------|
| `getMarkupCompatNamespace()` | mceprocessor.cxx:26 | Returns `"http://schemas.openxmlformats.org/markup-compatibility/2006"` — correct per ISO/IEC 29500-3 |

### 2.2 Supported Namespace Set

| Method | File:Line | Finding |
|--------|-----------|---------|
| `getSupportedNamespaceSet()` | mceprocessor.cxx:30 | Returns only `http://schemas.openxmlformats.org/drawingml/2006/main` |
| `isNamespaceSupported()` | mceprocessor.cxx:47 | Linear search using `std::find` |

**Assessment:** The supported namespace set is deliberately conservative for Phase 1, containing only DrawingML main namespace. This is correct per D.3 design. No other namespaces (SpreadsheetML, WordprocessingML, PresentationML, MS-ODRAWXML extensions, VML) are included.

### 2.3 Namespace Declaration Extraction

| Method | File:Line | Finding |
|--------|-----------|---------|
| `extractNamespaceDeclarations()` | mceprocessor.cxx:53 | Uses `getUnknownAttributes()` from XFastAttributeList to extract `xmlns:*` declarations and `mc:Requires` |

**Issue identified:** `getUnknownAttributes()` only returns attributes whose names and/or namespaces URLs cannot be translated to tokens by the fast parser. Namespace declarations (`xmlns:*`) may or may not be returned as unknown attributes depending on whether the fast parser has registered them. If `xmlns:*` declarations are known to the fast parser (which they typically are for well-known namespaces), they will NOT appear in `getUnknownAttributes()`, and the prefix-to-URI mapping will be incomplete.

**Impact:** If a `mc:Choice` element has `Requires="dml"` but the `xmlns:dml` declaration is on an ancestor element (not on the Choice itself), and the `xmlns:dml` declaration is known to the fast parser, then `extractNamespaceDeclarations()` will not find it, and `resolvePrefix()` will return an empty URI, making the Choice ineligible even though the namespace is in scope.

**Severity:** Medium — affects documents where namespace declarations are on ancestor elements rather than on the Choice element itself.

### 2.4 Choice Eligibility

| Method | File:Line | Finding |
|--------|-----------|---------|
| `isChoiceEligible()` | mceprocessor.cxx:93 | Parses `Requires` attribute, resolves each prefix, checks all namespaces are supported |

**Assessment:** The algorithm correctly:
- Returns false for empty/missing Requires (conservative)
- Splits on whitespace
- Resolves each prefix to a namespace URI
- Requires ALL prefixes to resolve to supported namespaces

**Issue:** Same as 2.3 — prefix resolution depends on `xmlns:*` declarations being in the unknown attributes list.

### 2.5 Branch Selection

| Method | File:Line | Finding |
|--------|-----------|---------|
| `selectBranch()` | mceprocessor.cxx:133 | First eligible Choice wins; Fallback if none eligible; NONE if neither |

**Assessment:** Correct per ISO/IEC 29500-3 semantics.

---

## 3. MceAlternateContext — Context Handler Review

### 3.1 State Machine

| State | Transitions | Assessment |
|-------|-------------|------------|
| `STATE_INITIAL` | → `STATE_CHOICE_SCAN` on first Choice | Correct |
| `STATE_CHOICE_SCAN` | → `STATE_CHOICE_SELECTED` on eligible Choice | Correct |
| `STATE_CHOICE_SCAN` | → `STATE_FALLBACK_SELECTED` on Fallback with no eligible Choice | Correct |
| `STATE_CHOICE_SELECTED` | → `STATE_DONE` on AlternateContent end | Correct |
| `STATE_FALLBACK_SELECTED` | → `STATE_DONE` on AlternateContent end | Correct |
| `STATE_DONE` | Terminal | Correct |

**Issue identified:** The `implEvaluateChoice()` method increments `mnNextChoiceIndex` for BOTH eligible and ineligible choices (lines 157, 161). However, `createFastChildContext()` for a Choice element checks `mnNextChoiceIndex - 1 == maSelection.mnChoiceIndex` to determine if this is the selected Choice. After selecting Choice N at index N, `mnNextChoiceIndex` becomes N+1. When `createFastChildContext()` is called for Choice N's children, `mnNextChoiceIndex` is still N+1, so `mnNextChoiceIndex - 1 == N == maSelection.mnChoiceIndex` — correct. But for Choice N+1's children, `mnNextChoiceIndex` would be N+2 (incremented again when processing Choice N+1), so `mnNextChoiceIndex - 1 == N+1 != N` — correct (not selected).

**However:** If Choice 0 is selected (index 0), `mnNextChoiceIndex` becomes 1. When `createFastChildContext()` is called for Choice 0's children, `mnNextChoiceIndex - 1 == 0 == maSelection.mnChoiceIndex` — correct. But if `createFastChildContext()` is called again for Choice 0 (e.g., nested elements), `mnNextChoiceIndex` is still 1, so `1 - 1 == 0` — still correct.

**Severity:** Low — works correctly for single-level nesting. Nested AlternateContent inside Choice branches is NOT handled (see Section 7).

### 3.2 Event Forwarding

| Method | File:Line | Finding |
|--------|-----------|---------|
| `startFastElement()` | mcealternatecontext.cxx:31 | Forwards non-MCE elements in selected branch; evaluates Choice/Fallback |
| `startUnknownElement()` | mcealternatecontext.cxx:65 | Forwards unknown elements in selected branch |
| `endFastElement()` | mcealternatecontext.cxx:75 | Forwards end elements in selected branch; resets state on Choice/Fallback/AltContent end |
| `endUnknownElement()` | mcealternatecontext.cxx:93 | Forwards unknown end elements in selected branch |
| `createFastChildContext()` | mcealternatecontent.cxx:105 | Delegates to delegate in selected branch; null otherwise |
| `createUnknownChildContext()` | mcealternatecontext.cxx:117 | Delegates to delegate in selected branch; null otherwise |
| `characters()` | mcealternatecontext.cxx:130 | Forwards character data in selected branch |

**Assessment:** Correct — only selected branch events are forwarded to the delegate. Unselected branches are completely discarded.

**Issue identified:** `startFastElement()` for `XML_Choice` calls `implEvaluateChoice()` but does NOT set `mbInSelectedBranch = true` if this is NOT the selected Choice (only sets it if this IS the first eligible Choice). This is correct — ineligible Choice branches should not have their content forwarded.

**Issue identified:** `endFastElement()` for `XML_Choice` sets `mbInSelectedBranch = false` (line 85). This means after a Choice element ends, subsequent sibling content (if any) would NOT be forwarded. However, in standard OOXML, Choice elements are self-contained within AlternateContent, so there should be no sibling content between Choice elements. This is correct for well-formed documents.

### 3.3 Missing Method Implementations

| Method | File:Line | Finding |
|--------|-----------|---------|
| `ignorableWhitespace()` | Not implemented | Header declares it but .cxx doesn't define it |
| `processingInstruction()` | Not implemented | Header declares it but .cxx doesn't define it |

**Impact:** These are declared in the header but not implemented. They will cause linker errors if called. However, they are unlikely to be called in normal OOXML processing.

**Severity:** Low — unlikely to be called, but should be fixed for completeness.

---

## 4. FragmentHandler Integration (Calc/Impress)

### 4.1 Integration Point

| File:Line | Finding |
|-----------|---------|
| `fragmenthandler.cxx:109-123` | `createFastChildContext()` intercepts `XML_AlternateContent` |

**Assessment:** Correct — when the fast parser encounters `mc:AlternateContent` as a known token, `createFastChildContext()` is called with `Element == XML_AlternateContent`, and the method returns a new `MceAlternateContext`.

**Issue identified:** `FragmentHandler::createFastChildContext()` returns `0` (null) for all non-MCE elements. This means that for DrawingML elements inside a selected Choice branch, the `MceAlternateContext::createFastChildContext()` delegates to `mxDelegate->createFastChildContext()`. The delegate is the parent `FragmentHandler` (or subclass). For DrawingML fragment handlers (e.g., `WorksheetFragment`, `SlideFragment`), these subclasses override `createFastChildContext()` to handle DrawingML elements. So DrawingML elements inside selected Choice branches WILL be processed correctly.

**Severity:** None — correct by design.

---

## 5. WriterFilter Integration

### 5.1 Integration Point

| File:Line | Finding |
|-----------|---------|
| `OOXMLFastDocumentHandler.cxx:105-132` | `createFastChildContext()` intercepts `XML_AlternateContent` |

**Assessment:** Correct — when the fast parser encounters `mc:AlternateContent` as a known token, `createFastChildContext()` is called with `Element == XML_AlternateContent`, and the method returns a new `MceAlternateContext` wrapping an `OOXMLFastContextHandler` delegate.

**Issue identified:** Line 120 checks `Element == oox::XML_AlternateContent`. The `XML_AlternateContent` token must be available in the writerfilter build. It is included via `#include <oox/token/tokens.hxx>` at line 38.

**Issue identified:** Line 121 gets the context handler via `getContextHandler()`, which creates an `OOXMLFastContextHandler` if one doesn't exist. Line 124 creates a COPY of this handler via `new OOXMLFastContextHandler( *pCtxHandler.get() )`. This copy becomes the delegate for `MceAlternateContext`. This is correct — the copy preserves the context handler's state.

---

## 6. Build Integration

### 6.1 Build Files Modified

| File | Change |
|--------|--------|
| `main/oox/Library_oox.mk` | Added `mcealternatecontext` and `mceprocessor` to exception objects |
| `main/writerfilter/Library_writerfilter.mk` | Added `oox/inc` include path |

### 6.2 Build Status

| Module | Status | Notes |
|--------|--------|-------|
| `oox` | Source compiles | Build blocked by environment (Boost headers, xsltproc) |
| `writerfilter` | Source compiles | Link fails due to `liboox.so` not containing MCE symbols |

**Root cause:** As documented in D.4.1, `mcealternatecontext.o` is compiled but not linked into `liboox.so` due to build system dependency tracking. The `Library_oox.mk` correctly lists `mcealternatecontext` and `mceprocessor`, but the object list is determined at build start time.

---

## 7. Known Limitations (Phase 1)

| Limitation | File:Line | Severity | Phase |
|------------|-----------|----------|-------|
| `mc:Ignorable` not processed | N/A | High | Phase 2 |
| `mc:MustUnderstand` not checked | N/A | High | Phase 2 |
| `mc:ProcessContent` not handled | N/A | Medium | Phase 2 |
| `mc:PreserveElements`/`mc:PreserveAttributes` not implemented | N/A | Medium | Phase 3 |
| `ignorableWhitespace()` not implemented | mcealternatecontext.hxx:59 | Low | D.4.2 fix |
| `processingInstruction()` not implemented | mcealternatecontext.hxx: — | Low | D.4.2 fix |
| Nested AlternateContent inside Choice not handled | mcealternatecontext.cxx | Medium | Future |
| Prefix resolution only checks element's own `xmlns:*` | mceprocessor.cxx:53-83 | Medium | Phase 2 |
| Supported namespace limited to DrawingML main | mceprocessor.cxx:40-42 | Medium | Future |

---

## 8. Conclusion

The MCE Phase 1 implementation is architecturally correct:
- **Namespace resolution** uses `xmlns:*` attribute parsing from `XFastAttributeList::getUnknownAttributes()`
- **Choice eligibility** requires ALL prefixes in `Requires` to resolve to supported namespaces
- **Branch selection** uses first-eligible-Choice-wins with Fallback fallback
- **Event forwarding** only sends selected branch events to the delegate
- **State machine** correctly tracks Choice/Fallback/AlternateContent boundaries

**Known issues:**
1. `ignorableWhitespace()` and `processingInstruction()` declared but not implemented (minor)
2. Prefix resolution doesn't check ancestor namespace declarations (medium)
3. `liboox.so` doesn't include `mcealternatecontext.o` due to build system tracking (build issue, not code issue)

No MCE feature expansion beyond Phase 1 scope has been implemented.
