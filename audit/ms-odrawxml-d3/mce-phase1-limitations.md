# MCE Phase 1 Limitations — Milestone D.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.3  
**Mode:** Implementation and documentation only — no production code changes beyond the MCE implementation

---

## 1. Explicitly Unimplemented MCE Constructs

The following MCE constructs from ISO/IEC 29500-3 are NOT implemented in Phase 1:

| Construct | Specification Reference | Reason for Deferral | Phase |
|-----------|------------------------|---------------------|-------|
| `mc:Ignorable` | ISO/IEC 29500-3, §11.2 | Requires namespace-level ignore list processing | Phase 2 |
| `mc:MustUnderstand` | ISO/IEC 29500-3, §11.2 | Requires error handling policy decisions | Phase 2 |
| `mc:ProcessContent` | ISO/IEC 29500-3, §11.2 | Requires content processing mode tracking | Phase 2 |
| `mc:PreserveElements` | ISO/IEC 29500-3, §11.2 | Requires element-level preservation tracking | Phase 3 |
| `mc:PreserveAttributes` | ISO/IEC 29500-3, §11.2 | Requires attribute-level preservation tracking | Phase 3 |
| Extension namespace preservation | ISO/IEC 29500-3, §11.2 | Requires raw XML retention or sidecar storage | Phase 3 |
| Extension-specific DrawingML (MS-ODRAWXML) | MS-ODRAWXML all sections | Requires per-extension implementation | Future milestones |

---

## 2. Supported Namespace Set (Phase 1)

The Phase 1 implementation uses a conservative supported namespace set:

| Namespace URI | Prefix | Supported? | Reasoning |
|--------------|--------|-----------|-----------|
| `http://schemas.openxmlformats.org/drawingml/2006/main` | `a:` | **Yes** | AOO implements DrawingML element parsing in `oox/source/drawingml/` (shape.cxx, fillproperties.cxx, lineproperties.cxx, textparagraph.cxx, textrun.cxx, etc.) |
| `http://schemas.openxmlformats.org/drawingml/2006/diagram` | `dgm:` | **No** | Diagram support exists but is less mature; excluded from Phase 1 |
| `http://schemas.openxmlformats.org/drawingml/2006/chart` | `c:` | **No** | Chart support exists but is less mature; excluded from Phase 1 |
| `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `w:` | **No** | Document-level markup, not DrawingML content |
| `http://schemas.openxmlformats.org/spreadsheetml/2006/main` | `x:` | **No** | Document-level markup, not DrawingML content |
| `http://schemas.openxmlformats.org/presentationml/2006/main` | `p:` | **No** | Document-level markup, not DrawingML content |
| All MS-ODRAWXML namespaces | various | **No** | Not implemented at all |
| All VML namespaces | `v:` | **No** | Legacy format, not DrawingML |

**Note:** The supported namespace set can be expanded in future phases by modifying `MceProcessor::getSupportedNamespaceSet()` in `main/oox/source/core/mceprocessor.cxx`.

---

## 3. Known Limitations

### 3.1 Choice Evaluation Timing

**Limitation:** `Requires` is evaluated when `startUnknownElement` is called for `mc:Choice`, before any of the Choice's children are processed. This is correct for most cases but means that if the Choice's eligibility depends on content within the Choice itself (not just the `Requires` attribute), that dependency is not considered.

**Impact:** Minimal. `Requires` only lists namespace prefixes, which are always available at element start.

**Mitigation:** None needed.

### 3.2 Namespace Prefix Resolution Scope

**Limitation:** `Requires` prefixes are resolved against namespace declarations found on the `mc:Choice` element itself (via `extractNamespaceDeclarations`). Namespace declarations on ancestor elements are NOT consulted.

**Impact:** If a prefix is declared on an ancestor element but not on the `mc:Choice` element itself, the prefix will be unresolvable and the Choice will be treated as ineligible.

**Mitigation:** For Phase 1, this is acceptable because `mc:AlternateContent` structures typically declare all needed namespaces on the `AlternateContent` or `Choice` elements themselves. Full ancestor namespace resolution can be added in Phase 2.

### 3.3 Context Delegation for oox/ Pipeline

**Limitation:** `MceAlternateContext` delegates non-MCE context creation to the parent `FragmentHandler` via `getFastContextHandler()`. For `FragmentHandler` subclasses that override `createFastChildContext` (e.g., DrawingML shape contexts), this works correctly. For the base `FragmentHandler` (which returns 0 from `createFastChildContext`), known elements inside the selected branch will be skipped.

**Impact:** DrawingML content inside selected Choice branches is processed correctly when the parent `FragmentHandler` subclass overrides `createFastChildContext` (which DrawingML handlers do). Base `FragmentHandler` usage (rare for document content) would skip known elements.

**Mitigation:** Application-specific `FragmentHandler` subclasses that override `createFastChildContext` work correctly. No mitigation needed for the common case.

### 3.4 Context Delegation for writerfilter Pipeline

**Limitation:** `MceAlternateContext` is created with an `OOXMLFastContextHandler` delegate (obtained via `getContextHandler()` and copied). The delegate processes events through `OOXMLFactory`. However, `OOXMLFastContextHandler::startUnknownElement` is a no-op in release mode, so unknown elements inside the selected branch are not logged or processed at the `startUnknownElement` level. Processing happens through `startFastElement` → `OOXMLFactory::startAction`.

**Impact:** Known elements inside selected Choice branches are processed correctly via `startFastElement`. Unknown elements inside selected Choice branches may be silently dropped (same as existing behavior for non-MCE content).

**Mitigation:** None needed for Phase 1.

### 3.5 No Raw XML Preservation

**Limitation:** Unsupported extension content is NOT preserved in any form (no raw XML retention, no sidecar storage, no relationship preservation).

**Impact:** Documents containing MS-ODRAWXML extensions lose that content on import and cannot round-trip it on export.

**Mitigation:** Phase 3 (preservation) will address this.

### 3.6 No mc:MustUnderstand Error Handling

**Limitation:** `mc:MustUnderstand` is not checked. If a document contains `mc:AlternateContent mc:MustUnderstand="true"` with an unsupported Choice, the document loads silently without the extension content.

**Impact:** Users are not alerted when critical extension content is lost.

**Mitigation:** Phase 2 will add `mc:MustUnderstand` checking.

---

## 4. Error-Handling Limitations

| Malformed Structure | Phase 1 Behavior | Risk |
|---------------------|-----------------|------|
| Missing Requires attribute | Choice treated as ineligible → Fallback used | Low |
| Empty Requires attribute | Choice treated as ineligible → Fallback used | Low |
| Unresolved prefix | Choice treated as ineligible → Fallback used | Low |
| Duplicate Fallback elements | First Fallback selected; subsequent Fallbacks discarded | Low |
| Choice outside AlternateContent | Treated as unknown element, skipped | Low |
| Fallback outside AlternateContent | Treated as unknown element, skipped | Low |
| AlternateContent without Choice | Fallback selected if present; otherwise BRANCH_NONE | Low |
| AlternateContent without Fallback | BRANCH_NONE if no eligible Choice | Low |
| Invalid nesting | Handled by state machine; unrecognized structure discarded | Low |
| Invalid namespace declarations | Prefix resolution fails → Choice ineligible | Low |

**No crashes or assertion failures** are expected from malformed MCE structures. The implementation follows AOO's existing parser error-handling conventions (silent skip of unknown content).

---

## 5. Compatibility Risks

| Risk | Likelihood | Severity | Mitigation Status |
|------|-----------|----------|-------------------|
| Incorrect branch selection due to incomplete supported namespace set | Medium | High | Documented supported set; expandable in future phases |
| Namespace-prefix resolution misses ancestor declarations | Medium | High | Documented limitation; Phase 2 will add full resolution |
| Writerfilter delegate reference counting | Low | Medium | Uses UNO_NOOWN reference; boost::shared_ptr owns the object |
| Legacy document regression | Low | Medium | MCE only activates for mc:AlternateContent elements |

---

## 6. Deferred Work (Future Milestones)

| Item | Description | Phase |
|------|-------------|-------|
| `mc:Ignorable` processing | Track ignorable namespace list; skip elements in ignorable namespaces without error | Phase 2 |
| `mc:MustUnderstand` | Check and report unsupported required extensions | Phase 2 |
| `mc:ProcessContent` | Control content processing mode (choice/fallback/all) | Phase 2 |
| `mc:PreserveElements`/`mc:PreserveAttributes` | Preserve specified elements/attributes in ignorable contexts | Phase 3 |
| Raw XML retention for round-trip | Store unsupported extension markup in sidecar storage | Phase 3 |
| Expanded supported namespace set | Add diagram, chart, wordprocessingml, etc. | Future |
| MS-ODRAWXML extension-specific implementations | Picture effects, 3D models, chart extensions, etc. | Future milestones |
