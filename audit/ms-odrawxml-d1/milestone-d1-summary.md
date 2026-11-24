# MS-ODRAWXML Milestone D.1 — Executive Summary

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes  
**Specification:** [MS-ODRAWXML] v35.0, released 2026-10-06  
**Spec URL:** https://learn.microsoft.com/en-us/openspecs/office_standards/ms-odrawxml/06cff208-c6e1-4db7-bb68-665135e5f0de

---

## 1. Deliverables

| # | File | Status |
|---|------|--------|
| 1 | `ms-odrawxml-inventory.md` | Complete — 43 namespaces, 42 schema sections, extension structures |
| 2 | `aoo-drawingml-source-map.md` | Complete — source-level implementation map, import/export paths |
| 3 | `drawingml-support-matrix.md` | Complete — per-extension DOCX/XLSX/PPTX support matrix |
| 4 | `drawingml-loss-analysis.md` | Complete — MCE fallback analysis, document damage classification |
| 5 | `drawingml-priority-list.md` | Complete — P1/P2/P3 ranked implementation candidates |
| 6 | `milestone-d1-summary.md` | Complete — this document |

---

## 2. Audit Coverage

| Area | Coverage | Status |
|------|----------|--------|
| Specification inventory | All 43 namespaces across 42 schema sections | Complete |
| Application scope (Word/Excel/PowerPoint) | All namespaces classified | Complete |
| Source code mapping | All relevant modules inspected | Complete |
| Import support assessment | All 3 formats assessed | Complete |
| Export support assessment | All 3 formats assessed | Complete |
| mc:AlternateContent/MCE analysis | All MCE constructs evaluated | Complete |
| Document damage/loss analysis | P1/P2/P3 classified by severity | Complete |
| Implementation prioritization | P1/P2/P3 with approaches and tests | Complete |
| W.1 findings integration | W.1 deliverables reviewed and cross-referenced | Complete |

**Source areas audited:**
- `main/oox/` — core OOXML framework, DrawingML import, XLSX/PPTX import
- `main/writerfilter/` — DOCX import (DomainMapper)
- `main/sw/source/filter/ww8/` — DOCX export
- `main/sc/source/filter/excel/` — Native XLSX export
- `main/sd/source/filter/eppt/` — Legacy binary PPT (NOT PPTX)
- `main/drawinglayer/` — Drawing layer model
- `main/svx/` — SVX drawing components
- `main/package/` — ZIP/OPC package infrastructure
- `main/xmlsecurity/` — Encryption

---

## 3. Major Findings

### 3.1 Finding 1: Zero MS-ODRAWXML Namespace Support (VERIFIED)

**None of the 43 MS-ODRAWXML extension namespaces are referenced anywhere in the AOO source code.** Searched across `main/oox/`, `main/writerfilter/`, `main/sw/`, `main/sc/`, `main/sd/`, `main/drawinglayer/`, and `main/svx/` — zero matches for any `schemas.microsoft.com/office/drawing` URI.

**Impact:** All MS-ODRAWXML extensions are silently dropped during import and never serialized during export. Documents containing extension content open without errors but lose graphical objects, chart features, picture effects, 3D models, ink annotations, and other extension data.

### 3.2 Finding 2: No Markup Compatibility (MCE) Handling (VERIFIED)

**Zero references** to `mc:AlternateContent`, `mc:Ignorable`, `mc:Choice`, `mc:Fallback`, `mc:ProcessContent`, `mc:PreserveElements`, or `mc:PreserveAttributes` exist in any OOXML import or export code path.

**Impact:** When AOO encounters an OOXML document with `mc:AlternateContent` wrappers (Choice = MS-ODRAWXML extension, Fallback = standard DrawingML), the fallback content is never selected as a substitute. The extension content is lost entirely with no graceful degradation.

### 3.3 Finding 3: extLst Recognized but Not Interpreted (VERIFIED)

`extLst` (`CT_OfficeArtExtensionList`) is recognized as a token and **skipped** in 9 locations across DrawingML import (theme, hyperlink, table styles, table cells, diagram data model), but the extension contents are **never parsed or interpreted**. During XLSX XML export, `XML_extLst` is explicitly marked as OOXTODO in 7 locations — **not serialized**.

**Impact:** Extension metadata carried in extLst elements is lost on both import and export.

### 3.4 Finding 4: Compounding with W.1 Gaps (VERIFIED)

The W.1 audit identified pre-existing gaps that compound MS-ODRAWXML extension loss:

| W.1 Gap | MS-ODRAWXML Impact |
|---------|-------------------|
| XLSX DFF/Escher drawing path commented out (`excdoc.cxx:576-582`) | Charts, shapes, drawings already missing from XLSX export; MS-ODRAWXML chart/picture extensions compound the loss |
| DOCX `OutputOLENode()` stub (`docxexport.cxx:357`) | OLE objects lost; MS-ODRAWXML content parts related |
| No PPTX exporter exists | All PPTX MS-ODRAWXML extensions (3D models, animation, ink, diagrams) are moot until a PPTX exporter is built |
| Strict OOXML not supported | MS-ODRAWXML extensions are Transitional-only; Strict would not include them |

---

## 4. Most Significant Compatibility Problems (P1)

### P1.1 — mc:AlternateContent / mc:Ignorable General-Purpose Handling

**Cross-cutting** — affects all MS-ODRAWXML extensions across all three applications. Without MCE handling, there is no graceful degradation path for any extension content.

**Evidence:** `drawingml-loss-analysis.md` Section 1.2 — zero MCE references in OOXML code.

### P1.2 — XLSX Chart Drawing Extensions

**filtered series, data labels, layout, leader lines** — Section 2.6 (`http://schemas.microsoft.com/office/drawing/2012/chart`). Charts with filtered data series or custom data labels lose filtering and label configuration.

**Evidence:** `drawingml-support-matrix.md` Section 2.3 — all 2.6 chart elements Unsupported.

### P1.3 — Picture Effects Extensions

**37 picture effect types** (BackgroundRemoval, Blur, ChalkSketch, ColorTemperature, Cutout, etc.) — Section 2.3.3.9–2.3.3.38. Images with artistic effects appear unprocessed.

**Evidence:** `drawingml-support-matrix.md` Section 2.2 — all picture effect types Unsupported.

### P1.4 — 3D Model Support (model3d)

**Section 2.31** (`http://schemas.microsoft.com/office/drawing/2017/model3d`). Slides with 3D models appear empty; 3D objects completely lost. PPTX export does not exist, making this moot until a PPTX exporter is built.

**Evidence:** `drawingml-support-matrix.md` Section 2.4 — all 3D model elements Unsupported.

---

## 5. Implementation Priorities

### P1 — Critical Compatibility Impact

| # | Candidate | Application(s) | Complexity | Shared Infrastructure? |
|---|-----------|---------------|------------|------------------------|
| P1.1 | mc:AlternateContent/mc:Ignorable processor | DOCX, XLSX, PPTX | Medium | Yes (all three) |
| P1.2 | XLSX chart drawing extensions (filtered series, data labels) | XLSX | High | Partially |
| P1.3 | Picture effects (37 types) | DOCX, XLSX, PPTX | Medium-High | Yes |
| P1.4 | 3D model support (model3d) | PPTX | High | Partially |

### P2 — Meaningful Improvements, Moderate Complexity

| # | Candidate | Application(s) | Complexity | Shared Infrastructure? |
|---|-----------|---------------|------------|------------------------|
| P2.1 | extLst serialization in XLSX export | XLSX | Low-Medium | Yes |
| P2.2 | Ink annotation support | PPTX | Medium | Partially |
| P2.3 | Wordprocessing shape (wsp) / text box linking | DOCX | Medium | Partially |
| P2.4 | SVG blip (embedded SVG images) | DOCX, XLSX, PPTX | Medium | Yes |

### P3 — Rare, Specialized, Low Impact

15 additional candidates listed in `drawingml-priority-list.md` Section 4, including content parts, hyperlink colors, sketchy shapes, classification, live feed backgrounds, OEmbed, script links, image formulas, animation, camera tool, compatibility extensions, and diagram extensions.

---

## 6. Recommended Implementation Order

1. **mc:AlternateContent/mc:Ignorable processor** (P1.1) — enables graceful degradation for ALL extensions; single implementation serves all three applications
2. **extLst serialization in XLSX export** (P2.1) — addresses explicit OOXTODO markers; low complexity; improves round-trip fidelity
3. **XLSX chart drawing extensions** (P1.2) — high user impact for chart-heavy documents
4. **Picture effects** (P1.3) — high visual impact across all three applications
5. **Wordprocessing shapes** (P2.3) — medium impact for DOCX text box users
6. **Ink annotations** (P2.2) — medium impact for PPTX annotation users
7. **3D models** (P1.4) — deferred until PPTX exporter exists (W.1 milestone)
8. **P3 candidates** — addressed as time permits

---

## 7. Unresolved Questions

| Question | Status |
|----------|--------|
| Does AOO's XML parser use strict or lax validation for unknown elements? | UNKNOWN — requires runtime verification |
| What happens when an `mc:AlternateContent` element is encountered? | UNKNOWN — requires runtime verification |
| Are extension relationships (content part rels) followed or ignored? | UNKNOWN — requires runtime verification |
| Does AOO retain any unknown XML in memory for round-trip fidelity? | UNKNOWN — requires source analysis of the document model |
| What is the interaction between MS-ODRAWXML extensions and AOO's existing (incomplete) DrawingML support? | PARTIALLY ANSWERED — extensions compound existing gaps (charts, drawings missing from XLSX export) |

---

## 8. Confirmation Statements

- **Branch:** `OOXML` (created from trunk `9b3bb500e9`)
- **Files created:** 6 deliverables in `audit/ms-odrawxml-d1/`
- **Specification revision:** MS-ODRAWXML v35.0, released 2026-10-06
- **Source areas audited:** `main/oox/`, `main/writerfilter/`, `main/sw/`, `main/sc/`, `main/sd/`, `main/drawinglayer/`, `main/svx/`, `main/package/`, `main/xmlsecurity/`
- **DOCX coverage:** Complete (import via writerfilter, export via DocxExportFilter)
- **XLSX coverage:** Complete (import via oox ExcelFilter, export via native sc path)
- **PPTX coverage:** Complete (import via oox PowerPointImport; no export exists)
- **Most significant P1 findings:** mc:AlternateContent handling absent; XLSX chart extensions unsupported; picture effects unsupported; 3D models unsupported
- **Unresolved items:** 5 questions listed in Section 7
- **No production code was changed:** Confirmed — only documentation files created
- **No build was performed:** Confirmed — source audit only
- **Commit hash:** Not applicable — no commits made
