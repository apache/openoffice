# OOXML Schema and Implementation Coverage — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

Apache OpenOffice has a schema-driven implementation-effort tracking mechanism for **DOCX only** (in `writerfilter/`). The mechanism uses `model.xml` (a 23,545-line RELAX NG schema) combined with `WRITERFILTERSTATUS` annotations in source code, processed through a multi-stage XSLT pipeline. No equivalent mechanism exists for SpreadsheetML (XLSX) or PresentationML (PPTX) in `oox/`.

The `model.xml` schema uses **Transitional OOXML namespaces only** (e.g., `.../wordprocessingml/2006/main`, `.../officeDocument/2006/relationships`). No Strict OOXML schema is present.

---

## 2. DOCX Schema Coverage Mechanism (writerfilter)

### 2.1 Schema File

| Property | Value |
|----------|-------|
| File | `main/writerfilter/source/ooxml/model.xml` |
| Size | 23,545 lines |
| Format | RELAX NG (Compact Syntax embedded in XML) |
| Namespaces declared | `wordprocessingml`, `drawingml`, `vml`, `office`, `relationships`, `theme`, `math`, `schemaLibrary`, `sprm`, `xml` |
| Namespace URIs | All Transitional (`.../2006/main`, `.../2006/relationships`) |
| Strict namespaces | None |

### 2.2 Effort Tracking Pipeline

| Component | File | Role |
|-----------|------|------|
| Schema model | `model.xml` | RELAX NG schema with element/attribute definitions |
| Status markup | `WRITERFILTERSTATUS` comments in `.cxx` files | Inline effort annotations (`done:`, `planned:`, `spent:`) |
| Stage 1 extraction | `status.sh` (MISSING — referenced but not found) | Extracts qname/status pairs from source |
| Stage 2 grouping | `analyzestage2.xsl` | Groups qname/status elements |
| Stage 3 association | `analyzestage3.xsl` | Associates schema elements with status data |
| TODO extraction | `todo.xsl` | Identifies unimplemented elements/attributes |
| Effort calculation | `effort.xsl` | Generates effort table in CSV |
| Pipeline driver | `efforts.sh` | Chains status.sh → todo.xsl → effort.xsl |
| Documentation | `README.efforts` | Explains the effort tracking process |

### 2.3 WRITERFILTERSTATUS Annotations Found

Annotations found in `main/writerfilter/source/dmapper/*.cxx` files:

| File | Sample Annotations |
|------|-------------------|
| `BorderHandler.cxx:67-98` | `table: BorderHandler_attributedata`, `done: 75, planned: 0, spent: 0` |
| `WrapPolygonHandler.cxx` | WRITERFILTERSTATUS present |
| `FFDataHandler.cxx` | WRITERFILTERSTATUS present |
| `GraphicImport.cxx` | WRITERFILTERSTATUS present |
| `SectionColumnHandler.cxx` | WRITERFILTERSTATUS present |
| `StyleSheetTable.cxx` | WRITERFILTERSTATUS present |
| `TablePropertiesHandler.cxx` | WRITERFILTERSTATUS present |
| `SettingsTable.cxx` | WRITERFILTERSTATUS present |
| `MeasureHandler.cxx` | WRITERFILTERSTATUS present |
| `CellColorHandler.cxx` | WRITERFILTERSTATUS present |

**Annotation format:** `/* WRITERFILTERSTATUS: done: <percent>, planned: <hours>, spent: <hours> */`

**Note:** No WRITERFILTERSTATUS annotations found in `main/writerfilter/source/ooxml/*.cxx` files directly — they are in the `dmapper/` subdirectory.

### 2.4 Pipeline Status

**`status.sh` is MISSING** — The `efforts.sh` script references `./status.sh` but this file does not exist in the repository. Without `status.sh`, the effort tracking pipeline cannot execute.

```
efforts.sh: ./status.sh | xmllint --format - | xsltproc todo.xsl - | xsltproc effort.xsl -
                                      ^
                                      status.sh not found
```

---

## 3. XLSX Schema Coverage

**No schema-driven coverage mechanism exists for XLSX in `oox/`.**

| Aspect | Status | Evidence |
|--------|--------|----------|
| RELAX NG schema for SpreadsheetML | MISSING | No `model.xml` or `.rng` found in `main/oox/` |
| Implementation-effort tracking | MISSING | No `WRITERFILTERSTATUS`-equivalent annotations |
| Coverage analysis scripts | MISSING | No `efforts.sh`, `todo.xsl`, `effort.xsl` equivalents |
| Token/factory generation | EXISTS | `oox/source/token/` — generated fast token headers for XLSX elements |

The `oox/` module uses generated fast token handlers for parsing, but there is no schema-to-implementation coverage mapping.

---

## 4. PPTX Schema Coverage

**No schema-driven coverage mechanism exists for PPTX in `oox/`.**

| Aspect | Status | Evidence |
|--------|--------|----------|
| RELAX NG schema for PresentationML | MISSING | No `model.xml` or `.rng` found in `main/oox/` |
| Implementation-effort tracking | MISSING | No effort annotations |
| Coverage analysis scripts | MISSING | No analysis scripts |
| Token/factory generation | EXISTS | `oox/source/token/` — generated fast token headers for PPTX elements |

---

## 5. Namespace Analysis in model.xml

The `model.xml` schema declares the following namespace aliases (all Transitional):

| Alias | Namespace URI | Strict Equivalent | In Schema? |
|-------|--------------|-------------------|------------|
| `doc` | `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `http://purl.oclc.org/ooxml/wordprocessingml/main` | No |
| `dml` | `http://schemas.openxmlformats.org/drawingml/2006/main` | Same | Yes |
| `wordprocessingDrawing` | `http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing` | Same | Yes |
| `vml` | `urn:schemas-microsoft-com:vml` | Forbidden | Yes |
| `office` | `urn:schemas-microsoft-com:office:office` | Deprecated | Yes |
| `officeRel` | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `.../2010/relationships` | No |
| `officeRelTheme` | `.../officeDocument/2006/relationships/theme` | `.../2010/relationships/theme` | No |

---

## 6. Proposed Equivalent Inventory for XLSX/PPTX

Since no equivalent coverage mechanism exists for `oox/`, the following approach is proposed (without modifying production code):

1. **Extract element/attribute inventory from fast token headers** in `main/oox/source/token/` — these are generated from the ECMA-376 schemas and list all token IDs for each format.

2. **Cross-reference with implementation files** — grep each `.cxx` file in `oox/source/xls/` and `oox/source/ppt/` for token ID usage to determine which schema elements are handled.

3. **Document gaps manually** — create a spreadsheet or markdown table mapping schema elements to implementation status (implemented / partial / missing / unknown).

4. **Do not create a new `model.xml`** for `oox/` unless the team decides to adopt the writerfilter approach — this would be a significant new infrastructure investment.

---

## 7. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| model.xml is 23,545-line RELAX NG schema for DOCX | VERIFIED | Direct file reading |
| model.xml uses Transitional namespaces only | VERIFIED | Lines 22-35 of model.xml |
| WRITERFILTERSTATUS annotations exist in dmapper/ | VERIFIED | grep found annotations in 10+ .cxx files |
| status.sh is missing | VERIFIED | `find` returned no results for status.sh |
| efforts.sh references missing status.sh | VERIFIED | efforts.sh line 23: `./status.sh` |
| No schema coverage for XLSX/PPTX in oox/ | VERIFIED | Searched `main/oox/` — no model.xml, .rng, or effort scripts |
| No WRITERFILTERSTATUS-equivalent in oox/ | VERIFIED | grep found no WRITERFILTERSTATUS in oox/ |
