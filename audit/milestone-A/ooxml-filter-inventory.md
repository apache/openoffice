# OOXML Filter Registration Audit — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

Three OOXML filter configurations exist for. All three declare `IMPORT` but **none declare `EXPORT`** in their Flags property. Two of three (`PowerPointImport`, `ExcelFilter`) declare `ExportFilter` as a UNO service but their `exportDocument()` implementations either do not exist or return `false`. The third (`WriterFilter`) points to an import-oriented implementation.

| Format | Filter Name | Flags | EXPORT? | FilterService | Export Implemented? |
|--------|------------|-------|---------|---------------|---------------------|
| XLSX | Calc MS Excel 2007 XML | `IMPORT ALIEN 3RDPARTYFILTER PREFERRED` | **No** | `com.sun.star.comp.oox.xls.ExcelFilter` | No (returns false) |
| XLSX Template | Calc MS Excel 2007 XML Template | `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH` | **No** | `com.sun.star.comp.oox.xls.ExcelFilter` | No (returns false) |
| DOCX | MS Word 2007 XML | `IMPORT ALIEN 3RDPARTYFILTER` | **No** | `com.sun.star.comp.Writer.WriterFilter` | No (import-oriented) |
| DOCX Template | MS Word 2007 XML Template | `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH` | **No** | `com.sun.star.comp.Writer.WriterFilter` | No (import-oriented) |
| PPTX | Impress MS PowerPoint 2007 XML | `IMPORT ALIEN 3RDPARTYFILTER PREFERRED` | **No** | `com.sun.star.comp.oox.ppt.PowerPointImport` | No (declared but not implemented) |
| PPTX Template | Impress MS PowerPoint 2007 XML Template | `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH PREFERRED` | **No** | `com.sun.star.comp.oox.ppt.PowerPointImport` | No (declared but not implemented) |

**Key finding:** No OOXML filter is registered as export-capable in the filter configuration. Export capability exists in code for XLSX (via the sc/native path, not the UNO ExcelFilter) and partially for DOCX (via DocxExportFilter), but neither is exposed through the filter registry.

---

## 2. Detailed Filter Registration Inventory

### 2.1 XLSX — Calc MS Excel 2007 XML

**Configuration file:** `main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML.xcu:22-31`

| Property | Value |
|----------|-------|
| Internal filter name | `Calc MS Excel 2007 XML` |
| Extensions | Not specified in xcu (derived from Type) |
| MIME types | Not specified in xcu |
| Import/export flags | `IMPORT ALIEN 3RDPARTYFILTER PREFERRED` |
| Filter service | `com.sun.star.comp.oox.xls.ExcelFilter` |
| UserData | (empty) |
| File format version | (empty) |
| Type | `MS Excel 2007 XML` |
| Template name | (empty) |
| Document service | `com.sun.star.sheet.SpreadsheetDocument` |
| Default filter | Yes (PREFERRED flag) |
| Template/macro-enabled variant | `Calc MS Excel 2007 XML Template` (separate xcu) |

**Export capability:**
- UNO service declares `ExportFilter` (`oox/source/xls/excelfilter.cxx:89-92`)
- `ExcelFilter::exportDocument()` returns `false` (`excelfilter.cxx:131-134`) — STUB, not implemented
- Native export exists via `ScFormatFilter::ScExportExcel5()` → `ExportXml2007::Write()` → `ExcDocument::WriteXml()` (reachable from Save As, bypasses UNO filter)

**Template variant:** `main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML_Template.xcu` — Flags: `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH` (no EXPORT)

### 2.2 DOCX — MS Word 2007 XML

**Configuration file:** `main/filter/source/config/fragments/filters/MS_Word_2007_XML.xcu:22-34`

| Property | Value |
|----------|-------|
| Internal filter name | `MS Word 2007 XML` |
| Extensions | Not specified in xcu |
| MIME types | Not specified in xcu |
| Import/export flags | `IMPORT ALIEN 3RDPARTYFILTER` |
| Filter service | `com.sun.star.comp.Writer.WriterFilter` |
| UserData | `OXML` |
| File format version | `0` |
| Type | `writer_MS_Word_2007` |
| Template name | (empty) |
| Document service | `com.sun.star.text.TextDocument` |
| Default filter | No (no PREFERRED flag) |
| Template/macro-enabled variant | `MS Word 2007 XML Template` (separate xcu) |

**Export capability:**
- UNO service `WriterFilter` declares `ExportFilter` (`writerfilter.component:23`)
- But `WriterFilter` is import-oriented (DomainMapper) — DOCX import, not export
- `DocxExportFilter` (`sw/source/filter/ww8/docxexportfilter.cxx`) is a separate UNO ExportFilter service (`com.sun.star.comp.Writer.DocxExport`) but is **not referenced by any xcu configuration**
- `DocxExportFilter::exportDocument()` is implemented (`docxexportfilter.cxx:46-88`) but not wired to the filter registry

**Template variant:** `main/filter/source/config/fragments/filters/MS_Word_2007_XML_Template.xcu` — Flags: `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH` (no EXPORT)

### 2.3 PPTX — Impress MS PowerPoint 2007 XML

**Configuration file:** `main/filter/source/config/fragments/filters/impress_MS_PowerPoint_2007_XML.xcu:22-31`

| Property | Value |
|----------|-------|
| Internal filter name | `Impress MS PowerPoint 2007 XML` |
| Extensions | Not specified in xcu |
| MIME types | Not specified in xcu |
| Import/export flags | `IMPORT ALIEN 3RDPARTYFILTER PREFERRED` |
| Filter service | `com.sun.star.comp.oox.ppt.PowerPointImport` |
| UserData | (empty) |
| File format version | (empty) |
| Type | `MS PowerPoint 2007 XML` |
| Template name | (empty) |
| Document service | `com.sun.star.presentation.PresentationDocument` |
| Default filter | Yes (PREFERRED flag) |
| Template/macro-enabled variant | `Impress MS PowerPoint 2007 XML Template` (separate xcu) |

**Export capability:**
- UNO service `PowerPointImport` declares both `ImportFilter` and `ExportFilter` (`oox/source/ppt/pptimport.cxx:46-49`)
- `PowerPointImport::importDocument()` is implemented (`pptimport.cxx:67-78`)
- No `exportDocument()` override exists in `PowerPointImport` class — inherits from `XmlFilterBase` which returns false by default
- No PresentationML export code exists anywhere in the codebase

**Template variant:** `main/filter/source/config/fragments/filters/impress_MS_PowerPoint_2007_XML_Template.xcu` — Flags: `IMPORT ALIEN 3RDPARTYFILTER TEMPLATE TEMPLATEPATH PREFERRED` (no EXPORT)

---

## 3. Comparison with Working Export Filters

For reference, here is how export-capable filters declare their flags:

| Filter | Flags | File |
|--------|-------|------|
| MS Excel 97 | `IMPORT EXPORT ALIEN` | `main/filter/source/config/fragments/MS_Excel_97.xcu` |
| MS Word 97 | `IMPORT EXPORT ALIEN` | `main/filter/source/config/fragments/MS_Word_97.xcu` |
| HTML | `IMPORT EXPORT` | `main/filter/source/config/fragments/HTML.xcu` |
| StarOffice XML Writer | `IMPORT EXPORT` | `main/filter/source/config/fragments/StarOffice_XML__Writer_.xcu` |

**Pattern:** Working export filters include `EXPORT` in their Flags property. None of the three OOXML filters follow this pattern.

---

## 4. UNO Service Registration Summary

### 4.1 `main/oox/util/oox.component`

| Implementation | Services | ExportCapable? |
|---------------|----------|----------------|
| `com.sun.star.comp.oox.xls.ExcelFilter` | ImportFilter, ExportFilter | **No** — `exportDocument()` returns false |
| `com.sun.star.comp.oox.xls.ExcelBiffFilter` | ImportFilter, ExportFilter | **No** — `exportDocument()` returns false |
| `com.sun.star.comp.oox.ppt.PowerPointImport` | ImportFilter, ExportFilter | **No** — no `exportDocument()` override |
| `com.sun.star.comp.oox.xls.ExcelVbaProjectFilter` | ImportFilter | N/A |
| `com.sun.star.comp.oox.core.FastTokenHandler` | FastTokenHandler | N/A |
| `com.sun.star.comp.oox.FormatDetector` | ExtendedTypeDetection | N/A |
| `com.sun.star.comp.oox.docprop.DocumentPropertiesImporter` | OOXMLDocumentPropertiesImporter | N/A |
| `com.sun.star.comp.oox.ShapeContextHandler` | FastShapeContextHandler | N/A |
| `com.sun.star.comp.oox.xls.BiffDetector` | ExtendedTypeDetection | N/A |
| `com.sun.star.comp.oox.xls.FormulaParser` | FilterFormulaParser | N/A |

### 4.2 `main/writerfilter/util/writerfilter.component`

| Implementation | Services | ExportCapable? |
|---------------|----------|----------------|
| `com.sun.star.comp.Writer.WriterFilter` | ImportFilter, ExportFilter | **No** — import-oriented (DomainMapper) |
| `com.sun.star.comp.Writer.WriterFilterDetector` | ExtendedTypeDetection | N/A |
| `com.sun.star.comp.Writer.RtfFilter` | ImportFilter, ExportFilter | N/A (RTF, not OOXML) |

### 4.3 `main/sw/source/filter/ww8/docxexportfilter.cxx` (standalone registration)

| Implementation | Services | ExportCapable? |
|---------------|----------|----------------|
| `com.sun.star.comp.Writer.DocxExport` | ExportFilter | **Yes** — `exportDocument()` implemented (`docxexportfilter.cxx:46-88`) |

**Not referenced by any xcu filter configuration.**

---

## 5. Default Filter Status

| Format | Default Filter? | Evidence |
|--------|----------------|----------|
| XLSX | Yes | PREFERRED flag in `calc_MS_Excel_2007_XML.xcu:23` |
| DOCX | No | No PREFERRED flag in `MS_Word_2007_XML.xcu:23` |
| PPTX | Yes | PREFERRED flag in `impress_MS_PowerPoint_2007_XML.xcu:23` |

---

## 6. Template and Macro-Enabled Variants

| Format | Template XCU | Macro-enabled? |
|--------|-------------|----------------|
| XLSX | `calc_MS_Excel_2007_XML_Template.xcu` | Flags include TEMPLATE, TEMPLATEPATH |
| DOCX | `MS_Word_2007_XML_Template.xcu` | Flags include TEMPLATE, TEMPLATEPATH |
| PPTX | `impress_MS_PowerPoint_2007_XML_Template.xcu` | Flags include TEMPLATE, TEMPLATEPATH |

**Note:** None of the template variants declare EXPORT either. Macro-enabled variants (`.xlsm`, `.docm`, `.pptm`) are not separately configured.

---

## 7. Verification Against Generated Installation Configuration

**Classification: UNKNOWN** — The xcu fragments are processed at build time into the final installation configuration (`registrymodifications.xcu` or similar). Without a successful build and installation, we cannot verify the generated configuration. However, the source fragments clearly show no EXPORT flag is present for any OOXML filter.

---

## 8. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| No OOXML filter declares EXPORT flag | VERIFIED | Direct source reading of all 6 xcu files |
| ExcelFilter::exportDocument() returns false | VERIFIED | `excelfilter.cxx:131-134` |
| DocxExportFilter::exportDocument() is implemented | VERIFIED | `docxexportfilter.cxx:46-88` |
| DocxExportFilter not referenced by any xcu | VERIFIED | Searched all xcu files — no reference to `com.sun.star.comp.Writer.DocxExport` |
| PowerPointImport has no exportDocument override | VERIFIED | `pptimport.cxx` — only `importDocument()` defined |
| Working export filters include EXPORT flag | VERIFIED | `MS_Excel_97.xcu`, `MS_Word_97.xcu`, `HTML.xcu` |
