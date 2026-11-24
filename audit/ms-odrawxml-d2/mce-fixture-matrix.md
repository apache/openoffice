# MCE Fixture Matrix — Milestone D.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Source audit only — no production code changes  
**Status:** Fixtures proposed but NOT executed (no runtime testing performed)

---

## 1. Fixture Construction Guidelines

All fixtures are minimal OOXML package fragments designed to test MCE behavior. Each fixture consists of:

- A package part (e.g., `word/document.xml`, `xl/worksheets/sheet1.xml`, `ppt/slides/slide1.xml`)
- Minimal XML content exercising one MCE construct
- Expected selection result based on ISO/IEC 29500-3 semantics
- Expected AOO behavior based on source code analysis (NOT runtime-tested)

**Namespace declarations used across fixtures:**
```xml
xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d"
```

---

## 2. Fixture Matrix

### F1 — Choice Supported, Fallback Present

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `dml` namespace is supported → use Choice content (picture).

**Expected AOO (source-based):** `mc:AlternateContent` treated as unknown element → entire wrapper skipped → neither picture nor fallback text appears.

**Documented as untested:** No runtime test executed.

---

### F2 — Choice Unsupported, Fallback Present

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `msodrawxml` not supported → skip Choice → use Fallback text.

**Expected AOO (source-based):** Entire wrapper skipped → neither model3d nor fallback text appears.

**Documented as untested:** No runtime test executed.

---

### F3 — Multiple Choice Alternatives

**Fixture:** `ppt/slides/slide1.xml` fragment

```xml
<p:sld xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"
       xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
       xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
       xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <p:cSld>
    <p:spTree>
      <mc:AlternateContent>
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
        <mc:Choice Requires="dml">
          <a:pic>...</a:pic>
        </mc:Choice>
        <mc:Fallback>
          <p:sp><p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Fallback</a:t></a:r></a:p></a:p></p:sp>
        </mc:Fallback>
      </mc:AlternateContent>
    </p:spTree>
  </p:cSld>
</p:sld>
```

**Expected (ISO 29500-3):** `msodrawxml` not supported → skip first Choice; `dml` supported → use second Choice (picture).

**Expected AOO (source-based):** Entire wrapper skipped → no picture, no shape, no fallback text.

**Documented as untested:** No runtime test executed.

---

### F4 — Nested AlternateContent

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:ns1="http://example.com/ns1"
            xmlns:ns2="http://example.com/ns2">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="ns1">
          <mc:AlternateContent>
            <mc:Choice Requires="ns2">
              <w:r><w:t>Deep content</w:t></w:r>
            </mc:Choice>
            <mc:Fallback>
              <w:r><w:t>Mid fallback</w:t></w:r>
            </mc:Fallback>
          </mc:AlternateContent>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Top fallback</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `ns1` not supported → skip outer Choice → use top fallback ("Top fallback").

**Expected AOO (source-based):** Entire outer wrapper skipped → no text at all.

**Documented as untested:** No runtime test executed.

---

### F5 — AlternateContent Inside a Drawing

**Fixture:** `xl/worksheets/sheet1.xml` fragment

```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
           xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
  <drawing>
    <mc:AlternateContent>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
      <mc:Fallback>
        <a:graphicFrame><a:pic>...</a:pic></a:graphicFrame>
      </mc:Fallback>
    </mc:AlternateContent>
  </drawing>
</worksheet>
```

**Expected (ISO 29500-3):** `msodrawxml` not supported → use fallback picture.

**Expected AOO (source-based):** `mc:AlternateContent` skipped → no picture rendered in worksheet.

**Documented as untested:** No runtime test executed.

---

### F6 — AlternateContent Inside a Worksheet

Same as F5 — worksheet context tested above.

**Documented as untested:** No runtime test executed.

---

### F7 — AlternateContent Inside a Word Document

Same as F1 — wordprocessing context tested above.

**Documented as untested:** No runtime test executed.

---

### F8 — AlternateContent Inside a PowerPoint Slide

Same as F3 — presentation context tested above.

**Documented as untested:** No runtime test executed.

---

### F9 — Ignorable Extension Namespaces

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d"
            mc:Ignorable="msodrawxml">
  <w:body>
    <w:p>
      <w:r><w:t>Normal text</w:t></w:r>
      <msodrawxml:model3d>...</msodrawxml:model3d>
      <w:r><w:t>More text</w:t></w:r>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `msodrawxml` is ignorable → skip model3d, process normal text.

**Expected AOO (source-based):** `mc:Ignorable` attribute not read → model3d skipped as unknown element (correct by accident), normal text processed normally.

**Documented as untested:** No runtime test executed.

---

### F10 — ProcessContent

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <mc:AlternateContent mc:ProcessContent="fallback">
        <mc:Choice Requires="unsupportedNs">
          <w:r><w:t>Extension text</w:t></w:r>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `ProcessContent="fallback"` → process fallback content normally even in ignorable context → "Fallback text" appears.

**Expected AOO (source-based):** Entire wrapper skipped → no text appears.

**Documented as untested:** No runtime test executed.

---

### F11 — MustUnderstand

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <w:body>
    <w:p>
      <mc:AlternateContent mc:MustUnderstand="true">
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `msodrawxml` not supported and `MustUnderstand="true"` → fail to load document (or at least warn user).

**Expected AOO (source-based):** `mc:MustUnderstand` not read → no error → document loads silently without model3d content.

**Documented as untested:** No runtime test executed.

---

### F12 — Preservation Directives

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
            mc:PreserveElements="a:effectLst"
            mc:PreserveAttributes="a:gamma">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="unsupportedNs">
          <a:pic><a:blipFill><a:effectLst><a:gamma val="50000"/></a:effectLst></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `a:effectLst` and `a:gamma` should be preserved even though in ignorable context.

**Expected AOO (source-based):** Preservation directives not read → no preservation of any kind.

**Documented as untested:** No runtime test executed.

---

### F13 — AlternateContent with No Fallback

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `msodrawxml` not supported → no Choice matches → no Fallback → no content displayed.

**Expected AOO (source-based):** Entire wrapper skipped → no content (correct by accident).

**Documented as untested:** No runtime test executed.

---

### F14 — Invalid Requires Declarations

**Fixture:** `word/document.xml` fragment

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="undeclaredPrefix">
          <w:r><w:t>Extension text</w:t></w:r>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected (ISO 29500-3):** `undeclaredPrefix` not declared as namespace → Choice does not match → use Fallback.

**Expected AOO (source-based):** `Requires` attribute not read → entire wrapper skipped → neither text appears.

**Documented as untested:** No runtime test executed.

---

## 3. Test Requirements

| Fixture | Test Method | Assertion | Status |
|---------|-------------|-----------|--------|
| F1 | Import DOCX with AlternateContent (Choice=dml supported) | Picture from Choice appears | Not executed |
| F2 | Import DOCX with AlternateContent (Choice=unsupported) | Fallback text appears | Not executed |
| F3 | Import PPTX with multiple Choice alternatives | First supported Choice appears | Not executed |
| F4 | Import DOCX with nested AlternateContent | Top fallback appears | Not executed |
| F5 | Import XLSX with AlternateContent in drawing | Fallback picture appears | Not executed |
| F9 | Import DOCX with Ignorable namespaces | Normal text preserved, extension skipped | Not executed |
| F10 | Import DOCX with ProcessContent=fallback | Fallback text appears | Not executed |
| F11 | Import DOCX with MustUnderstand=true (unsupported) | Error or warning to user | Not executed |
| F13 | Import DOCX with AlternateContent no fallback | No content, no error | Not executed |
| F14 | Import DOCX with invalid Requires | Fallback text appears | Not executed |

**Note:** No runtime tests were executed for this milestone. All "Expected AOO" behaviors are source-code predictions, not verified results.

---

## 5. Fixture Construction Notes

- All fixtures use minimal XML fragments that can be embedded in a valid OOXML package
- Fixtures F1–F5, F9–F14 are DOCX; F3 is PPTX; F5 is XLSX
- Each fixture isolates one MCE construct for unambiguous testing
- Fixtures are designed to be reproducible by creating a minimal OOXML package with the specified part content
- No test documents were committed to the repository
