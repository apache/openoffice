# MCE Phase 1 Test Plan — Milestone D.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.3  
**Mode:** Implementation and test preparation only — no build or runtime testing performed

---

## 1. Test Scope

This test plan covers the Phase 1 MCE AlternateContent selection engine implemented in Milestone D.3. Tests verify:

- `mc:AlternateContent` with `mc:Choice` and `mc:Fallback` selection
- `mc:Requires` namespace prefix resolution
- First eligible Choice wins
- Fallback selection when no Choice is eligible
- Nested `mc:AlternateContent` handling
- Malformed structure handling
- Ordinary (non-MCE) XML processing unchanged

Tests are organized by application pipeline: Calc/XLSX (oox/xls), Impress/PPTX (oox/ppt), and Writer/DOCX (writerfilter).

**Status:** Test fixtures are defined but NOT executed. No build or runtime testing was performed for this milestone.

---

## 2. Test Fixtures

All fixtures are minimal OOXML package fragments. Each fixture specifies:

- The package part (e.g., `word/document.xml`, `xl/worksheets/sheet1.xml`, `ppt/slides/slide1.xml`)
- The XML content
- The expected MCE selection result
- The expected document-level outcome

Namespace declarations used across fixtures:

```xml
xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d"
```

### F1 — One Eligible Choice with Fallback

**Package part:** `word/document.xml` (DOCX), `xl/worksheets/sheet1.xml` (XLSX), `ppt/slides/slide1.xml` (PPTX)

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback text</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_CHOICE`, index 0 (Choice with `Requires="dml"` is eligible because `dml` resolves to `http://schemas.openxmlformats.org/drawingml/2006/main` which is in the Phase 1 supported set)

**Expected document outcome:** Picture from Choice is processed; fallback text is not

**Tested in:** DOCX (writerfilter), XLSX (oox/xls), PPTX (oox/ppt)

---

### F2 — No Eligible Choice with Fallback

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback text</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (msodrawxml namespace is NOT in Phase 1 supported set)

**Expected document outcome:** Fallback text is processed; model3d content is not

**Tested in:** DOCX (writerfilter)

---

### F3 — Multiple Eligible Choices — First Wins

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <content>
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
  </content>
</root>
```

**Expected MCE result:** `BRANCH_CHOICE`, index 0 (first eligible Choice wins)

**Expected document outcome:** "First choice" text is processed; second choice and fallback are not

**Tested in:** DOCX (writerfilter), XLSX (oox/xls), PPTX (oox/ppt)

---

### F4 — Multiple Unsupported Choices

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (no Choice eligible)

**Expected document outcome:** Fallback text is processed; both model3d choices are not

**Tested in:** DOCX (writerfilter)

---

### F5 — Choice with Multiple Requires Prefixes

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
      xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="dml msodrawxml">
        <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (msodrawxml is NOT in Phase 1 supported set, so the Choice is NOT eligible even though dml is)

**Expected document outcome:** Fallback text is processed; picture is not

**Tested in:** DOCX (writerfilter), XLSX (oox/xls), PPTX (oox/ppt)

---

### F6 — Unresolved Requires Prefix

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="undeclaredPrefix">
        <w:r><w:t>Choice text</w:t></w:r>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (undeclaredPrefix cannot be resolved to a namespace URI)

**Expected document outcome:** Fallback text is processed; choice text is not

**Tested in:** DOCX (writerfilter)

---

### F7 — Nested AlternateContent

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <mc:AlternateContent>
          <mc:Choice Requires="dml">
            <w:r><w:t>Nested choice</w:t></w:r>
          </mc:Choice>
          <mc:Fallback>
            <w:r><w:t>Nested fallback</w:t></w:r>
          </mc:Fallback>
        </mc:AlternateContent>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Top fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** Outer: `BRANCH_CHOICE`, index 0. Inner: `BRANCH_CHOICE`, index 0.

**Expected document outcome:** "Nested choice" text is processed

**Tested in:** DOCX (writerfilter), XLSX (oox/xls), PPTX (oox/ppt)

---

### F8 — AlternateContent with No Fallback

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_NONE` (no eligible Choice, no Fallback)

**Expected document outcome:** No content from AlternateContent is processed

**Tested in:** DOCX (writerfilter)

---

### F9 — Missing Requires Attribute

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
  <content>
    <mc:AlternateContent>
      <mc:Choice>
        <w:r><w:t>Choice without Requires</w:t></w:r>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (Choice without Requires is NOT eligible per Phase 1 conservative policy)

**Expected document outcome:** Fallback text is processed

**Tested in:** DOCX (writerfilter)

---

### F10 — Empty Requires Attribute

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="">
        <w:r><w:t>Empty Requires</w:t></w:r>
      </mc:Choice>
      <mc:Fallback>
        <w:r><w:t>Fallback</w:t></w:r>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_FALLBACK` (empty Requires is NOT eligible)

**Expected document outcome:** Fallback text is processed

**Tested in:** DOCX (writerfilter)

---

### F11 — Non-MCE XML Unchanged (Regression)

**Package part:** `xl/worksheets/sheet1.xml` (XLSX)

**XML:**
```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
</worksheet>
```

**Expected MCE result:** N/A (no mc:AlternateContent present)

**Expected document outcome:** Normal processing; cell A1 with value 42 is imported

**Purpose:** Verify that adding MCE support does not change behavior for documents without mc:AlternateContent

**Tested in:** XLSX (oox/xls)

---

### F12 — Choice and Fallback with Visibly Different Content

**Package part:** `word/document.xml`

**XML:**
```xml
<root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
      xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
      xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <content>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <w:p><w:pPr><w:pStyle w:val="Title"/></w:pPr><w:r><w:t>Styled title</w:t></w:r></w:p>
      </mc:Choice>
      <mc:Fallback>
        <w:p><w:r><w:t>Plain fallback</w:t></w:r></w:p>
      </mc:Fallback>
    </mc:AlternateContent>
  </content>
</root>
```

**Expected MCE result:** `BRANCH_CHOICE`, index 0

**Expected document outcome:** Styled title paragraph is processed; plain fallback is not

**Purpose:** Verify that selected branch content differs visibly from fallback

**Tested in:** DOCX (writerfilter)

---

## 3. Test Execution

| Test Type | Method | Tooling | Status |
|-----------|--------|---------|--------|
| Unit test for MceProcessor | C++ unit test | Existing cppunit framework | Not executed |
| Unit test for MceAlternateContext | C++ unit test | Existing cppunit framework | Not executed |
| Import fixture test | Load fixture document, inspect imported content | AOO binary, headless | Not executed |
| Round-trip test | Import fixture → export → re-import → compare | AOO binary | Not executed |
| Regression test | Existing OOXML test corpus | AOO binary | Not executed |

**Where to add tests:**

- `MceProcessor` unit tests: `main/oox/qa/cppunittests/core/` (new directory)
- `MceAlternateContext` unit tests: `main/oox/qa/cppunittests/core/` (new directory)
- Import fixture tests: `main/oox/qa/` or `main/writerfilter/qa/` (new test documents)

**Build files to update for tests:**
- `main/oox/qa/cppunittests/core/makefile.mk` (new)
- `main/writerfilter/qa/cppunittests/` makefile.mk (extend existing)

---

## 4. Expected Behavior Summary

| Fixture | Expected Selection | Rationale |
|---------|-------------------|-----------|
| F1 | BRANCH_CHOICE (idx 0) | dml is supported |
| F2 | BRANCH_FALLBACK | msodrawxml not supported |
| F3 | BRANCH_CHOICE (idx 0) | First eligible Choice wins |
| F4 | BRANCH_FALLBACK | No Choice eligible |
| F5 | BRANCH_FALLBACK | msodrawxml in Requires makes Choice ineligible |
| F6 | BRANCH_FALLBACK | undeclaredPrefix unresolvable |
| F7 | BRANCH_CHOICE (idx 0) both levels | Nested selection independent |
| F8 | BRANCH_NONE | No eligible Choice, no Fallback |
| F9 | BRANCH_FALLBACK | Missing Requires → not eligible |
| F10 | BRANCH_FALLBACK | Empty Requires → not eligible |
| F11 | N/A | No mc:AlternateContent |
| F12 | BRANCH_CHOICE (idx 0) | dml is supported |

---

## 5. Tests Not Executed

All tests in this plan are **planned but not executed**. No build or runtime testing was performed for Milestone D.3. The following should be verified in a subsequent milestone with a working build:

1. MceProcessor unit tests (namespace resolution, eligibility)
2. MceAlternateContext unit tests (branch selection, event forwarding)
3. Import fixture tests for DOCX, XLSX, PPTX
4. Regression tests on existing OOXML test corpus
5. Round-trip preservation tests
