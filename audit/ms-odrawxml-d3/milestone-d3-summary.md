# Milestone D.3 — Summary: Minimal MCE AlternateContent Selection Engine

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Starting commit:** trunk HEAD (branch OOXML created from trunk)  
**Milestone:** D.3  
**Mode:** Implementation — production code changes made

---

## 1. Implementation Summary

Milestone D.3 implements a minimal, reusable Markup Compatibility and Extensibility (MCE) selection engine for `mc:AlternateContent`, `mc:Choice`, and `mc:Fallback` processing. This is Phase 1 of the three-phase MCE support plan defined in Milestone D.2.

**Problem addressed:** AOO's OOXML import paths silently skipped all `mc:AlternateContent` content, including `mc:Fallback` content that AOO could render. This caused document content loss when opening OOXML files containing DrawingML extensions wrapped in MCE constructs.

**Solution:** A shared `MceProcessor` selection engine and `MceAlternateContext` context handler that:
1. Evaluates `mc:Choice` branches in document order using `mc:Requires` namespace resolution
2. Selects the first eligible Choice (where all required namespaces are in the Phase 1 supported set)
3. Falls back to `mc:Fallback` when no Choice is eligible
4. Forwards only the selected branch's events to the application's context handlers
5. Discards unselected branch events (no content from multiple branches is parsed)

---

## 2. Source Changes

### 2.1 New Files (4)

| File | Path | Lines | Purpose |
|------|------|-------|---------|
| Header | `main/oox/inc/oox/core/mceprocessor.hxx` | ~85 | MceProcessor class declaration |
| Implementation | `main/oox/source/core/mceprocessor.cxx` | ~120 | MceProcessor: namespace resolution, eligibility, selection |
| Header | `main/oox/inc/oox/core/mcealternatecontext.hxx` | ~117 | MceAlternateContext class declaration |
| Implementation | `main/oox/source/core/mcealternatecontext.cxx` | ~200 | MceAlternateContext: branch selection, event forwarding |

### 2.3 Modified Files (4)

| File | Path | Change |
|------|------|--------|
| Header | `main/oox/inc/oox/core/fragmenthandler.hxx` | Added `getMarkupCompatNamespace()` static method, `MceAlternateContext` forward declaration, friend declaration |
| Implementation | `main/oox/source/core/fragmenthandler.cxx` | Added `getMarkupCompatNamespace()` impl; modified `createUnknownChildContext()` to intercept `mc:AlternateContent` |
| Build | `main/oox/Library_oox.mk` | Added `mcealternatecontext` and `mceprocessor` to exception objects |
| Implementation | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Added `#include <oox/core/mcealternatecontext.hxx>`; modified `createUnknownChildContext()` to intercept `mc:AlternateContent` |

### 2.4 Build Integration

The two new source files are added to `gb_Library_add_exception_objects` in `main/oox/Library_oox.mk`. Header files are in `main/oox/inc/oox/core/`, covered by the existing `oox_inc` package headers.

Writerfilter links against `oox` (already configured in `Library_writerfilter.mk`), so symbol resolution for `MceAlternateContext` works without additional build changes.

---

## 3. Supported Namespace Set (Phase 1)

| Namespace URI | Prefix | Supported |
|--------------|--------|-----------|
| `http://schemas.openxmlformats.org/drawingml/2006/main` | `a:` | Yes |

**Reasoning:** Only namespaces with actual element-level implementation in AOO's DrawingML import code (`oox/source/drawingml/`) are included. All other namespaces (SpreadsheetML, WordprocessingML, PresentationML, MS-ODRAWXML extensions, VML, diagram, chart) are excluded from Phase 1.

The supported set is defined in `MceProcessor::getSupportedNamespaceSet()` and can be expanded in future phases.

---

## 4. Deliverables (Audit Documentation)

| # | File | Status |
|---|------|--------|
| 1 | `audit/ms-odrawxml-d3/mce-phase1-implementation.md` | Complete — design, source files, integration points, algorithm |
| 2 | `audit/ms-odrawxml-d3/mce-phase1-test-plan.md` | Complete — 12 test fixtures, expected results, execution method |
| 3 | `audit/ms-odrawxml-d3/mce-phase1-limitations.md` | Complete — unimplemented constructs, known limitations, error handling |
| 4 | `audit/ms-odrawxml-d3/milestone-d3-summary.md` | Complete — this document |

---

## 5. Integration Points

| Pipeline | Integration Point | File | Line |
|----------|-------------------|------|------|
| Calc XLSX | `FragmentHandler::createUnknownChildContext()` | `main/oox/source/core/fragmenthandler.cxx` | ~112 |
| Impress PPTX | `FragmentHandler::createUnknownChildContext()` | `main/oox/source/core/fragmenthandler.cxx` | ~112 |
| Writer DOCX | `OOXMLFastDocumentHandler::createUnknownChildContext()` | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | ~117 |

All three pipelines use the same `MceProcessor` selection logic. The context handling differs per pipeline:
- oox/: `MceAlternateContext` delegates to the parent `FragmentHandler` (which subclasses override `createFastChildContext` for DrawingML elements)
- writerfilter: `MceAlternateContext` delegates to an `OOXMLFastContextHandler` (which processes events through `OOXMLFactory`)

---

## 6. Verification Status

| Verification Item | Status | Notes |
|------------------|--------|-------|
| Source code compiles (conceptual) | Not verified | No build performed |
| MceProcessor namespace resolution | Not tested | Unit tests planned but not executed |
| MceAlternateContext branch selection | Not tested | Unit tests planned but not executed |
| oox/ pipeline integration | Not tested | Requires build |
| writerfilter pipeline integration | Not tested | Requires build |
| Regression on existing OOXML corpus | Not tested | Requires build |
| Round-trip preservation | Not tested | Phase 3 (future milestone) |

**No build was performed.** **No runtime tests were executed.** All implementation is source-verified by inspection only.

---

## 7. Known Limitations

| Limitation | Impact | Phase |
|------------|--------|-------|
| `mc:Ignorable` not processed | Extension elements in ignorable namespaces skipped silently | Phase 2 |
| `mc:MustUnderstand` not checked | Critical extensions silently lost with no error | Phase 2 |
| `mc:ProcessContent` not handled | Fallback content in ignorable regions not processed | Phase 2 |
| `mc:PreserveElements`/`mc:PreserveAttributes` not implemented | No preservation of extension markup | Phase 3 |
| Raw XML retention not implemented | No round-trip fidelity for unsupported extensions | Phase 3 |
| Namespace prefix resolution only checks element's own `xmlns:*` | Ancestor declarations not consulted | Phase 2 |
| Supported namespace set limited to DrawingML main | Other namespaces (diagram, chart, etc.) not eligible | Future |
| No `mc:MustUnderstand` error reporting | Users not alerted to lost critical content | Phase 2 |

---

## 8. Recommended Next Milestone

**Milestone D.4 — Phase 2 MCE: Ignorable, MustUnderstand, ProcessContent**

Scope:
1. Implement `mc:Ignorable` namespace list processing
2. Implement `mc:MustUnderstand` checking with error reporting
3. Implement `mc:ProcessContent` directive handling
4. Expand supported namespace set to include diagram and chart namespaces
5. Execute Phase 1 test fixtures (F1-F12)
6. Add regression tests for existing OOXML test corpus

**Do not proceed to Phase 3 (preservation) until Phase 2 is complete and tested.**

---

## 10. Confirmation Statements

- **Branch:** `OOXML` (created from trunk)
- **Production code modified:** Yes — 4 files modified, 4 new files created
- **Build performed:** No — intentionally deferred per milestone instructions
- **Runtime tests executed:** No — test fixtures defined but not executed
- **LibreOffice code imported:** No
- **External dependencies introduced:** No
- **Commit hash:** Not applicable — changes not committed
