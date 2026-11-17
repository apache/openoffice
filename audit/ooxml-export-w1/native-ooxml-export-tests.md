# Native OOXML Export Tests — Milestone W.1

**Source:** Source audit only. No tests were executed.  
**Confidence:** confirmed | probable | requires runtime verification

---

## 1. Existing Tests Relevant to OOXML Export

### 1.1 DOCX — Import test documents (import only, not export)

| Test | Location | Coverage | Limitations | Executed? |
|------|----------|----------|-------------|-----------|
| DOCX import test documents | `main/writerfilter/qa/documents/*.docx` | DOCX import (footnotes, fields, tables, bookmarks, headers/footers, redlines, pictures, numbering) | Import only; no export testing | No |
| DOCX complex import tests | `main/writerfilter/qa/complex/ooxml/testdocuments/*.docx` | Complex table properties, preferred width, different columns, row properties, cell borders | Import only | No |
| DOCX test subdirectories | `main/writerfilter/qa/documents/docx/` (numbering, page-format, redlines, pictures) | Import of numbered lists, page format, redlines, pictures | Import only | No |

**Assessment:** All writerfilter QA documents are import fixtures. None test DOCX export.

### 1.2 XLSX — Import test fixtures (import only, not export)

| Test | Location | Coverage | Limitations | Executed? |
|------|----------|----------|-------------|-----------|
| XLSX test fixtures | `test/testgui/data/pvt/complex_29s.xlsx`, `plain_11s.xlsx` | GUI import tests | Import only | No |

**Assessment:** No XLSX export tests found.

### 1.3 PPTX — Import test fixtures (import only, not export)

| Test | Location | Coverage | Limitations | Executed? |
|------|----------|----------|-------------|-----------|
| PPTX test fixtures | `test/testgui/data/pvt/complex_100p.pptx`, `plain_200p.pptx` | GUI import tests | Import only | No |

**Assessment:** No PPTX export tests found.

### 1.4 ODF validation fixtures using DOCX (import/validation only)

| Test | Location | Coverage | Limitations | Executed? |
|------|----------|----------|-------------|-----------|
| ODF validate DOCX fixtures | `test/odf-validate/fixtures/*.docx` (table-colspan, anchor-page, redline-stack) | ODF validation of DOCX-derived documents | Import/validation only, not export | No |

### 1.5 Unit test directories (no export tests found)

| Directory | Contents | Export tests? |
|-----------|----------|---------------|
| `main/qa/` | Various unit tests | No docx/xlsx/pptx export tests found |
| `main/writerfilter/qa/` | DOCX import test documents | Import only |
| `main/sc/qa/` | Calc unit tests | No XLSX export tests found |
| `main/sd/qa/` | Impress unit tests | No PPTX export tests found |

**Conclusion:** Zero export tests exist for DOCX, XLSX, or PPTX.

---

## 2. Recommended Feature-Oriented Regression Matrix

### 2.1 Test Category A: AOO document → OOXML export → package inspection

| Test ID | Description | Method | Expected Result | Status |
|---------|-------------|--------|-----------------|--------|
| A1 | Export simple Calc document to XLSX, inspect package | Save As → XLSX, unzip, inspect `xl/workbook.xml`, `xl/worksheets/sheet1.xml`, `xl/styles.xml` | Package opens; workbook, sheet, styles present | Not executed |
| A2 | Export Calc document with formulas to XLSX | Save As → XLSX, inspect `<c>` elements with `<f>` | Formulas preserved | Not executed |
| A3 | Export Calc document with shared strings to XLSX | Save As → XLSX, inspect `xl/sharedStrings.xml` | Strings in SST, cells reference via `t="s"` | Not executed |
| A4 | Export Calc document with merged cells to XLSX | Save As → XLSX, inspect `<mergeCells>` | Merged ranges present | Not executed |
| A5 | Export Calc document with hyperlinks to XLSX | Save As → XLSX, inspect `<hyperlinks>` | Hyperlink relationships present | Not executed |
| A6 | Export Calc document with data validation to XLSX | Save As → XLSX, inspect `<dataValidation>` | Validation rules present | Not executed |
| A7 | Export Calc document with pivot tables to XLSX | Save As → XLSX, inspect `xl/pivotTables/` | Pivot XML present | Not executed |
| A8 | Export Calc document with conditional formatting to XLSX | Save As → XLSX, inspect `<conditionalFormatting>` | CF rules partially present | Not executed |
| A9 | Export Calc document with charts to XLSX | Save As → XLSX, inspect `xl/charts/` or `xl/drawings/` | **Expected: absent** (DFF path disabled) | Not executed |
| A10 | Export Calc document with images to XLSX | Save As → XLSX, inspect `xl/media/` | **Expected: absent** (DFF path disabled) | Not executed |
| A11 | Export Writer document to DOCX, inspect package | Save As → DOCX, unzip, inspect `word/document.xml`, `word/styles.xml` | Package opens; document, styles present | Not executed |
| A12 | Export Writer document with tables to DOCX | Save As → DOCX, inspect `w:tbl` | Tables present (via WriteText path, not OutputTableNode stub) | Not executed |
| A13 | Export Writer document with fields to DOCX | Save As → DOCX, inspect `w:instrText` / `w:fldSimple` | Fields present | Not executed |
| A14 | Export Writer document with bookmarks to DOCX | Save As → DOCX, inspect `w:bookmarkStart`/`w:bookmarkEnd` | Bookmarks present | Not executed |
| A15 | Export Writer document with headers/footers to DOCX | Save As → DOCX, inspect `word/headerN.xml`, `word/footerN.xml` | Headers/footers present | Not executed |
| A16 | Export Writer document with footnotes to DOCX | Save As → DOCX, inspect `word/footnotes.xml` | Footnotes present | Not executed |
| A17 | Export Writer document with OLE objects to DOCX | Save As → DOCX | **Expected: OLE absent** (OutputOLENode stub) | Not executed |
| A18 | Export Writer document with ruby text to DOCX | Save As → DOCX | **Expected: ruby absent** (StartRuby stub) | Not executed |

### 2.2 Test Category B: OOXML export → reopen in AOO

| Test ID | Description | Method | Expected Result | Status |
|---------|-------------|--------|-----------------|--------|
| B1 | Round-trip: AOO → XLSX → AOO | Export to XLSX, reopen in Calc | Data preserved (modulo gaps A9-A10) | Not executed |
| B2 | Round-trip: AOO → DOCX → AOO | Export to DOCX, reopen in Writer | Data preserved (modulo gaps A17-A18) | Not executed |

### 2.3 Test Category C: Export → reopen in Microsoft Office-compatible software

| Test ID | Description | Method | Expected Result | Status |
|---------|-------------|--------|-----------------|--------|
| C1 | XLSX → Excel | Export Calc to XLSX, open in Excel | File opens without repair warning | Not executed |
| C2 | DOCX → Word | Export Writer to DOCX, open in Word | File opens without repair warning | Not executed |
| C3 | XLSX with charts → Excel | Export Calc with charts to XLSX, open in Excel | **Expected: charts missing** | Not executed |

### 2.4 Test Category D: Preservation of formulas, styles, objects, drawings, relationships

| Test ID | Feature | Method | Expected Result | Status |
|---------|---------|--------|-----------------|--------|
| D1 | Formulas | Export Calc with SUM, IF, VLOOKUP → XLSX | Formulas preserved | Not executed |
| D2 | Styles | Export Writer with character/paragraph styles → DOCX | Styles in `word/styles.xml` | Not executed |
| D3 | Objects | Export Writer with images → DOCX | **Expected: partial** (FlyFrameGraphic TODO) | Not executed |
| D4 | Drawings | Export Calc with drawing objects → XLSX | **Expected: absent** (DFF disabled) | Not executed |
| D5 | Relationships | Inspect `_rels/` parts after XLSX/DOCX export | Relationships present and valid | Not executed |

### 2.5 Test Category E: Negative tests for unsupported features and silent data loss

| Test ID | Feature | Method | Expected Result | Status |
|---------|---------|--------|-----------------|--------|
| E1 | XLSX charts | Calc with chart → XLSX → inspect package | **No chart parts**; no silent corruption | Not executed |
| E2 | XLSX images | Calc with image → XLSX → inspect package | **No media parts**; no silent corruption | Not executed |
| E3 | DOCX OLE | Writer with OLE → DOCX → inspect package | **No OLE parts**; no silent corruption | Not executed |
| E4 | DOCX ruby | Writer with ruby → DOCX → inspect package | **No w:ruby elements**; no silent corruption | Not executed |

### 2.6 Test Category F: Independent OOXML package validation

| Test ID | Description | Method | Expected Result | Status |
|---------|-------------|--------|-----------------|--------|
| F1 | Validate XLSX package structure | Export Calc → XLSX → validate with Open XML SDK validator or xmllint against ECMA-376 schemas | Package is well-formed; required parts present | Not executed |
| F2 | Validate DOCX package structure | Export Writer → DOCX → validate with independent validator | Package is well-formed; required parts present | Not executed |
| F3 | Validate content types | Inspect `[Content_Types].xml` after export | Correct content types for all parts | Not executed |
| F4 | Validate relationships | Inspect `.rels` files after export | All relationship targets resolve | Not executed |

---

## 3. Test Execution Method

| Test Type | Execution Method | Tooling |
|-----------|-----------------|---------|
| Save As export | GUI or headless `soffice --convert-to` | AOO binary |
| UNO filter invocation | UNO API script calling `XFilter::filter()` | AOO SDK + scripting |
| Package inspection | Unzip + xmllint / XML parser | Standard tools |
| OOXML validation | Open XML SDK validator, xmllint, or schema validation | External tools |
| Round-trip | Export → reopen in AOO → compare | AOO binary |

---

## 4. Limitations

- No existing tests were executed for this audit (all marked "Not executed").
- No export test infrastructure exists in the repository.
- Microsoft Office compatibility testing (Category C) requires Microsoft Office installation, which may not be available.
- Independent OOXML validation (Category F) requires external validator tools.
