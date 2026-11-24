# IBM Symphony Comparison — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

IBM Symphony source code is **not available** in the Apache OpenOffice repository. The only Symphony references in the AOO codebase are limited to UI theme icons (`Bool_UseSymphonyIcons`) and accessibility code comments — none related to OOXML file format implementation. A meaningful comparison of IBM Symphony's OOXML implementation against AOO's cannot be performed without obtaining Symphony source code separately.

Per Milestone A restrictions, no code is imported from Symphony.

---

## 2. Symphony References in AOO Codebase

| File | Line | Context | OOXML Relevance |
|------|------|---------|-----------------|
| `main/sfx2/inc/sfx2/sidebar/Theme.hxx:135` | `Bool_UseSymphonyIcons` | UI theme property | None |
| `main/sfx2/source/sidebar/Theme.cxx:160,757,828` | `Bool_UseSymphonyIcons` | Theme icon selection | None |
| `main/accessibility/source/standard/vclxaccessiblelist.cxx:457` | "Added by IBM Symphony Acc team" | Accessibility comment | None |
| `main/svx/source/sidebar/graphic/GraphicPropertyPanel.cxx:291` | `Bool_UseSymphonyIcons` | Graphic panel theming | None |
| `main/svx/source/sidebar/possize/PosSizePropertyPanel.cxx:334` | `Bool_UseSymphonyIcons` | Position/size panel theming | None |
| `main/svx/source/sidebar/line/LinePropertyPanel.cxx:403` | `Bool_UseSymphonyIcons` | Line panel theming | None |
| `main/svx/source/sidebar/area/AreaPropertyPanel.cxx:674` | `Bool_UseSymphonyIcons` | Area panel theming | None |
| `main/vcl/inc/vcl/settings.hxx:949` | Symphony color scheme comment | VCL settings | None |

**None of these relate to OOXML file format import/export.**

---

## 3. Comparison Classification (UNKNOWN — Requires External Research)

| Potential Improvement Area | AOO Status | Symphony Status | Classification |
|---------------------------|------------|-----------------|----------------|
| DOCX import (DomainMapper) | Implemented | UNKNOWN | Requires investigation |
| DOCX export (DocxExportFilter) | Partially implemented | UNKNOWN | Requires investigation |
| XLSX import (ExcelFilter) | Implemented | UNKNOWN | Requires investigation |
| XLSX export (native sc exporter) | Complete, not exposed | UNKNOWN | Requires investigation |
| PPTX import (PowerPointImport) | Implemented | UNKNOWN | Requires investigation |
| PPTX export | ABSENT | UNKNOWN | Requires investigation |
| Strict OOXML support | ABSENT | UNKNOWN | Requires investigation |
| DrawingML export | Used by DOCX only | UNKNOWN | Requires investigation |
| Schema coverage tracking | writerfilter only | UNKNOWN | Requires investigation |
| OOXML package infrastructure | ZIP-based | UNKNOWN | Requires investigation |
| Filter exposure (EXPORT flag) | Missing for all OOXML | UNKNOWN | Requires investigation |
| Export regression tests | NONE | UNKNOWN | Requires investigation |

---

## 4. Recommended Next Steps for Symphony Comparison

To perform a meaningful IBM Symphony comparison, the following would be required:

1. **Obtain IBM Symphony source code** — Symphony was open-sourced by IBM; source may be available through Apache or other repositories
2. **Identify Symphony's OOXML implementation modules** — equivalent to AOO's `oox/`, `writerfilter/`, `sw/source/filter/ww8/`
3. **Compare import/export coverage** — which OOXML features Symphony supports that AOO does not
4. **Compare Strict OOXML support** — whether Symphony implements ISO/IEC 29500 Strict
5. **Compare package handling** — ZIP/OPC package implementation differences
6. **Compare test coverage** — whether Symphony has export tests AOO could adopt

**No code would be copied** — this comparison would be purely informational for prioritization.

---

## 5. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| Symphony source not in AOO repo | VERIFIED | Searched entire `main/` tree — no Symphony OOXML implementation |
| Only UI/accessibility Symphony references found | VERIFIED | grep for "symphony" found 8 matches, none OOXML-related |
| Cannot compare OOXML implementations | VERIFIED | No Symphony OOXML source code available for comparison |
