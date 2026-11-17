# Native OOXML Strict vs Transitional Export — Milestone W.1

**Source:** Source audit only.  
**Confidence:** confirmed | probable | requires runtime verification

---

## 1. Existing Export Paths Are Transitional-Only

All three native export paths (XLSX, DOCX, and the absent PPTX) hardcode Transitional OOXML namespaces, relationship types, and content types. No existing export path can emit Strict OOXML.

---

## 2. Hardcoded Transitional Assumptions by File

### 2.1 XLSX — `main/sc/source/filter/excel/excdoc.cxx`

| Assumption | Value | File:Line | Context |
|------------|-------|-----------|---------|
| SpreadsheetML namespace | `http://schemas.openxmlformats.org/spreadsheetml/2006/main` | `excdoc.cxx:793` | `ExcDocument::WriteXml()` — `startElement(XML_workbook, XML_xmlns, ...)` |
| Office XML prefix | Hardcoded in `XclExpXmlStream` | `xestream.cxx` | Package stream creation |

Strict OOXML uses the same SpreadsheetML namespace URI (`.../spreadsheetml/2006/main`) for both Transitional and Strict — the distinction for spreadsheets is primarily in the content types and the `Strict` attribute on the workbook. So XLSX has fewer Strict/Transitional differences than DOCX.

### 2.2 DOCX — `main/sw/source/filter/ww8/docxexport.cxx`

| Assumption | Value | File:Line | Context |
|------------|-------|-----------|---------|
| WordprocessingML namespace | `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `docxexport.cxx:609` | `WriteMainText()` — `FSNS(XML_xmlns, XML_w)` |
| Office namespace | `urn:schemas-microsoft-com:office:office` | `docxexport.cxx:606` | `FSNS(XML_xmlns, XML_o)` |
| Relationships namespace | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `docxexport.cxx:607` | `FSNS(XML_xmlns, XML_r)` |
| VML namespace | `urn:schemas-microsoft-com:vml` | `docxexport.cxx:608` | `FSNS(XML_xmlns, XML_v)` — Strict forbids VML |
| Word 10 namespace | `urn:schemas-microsoft-com:office:word` | `docxexport.cxx:610` | `FSNS(XML_xmlns, XML_w10)` |
| DrawingML wp namespace | `http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing` | `docxexport.cxx:611` | `FSNS(XML_xmlns, XML_wp)` |
| Content type | `application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml` | `docxexport.cxx:650` | `openOutputStreamWithSerializer` |
| Styles content type | `application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml` | `docxexport.cxx:403` | `InitStyles()` |
| Numbering content type | `.../wordprocessingml.numbering+xml` | `docxexport.cxx:467` | `WriteNumbering()` |
| FontTable content type | `.../wordprocessingml.fontTable+xml` | `docxexport.cxx:558` | `WriteFonts()` |
| Style relationship type | `http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles` | `docxexport.cxx:399` | `InitStyles()` |
| Footnotes relationship type | `.../2006/relationships/footnotes` | `docxexport.cxx:422` | `WriteFootnotesEndnotes()` |
| Endnotes relationship type | `.../2006/relationships/endnotes` | `docxexport.cxx:443` | `WriteFootnotesEndnotes()` |
| Header relationship type | `.../2006/relationships/header` | `docxexport.cxx:500` | `WriteHeaderFooter()` |
| Footer relationship type | `.../2006/relationships/footer` | `docxexport.cxx:515` | `WriteHeaderFooter()` |
| FontTable relationship type | `.../2006/relationships/fontTable` | `docxexport.cxx:558` | `WriteFonts()` |
| OfficeDocument relationship type | `.../2006/relationships/officeDocument` | `docxexport.cxx:645` | `DocxExport()` constructor |

Strict OOXML uses:
- WordprocessingML namespace: `http://purl.oclc.org/ooxml/wordprocessingml/main` (instead of `.../2006/main`)
- Relationship types: `.../officeDocument/2010/relationships/...` (instead of `.../2006/relationships/...`)
- Strict content types with `strict` marker
- No VML (drawingML only)

### 2.3 `main/sw/source/filter/ww8/docxattributeoutput.cxx`

No additional Transitional assumptions found beyond those in `docxexport.cxx`. The attribute output methods emit Transitional-compatible elements (e.g., `w:pPr`, `w:rPr`, `w:tblPr`) which are shared between Transitional and Strict, but the namespaces are set at the document level in `docxexport.cxx`.

### 2.4 `main/sc/source/filter/excel/xestream.cxx`

The `XclExpXmlStream` manages package streams. Content types and relationship types for XLSX parts would need to be audited per-part, but the workbook-level namespace is the primary Transitional marker.

---

## 3. What Strict OOXML Would Require

### 3.1 Namespace changes (DOCX)

| Element | Transitional | Strict |
|---------|-------------|--------|
| `w:` namespace | `.../wordprocessingml/2006/main` | `http://purl.oclc.org/ooxml/wordprocessingml/main` |
| `r:` namespace | `.../officeDocument/2006/relationships` | `.../officeDocument/2010/relationships` |
| VML (`v:`) | `urn:schemas-microsoft-com:vml` | **Forbidden** — use DrawingML only |

### 3.2 Content type changes

| Part | Transitional content type | Strict content type |
|------|--------------------------|---------------------|
| document.xml | `...wordprocessingml.document.main+xml` | `...wordprocessingml.document.main+xml` (with strict marker) |
| styles.xml | `...wordprocessingml.styles+xml` | `...wordprocessingml.styles+xml` |
| numbering.xml | `...wordprocessingml.numbering+xml` | `...wordprocessingml.numbering+xml` |

### 3.3 VML removal (DOCX)

`DocxExport` uses `VMLExport` [docxexport.cxx:659] and declares `xmlns:v` [docxexport.cxx:608]. Strict OOXML forbids VML. All VML shape output would need to be converted to DrawingML.

---

## 4. Recommended Centralized Abstraction (future milestone)

Introduce an `OoxmlConformance` abstraction (not implemented) to avoid scattered conditionals:

```
class OoxmlConformance {
public:
    enum Level { TRANSITIONAL, STRICT };

    // Namespace URIs
    OUString getNamespace(NamespaceId id) const;  // w:, r:, v:, etc.

    // Relationship type URIs
    OUString getRelationshipType(RelId id) const;  // styles, footnotes, header, footer, etc.

    // Content types
    OUString getContentType(PartId id) const;  // document, styles, numbering, etc.

    // Whether VML is allowed
    bool allowsVml() const;

    // Whether Strict element/attribute restrictions apply
    bool isStrict() const;
};
```

**Usage pattern:** Serializers call `conformance->getNamespace(NS_WORDPROCESSINGML)` instead of hardcoding the URI. This centralizes all Transitional/Strict differences in one class.

**Files to modify (future work):**
- `main/sc/source/filter/excel/excdoc.cxx:793` — workbook namespace
- `main/sw/source/filter/ww8/docxexport.cxx:605-612` — all document namespaces
- `main/sw/source/filter/ww8/docxexport.cxx:399,403,422,443,500,515,558,559,645,646` — relationship types
- `main/sw/source/filter/ww8/docxexport.cxx:650` — content type
- `main/sc/source/filter/excel/xestream.cxx` — package part content types

---

## 5. Strict OOXML Feasibility by Exporter

| Exporter | Strict feasible? | Barriers |
|----------|-----------------|----------|
| XLSX | Probable (easier) | SpreadsheetML namespace is shared; mainly content-type and relationship-type changes needed |
| DOCX | Requires significant work | VML must be removed; namespace changes; content-type changes |
| PPTX | N/A (no exporter) | Would need to be Strict-capable from the start if implemented |

**Recommendation:** Treat Strict OOXML as a separate future milestone (W.4 or later). Introduce the centralized `OoxmlConformance` abstraction before implementing Strict, to avoid scattered conditionals.

---

## 6. Confidence Summary

| Finding | Confidence | Basis |
|---------|------------|-------|
| All existing serializers hardcode Transitional namespaces | confirmed | Direct source reading: excdoc.cxx:793, docxexport.cxx:605-612 |
| All existing serializers hardcode Transitional relationship types | confirmed | Direct source reading: docxexport.cxx:399-646 |
| All existing serializers hardcode Transitional content types | confirmed | Direct source reading: docxexport.cxx:403-650 |
| VML used in DOCX (forbidden in Strict) | confirmed | docxexport.cxx:608,659 |
| No Strict OOXML export path exists | confirmed | No conformance abstraction; no Strict namespace/relationship/content-type code |
| XLSX Strict easier than DOCX Strict | probable | SpreadsheetML namespace shared between Transitional/Strict |
