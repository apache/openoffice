# OOXML Milestone A — Executive Summary

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Deliverables

| # | File | Status |
|---|------|--------|
| 1 | `ooxml-architecture.md` | Complete — module map, import/export architecture, dependency diagram |
| 2 | `ooxml-filter-inventory.md` | Complete — all filter registrations, import/export capabilities, UNO services |
| 3 | `ooxml-strict-transitional.md` | Complete — namespace/packaging differences, Strict support assessment |
| 4 | `ooxml-schema-coverage.md` | Complete — schema inventory, implemented vs unimplemented, coverage tools |
| 5 | `ooxml-testing-inventory.md` | Complete — existing tests, execution instructions, missing coverage |
| 6 | `ooxml-symphony-comparison.md` | Complete — Symphony source not available, comparison requires external research |
| 7 | `ooxml-milestone-A-summary.md` | Complete — this document |

---

## 2. Confirmed Findings (VERIFIED)

### 2.1 Architecture

| Finding | Evidence |
|---------|----------|
| Three OOXML subsystems: `oox/` (XLSX/PPTX import), `writerfilter/` (DOCX import), `sc/source/filter/excel/` (XLSX export) | Directory structure + source inspection |
| `main/oox/source/core/xmlfilterbase.cxx` — core OOXML filter framework with `XmlFilterBase` | `xmlfilterbase.cxx:79` |
| `main/oox/source/core/relations.cxx` — OPC relationship handling | `relations.cxx:56` |
| `main/oox/source/export/drawingml.cxx` — DrawingML export used by DOCX | `drawingml.cxx:1` |
| `main/oox/source/ppt/pptimport.cxx` — PPTX import only, declares ExportFilter but no exportDocument override | `pptimport.cxx:67-78` |
| `main/oox/source/xls/excelfilter.cxx` — XLSX import implemented, `exportDocument()` returns false | `excelfilter.cxx:131-134` |
| `main/sw/source/filter/ww8/docxexportfilter.cxx` — DOCX ExportFilter with working exportDocument | `docxexportfilter.cxx:46-88` |
| `main/sc/source/filter/excel/` — native XLSX XML exporter reachable via ScDocShell::ConvertTo | `excel.cxx:269-303`, `expop2.cxx:165-191` |
| `main/sd/source/filter/eppt/` — legacy binary PPT export only, NOT PPTX | `eppt.cxx:268` |
| `main/package/source/zippackage/` — ZIP/OPC package infrastructure | Directory listing |
| `main/xmlsecurity/` — encryption support | Directory listing |

### 2.2 Filter Registration

| Finding | Evidence |
|---------|----------|
| No OOXML filter declares EXPORT flag | `calc_MS_Excel_2007_XML.xcu:23`, `MS_Word_2007_XML.xcu:23`, `impress_MS_PowerPoint_2007_XML.xcu:23` |
| XLSX FilterService: `com.sun.star.comp.oox.xls.ExcelFilter` | `calc_MS_Excel_2007_XML.xcu:25` |
| DOCX FilterService: `com.sun.star.comp.Writer.WriterFilter` (import-oriented) | `MS_Word_2007_XML.xcu:25` |
| PPTX FilterService: `com.sun.star.comp.oox.ppt.PowerPointImport` | `impress_MS_PowerPoint_2007_XML.xcu:25` |
| `DocxExportFilter` (`com.sun.star.comp.Writer.DocxExport`) exists as standalone ExportFilter but not referenced by any xcu | `docxexportfilter.cxx:94-111`, no xcu references found |
| Template variants exist for all three formats but also lack EXPORT flag | Template xcu files |

### 2.3 Import/Export Implementation

| Format | Import | Export | Evidence |
|--------|--------|--------|----------|
| DOCX | Fully implemented (writerfilter/DomainMapper) | Partially implemented (DocxExportFilter with stubs) | `DomainMapper.cxx`, `docxexportfilter.cxx:46-88`, `docxattributeoutput.cxx` (many TODO stubs) |
| XLSX | Fully implemented (oox ExcelFilter) | Complete native exporter (sc path), UNO export stub | `excelfilter.cxx:112-134`, `excel.cxx:269-303` |
| PPTX | Fully implemented (oox PowerPointImport) | ABSENT — no PresentationML exporter | `pptimport.cxx:67-78`, no export code found |

### 2.4 Strict vs Transitional

| Finding | Evidence |
|---------|----------|
| All serializers hardcode Transitional namespaces | `docxexport.cxx:607-611`, `excdoc.cxx:681-682`, `excdoc.cxx:793-794` |
| All relationship types use `.../2006/relationships/...` | `relations.hxx:38-49`, `docxexport.cxx:399-645` |
| VML used in DOCX (forbidden in Strict) | `docxexport.cxx:608`, `docxexport.cxx:659` |
| No Strict OOXML export path exists | No conformance abstraction, no Strict namespace/relationship/content-type code |
| No centralized conformance abstraction | Searched `oox/` and `sw/source/filter/ww8/` — none found |

### 2.5 Schema Coverage

| Finding | Evidence |
|---------|----------|
| `model.xml` is 23,545-line RELAX NG schema for DOCX | Direct file reading |
| model.xml uses Transitional namespaces only | Lines 22-35 of model.xml |
| WRITERFILTERSTATUS annotations exist in dmapper/ | grep found annotations in 10+ .cxx files |
| `status.sh` is MISSING — efforts pipeline cannot execute | `find` returned no results |
| No schema coverage mechanism for XLSX/PPTX in `oox/` | Searched entire `main/oox/` tree |

### 2.6 Package Infrastructure

| Finding | Evidence |
|---------|----------|
| ZIP package via `ZipPackage` in `main/package/source/zippackage/` | Directory listing |
| Low-level ZIP API in `main/package/source/zipapi/` | Directory listing |
| xstorage (OLE-over-ZIP) in `main/package/source/xstor/xstorage.cxx` | `xstorage.cxx:1` |
| Zip storage helper in `oox/source/helper/zipstorage.cxx` | `zipstorage.cxx:1` |
| Relations handling in `oox/source/core/relations.cxx` | `relations.cxx:56` |
| Encryption support in `main/xmlsecurity/` | Directory listing |
| No content types validation in export path | No schema validation code found |
| No malformed package handling tests | No tests found |

### 2.7 Testing

| Finding | Evidence |
|---------|----------|
| No OOXML export tests exist | Searched `main/*/qa/`, `test/` — none found |
| Import-only test documents exist | `.docx`, `.xlsx`, `.pptx` in `qa/documents/`, `qa/complex/`, `test/testgui/data/pvt/` |
| Package infrastructure tests exist | `main/package/qa/ofopxmlstorages/` — 10 Java test files |
| ODF validation framework has DOCX fixtures | 3 `.docx` files in `test/odf-validate/fixtures/` |
| No export test framework exists | No code exports documents and inspects packages |

---

## 3. Unverified Findings (UNKNOWN)

| Finding | Reason |
|---------|--------|
| Strict OOXML document import recognition | Requires runtime verification of token/namespace handling |
| Strict OOXML document round-trip preservation | No Strict export exists to test round-trip |
| Filter registry behavior after adding EXPORT flag | Requires build and installation to verify generated configuration |
| Malformed package handling security | Requires runtime testing with corrupt ZIP files |
| IBM Symphony OOXML implementation comparison | Symphony source code not available in AOO repository |

---

## 4. Priority Recommendations for Milestone B

### 4.1 Immediate (Milestone B.1 — XLSX Export Exposure)

1. **Add EXPORT flag to `calc_MS_Excel_2007_XML.xcu`** — The native XLSX exporter is complete and reachable via `ScDocShell::ConvertTo()`. Adding `EXPORT` to the Flags property is a one-line change that exposes export through Save As.

2. **Add automated XLSX export regression tests** — Create tests that export a Calc document to XLSX and inspect the OPC package (`[Content_Types].xml`, `_rels`, `xl/workbook.xml`, sheet XML, styles, shared strings).

3. **Document UNO API limitation** — `ExcelFilter::exportDocument()` returns false; UNO-based XLSX export requires implementing this method.

### 4.2 Short-term (Milestone B.2 — DOCX Export Exposure)

1. **Add EXPORT flag to `MS_Word_2007_XML.xcu`** — Change FilterService from `WriterFilter` to `DocxExportFilter` or register `DocxExportFilter` as an additional export filter.

2. **Fix DocxAttributeOutput stubs** — Address TODO stubs for Ruby, drop caps, OLE objects, redlines, form fields.

3. **Add DOCX export regression tests** — Package inspection, round-trip validation.

### 4.3 Medium-term (Milestone B.3 — PPTX Export)

1. **Implement PPTX exporter** — Reuse `DrawingML` (`oox/source/export/drawingml.cxx`), `XmlFilterBase` (`oox/source/core/xmlfilterbase.cxx`), and package infrastructure (`main/package/`).

2. **Create `PptxExportFilter` UNO service** — Analogous to `DocxExportFilter`.

3. **Add PPTX export regression tests.**

### 4.4 Long-term (Milestone B.4 — Strict OOXML)

1. **Create `OoxmlConformance` abstraction** — Centralized namespace, relationship type, and content-type selection by conformance level.

2. **Update all serializers** to use the abstraction instead of hardcoded Transitional values.

3. **Remove VML from DOCX export** — Convert to DrawingML-only output.

---

## 5. Completion Criteria Status

| Criterion | Status |
|-----------|--------|
| All three applications investigated | Complete |
| Import and export assessed independently | Complete |
| Strict and Transitional separately assessed | Complete |
| Existing schema coverage tools evaluated | Complete |
| Existing tests inventoried | Complete |
| Every major finding has source evidence | Complete |
| No production code modified | Confirmed |
| All seven reports present and internally consistent | Complete |

---

## 6. Files Produced

| File | Path |
|------|------|
| Architecture inventory | `audit/milestone-A/ooxml-architecture.md` |
| Filter registration audit | `audit/milestone-A/ooxml-filter-inventory.md` |
| Strict vs Transitional | `audit/milestone-A/ooxml-strict-transitional.md` |
| Schema coverage | `audit/milestone-A/ooxml-schema-coverage.md` |
| Testing inventory | `audit/milestone-A/ooxml-testing-inventory.md` |
| Symphony comparison | `audit/milestone-A/ooxml-symphony-comparison.md` |
| Milestone A summary | `audit/milestone-A/ooxml-milestone-A-summary.md` |

**No production code was modified.** All findings are based on source code inspection only.
