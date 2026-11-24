# OOXML Testing Inventory — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

**No OOXML export tests exist anywhere in the Apache OpenOffice codebase.** All existing OOXML-related tests are import-only or package-infrastructure tests. There are no tests that:

- Export an AOO document to DOCX/XLSX/PPTX
- Inspect the generated OPC package
- Validate OOXML package integrity
- Test filter registration for export
- Test UNO filter invocation for export
- Perform round-trip (export → reopen) validation

---

## 2. Existing Test Categories

### 2.1 DOCX Import Tests

| Test | Location | Type | Coverage | Export? |
|------|----------|------|----------|---------|
| DOCX import documents | `main/writerfilter/qa/documents/*.docx` | Test documents | Footnotes, fields, tables, bookmarks, headers/footers, redlines, pictures, numbering | No (import only) |
| DOCX complex import tests | `main/writerfilter/qa/complex/ooxml/testdocuments/*.docx` | Test documents | Table properties, preferred width, different columns, row properties, cell borders | No (import only) |
| DOCX Java load test | `main/writerfilter/qa/complex/ooxml/LoadDocuments.java` | Java test | Loads and validates DOCX documents | No (import only) |
| DOCX test document | `main/writerfilter/qa/complex/ooxml/TestDocument.java` | Java test | Test document infrastructure | No (import only) |
| C++ unit tests (doctok) | `main/writerfilter/qa/cppunittests/doctok/` | C++ unit test | Document tokenizer | No |
| C++ unit tests (xxml) | `main/writerfilter/qa/cppunittests/xxml/` | C++ unit test | XXML export/import | No (not OOXML) |
| C++ unit tests (odiapi) | `main/writerfilter/qa/cppunittests/odiapi/` | C++ unit test | ODF document API | No |
| C++ unit tests (sl) | `main/writerfilter/qa/cppunittests/sl/` | C++ unit test | Style layer | No |
| C++ unit tests (qname) | `main/writerfilter/qa/cppunittests/qname/` | C++ unit test | QName handling | No |

### 2.2 XLSX Import Tests

| Test | Location | Type | Coverage | Export? |
|------|----------|------|----------|---------|
| XLSX test fixtures | `test/testgui/data/pvt/plain_11s.xlsx` | GUI test data | Simple XLSX document | No (import only) |
| XLSX test fixtures | `test/testgui/data/pvt/complex_29s.xlsx` | GUI test data | Complex XLSX document | No (import only) |
| XLSX QA unit tests | None found | N/A | N/A | No |

### 2.3 PPTX Import Tests

| Test | Location | Type | Coverage | Export? |
|------|----------|------|----------|---------|
| PPTX test fixtures | `test/testgui/data/pvt/plain_200p.pptx` | GUI test data | Simple PPTX document | No (import only) |
| PPTX test fixtures | `test/testgui/data/pvt/complex_100p.pptx` | GUI test data | Complex PPTX document | No (import only) |
| PPTX QA unit tests | None found | N/A | N/A | No |

### 2.4 OOXML Package Infrastructure Tests

| Test | Location | Type | Coverage | Export? |
|------|----------|------|----------|---------|
| OOXML storage unit tests | `main/package/qa/ofopxmlstorages/StorageUnitTest.java` | Java unit test | ZIP package storage, OPC part relationships | No (infrastructure only) |
| OOXML storage tests | `main/package/qa/ofopxmlstorages/Test01.java`–`Test08.java` | Java unit tests | Package read/write, relationship handling | No (infrastructure only) |
| OOXML storage helper | `main/package/qa/ofopxmlstorages/TestHelper.java` | Java helper | Test utilities for package tests | No |

### 2.5 ODF Validation Framework (with DOCX fixtures)

| Test | Location | Type | Coverage | Export? |
|------|----------|------|----------|---------|
| ODF validate framework | `test/odf-validate/` | Python scripts | Document validation, round-trip, package checking | No (validation only) |
| DOCX fixtures | `test/odf-validate/fixtures/table-colspan/MergedTable_3_3.docx` | Test document | Table colspan | No (import/validation only) |
| DOCX fixtures | `test/odf-validate/fixtures/anchor-page/test-image.docx` | Test document | Anchor page, images | No (import/validation only) |
| DOCX fixtures | `test/odf-validate/fixtures/redline-stack/test-review-stack.docx` | Test document | Redline stack | No (import/validation only) |

---

## 3. Test Frameworks and Execution

| Framework | Location | Language | How to Execute |
|-----------|----------|----------|----------------|
| C++ unit tests | `main/*/qa/cppunittests/` | C++ | Build with `dmake` or `make`, run test binaries |
| Java unit tests | `main/package/qa/ofopxmlstorages/` | Java | Compile with `javac`, run with `java` |
| Python validation | `test/odf-validate/` | Python | Run `validate.py`, `drive.sh`, `smoke13.py` etc. |
| GUI test data | `test/testgui/data/pvt/` | N/A | Load documents in AOO GUI for manual testing |

**No export test framework exists.** A new framework would need to be created to:
1. Export documents via Save As or UNO API
2. Unzip and inspect the OPC package
3. Validate XML parts against schemas
4. Perform round-trip validation (reopen in AOO)

---

## 4. Test Coverage Gaps

| Gap Area | Status | Description |
|----------|--------|-------------|
| DOCX export tests | MISSING | No tests exercise `DocxExportFilter::exportDocument()` |
| XLSX export tests | MISSING | No tests exercise `ScExportExcel5()` → XML path |
| PPTX export tests | MISSING | No PPTX export exists to test |
| OOXML package integrity tests | MISSING | No tests validate `[Content_Types].xml`, relationships, part structure |
| Filter registration export tests | MISSING | No tests verify EXPORT flag presence in filter registry |
| UNO filter export tests | MISSING | No tests invoke `XFilter::exportDocument()` for OOXML |
| Round-trip tests | MISSING | No export → reopen validation tests |
| Strict OOXML tests | MISSING | No Strict OOXML support exists to test |
| DrawingML export tests | MISSING | No tests for DrawingML shape serialization |
| Malformed package handling | MISSING | No tests for corrupt ZIP, bad relationships, missing parts |

---

## 5. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| No OOXML export tests exist | VERIFIED | Searched `main/*/qa/`, `test/`, `main/writerfilter/qa/` — no export tests found |
| Import-only test documents exist | VERIFIED | `.docx`, `.xlsx`, `.pptx` files in `qa/documents/`, `qa/complex/`, `test/testgui/data/pvt/` |
| Package infrastructure tests exist | VERIFIED | `main/package/qa/ofopxmlstorages/` — 10 Java test files |
| ODF validation framework has DOCX fixtures | VERIFIED | 3 `.docx` files in `test/odf-validate/fixtures/` |
| No export test framework exists | VERIFIED | No code found that exports documents and inspects packages |
