# MS-ODRAWXML Specification Extension Inventory — Milestone D.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes

---

## 1. Specification Reference

| Property | Value |
|----------|-------|
| Specification | [MS-ODRAWXML]: Office Drawing Extensions to Office Open XML Structure |
| Document ID | ecdeeaa7-1259-7263-e21f-c15219fb0faa |
| Current published version | 35.0 (Major), released 2026-10-06 |
| Reference URL | https://learn.microsoft.com/en-us/openspecs/office_standards/ms-odrawxml/06cff208-c6e1-4db7-bb68-665135e5f0de |
| Normative sections | Sections 1.7 and 2 |
| Normative basis | ISO/IEC 29500-1:2016 (DrawingML), ISO/IEC 29500-3:2015 (Markup Compatibility) |
| Total pages | 423 |
| Appendix A | Full XML schemas for 42 namespaces |

---

## 2. Extension Namespace Inventory

The MS-ODRAWXML specification defines extensions across **43 namespaces**, organized into 42 schema sections (Section 2.x) plus Appendix A schemas (Section 5.x). Each namespace corresponds to a specific extension domain (charts, diagrams, pictures, wordprocessing shapes, ink, 3D models, animation, etc.).

### 2.1 Namespace Summary Table

| # | Namespace URI | Section | Application Scope | Primary Purpose |
|---|--------------|---------|-------------------|-----------------|
| 1 | `http://schemas.microsoft.com/office/drawing/2010/main` | 2.3 | Shared (Word, Excel, PowerPoint) | Core drawing extensions: cameraTool, picture effects, content parts, hidden elements, image properties |
| 2 | `http://schemas.microsoft.com/office/drawing/2010/chartDrawing` | 2.5 | Excel | Chart drawing content parts and non-visual properties |
| 3 | `http://schemas.microsoft.com/office/drawing/2012/chart` | 2.6 | Excel | Chart filtering, data labels, series filtering, layout extensions |
| 4 | `http://schemas.microsoft.com/office/drawing/2012/main` | 2.7 | Shared | Invert solid fill, pivot options, style definitions |
| 5 | `http://schemas.microsoft.com/office/drawing/2012/chartStyle` | 2.8 | Excel | Chart styles, color styles, font references, marker layouts |
| 6 | `http://schemas.microsoft.com/office/drawing/2010/diagram` | 2.9 | PowerPoint | Diagram non-visual properties, image recoloring |
| 7 | `http://schemas.microsoft.com/office/drawing/2008/diagram` | 2.10 | PowerPoint | Diagram data model, group shapes, shape non-visual properties |
| 8 | `http://schemas.microsoft.com/ink/2010/main` | 2.11 | PowerPoint | Ink context nodes, properties, known context/semantic types |
| 9 | `http://schemas.microsoft.com/office/drawing/2007/8/2/chart` | 2.12 | Excel | Chart object properties, signature lines, background properties |
| 10 | `http://schemas.microsoft.com/office/word/2010/wordprocessingShape` | 2.13 | Word | Wordprocessing shape (text box linking, wordprocessing shape) |
| 11 | `http://schemas.microsoft.com/office/word/2010/wordml` | 2.14 | Word | WordprocessingML drawing extensions |
| 12 | `http://schemas.microsoft.com/office/word/2010/wordprocessingGroup` | 2.15 | Word | Wordprocessing group shape content parts |
| 13 | `http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas` | 2.16 | Word | Wordprocessing canvas extensions |
| 14 | `http://schemas.microsoft.com/office/drawing/2010/picture` | 2.17 | Shared | Picture-specific extensions |
| 15 | `http://schemas.microsoft.com/office/drawing/2014/main` | 2.18 | Shared | Column/row IDs, creation IDs, connectable references, predecessor references |
| 16 | `http://schemas.microsoft.com/office/drawing/2016/SVG/main` | 2.26 | Shared | SVG blip (embedded SVG images) |
| 17 | `http://schemas.microsoft.com/office/drawing/2016/11/main` | 2.27 | Shared | Picture attribution source URL |
| 18 | `http://schemas.microsoft.com/office/drawing/2016/11/diagram` | 2.28 | PowerPoint | Diagram auto-bullet, number diagram info |
| 19 | `http://schemas.microsoft.com/office/drawing/2013/main/command` | 2.29 | Shared | Change tracking data for shapes, connectors, pictures, group shapes |
| 20 | `http://schemas.microsoft.com/office/drawing/2016/ink` | 2.30 | PowerPoint | Extended brush properties, ink effects types |
| 21 | `http://schemas.microsoft.com/office/drawing/2017/model3d` | 2.31 | PowerPoint | 3D model support, lights, cameras, transforms, viewports |
| 22 | `http://schemas.microsoft.com/office/drawing/2016/12/diagram` | 2.32 | PowerPoint | Diagram list style, shape properties |
| 23 | `http://schemas.microsoft.com/office/drawing/2017/03/chart` | 2.33 | Excel | Data display options for charts |
| 24 | `http://schemas.microsoft.com/office/drawing/2017/decorative` | 2.34 | Shared | Decorative shape flag |
| 25 | `http://schemas.microsoft.com/office/drawing/2018/hyperlinkcolor` | 2.35 | Shared | Hyperlink color definitions |
| 26 | `http://schemas.microsoft.com/office/drawing/2018/animation/model3d` | 2.36 | PowerPoint | 3D model animation poster frame, embedded animation |
| 27 | `http://schemas.microsoft.com/office/drawing/2018/animation` | 2.37 | PowerPoint | Animation properties, animation names, play counts, time offsets |
| 28 | `http://schemas.microsoft.com/office/drawing/2018/sketchyshapes` | 2.38 | Shared | Line sketch style properties, sketch line types |
| 29 | `http://schemas.microsoft.com/office/drawing/2020/classificationShape` | 2.39 | Shared | Shape classification outcome |
| 30 | `http://schemas.microsoft.com/office/word/2020/oembed` | 2.40 | Word | OEmbed (online video/audio embedding) |
| 31 | `http://schemas.microsoft.com/office/drawing/2021/scriptlink` | 2.41 | Shared | Script link for shapes |
| 32 | `http://schemas.microsoft.com/office/drawing/2021/livefeed` | 2.42 | PowerPoint | Live feed background properties (blur, custom, normal, removed) |
| 33 | `http://schemas.microsoft.com/office/drawing/2022/imageformula` | 2.43 | Shared | Image formula |
| 34 | `http://schemas.microsoft.com/office/drawing/2015/06/chart` | 2.44 | Excel | Chart extensions (2015) |

**Note:** Namespaces 2.1–2.4, 2.19–2.25 appear in the table of contents but were partially captured. Sections 2.1–2.2 cover introduction/overview (normative) and applicability.

---

## 3. Key Extension Structures by Namespace

### 2.3 — `http://schemas.microsoft.com/office/drawing/2010/main` (Core Drawing Extensions)

| Element/Attribute | Subsection | Purpose | Integration Mechanism |
|-------------------|------------|---------|----------------------|
| `cameraTool` | 2.3.1.1 | Camera tool for 3D rotation of pictures | Direct element in DrawingML |
| `compatExt` | 2.3.1.2 | Compatibility extension | Extension list |
| `contentPart` | 2.3.1.3 | Content part reference | Relationship-based |
| `hiddenEffects` | 2.3.1.4 | Hidden picture effects | Direct element |
| `hiddenFill` | 2.3.1.5 | Hidden fill properties | Direct element |
| `hiddenLine` | 2.3.1.6 | Hidden line properties | Direct element |
| `hiddenScene3d` | 2.3.1.7 | Hidden 3D scene | Direct element |
| `hiddenSp3d` | 2.3.1.8 | Hidden 3D shape properties | Direct element |
| `imgProps` | 2.3.1.9 | Image properties | Direct element |
| `isCanvas` | 2.3.1.10 | Canvas flag attribute | Attribute |
| `m` | 2.3.1.11 | Unknown/minor element | Direct element |
| `shadowObscured` | 2.3.1.12 | Shadow obscured flag | Attribute |
| `useLocalDpi` | 2.3.1.13 | Local DPI flag | Attribute |
| `legacySpreadsheetColorIndex` | 2.3.2.1 | Legacy spreadsheet color index | Attribute |
| `CT_CameraTool` | 2.3.3.1 | Camera tool complex type | Type definition |
| `CT_CompatExt` | 2.3.3.2 | Compatibility extension type | Type definition |
| `CT_ContentPartLocking` | 2.3.3.3 | Content part locking type | Type definition |
| `CT_GvmlContentPart` | 2.3.3.4 | GVML content part type | Type definition |
| `CT_GvmlContentPartNonVisual` | 2.3.3.5 | GVML content part non-visual type | Type definition |
| `CT_IsGvmlCanvas` | 2.3.3.6 | GVML canvas type | Type definition |
| `CT_NonVisualInkContentPartProperties` | 2.3.3.7 | Ink content part properties type | Type definition |
| `CT_Photo` | 2.3.3.8 | Photo type | Type definition |
| `CT_PictureEffect*` (37 types) | 2.3.3.9–2.3.3.38 | Picture effects (background removal, blur, artistic effects, etc.) | Type definitions |
| `CT_ThemeFamily` | 2.4.3.1 | Theme family type | Type definition |

**Picture effects include:** BackgroundRemoval, Blur, BrightnessContrast, Cement, ChalkSketch, ColorTemperature, CrisscrossEtching, Cutout, FilmGrain, Glass, GlowDiffused, GlowEdges, LightScreen, LineDrawing, Marker, MosiaicBubbles, PaintBrush, PaintStrokes, PastelsSmooth, PencilGrayscale, PencilSketch, Photocopy, PlasticWrap, Saturation, SharpenSoften, Texturizer, WatercolorSponge

### 2.6 — `http://schemas.microsoft.com/office/drawing/2012/chart` (Chart Extensions)

| Element | Subsection | Purpose |
|---------|------------|---------|
| `autoCat` | 2.6.1.1 | Automatic category axis |
| `categoryFilterExceptions` | 2.6.1.2 | Category filter exceptions |
| `datalabelsRange` | 2.6.1.3 | Data labels range |
| `dlblFieldTable` | 2.6.1.4 | Data label field table |
| `filteredAreaSeries` | 2.6.1.5 | Filtered area series |
| `filteredBarSeries` | 2.6.1.6 | Filtered bar series |
| `filteredBubbleSeries` | 2.6.1.7 | Filtered bubble series |
| `filteredCategoryTitle` | 2.6.1.8 | Filtered category title |
| `filteredLineSeries` | 2.6.1.9 | Filtered line series |
| `filteredPieSeries` | 2.6.1.10 | Filtered pie series |
| `filteredRadarSeries` | 2.6.1.11 | Filtered radar series |
| `filteredScatterSeries` | 2.6.1.12 | Filtered scatter series |
| `filteredSeriesTitle` | 2.6.1.13 | Filtered series title |
| `filteredSurfaceSeries` | 2.6.1.14 | Filtered surface series |
| `formulaRef` | 2.6.1.15 | Formula reference |
| `fullRef` | 2.6.1.16 | Full reference |
| `layout` | 2.6.1.17 | Chart layout |
| `leaderLines` | 2.6.1.18 | Leader lines |
| `levelRef` | 2.6.1.19 | Level reference |
| `numFmt` | 2.6.1.20 | Number format |
| `pivotSource` | 2.6.1.21 | Pivot source |
| `showDataLabelsRange` | 2.6.1.22 | Show data labels range |
| `showLeaderLines` | 2.6.1.23 | Show leader lines |
| `spPr` | 2.6.1.24 | Shape properties |
| `tx` | 2.6.1.25 | Text |
| `xForSave` | 2.6.1.26 | X for save |

### 2.31 — `http://schemas.microsoft.com/office/drawing/2017/model3d` (3D Model Extensions)

| Element | Subsection | Purpose |
|---------|------------|---------|
| `model3d` | 2.31.1.1 | 3D model embedding |

| Complex Type | Subsection | Purpose |
|-------------|------------|---------|
| `CT_AmbientLight` | 2.31.3.1 | Ambient light |
| `CT_DirectionalLight` | 2.31.3.2 | Directional light |
| `CT_Model3D` | 2.31.3.3 | 3D model |
| `CT_Model3DCamera` | 2.31.3.4 | 3D camera |
| `CT_Model3DRaster` | 2.31.3.5 | 3D raster |
| `CT_Model3DTransform` | 2.31.3.6 | 3D transform |
| `CT_ObjectViewport` | 2.31.3.7 | Object viewport |
| `CT_OrthographicProjection` | 2.31.3.8 | Orthographic projection |
| `CT_PerspectiveProjection` | 2.31.3.9 | Perspective projection |
| `CT_PointLight` | 2.31.3.10 | Point light |
| `CT_Rotate3D` | 2.31.3.11 | 3D rotation |
| `CT_Scale3D` | 2.31.3.12 | 3D scale |
| `CT_SpotLight` | 2.31.3.13 | Spot light |
| `CT_UnknownLight` | 2.31.3.14 | Unknown light |
| `CT_WindowViewport` | 2.31.3.15 | Window viewport |

### 2.29 — `http://schemas.microsoft.com/office/drawing/2013/main/command` (Change Tracking)

| Element | Subsection | Purpose |
|---------|------------|---------|
| `cxnSpMkLst` | 2.29.1.1 | Connector shape moniker list |
| `deMkLst` | 2.29.1.2 | Drawing element moniker list |
| `graphicFrameMkLst` | 2.29.1.3 | Graphic frame moniker list |
| `grpSpMkLst` | 2.29.1.4 | Group shape moniker list |
| `inkMkLst` | 2.29.1.5 | Ink moniker list |
| `picMkLst` | 2.29.1.6 | Picture moniker list |
| `spMkLst` | 2.29.1.7 | Shape moniker list |
| `txMkLst` | 2.29.1.8 | Text moniker list |

---

## 4. Integration Mechanisms

### 4.1 Markup Compatibility and Extensibility (MCE)

Per Section 1.3 (Overview):

> "The extensions specified in this format are integrated into Office Open XML file formats by means of the Markup Compatibility and Extensibility features described in [ISO/IEC29500-3:2015]. Specifically, the **Ignorable attribute, the AlternateContent element, and application-defined extension elements** maintain compatibility with Office Open XML file formats implementations when integrating the extensions from this format."

Key MCE mechanisms:
- **`mc:Ignorable`** — Declares extension namespaces that consumers may ignore
- **`mc:AlternateContent`** — Provides choice/fallback pairs for extension content
- **`mc:Choice`** — Contains the preferred (extension) content
- **`mc:Fallback`** — Contains the fallback (standard) content
- **`mc:ProcessContent`** — Controls content processing in ignorable regions
- **`mc:PreserveElements` / `mc:PreserveAttributes`** — Preserves specific elements/attributes within ignorable regions

### 4.2 Extension Lists (`extLst`)

Per ISO/IEC 29500-1:2016 section A.4.1, `CT_OfficeArtExtensionList` enables future extensions to DrawingML elements. Many DrawingML elements include `extLst` as a child element.

### 4.3 Relationships

Content parts (`contentPart`) use OPC relationships to reference external parts. Diagram data models use relationship-based references.

---

## 5. Application Scope Classification

| Application | Primary Extension Namespaces | Key Extension Types |
|-------------|------------------------------|---------------------|
| **Word (DOCX)** | 2.3 (main), 2.13 (wordprocessingShape), 2.14 (wordml), 2.15 (wordprocessingGroup), 2.16 (wordprocessingCanvas), 2.40 (oEmbed) | wsp, text box linking, content parts, OEmbed |
| **Excel (XLSX)** | 2.3 (main), 2.5 (chartDrawing), 2.6 (2012/chart), 2.12 (2007/8/2/chart), 2.14 (2014/main), 2.33 (2017/03/chart), 2.44 (2015/06/chart) | Chart filtering, series filtering, data labels, invert fill, object properties |
| **PowerPoint (PPTX)** | 2.3 (main), 2.6 (diagram), 2.7 (2008/diagram), 2.8 (ink), 2.28 (2016/11/diagram), 2.29 (command), 2.30 (2016/ink), 2.31 (model3d), 2.32 (2016/12/diagram), 2.36 (anim/model3d), 2.37 (animation), 2.42 (livefeed) | 3D models, animation, ink, diagrams, change tracking, live feed |
| **Shared** | 2.3 (main), 2.12 (2014/main), 2.26 (SVG), 2.27 (attribution), 2.34 (decorative), 2.35 (hyperlinkcolor), 2.38 (sketchyshapes), 2.39 (classification), 2.41 (scriptlink), 2.43 (imageformula) | SVG blip, picture attribution, sketch shapes, classification, script link |

---

## 6. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| MS-ODRAWXML v35.0 defines 43 namespaces across 42 schema sections | VERIFIED | Web search results from spec PDF table of contents (officeprotocoldoc.z19.web.core.windows.net) |
| Current published version is 35.0, released 2026-10-06 | VERIFIED | Microsoft Learn landing page |
| Sections 1.7 and 2 are normative; rest informative | VERIFIED | Spec Section 1 (Introduction) |
| MCE integration via Ignorable/AlternateContent/extension elements | VERIFIED | Spec Section 1.3 (Overview) |
| No MS-ODRAWXML namespace URIs referenced in AOO source code | VERIFIED | grep across main/oox, main/writerfilter, main/sw, main/sc, main/sd, main/drawinglayer, main/svx — zero matches |
