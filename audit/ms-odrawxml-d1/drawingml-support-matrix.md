# DrawingML Support Matrix — Milestone D.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes

---

## 1. Classification Definitions

| Classification | Meaning |
|---------------|---------|
| **Implemented** | Extension is parsed/serialized and its semantics are preserved in AOO's internal model |
| **Partially implemented** | Extension is recognized but only partially processed; some data may be lost |
| **Unsupported** | Extension is not recognized; data is silently dropped or ignored |
| **Unknown / not verified** | Source evidence is insufficient to determine support level |

**Note:** Runtime verification was not performed for this milestone. All classifications are based on source code inspection.

---

## 2. Cross-Format Support Summary

### 2.1 Extension List (extLst) — Cross-Cutting

| Extension | Spec Section | Namespace | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|-----------|-------------|-----------|-------------|-------------|-------------|-------------|-------------|-------------|
| `extLst` / `ext` (CT_OfficeArtExtensionList) | ISO 29500-1 A.4.1 | All MS-ODRAWXML namespaces | Partially (recognized, skipped) | Unsupported | Partially (recognized, skipped) | Unsupported (OOXTODO) | Partially (recognized, skipped) | N/A (no exporter) |

**Evidence:**
- Import: `extLst` recognized and skipped in `themefragmenthandler.cxx:71`, `hyperlinkcontext.cxx:158`, `tablerowcontext.cxx:61`, `tablestylecontext.cxx:100`, `tablestyletextstylecontext.cxx:94`, `tablecellcontext.cxx:105`, `datamodelcontext.cxx:68,138,154`, `layoutnodecontext.cxx:279`
- Export: `XML_extLst` OOXTODO markers in `xestyle.cxx:2138,2282`, `xecontent.cxx:867`, `xeview.cxx:465`, `xetable.cxx:2089`, `excrecds.cxx:825,999`

---

### 2.2 Namespace: `http://schemas.microsoft.com/office/drawing/2010/main` (Core Drawing Extensions)

| Element | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|---------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `cameraTool` | 2.3.1.1 | 3D camera tool for pictures | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `compatExt` | 2.3.1.2 | Compatibility extension | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `contentPart` | 2.3.1.3 | Content part reference | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `hiddenEffects` | 2.3.1.4 | Hidden picture effects | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `hiddenFill` | 2.3.1.5 | Hidden fill | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `hiddenLine` | 2.3.1.6 | Hidden line | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `hiddenScene3d` | 2.3.1.7 | Hidden 3D scene | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `hiddenSp3d` | 2.3.1.8 | Hidden 3D shape props | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `imgProps` | 2.3.1.9 | Image properties | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `isCanvas` | 2.3.1.10 | Canvas flag | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `m` | 2.3.1.11 | Minor element | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `shadowObscured` | 2.3.1.12 | Shadow obscured | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `useLocalDpi` | 2.3.1.13 | Local DPI flag | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `legacySpreadsheetColorIndex` | 2.3.2.1 | Legacy color index | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_CameraTool | 2.3.3.1 | Camera tool type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_CompatExt | 2.3.3.2 | Compat extension type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_ContentPartLocking | 2.3.3.3 | Content part locking | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_GvmlContentPart | 2.3.3.4 | GVML content part | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_GvmlContentPartNonVisual | 2.3.3.5 | GVML content part NV | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_IsGvmlCanvas | 2.3.3.6 | GVML canvas type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_NonVisualInkContentPartProperties | 2.3.3.7 | Ink content part props | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Photo | 2.3.3.8 | Photo type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_PictureEffect* (37 types) | 2.3.3.9–2.3.3.38 | Picture effects | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_ThemeFamily | 2.4.3.1 | Theme family type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

**Picture effects include:** BackgroundRemoval, Blur, BrightnessContrast, Cement, ChalkSketch, ColorTemperature, CrisscrossEtching, Cutout, FilmGrain, Glass, GlowDiffused, GlowEdges, LightScreen, LineDrawing, Marker, MosiaicBubbles, PaintBrush, PaintStrokes, PastelsSmooth, PencilGrayscale, PencilSketch, Photocopy, PlasticWrap, Saturation, SharpenSoften, Texturizer, WatercolorSponge

---

### 2.3 Namespace: `http://schemas.microsoft.com/office/drawing/2012/chart` (Chart Extensions)

| Element | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|---------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `autoCat` | 2.6.1.1 | Auto category axis | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `categoryFilterExceptions` | 2.6.1.2 | Category filter exceptions | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `datalabelsRange` | 2.6.1.3 | Data labels range | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `dlblFieldTable` | 2.6.1.4 | Data label field table | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredAreaSeries` | 2.6.1.5 | Filtered area series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredBarSeries` | 2.6.1.6 | Filtered bar series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredBubbleSeries` | 2.6.1.7 | Filtered bubble series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredCategoryTitle` | 2.6.1.8 | Filtered category title | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredLineSeries` | 2.6.1.9 | Filtered line series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredPieSeries` | 2.6.1.10 | Filtered pie series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredRadarSeries` | 2.6.1.11 | Filtered radar series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredScatterSeries` | 2.6.1.12 | Filtered scatter series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredSeriesTitle` | 2.6.1.13 | Filtered series title | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `filteredSurfaceSeries` | 2.6.1.14 | Filtered surface series | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `formulaRef` | 2.6.1.15 | Formula reference | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `fullRef` | 2.6.1.16 | Full reference | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `layout` | 2.6.1.17 | Chart layout | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `leaderLines` | 2.6.1.18 | Leader lines | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `levelRef` | 2.6.1.19 | Level reference | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `numFmt` | 2.6.1.20 | Number format | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `pivotSource` | 2.6.1.21 | Pivot source | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `showDataLabelsRange` | 2.6.1.22 | Show data labels range | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `showLeaderLines` | 2.6.1.23 | Show leader lines | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `spPr` (chart extension) | 2.6.1.24 | Chart shape properties | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `tx` (chart extension) | 2.6.1.25 | Chart text | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `xForSave` | 2.6.1.26 | X for save | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

---

### 2.4 Namespace: `http://schemas.microsoft.com/office/drawing/2017/model3d` (3D Model Extensions)

| Element/Type | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|-------------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `model3d` | 2.31.1.1 | 3D model embedding | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_AmbientLight | 2.31.3.1 | Ambient light | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_DirectionalLight | 2.31.3.2 | Directional light | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Model3D | 2.31.3.3 | 3D model | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Model3DCamera | 2.31.3.4 | 3D camera | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Model3DRaster | 2.31.3.5 | 3D raster | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Model3DTransform | 2.31.3.6 | 3D transform | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_ObjectViewport | 2.31.3.7 | Object viewport | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_OrthographicProjection | 2.31.3.8 | Orthographic projection | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_PerspectiveProjection | 2.31.3.9 | Perspective projection | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_PointLight | 2.31.3.10 | Point light | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Rotate3D | 2.31.3.11 | 3D rotation | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_Scale3D | 2.31.3.12 | 3D scale | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_SpotLight | 2.31.3.13 | Spot light | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_UnknownLight | 2.31.3.14 | Unknown light | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_WindowViewport | 2.31.3.15 | Window viewport | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

---

### 2.5 Namespace: `http://schemas.microsoft.com/office/drawing/2013/main/command` (Change Tracking)

| Element | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|---------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `cxnSpMkLst` | 2.29.1.1 | Connector shape moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `deMkLst` | 2.29.1.2 | Drawing element moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `graphicFrameMkLst` | 2.29.1.3 | Graphic frame moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `grpSpMkLst` | 2.29.1.4 | Group shape moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `inkMkLst` | 2.29.1.5 | Ink moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `picMkLst` | 2.29.1.6 | Picture moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `spMkLst` | 2.29.1.7 | Shape moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| `txMkLst` | 2.29.1.8 | Text moniker list | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

---

### 2.6 Namespace: `http://schemas.microsoft.com/office/drawing/2016/SVG/main` (SVG Blip)

| Element | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|---------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `svgBlip` | 2.26.1.1 | Embedded SVG image | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_SVGBlip | 2.26.3.1 | SVG blip type | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

---

### 2.7 Namespace: `http://schemas.microsoft.com/office/drawing/2021/livefeed` (Live Feed Background)

| Element | Spec Section | Purpose | DOCX Import | DOCX Export | XLSX Import | XLSX Export | PPTX Import | PPTX Export |
|---------|-------------|---------|-------------|-------------|-------------|-------------|-------------|-------------|
| `liveFeedProps` | 2.42.1.1 | Live feed properties | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_BackgroundBlurProperties | 2.42.3.2 | Background blur | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_BackgroundCustomProperties | 2.42.3.3 | Background custom | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_BackgroundNormalProperties | 2.42.3.4 | Background normal | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_BackgroundRemovedProperties | 2.42.3.5 | Background removed | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_LiveFeedBackgroundProperties | 2.42.3.6 | Live feed background | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |
| CT_LiveFeedProperties | 2.42.3.7 | Live feed properties | Unsupported | Unsupported | Unsupported | Unsupported | Unsupported | N/A |

---

### 2.8 Remaining Namespaces (All Unsupported)

| Namespace | Section | Application | Status (Import/Export) |
|-----------|---------|-------------|----------------------|
| `http://schemas.microsoft.com/office/drawing/2010/chartDrawing` | 2.5 | Excel | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2012/main` | 2.7 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2012/chartStyle` | 2.8 | Excel | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2010/diagram` | 2.9 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2008/diagram` | 2.10 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/ink/2010/main` | 2.11 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2007/8/2/chart` | 2.12 | Excel | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/word/2010/wordprocessingShape` | 2.13 | Word | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/word/2010/wordml` | 2.14 | Word | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/word/2010/wordprocessingGroup` | 2.15 | Word | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas` | 2.16 | Word | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2010/picture` | 2.17 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2014/main` | 2.18 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2016/11/main` | 2.27 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2016/11/diagram` | 2.28 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2016/ink` | 2.30 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2016/12/diagram` | 2.32 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2017/03/chart` | 2.33 | Excel | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2017/decorative` | 2.34 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2018/hyperlinkcolor` | 2.35 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2018/animation/model3d` | 2.36 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2018/animation` | 2.37 | PowerPoint | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2018/sketchyshapes` | 2.38 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2020/classificationShape` | 2.39 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/word/2020/oembed` | 2.40 | Word | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2021/scriptlink` | 2.41 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2022/imageformula` | 2.43 | Shared | Unsupported / Unsupported |
| `http://schemas.microsoft.com/office/drawing/2015/06/chart` | 2.44 | Excel | Unsupported / Unsupported |

---

## 3. Support Classification by Format

### 3.1 DOCX (Writer)

| Format | Import | Export | Evidence |
|--------|--------|--------|----------|
| Standard DrawingML | Implemented | Partially implemented | DomainMapper import; DocxExport + DocxAttributeOutput export (with TODO stubs) |
| MS-ODRAWXML extensions | Unsupported | Unsupported | Zero namespace references in `writerfilter/` or `sw/source/filter/ww8/` |
| extLst | Partially (skip only) | Unsupported | No extLst handling in writerfilter; DocxAttributeOutput has no extension serialization |
| mc:AlternateContent | Unsupported | Unsupported | No mc:AlternateContent handling anywhere in DOCX path |

### 3.2 XLSX (Calc)

| Format | Import | Export | Evidence |
|--------|--------|--------|----------|
| Standard DrawingML | Implemented | Partial (charts/drawings missing) | ExcelFilter import; native sc XML exporter (charts/drawings DFF path commented out) |
| MS-ODRAWXML extensions | Unsupported | Unsupported | Zero namespace references in `oox/source/xls/` or `sc/source/filter/excel/` |
| extLst | Partially (skip only) | Unsupported (OOXTODO) | extLst recognized and skipped in drawingml import; XML_extLst OOXTODO in 7 locations in sc exporter |

### 3.3 PPTX (Impress)

| Format | Import | Export | Evidence |
|--------|--------|--------|----------|
| Standard DrawingML | Implemented | N/A (no exporter) | PowerPointImport handles slide XML |
| MS-ODRAWXML extensions | Unsupported | N/A | Zero namespace references in `oox/source/ppt/` |
| extLst | Partially (skip only) | N/A | extLst recognized and skipped in drawingml diagram import |
| mc:AlternateContent | Unsupported | N/A | No mc:AlternateContent handling anywhere |

---

## 4. Cross-Reference with W.1 Audit Findings

The W.1 audit (`audit/ooxml-export-w1/`) identified these related gaps:

| W.1 Finding | Relevance to MS-ODRAWXML |
|-------------|-------------------------|
| XLSX DFF/Escher path commented out (`excdoc.cxx:576-582`) | Charts, shapes, drawings omitted from XLSX export — MS-ODRAWXML chart extensions would also be lost |
| DOCX `OutputOLENode()` stub (`docxexport.cxx:357`) | OLE objects lost — MS-ODRAWXML content parts related |
| DOCX `FlyFrameGraphic()` TODO (`docxattributeoutput.cxx:1502`) | Floating graphics partially supported — picture extensions lost |
| No PPTX exporter exists | All PPTX MS-ODRAWXML extensions moot until exporter built |
| Strict OOXML not supported | MS-ODRAWXML extensions are Transitional-only; Strict would not include them |

---

## 5. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| All MS-ODRAWXML extensions unsupported for import/export | VERIFIED | Zero namespace URI references in any AOO source file |
| extLst recognized but skipped during import | VERIFIED | 9 source locations with `A_TOKEN(ext)`/`A_TOKEN(extLst)` / `DGM_TOKEN(extLst)` |
| extLst not serialized during XLSX export | VERIFIED | 7 OOXTODO markers for XML_extLst in sc exporter |
| No mc:AlternateContent handling | VERIFIED | Zero references (only UCB/ODMA unrelated) |
| No PPTX export | VERIFIED | W.1 audit confirmed no PresentationML exporter exists |
| Runtime behavior not verified | UNKNOWN | No runtime testing performed per milestone restrictions |
