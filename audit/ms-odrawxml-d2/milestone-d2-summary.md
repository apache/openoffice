# Milestone D.2 — MCE/Fallback Validation and Design — Executive Summary

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Design only — no production code changes

---

## 1. Deliverables

| # | File | Status |
|---|------|--------|
| 1 | `mce-source-validation.md` | Complete — independent validation of D.1 findings |
| 2 | `mce-behavior-analysis.md` | Complete — MCE behavior analysis and deficiency catalog |
| 3 | `mce-fixture-matrix.md` | Complete — 14 proposed test fixtures with expected results |
| 4 | `mce-implementation-design.md` | Complete — phased implementation design |
| 5 | `mce-risk-analysis.md` | Complete — compatibility, security, regression risks |
| 6 | `milestone-d2-summary.md` | Complete — this document |

---

## 2. Starting Point

- **Branch:** `OOXML` (created from trunk)
- **Starting state:** D.1 findings (`audit/ms-odrawxml-d1/`) reported no MCE handling
- **No production code modified:** Confirmed — only documentation files created
- **No build performed:** Confirmed — source audit and design only

---

## 3. Findings That Confirm D.1

| D.1 Finding | D.2 Validation | Evidence |
|-------------|---------------|----------|
| No general `mc:AlternateContent`/MCE handling | CONFIRMED | Zero MCE element handler code across all source paths |
| No recognized MS-ODRAWXML extension namespaces | CONFIRMED | Zero `schemas.microsoft.com/office/drawing` URI references |
| Multiple import locations skip `extLst` | CONFIRMED | 9 locations in `oox/source/drawingml/` |
| Multiple export locations contain `XML_extLst` OOXTODO | CONFIRMED | 7 locations in `sc/source/filter/excel/` |
| Significant potential document-content loss | CONFIRMED | All extension content silently dropped |

---

## 4. Findings That Revise D.1

### 4.1 Namespace Registration EXISTS (D.1 Under-reported)

D.1 stated "Zero references to any MS-ODRAWXML namespace URI" — correct for MS-ODRAWXML namespaces, but D.1 did not check the **Markup Compatibility namespace** specifically.

**New finding:** `NMSP_markupCompat` IS registered at `main/oox/source/core/xmlfilterbase.cxx:122`:
```cpp
maFastParser.registerNamespace( NMSP_markupCompat );    // i123528
```
**Namespace URI:** `http://schemas.openxmlformats.org/markup-compatibility/2006`  
**Defined at:** `main/oox/source/token/namespaces.txt:78`

**Significance:** The MCE namespace URI is known to the fast parser, so `mc:`-prefixed elements will not cause namespace resolution errors. However, registration alone does NOT provide MCE semantics — there is no code that processes `mc:AlternateContent`, `mc:Choice`, `mc:Fallback`, etc.

### 4.2 MCE Tokens Exist but Are Unused

Tokens `AlternateContent` (`tokens.txt:40`), `Choice` (`tokens.txt:83`), and `Fallback` (`tokens.txt:147`) ARE defined in the token system. However:
- Zero source code references to `TOKEN(AlternateContent)`, `TOKEN(Choice)`, or `TOKEN(Fallback)` for MCE processing
- These tokens exist in the global token table but are never dispatched to any handler

---

## 5. MCE Behavior Deficiencies

AOO's actual behavior when encountering MCE constructs:

| Construct | Expected (ISO 29500-3) | AOO Actual | Deficiency Severity |
|-----------|----------------------|------------|-------------------|
| `mc:AlternateContent` | Evaluate Choice/Fallback pairs | Entire wrapper skipped | Critical |
| `mc:Choice` | Evaluate `Requires`, select matching branch | Treated as unknown, skipped | Critical |
| `mc:Fallback` | Use when no Choice matches | Treated as unknown, skipped | Critical |
| `mc:Ignorable` | Declare ignorable namespaces | Not read | High |
| `mc:MustUnderstand` | Fail or warn if extension not understood | Not read | High |
| `mc:ProcessContent` | Control content processing in ignorable regions | Not read | Medium |
| `mc:PreserveElements`/`mc:PreserveAttributes` | Preserve specified elements/attributes | Not read | Medium |
| `Requires` prefix resolution | Resolve prefixes to namespace URIs | Not implemented | High |

**Root cause:** Unknown elements are handled by `FragmentHandler::createUnknownChildContext()` returning 0 (`fragmenthandler.cxx:102-105`) and `OOXMLFastContextHandler::createUnknownChildContext()` returning null (`OOXMLFastContextHandler.cxx:287-296`). All `mc:*` elements are treated as unknown and their content is skipped, including Fallback content that AOO could render.

---

## 6. Proposed MCE Architecture

### 6.1 Design Summary

A **shared MCE processor** in `oox/source/core/mceprocessor.cxx` (new file) that:

1. Evaluates `mc:AlternateContent` Choice/Fallback branches
2. Resolves `Requires` prefix declarations to namespace URIs
3. Checks `mc:MustUnderstand` and reports errors
4. Processes `mc:ProcessContent` directives
5. Tracks `mc:Ignorable` namespace lists

### 6.2 Integration Points

| Application | Integration Point | File |
|-------------|------------------|------|
| Calc (XLSX) | `XmlFilterBase` (base class) | `oox/source/core/xmlfilterbase.cxx` |
| Impress (PPTX) | `XmlFilterBase` (base class) | `oox/source/core/xmlfilterbase.cxx` |
| Writer (DOCX) | `OOXMLFastDocumentHandler` | `writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` |

### 6.3 Phased Implementation

| Phase | Goal | Effort |
|-------|------|--------|
| **Phase 1** | Minimal selection and fallback — `mc:AlternateContent` branch evaluation | Low-Medium |
| **Phase 2** | Ignorable content, processing directives, `MustUnderstand` | Low-Medium |
| **Phase 3** | Preservation of unsupported extension markup (optional) | Medium-High |

**Recommended starting point:** Phase 1 only — enables graceful degradation for ALL MS-ODRAWXML extensions with a single shared implementation.

---

## 7. Preservation Feasibility

**Raw XML retention:** NOT RECOMMENDED — high risk of relationship ID and drawing object inconsistency.

**Package-part preservation:** NOT RECOMMENDED — too high risk of producing inconsistent packages.

**Object-level preservation:** RECOMMENDED — convert extension content to AOO's internal drawing model where possible; skip what cannot be converted. This is consistent with how AOO already handles unsupported features.

---

## 8. Top Risks

| Risk | Likelihood | Severity | Mitigation |
|------|-----------|----------|------------|
| Incorrect branch selection | Medium | High | Comprehensive fixture testing (F1-F14) |
| Namespace-prefix resolution errors | Medium | High | Proper namespace context stack |
| Unsupported content discarded | Medium | High | Conservative fail-safe defaults |
| Malformed/hostile XML | Low | High | Nesting limits, attribute length limits |
| Legacy document regression | Low | Medium | MCE only activates for `mc:AlternateContent` |

---

## 9. Recommended Next Milestone

**Milestone D.3 — Phase 1 MCE Implementation**

Scope:
1. Create `MceProcessor` class in `oox/source/core/mceprocessor.cxx` / `.hxx`
2. Integrate into `XmlFilterBase` for Calc and Impress import
3. Integrate into `OOXMLFastDocumentHandler` for Writer import
4. Implement fixture tests F1-F3, F9, F13, F14
5. Verify no regressions in existing DOCX/XLSX/PPTX test corpus

**Do NOT proceed to Phase 2 or Phase 3 until Phase 1 is complete and tested.**

---

## 10. Unresolved Questions

| Question | Status |
|----------|--------|
| Does AOO's XML parser use strict or lax validation for unknown elements? | UNKNOWN — requires source analysis of parser configuration |
| What happens when an `mc:AlternateContent` element is encountered today? | ANSWERED — entire wrapper skipped (source verified) |
| Are extension relationships (content part rels) followed or ignored? | UNKNOWN — requires source analysis of relationship handling in filters |
| Does AOO retain any unknown XML in memory for round-trip fidelity? | UNKNOWN — requires source analysis of document model |
| What is the interaction between MS-ODRAWXML extensions and AOO's existing DrawingML support? | PARTIALLY ANSWERED — extensions compound existing gaps (charts, drawings missing from XLSX export per W.1) |

---

## 11. Confirmation Statements

- **Branch:** `OOXML` (from trunk)
- **Files created:** 6 deliverables in `audit/ms-odrawxml-d2/`
- **Findings that confirm D.1:** No MCE handling, no MS-ODRAWXML namespaces, extLst skipped, XML_extLst OOXTODOs
- **Findings that revise D.1:** `NMSP_markupCompat` namespace IS registered at `xmlfilterbase.cxx:122`; MCE tokens exist but are unused
- **Recommended MCE architecture:** Shared `MceProcessor` in `oox/source/core/`, integrated via `XmlFilterBase` (Calc/Impress) and `OOXMLFastDocumentHandler` (Writer)
- **Recommended implementation phases:** Phase 1 (selection/fallback) → Phase 2 (ignorable/directives) → Phase 3 (preservation, optional)
- **Unresolved items:** 5 questions listed in Section 10
- **No production code was changed:** Confirmed — only documentation files created
- **No build was performed:** Confirmed — source audit and design only
- **Commit hash:** Not applicable — no commits made
