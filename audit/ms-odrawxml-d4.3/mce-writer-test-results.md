# MCE Phase 1 Writer (DOCX) Test Results — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Test planning — tests NOT executed (build integration blocked)

---

## 1. Test Execution Status

| Test ID | Description | Status | Reason |
|---------|-------------|--------|--------|
| W1 | DOCX with AlternateContent, eligible Choice | NOT RUN | Build blocked |
| W2 | DOCX with AlternateContent, ineligible Choice, fallback present | NOT RUN | Build blocked |
| W3 | DOCX with multiple eligible Choices | NOT RUN | Build blocked |
| W4 | DOCX with AlternateContent in paragraph | NOT RUN | Build blocked |
| W5 | DOCX with AlternateContent in drawing | NOT RUN | Build blocked |
| W6 | DOCX round-trip with MCE | NOT RUN | Build blocked |
| W7 | DOCX regression (no MCE) | NOT RUN | Build blocked |

**All Writer tests blocked** because `liboox.so` doesn't include `mcealternatecontext.o` (as documented in D.4.1).

---

## 2. Writer Test Fixtures

### W1: DOCX with AlternateContent, Eligible Choice

**Package:** `word/document.xml`

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <w:r><w:t>Before MCE</w:t></w:r>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <w:r><w:t>DrawingML choice content</w:t></w:r>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
      <w:r><w:t>After MCE</w:t></w:r>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** "Before MCE", "DrawingML choice content", "After MCE" all present. "Fallback text" NOT present.

### W2: DOCX with AlternateContent, Ineligible Choice, Fallback Present

**Package:** `word/document.xml`

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <w:body>
    <w:p>
      <w:r><w:t>Before MCE</w:t></w:r>
      <mc:AlternateContent>
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
      <w:r><w:t>After MCE</w:t></w:r>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** "Before MCE", "Fallback text", "After MCE" present. "DrawingML choice content" NOT present.

### W3: DOCX with Multiple Eligible Choices

**Package:** `word/document.xml`

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <w:r><w:t>First choice</w:t></w:r>
        </mc:Choice>
        <mc:Choice Requires="dml">
          <w:r><w:t>Second choice</w:t></w:r>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** "First choice" present. "Second choice" and "Fallback" NOT present.

### W4: DOCX with AlternateContent in Paragraph Drawing

**Package:** `word/document.xml`

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
            xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
  <w:body>
    <w:p>
      <w:r>
        <w:rPr>
          <w:rFonts w:ascii="Arial" w:hAnsi="Arial"/>
          <w:sz w:val="24"/>
        </w:rPr>
        <w:drawing>
          <mc:AlternateContent>
            <mc:Choice Requires="dml">
              <a:graphicFrame>
                <a:graphic>
                  <a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
                    <pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
                      <pic:blipFill>
                        <a:blip r:embed="rId1"/>
                      </a:blipFill>
                    </pic:pic>
                  </a:graphicData>
                </a:graphicFrame>
              </mc:Choice>
              <mc:Fallback>
                <w:r><w:t>Image fallback</w:t></w:r>
              </mc:Fallback>
            </mc:AlternateContent>
          </w:drawing>
      </w:r>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** Picture from Choice imported. "Image fallback" NOT present.

---

## 3. Writer Pipeline Integration

| Component | File | Role |
|-----------|------|------|
| Document handler | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx:104` | Intercepts `XML_AlternateContent`, creates `MceAlternateContext` |
| Context handler | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx` | Delegate for `MceAlternateContext`, processes via `OOXMLFactory` |
| Factory | `main/writerfilter/source/ooxml/OOXMLFactory.cxx` | Creates application-specific context handlers for DrawingML |

### 3.1 Writer Pipeline Integration Points

| Component | File | Role |
|-----------|------|------|
| Document handler | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx:104` | Intercepts `mc:AlternateContent`, creates `MceAlternateContext` |
| Context handler | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx` | Delegate for `MceAlternateContext`, processes via `OOXMLFactory` |
| Factory | `main/writerfilter/source/ooxml/OOXMLFactory.cxx` | Creates application-specific context handlers for DrawingML |

### 3.2 Writer-Specific Concerns

| Concern | Assessment |
|-----------|------------|
| `OOXMLFastContextHandler` copy constructor | Used to create delegate for `MceAlternateContext`. `OOXMLFastContextHandler(m_xContext)` creates new context with same model context. State is preserved. |
| `startUnknownElement` no-op in release mode | `OOXMLFastContextHandler::startUnknownElement` is `(void)` cast in release mode. Processing goes through `startFastElement()` → `OOXMLFactory::startAction()`. MCE `startUnknownElement()` forwarding to delegate is correct. |
| `createFastChildContext` delegation | `MceAlternateContext::createFastChildContext` delegates to `OOXMLFastContextHandler::createFastChildContext()` which calls `OOXMLFactory::createFastChildContextFromStart()`. Correct for known DrawingML elements. |

---

## 4. Writer Test Results (NOT RUN)

| Test | Expected | Actual | Status |
|------|----------|--------|--------|
| W1 | Choice content present, fallback absent | N/A | NOT RUN |
| W2 | Fallback present, choice content absent | N/A | NOT RUN |
| W3 | First choice present, second absent | N/A | NOT RUN |
| W4 | Picture from Choice imported | N/A | NOT RUN |
| W5 | Drawing object from Choice imported | N/A | NOT RUN |
| W6 | Round-trip preserves MCE content | N/A | NOT RUN |
| W7 | No regression for non-MCE DOCX | N/A | NOT RUN |
