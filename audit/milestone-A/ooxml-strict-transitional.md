# OOXML Strict vs Transitional Assessment — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

**No Strict OOXML support exists anywhere in the Apache OpenOffice codebase.** All serializers, relationship type URIs, namespace declarations, and content types are hardcoded to Transitional OOXML (ECMA-376 1st edition, ISO/IEC 29500 Transitional). Strict OOXML (ISO/IEC 29500 Strict) uses different namespace URIs, different relationship type URIs, forbids VML, and has different content type markers. None of these differences are accommodated by any existing code.

| Aspect | Transitional (current) | Strict (required) | Support? |
|--------|----------------------|-------------------|----------|
| WordprocessingML namespace | `.../wordprocessingml/2006/main` | `http://purl.oclc.org/ooxml/wordprocessingml/main` | **None** |
| SpreadsheetML namespace | `.../spreadsheetml/2006/main` | Same as Transitional | Partial |
| PresentationML namespace | `.../presentationml/2006/main` | Same as Transitional | N/A (no exporter) |
| DrawingML namespace | `.../drawingml/2006/main` | Same as Transitional | Partial |
| Relationship type base | `.../officeDocument/2006/relationships` | `.../officeDocument/2010/relationships` | **None** |
| Package relationship base | `.../package/2006/relationships` | `.../package/2010/relationships` | **None** |
| VML support | Yes (`urn:schemas-microsoft-com:vml`) | Forbidden | **Transitional only** |
| Content types | `...document.main+xml` | `...document.main+xml` (strict marker) | **None** |
| Markup Compatibility | Not used | `AlternateContent` required | **None** |
| Extension handling | Not supported | `Extensibility` elements | **None** |

---

## 2. Hardcoded Transitional Assumptions — Evidence by File

### 2.1 DOCX — `main/sw/source/filter/ww8/docxexport.cxx`

| Assumption | Value | File:Line | Context | Strict Equivalent |
|------------|-------|-----------|---------|-------------------|
| Relationships namespace | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `docxexport.cxx:607` | `WriteMainText()` — `xmlns:r` declaration | `.../officeDocument/2010/relationships` |
| WordprocessingML namespace | `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `docxexport.cxx:609` | `WriteMainText()` — `xmlns:w` declaration | `http://purl.oclc.org/ooxml/wordprocessingml/main` |
| DrawingML wp namespace | `http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing` | `docxexport.cxx:611` | `WriteMainText()` — `xmlns:wp` declaration | Same (shared) |
| VML namespace | `urn:schemas-microsoft-com:vml` | `docxexport.cxx:608` | `WriteMainText()` — `xmlns:v` declaration | **Forbidden in Strict** |
| Office namespace | `urn:schemas-microsoft-com:office:office` | `docxexport.cxx:606` | `WriteMainText()` — `xmlns:o` declaration | Deprecated |
| Word 10 namespace | `urn:schemas-microsoft-com:office:word` | `docxexport.cxx:610` | `WriteMainText()` — `xmlns:w10` declaration | Deprecated |
| Styles relationship type | `.../officeDocument/2006/relationships/styles` | `docxexport.cxx:399` | `InitStyles()` | `.../2010/relationships/styles` |
| Footnotes relationship type | `.../officeDocument/2006/relationships/footnotes` | `docxexport.cxx:422` | `WriteFootnotesEndnotes()` | `.../2010/relationships/footnotes` |
| Endnotes relationship type | `.../officeDocument/2006/relationships/endnotes` | `docxexport.cxx:443` | `WriteFootnotesEndnotes()` | `.../2010/relationships/endnotes` |
| Numbering relationship type | `.../officeDocument/2006/relationships/numbering` | `docxexport.cxx:467` | `WriteNumbering()` | `.../2010/relationships/numbering` |
| Header relationship type | `.../officeDocument/2006/relationships/header` | `docxexport.cxx:500` | `WriteHeaderFooter()` | `.../2010/relationships/header` |
| Footer relationship type | `.../officeDocument/2006/relationships/footer` | `docxexport.cxx:515` | `WriteHeaderFooter()` | `.../2010/relationships/footer` |
| FontTable relationship type | `.../officeDocument/2006/relationships/fontTable` | `docxexport.cxx:558` | `WriteFonts()` | `.../2010/relationships/fontTable` |
| OfficeDocument relationship type | `.../officeDocument/2006/relationships/officeDocument` | `docxexport.cxx:645` | `DocxExport()` constructor | `.../2010/relationships/officeDocument` |
| Document content type | `...wordprocessingml.document.main+xml` | `docxexport.cxx:650` | `openOutputStreamWithSerializer` | Strict variant |
| Styles content type | `...wordprocessingml.styles+xml` | `docxexport.cxx:403` | `InitStyles()` | Strict variant |
| Numbering content type | `...wordprocessingml.numbering+xml` | `docxexport.cxx:467` | `WriteNumbering()` | Strict variant |
| FontTable content type | `...wordprocessingml.fontTable+xml` | `docxexport.cxx:558` | `WriteFonts()` | Strict variant |

### 2.2 XLSX — `main/sc/source/filter/excel/excdoc.cxx`

| Assumption | Value | File:Line | Context | Strict Equivalent |
|------------|-------|-----------|---------|-------------------|
| SpreadsheetML namespace | `http://schemas.openxmlformats.org/spreadsheetml/2006/main` | `excdoc.cxx:681`, `excdoc.cxx:793` | `WriteXml()` | Same URI for Strict/Transitional |
| Relationships namespace | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `excdoc.cxx:682`, `excdoc.cxx:794` | `WriteXml()` | `.../2010/relationships` |

**Note:** SpreadsheetML uses the same namespace URI for both Transitional and Strict. The Strict/Transitional distinction for XLSX is primarily in content types and the `Strict` attribute on `workbookPr`.

### 2.3 DrawingML — `main/oox/source/export/drawingml.cxx`

| Assumption | Value | File:Line | Context | Strict Equivalent |
|------------|-------|-----------|---------|-------------------|
| Image relationship type | `.../officeDocument/2006/relationships/image` | `drawingml.cxx:513` | `WriteImage()` | `.../2010/relationships/image` |

### 2.4 Relationship Type Macros — `main/oox/inc/oox/core/relations.hxx`

| Macro | Value | File:Line |
|-------|-------|-----------|
| `CREATE_OFFICEDOC_RELATION_TYPE` | `.../officeDocument/2006/relationships/` | `relations.hxx:38-39` |
| `CREATE_PACKAGE_RELATION_TYPE` | `.../package/2006/relationships/` | `relations.hxx:43-44` |
| `CREATE_MSOFFICE_RELATION_TYPE` | `http://schemas.microsoft.com/office/2006/relationships/` | `relations.hxx:48-49` |

All three macros hardcode `2006` (Transitional). Strict requires `2010`.

---

## 3. Strict OOXML Support Assessment by Format

### 3.1 DOCX — Strict Not Supported

**Evidence:** All namespaces, relationship types, and content types in `docxexport.cxx` are hardcoded to Transitional. VML is used (`xmlns:v` at line 608, `VMLExport` at line 659) which is forbidden in Strict. No `AlternateContent` or `MarkupCompatibility` handling exists.

**Barriers to Strict support:**
1. Namespace URI change: `.../2006/main` → `http://purl.oclc.org/ooxml/wordprocessingml/main`
2. Relationship type change: `.../2006/relationships/...` → `.../2010/relationships/...`
3. VML removal: All VML shape output must be converted to DrawingML
4. Content type changes for Strict variants
5. `AlternateContent` support for fallback elements

### 3.2 XLSX — Partial (Namespace Shared, Other Differences Unaddressed)

**Evidence:** SpreadsheetML namespace URI is identical for Transitional and Strict. However, relationship types still use `2006`, content types are not Strict-variant aware, and no `Strict` attribute handling on `workbookPr` exists.

**Barriers to Strict support:**
1. Relationship type change: `.../2006/relationships/...` → `.../2010/relationships/...`
2. Content type Strict variants
3. `workbookPr` Strict attribute

### 3.3 PPTX — Not Applicable (No Exporter Exists)

No PPTX export code exists, so there is nothing to make Strict-compliant. A future PPTX exporter should be designed with Strict/Transitional conformance abstraction from the start.

---

## 4. Required Abstraction for Strict Support

A centralized `OoxmlConformance` abstraction is needed (does not currently exist):

```cpp
class OoxmlConformance {
public:
    enum Level { TRANSITIONAL, STRICT };

    OUString getNamespace(NamespaceId id) const;     // w:, r:, v:, etc.
    OUString getRelationshipType(RelId id) const;    // styles, footnotes, etc.
    OUString getContentType(PartId id) const;         // document, styles, etc.
    bool allowsVml() const;                           // false for Strict
    bool isStrict() const;
};
```

**Files requiring modification (future work):**
- `main/sw/source/filter/ww8/docxexport.cxx` — all namespace/relationship/content-type literals
- `main/sc/source/filter/excel/excdoc.cxx` — relationship type literals
- `main/oox/source/export/drawingml.cxx` — relationship type literals
- `main/oox/inc/oox/core/relations.hxx` — `CREATE_OFFICEDOC_RELATION_TYPE`, `CREATE_PACKAGE_RELATION_TYPE` macros

**Recommendation:** Introduce the abstraction before implementing Strict support, to avoid scattered conditionals throughout serializers. Strict support should be a separate milestone (not part of Milestone A or B).

---

## 5. Strict Document Recognition and Parsing

| Aspect | Support Status | Evidence |
|--------|---------------|----------|
| Strict namespace recognition | UNKNOWN | Import code uses `FSHelper` which matches on token IDs, not namespace URIs directly. Need to verify if Strict namespace tokens are registered. |
| Strict relationship type parsing | UNKNOWN | `Relations::getRelationFromFirstType()` matches by type string, so Strict URIs would not match Transitional hardcoded types. |
| Strict document import | UNKNOWN | No explicit Strict handling found. |
| Strict round-trip preservation | UNKNOWN | No Strict export exists, so round-trip is impossible. |
| Strict document export | MISSING | No code path can emit Strict namespaces or relationship types. |

**Classification:** MISSING — No Strict OOXML export capability exists anywhere. Import support for Strict documents is UNKNOWN and requires runtime verification.

---

## 6. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| All serializers hardcode Transitional namespaces | VERIFIED | `docxexport.cxx:605-612`, `excdoc.cxx:681-682`, `excdoc.cxx:793-794`, `drawingml.cxx:513` |
| All relationship types use `2006` (Transitional) | VERIFIED | `relations.hxx:38-49`, `docxexport.cxx:399-645` |
| VML used in DOCX (forbidden in Strict) | VERIFIED | `docxexport.cxx:608`, `docxexport.cxx:659` |
| No Strict OOXML export path exists | VERIFIED | No conformance abstraction, no Strict namespace/relationship/content-type code |
| No centralized conformance abstraction | VERIFIED | Searched entire `oox/` and `sw/source/filter/ww8/` — no `OoxmlConformance` or equivalent |
| Strict import support unknown | UNKNOWN | Requires runtime verification of token/namespace handling |
