# MCE Phase 1 Application Test Results — Milestone D.4

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4  
**Mode:** Application validation planning (not executed — build blocked)

---

## 1. Application Validation Status

| Application | Format | Pipeline | Planned | Executed | Status |
|-------------|--------|----------|---------|----------|--------|
| Writer | DOCX | writerfilter → OOXMLFastDocumentHandler | Yes | No | Blocked |
| Calc | XLSX | oox/xls → FragmentHandler | Yes | No | Blocked |
| Impress | PPTX | oox/ppt → FragmentHandler | Yes | No | Blocked |

All application-level validation blocked due to missing Boost development headers preventing full build (see `mce-build-results.md`).

---

## 2. Writer / DOCX Validation Plan

### 2.1 Test Document Structure

**Package:** `word/document.xml` with `mc:AlternateContent` in a paragraph

```xml
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
            xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <w:r><w:t>Before alternate content</w:t></w:r>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <w:r><w:t>DrawingML choice content</w:t></w:r>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback text</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
      <w:r><w:t>After alternate content</w:t></w:r>
    </w:p>
  </w:body>
</w:document>
```

### 2.2 Expected Results

| Test | Expected Behavior | Verification Method |
|------|-------------------|---------------------|
| AV-1: Choice eligible | "DrawingML choice content" appears in document | Open document, check text content |
| AV-2: Choice ineligible | "Fallback text" appears in document | Change Requires to unsupported namespace |
| AV-3: Text before/after preserved | "Before alternate content" and "After alternate content" present | Open document, check full text |
| AV-4: No event leakage | No duplicate or missing text | Compare document model before/after |

### 2.3 Writer Pipeline Integration Points

| Component | File | Role |
|-----------|------|------|
| Document handler | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Intercepts `mc:AlternateContent`, creates `MceAlternateContext` |
| Context handler | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx` | Delegate for `MceAlternateContext`, processes via `OOXMLFactory` |
| Factory | `main/writerfilter/source/ooxml/OOXMLFactory.cxx` | Creates application-specific context handlers for DrawingML |

### 2.4 Writer-Specific Concerns

| Concern | Assessment |
|---------|------------|
| `OOXMLFastContextHandler` copy constructor | Used to create delegate for `MceAlternateContext`. Existing code pattern (line 123-124 before D.3 change). Should work correctly. |
| `startUnknownElement` no-op in release mode | `OOXMLFastContextHandler::startUnknownElement` is `(void)` cast in release mode. Processing goes through `startFastElement` → `OOXMLFactory::startAction`. MCE `startUnknownElement` forwarding to delegate is correct. |
| `createFastChildContext` delegation | `MceAlternateContext::createFastChildContext` delegates to `OOXMLFastContextHandler::createFastChildContext` which calls `OOXMLFactory::createFastChildContextFromStart`. Correct for known DrawingML elements. |

---

## 3. Calc / XLSX Validation Plan

### 3.1 Test Document Structure

**Package:** `xl/worksheets/sheet1.xml` with `mc:AlternateContent` in drawing content

```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
  <drawing>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <a:graphicFrame>
          <a:graphic><a:graphicData uri="http://...">...</a:graphicData></a:graphic>
        </a:graphicFrame>
      </mc:Choice>
      <mc:Fallback>
        <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
      </mc:Fallback>
    </mc:AlternateContent>
  </drawing>
</worksheet>
```

### 3.2 Expected Results

| Test | Expected Behavior | Verification Method |
|------|-------------------|---------------------|
| AV-5: Choice eligible | DrawingML graphic frame imported | Open XLSX, check drawing objects |
| AV-6: Choice ineligible | Fallback picture imported | Change Requires to unsupported namespace |
| AV-7: Cell data preserved | Cell A1 with value 42 present | Open XLSX, check cell values |
| AV-8: Drawing relationships preserved | `r:embed="rId1"` resolved correctly | Check `_rels` parts and media |

### 3.3 Calc Pipeline Integration Points

| Component | File | Role |
|-----------|------|------|
| Fragment handler | `main/oox/source/core/fragmenthandler.cxx` | Intercepts `mc:AlternateContent`, creates `MceAlternateContext` |
| Worksheet fragment | `main/oox/source/xls/worksheetfragment.cxx` | Subclass of `FragmentHandler`, overrides `createFastChildContext` for DrawingML |
| Drawing manager | `main/oox/source/xls/drawingmanager.cxx` | Manages drawing import for Calc |

### 3.4 Calc-Specific Concerns

| Concern | Assessment |
|---------|------------|
| `WorksheetFragment` overrides `createFastChildContext` | `MceAlternateContext` delegates to `WorksheetFragment` via `getFastContextHandler()`. DrawingML elements like `a:pic` will be correctly handled. |
| `FragmentHandler::createUnknownChildContext` returns 0 for base class | Only relevant if `WorksheetFragment` doesn't override `createUnknownChildContext`. Need to verify `WorksheetFragment` handles `a:pic` through `createFastChildContext` (known token) rather than `createUnknownChildContext`. |

---

## 4. Impress / PPTX Validation Plan

### 4.1 Test Document Structure

**Package:** `ppt/slides/slide1.xml` with `mc:AlternateContent` in shape tree

```xml
<p:sld xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"
       xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
       xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <p:cSld>
    <p:spTree>
      <p:sp>
        <p:nvSpPr><p:cNvPr id="1" name="Rect 1"/></p:nvSpPr>
        <p:spPr><a:geomPr><a:prstGeom prst="rect"/></a:geomPr></p:spPr>
        <p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Original shape</a:t></a:r></a:p></a:p></p:txBody>
      </p:sp>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
        </mc:Choice>
        <mc:Fallback>
          <p:sp><p:nvSpPr><p:cNvPr id="2" name="Fallback"/></p:nvSpPr>
          <p:spPr/><p:txBody><a:bodyPr/><a:lstStyle/><a:p><a:r><a:t>Fallback</a:t></a:r></a:p></a:p></p:txBody>
          </p:sp>
        </mc:Fallback>
      </mc:AlternateContent>
    </p:spTree>
  </p:cSld>
</p:sld>
```

### 4.2 Expected Results

| Test | Expected Behavior | Verification Method |
|------|-------------------|---------------------|
| AV-9: Choice eligible | Picture from Choice imported on slide | Open PPTX, check slide content |
| AV-10: Choice ineligible | Fallback shape imported | Change Requires to unsupported namespace |
| AV-11: Original shapes preserved | "Rect 1" shape present alongside AlternateContent content | Open PPTX, check all shapes |
| AV-12: Drawing relationships resolved | `r:embed="rId2"` resolved to media part | Check `_rels` and media |

### 4.3 Impress Pipeline Integration Points

| Component | File | Role |
|-----------|------|------|
| Fragment handler | `main/oox/source/core/fragmenthandler.cxx` | Intercepts `mc:AlternateContent`, creates `MceAlternateContext` |
| Slide fragment handler | `main/oox/source/ppt/slidefragmenthandler.cxx` | Subclass of `FragmentHandler`, handles slide content |
| Presentation fragment | `main/oox/source/ppt/presentationfragmenthandler.cxx` | Handles presentation-level content |

### 4.4 Impress-Specific Concerns

| Concern | Assessment |
|---------|------------|
| `SlideFragmentHandler` overrides `createFastChildContext` | `MceAlternateContext` delegates to `SlideFragmentHandler`. DrawingML elements inside Choice branch should be processed correctly. |
| Shape tree processing | `p:spTree` contains both regular shapes and `mc:AlternateContent`. The `MceAlternateContext` is created by `FragmentHandler::createUnknownChildContext` when the parser encounters `mc:AlternateContent` as an unknown child of `p:spTree`. |

---

## 5. Cross-Application Consistency

| Behavior | Writer | Calc | Impress |
|----------|--------|------|---------|
| `mc:AlternateContent` interception | `OOXMLFastDocumentHandler` | `FragmentHandler` | `FragmentHandler` |
| `MceProcessor` shared logic | Yes | Yes | Yes |
| `MceAlternateContext` context handler | Yes | Yes | Yes |
| Delegate for selected branch | `OOXMLFastContextHandler` | `FragmentHandler` subclass | `FragmentHandler` subclass |
| DrawingML element processing | Via `OOXMLFactory` | Via `WorksheetFragment` | Via `SlideFragmentHandler` |

**Consistency:** All three applications use the same `MceProcessor` selection logic. The only difference is the delegate context handler, which is application-specific. This is by design — the selection logic is shared, the event forwarding is application-specific.

---

## 6. Known Limitations (Application-Level)

| Limitation | Application(s) | Impact |
|------------|---------------|--------|
| Nested `mc:AlternateContent` inside Choice branch not handled | All | Inner AlternateContent not re-evaluated with MCE logic |
| `mc:Ignorable` not processed | All | Extension elements in ignorable namespaces skipped silently |
| `mc:MustUnderstand` not checked | All | Critical extensions silently lost with no error |
| Supported namespace limited to DrawingML main | All | Choice branches requiring other namespaces fall back |

---

## 7. Recommendation

Once the build environment is fixed (install `libboost-dev`), application-level validation should proceed in this order:

1. **Writer/DOCX** — simplest pipeline, easiest to verify text content
2. **Calc/XLSX** — DrawingML in worksheet context
3. **Impress/PPTX** — DrawingML in slide context with shape tree

For each application, verify:
- Choice branch selected when `Requires` lists only supported namespaces
- Fallback selected when `Requires` lists unsupported namespaces
- Non-MCE content before/after AlternateContent preserved
- No event leakage or duplication
