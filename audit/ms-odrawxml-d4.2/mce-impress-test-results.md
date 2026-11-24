# MCE Phase 1 Impress (PPTX) Test Results — Milestone D.4.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.2  
**Mode:** Test planning — tests NOT executed (build integration blocked)

---

## 1. Test Execution Status

| Test ID | Description | Status | Reason |
|---------|-------------|--------|--------|
| P1 | PPTX with AlternateContent in slide, eligible Choice | NOT RUN | Build blocked |
| P2 | PPTX with AlternateContent, ineligible Choice, fallback present | NOT RUN | Build blocked |
| P3 | PPTX with multiple eligible Choices | NOT RUN | Build blocked |
| P4 | PPTX regression (no MCE) | NOT RUN | Build blocked |

**All Impress tests blocked** because `liboox.so` doesn't include `mcealternatecontext.o` (as documented in D.4.1).

---

## 2. Impress Test Fixtures

### P1: PPTX with AlternateContent in Slide Drawing

**Package:** `ppt/slides/slide1.xml`

```xml
<p:sld xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"
       xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
       xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <p:cSld>
    <p:spTree>
      <p:sp>
        <p:nvSpPr><p:cNvPr id="1" name="Rect 1"/></p:nvSpPr>
        <p:spPr><a:geomPr><a:prstGeom prst="rect"/></a:geomPr></p:spPr>
        <p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Original shape</a:t></a:r></a:p></p:txBody>
      </p:sp>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Fallback>
          <p:sp><p:nvSpPr><p:cNvPr id="2" name="Fallback"/></p:nvSpPr>
          <p:spPr/><p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Fallback</a:t></a:r></a:p></p:txBody>
          </p:sp>
        </mc:Fallback>
      </mc:AlternateContent>
    </p:spTree>
  </p:cSld>
</p:sld>
```

**Expected:** Picture from Choice imported on slide. Original shape (Rect 1) present. Fallback shape NOT present.

### P2: PPTX with AlternateContent, Ineligible Choice, Fallback Present

**Package:** `ppt/slides/slide1.xml`

```xml
<p:sld xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"
       xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
       xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
       xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <p:cSld>
    <p:spTree>
      <p:sp>
        <p:nvSpPr><p:cNvPr id="1" name="Rect 1"/></p:nvSpPr>
        <p:spPr><a:geomPr><a:prstGeom prst="rect"/></a:geomPr></p:spPr>
        <p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Original shape</a:t></a:r></a:p></p:txBody>
      </p:sp>
      <mc:AlternateContent>
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
        <mc:Fallback>
          <p:sp><p:nvSpPr><p:cNvPr id="2" name="Fallback"/></p:nvSpPr>
          <p:spPr/><p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Fallback</a:t></a:r></a:p></p:txBody>
          </p:sp>
        </mc:Fallback>
      </mc:AlternateContent>
    </p:spTree>
  </p:cSld>
</p:sld>
```

**Expected:** Fallback shape imported. Original shape present. 3D model NOT present.

### P3: PPTX with Multiple Eligible Choices

**Package:** `ppt/slides/slide1.xml`

```xml
<p:sld xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"
       xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
       xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <p:cSld>
    <p:spTree>
      <p:sp>
        <p:nvSpPr><p:cNvPr id="1" name="Rect 1"/></p:nvSpPr>
        <p:spPr><a:geomPr><a:prstGeom prst="rect"/></a:geomPr></p:spPr>
        <p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Original shape</a:t></a:r></a:p></p:txBody>
      </p:sp>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Choice Requires="dml">
          <a:pic><a:blipFill><a:blip r:embed="rId3"/></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Fallback>
          <p:sp><p:nvSpPr><p:cNvPr id="3" name="Fallback"/></p:nvSpPr>
          <p:spPr/><p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Fallback</a:t></a:r></a:p></p:txBody>
          </p:sp>
        </mc:Fallback>
      </mc:AlternateContent>
    </p:spTree>
  </p:cSld>
</p:sld>
```

**Expected:** First Choice picture (rId2) imported. Original shape present. Second Choice and Fallback NOT present.

---

## 3. Impress Pipeline Integration

| Component | File | Role |
|-----------|------|------|
| Fragment handler | `main/oox/source/core/fragmenthandler.cxx:109` | Intercepts `XML_AlternateContent`, creates `MceAlternateContext` |
| Slide fragment handler | `main/oox/source/ppt/slidefragmenthandler.cxx` | Subclass of `FragmentHandler`, handles slide content |
| Presentation fragment | `main/oox/source/ppt/presentationfragmenthandler.cxx` | Handles presentation-level content |

**Integration flow:**
1. Fast parser encounters `mc:AlternateContent` as known token
2. `FragmentHandler::createFastChildContext(XML_AlternateContent, ...)` called
3. Returns `MceAlternateContext` wrapping `getFastContextHandler()` (the `FragmentHandler` itself or subclass)
4. `MceAlternateContext` evaluates Choice branches
5. Selected branch events forwarded to delegate (SlideFragmentHandler subclass)
6. DrawingML elements processed by DrawingML fragment handlers

---

## 4. Impress-Specific Concerns

| Concern | Assessment |
|---------|------------|
| `SlideFragmentHandler` overrides `createFastChildContext` | `MceAlternateContext` delegates to `SlideFragmentHandler`. DrawingML elements inside Choice branch should be processed correctly. |
| Shape tree processing | `p:spTree` contains both regular shapes and `mc:AlternateContent`. The `MceAlternateContext` is created by `FragmentHandler::createFastChildContext` when the parser encounters `mc:AlternateContent` as a child of `p:spTree`. |
| Shape ordering | Shapes before `mc:AlternateContent` are processed normally. Shapes in selected Choice branch processed via delegate. Shapes after `mc:AlternateContent` processed normally (after AlternateContent ends, `mbInSelectedBranch` is false). |

---

## 5. Impress Test Results (NOT RUN)

| Test | Expected | Actual | Status |
|------|----------|--------|--------|
| P1 | Picture from Choice present, original shape present, fallback absent | N/A | NOT RUN |
| P2 | Fallback shape present, 3D model absent | N/A | NOT RUN |
| P3 | First choice picture present, second absent | N/A | NOT RUN |
| P4 | No regression for non-MCE PPTX | N/A | NOT RUN |
