# DrawingML Extension Implementation Priority List — Milestone D.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes

---

## 1. Priority Definitions

| Priority | Definition |
|----------|------------|
| **P1** | Common features with substantial compatibility impact, especially where AOO drops content or substantially alters documents |
| **P2** | Meaningful compatibility improvements with narrower prevalence, moderate complexity, or dependencies |
| **P3** | Rare, specialized, complex, or low-impact features |

---

## 2. P1 Candidates — Critical Compatibility Impact

### P1.1 — mc:AlternateContent / mc:Ignorable General-Purpose Handling

| Property | Value |
|----------|-------|
| Extension | All MS-ODRAWXML extensions (cross-cutting) |
| Specification section | ISO/IEC 29500-3 (Markup Compatibility); MS-ODRAWXML Section 1.3 |
| Affected applications | Word (DOCX), Excel (XLSX), PowerPoint (PPTX) |
| Current AOO support | **Unsupported** — zero references to mc:AlternateContent, mc:Ignorable, mc:Choice, mc:Fallback in OOXML code |
| Evidence of limitation | `drawingml-loss-analysis.md` Section 1.2 — no MCE handling anywhere in OOXML import/export paths |
| Expected user impact | **High** — Documents with AlternateContent wrappers lose extension content entirely; no graceful degradation via fallback |
| Implementation approach | Add generic mc:AlternateContent processor in `oox/source/core/xmlfilterbase.cxx` or `fragmenthandler.cxx` that evaluates Choice/Fallback pairs and selects appropriate content based on ignorable namespace list |
| Shared infrastructure | Yes — single implementation serves all three applications |
| Estimated complexity | Medium |
| Dependencies | Requires namespace-aware XML parsing; integration with existing FastTokenizer |
| Suggested tests | Import DOCX/XLSX/PPTX with AlternateContent; verify fallback content is used when extension namespace is ignorable |

### P1.2 — XLSX Chart Drawing Extensions (filtered series, data labels)

| Property | Value |
|----------|-------|
| Extension | `filteredAreaSeries`, `filteredBarSeries`, `filteredPieSeries`, `filteredLineSeries`, `datalabelsRange`, `showDataLabelsRange`, `dlblFieldTable`, `layout`, `leaderLines`, `numFmt`, `pivotSource` |
| Specification section | MS-ODRAWXML Section 2.6 (`http://schemas.microsoft.com/office/drawing/2012/chart`) |
| Affected applications | Excel (XLSX) |
| Current AOO support | **Unsupported** — no namespace references in `oox/source/xls/` or `sc/source/filter/excel/`; chart export already missing (DFF path commented out per W.1) |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.3 — all 2.6 chart elements classified as Unsupported |
| Expected user impact | **High** — Charts with filtered data series or custom data labels lose filtering and label configuration; charts display incorrectly |
| Implementation approach | Add chart extension fragment handlers in `oox/source/drawingml/chart/`; register namespace tokens; serialize in XLSX XML export (address `XML_extLst` OOXTODOs) |
| Shared infrastructure | Partially — chart import infrastructure exists; export needs DFF/Escher path restoration first (per W.1, this is a separate concern) |
| Estimated complexity | High |
| Dependencies | XLSX chart export infrastructure (W.1 milestone); DrawingML chart fragment handlers |
| Suggested tests | Import XLSX with filtered chart series; verify filtering preserved; export XLSX with charts; verify series filtering in output |

### P1.3 — Picture Effects Extensions (artistic effects, background removal, etc.)

| Property | Value |
|----------|-------|
| Extension | CT_PictureEffect* (37 types: BackgroundRemoval, Blur, ChalkSketch, ColorTemperature, Cutout, FilmGrain, Glass, GlowDiffused, LineDrawing, Marker, PaintBrush, PencilSketch, Photocopy, PlasticWrap, Texturizer, WatercolorSponge, etc.) |
| Specification section | MS-ODRAWXML Section 2.3.3.9–2.3.3.38 (`http://schemas.microsoft.com/office/drawing/2010/main`) |
| Affected applications | Word (DOCX), Excel (XLSX), PowerPoint (PPTX) |
| Current AOO support | **Unsupported** — no namespace references; picture effects not parsed or serialized |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.2 — all picture effect types classified as Unsupported |
| Expected user impact | **High** — Images with artistic effects, background removal, or other picture effects appear unprocessed; visual fidelity lost |
| Implementation approach | Add picture effect fragment handlers in `oox/source/drawingml/`; map effect types to AOO's graphic effect model; serialize in DOCX/XLSX/PPTX export |
| Shared infrastructure | Yes — graphic effect model in `main/drawinglayer/` or `main/svx/` can be extended |
| Estimated complexity | Medium-High |
| Dependencies | Graphic effect framework; picture import/export pipelines |
| Suggested tests | Import DOCX with blurred image; verify blur effect; import PPTX with background-removed picture; verify transparency |

### P1.4 — 3D Model Support (model3d)

| Property | Value |
|----------|-------|
| Extension | `model3d`, CT_Model3D, CT_Model3DCamera, CT_Model3DTransform, CT_AmbientLight, CT_DirectionalLight, CT_PointLight, CT_SpotLight, CT_Model3DRaster, CT_ObjectViewport, CT_PerspectiveProjection, CT_OrthographicProjection |
| Specification section | MS-ODRAWXML Section 2.31 (`http://schemas.microsoft.com/office/drawing/2017/model3d`) |
| Affected applications | PowerPoint (PPTX) |
| Current AOO support | **Unsupported** — no 3D model support at all in AOO DrawingML; no namespace references |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.4 — all 3D model elements classified as Unsupported; PPTX export does not exist |
| Expected user impact | **High** — Slides with 3D models appear empty; 3D objects completely lost |
| Implementation approach | Add 3D model fragment handlers in `oox/source/drawingml/` or `drawinglayer/primitive3d/`; extend drawing layer with 3D primitives; PPTX export must be built first (W.1 milestone) |
| Shared infrastructure | Partially — `drawinglayer/primitive3d/` exists but has no model3d integration |
| Estimated complexity | High |
| Dependencies | PPTX exporter (W.1 milestone); 3D rendering pipeline |
| Suggested tests | Import PPTX with 3D model; verify model present; export PPTX with 3D model; verify model preserved |

---

## 3. P2 Candidates — Meaningful Improvements, Moderate Complexity

### P2.1 — extLst (Extension List) Serialization in XLSX Export

| Property | Value |
|----------|-------|
| Extension | `extLst` / `ext` (CT_OfficeArtExtensionList) |
| Specification section | ISO/IEC 29500-1:2016 section A.4.1; MS-ODRAWXML all namespaces |
| Affected applications | Excel (XLSX) |
| Current AOO support | **Partially** — extLst recognized and skipped during import; `XML_extLst` OOXTODO markers in 7 locations during export |
| Evidence of limitation | `aoo-drawingml-source-map.md` Section 2.3 — OOXTODO markers in `xestyle.cxx:2138,2282`, `xecontent.cxx:867`, `xeview.cxx:465`, `xetable.cxx:2089`, `excrecds.cxx:825,999` |
| Expected user impact | **Medium** — Extension metadata lost on XLSX export; round-trip fidelity reduced |
| Implementation approach | Implement XML_extLst serialization in the 7 marked locations in `sc/source/filter/excel/`; store extension data in internal model during import |
| Shared infrastructure | Yes — XLSX exporter already exists; just needs extension list serialization |
| Estimated complexity | Low-Medium |
| Dependencies | Internal extension data model (needs to be designed) |
| Suggested tests | Export XLSX with extension data; verify extLst present in output; round-trip test |

### P2.2 — Ink Annotation Support

| Property | Value |
|----------|-------|
| Extension | CT_CtxNode, CT_Property, ST_CtxNodeType, ST_KnownCtxNodeType, ST_KnownSemanticType, ST_Point, ST_Points |
| Specification section | MS-ODRAWXML Section 2.11 (`http://schemas.microsoft.com/ink/2010/main`) |
| Affected applications | PowerPoint (PPTX) |
| Current AOO support | **Unsupported** — no ink namespace references; no ink annotation handling |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.8 — all ink elements classified as Unsupported |
| Expected user impact | **Medium** — Ink annotations and drawings on slides lost; annotation data not preserved |
| Implementation approach | Add ink context handlers in `oox/source/ppt/` or `oox/source/drawingml/`; map ink data to AOO's annotation/drawing model |
| Shared infrastructure | Partially — annotation infrastructure exists in `main/sw/` but not for PPTX |
| Estimated complexity | Medium |
| Dependencies | PPTX import pipeline; drawing annotation model |
| Suggested tests | Import PPTX with ink annotations; verify ink present |

### P2.3 — Wordprocessing Shape (wsp) and Text Box Linking

| Property | Value |
|----------|-------|
| Extension | `wsp`, CT_WordprocessingShape, CT_TextboxInfo, CT_LinkedTextboxInformation |
| Specification section | MS-ODRAWXML Section 2.13 (`http://schemas.microsoft.com/office/word/2010/wordprocessingShape`) |
| Affected applications | Word (DOCX) |
| Current AOO support | **Unsupported** — no wordprocessingShape namespace references in `writerfilter/` |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.8 — all wordprocessingShape elements classified as Unsupported |
| Expected user impact | **Medium** — Linked text boxes and wordprocessing shapes lost; text flow between boxes broken |
| Implementation approach | Add wordprocessing shape handlers in `writerfilter/source/ooxml/`; map to AOO's text frame model |
| Shared infrastructure | Partially — text frame support exists in Writer |
| Estimated complexity | Medium |
| Dependencies | Writer text frame model; DomainMapper integration |
| Suggested tests | Import DOCX with linked text boxes; verify text flow |

### P2.4 — SVG Blip (Embedded SVG Images)

| Property | Value |
|----------|-------|
| Extension | `svgBlip`, CT_SVGBlip |
| Specification section | MS-ODRAWXML Section 2.26 (`http://schemas.microsoft.com/office/drawing/2016/SVG/main`) |
| Affected applications | Word (DOCX), Excel (XLSX), PowerPoint (PPTX) |
| Current AOO support | **Unsupported** — no SVG namespace references; SVG images not rendered |
| Evidence of limitation | `drawingml-support-matrix.md` Section 2.6 — svgBlip classified as Unsupported |
| Expected user impact | **Medium** — SVG images in documents missing; fallback not provided |
| Implementation approach | Add SVG blip handler in `oox/source/drawingml/`; convert SVG to AOO's graphic format; serialize in export |
| Shared infrastructure | Yes — graphic import/export pipelines exist |
| Estimated complexity | Medium |
| Dependencies | SVG rendering library; graphic conversion pipeline |
| Suggested tests | Import DOCX with SVG image; verify image rendered |

---

## 4. P3 Candidates — Rare, Specialized, or Low Impact

| Priority | Extension | Section | Application | Impact | Complexity |
|----------|-----------|---------|-------------|--------|------------|
| P3.1 | `contentPart`, CT_GvmlContentPart, CT_ContentPartLocking | 2.3.1.3, 2.3.3.4-5 | All | Medium — linked content parts lost | Low-Medium |
| P3.2 | `hlinkClr`, CT_HyperlinkColor, ST_HyperlinkColor | 2.35 | All | Low — hyperlink colors lost | Low |
| P3.3 | `imgProps`, `useLocalDpi`, `shadowObscured`, `isCanvas` | 2.3.1.9-13 | All | Medium — image properties lost | Low |
| P3.4 | `svgBlip` (also P2) | 2.26 | All | Medium | Medium |
| P3.5 | `lineSketchStyleProps`, CT_LineSketchStyleProperties, ST_LineSketchSeed | 2.38 | All | Low — sketchy line styles lost | Low |
| P3.6 | `classification`, CT_ClassificationOutcome, ST_ClassificationOutcomeType | 2.39 | All | Low — classification metadata lost | Low |
| P3.7 | `picAttrSrcUrl`, CT_PictureAttributionSourceURL | 2.27 | All | Low — picture attribution lost | Low |
| P3.8 | `liveFeedProps`, background blur/custom/normal/removed | 2.42 | PPTX | Low — video call backgrounds lost | Low-Medium |
| P3.9 | `oembed`, CT_OEmbed | 2.40 | Word | Medium — online embeddings lost | Medium |
| P3.10 | `scriptLink`, CT_ScriptLink | 2.41 | All | Low — shape scripts lost | Low |
| P3.11 | `imageFormula`, CT_ImageFormula | 2.43 | All | Low — dynamic image formulas lost | Low |
| P3.12 | `animPr`, `posterFrame`, `embedAnim` | 2.36-37 | PPTX | Medium — animations lost | Medium |
| P3.13 | `cameraTool`, CT_CameraTool | 2.3.1.1 | All | Low — 3D camera orientation lost | Low |
| P3.14 | `compatExt`, CT_CompatExt | 2.3.1.2 | All | Low — compatibility hints lost | Low |
| P3.15 | Diagram extensions (auto-bullet, number diagram info) | 2.28, 2.32 | PPTX | Medium — diagrams degraded | Medium |

---

## 5. Shared Infrastructure Opportunities

| Opportunity | Description | Applications Served | Estimated Effort |
|-------------|-------------|---------------------|------------------|
| **mc:AlternateContent processor** | Generic MCE choice/fallback evaluator in `oox/source/core/` | DOCX, XLSX, PPTX | Medium |
| **Extension namespace registry** | Central registry of MS-ODRAWXML namespaces with handler dispatch | DOCX, XLSX, PPTX | Low-Medium |
| **extLst storage model** | Internal data model for extension lists with serialization | XLSX (primary), DOCX, PPTX | Low |
| **Graphic effect framework** | Common picture effect model for all 37 picture effect types | DOCX, XLSX, PPTX | Medium-High |
| **3D model primitives** | 3D model support in drawing layer | PPTX (primary) | High |

**Recommendation:** Implement the mc:AlternateContent processor first (P1.1) as it enables graceful degradation for ALL extensions. Then implement extension namespace registry (shared infrastructure) to enable systematic extension support.

---

## 6. Dependencies and Risks

| Dependency | Risk Level | Mitigation |
|------------|------------|------------|
| PPTX exporter (W.1 milestone) | High | PPTX MS-ODRAWXML support moot until exporter exists |
| XLSX chart export (W.1 milestone) | High | Chart extensions require chart export infrastructure |
| Drawing layer 3D primitives | Medium | Extend `drawinglayer/primitive3d/` incrementally |
| SVG rendering library | Medium | Evaluate existing SVG libraries for AOO compatibility |
| Graphic effect model extension | Medium | Extend existing effect framework in `main/svx/` or `main/drawinglayer/` |

---

## 7. Suggested Regression Tests

| Test | Description | Expected Result |
|------|-------------|-----------------|
| Import DOCX with AlternateContent (picture effect) | DOCX with mc:AlternateContent: Choice=picture effect, Fallback=standard image | Fallback image displayed; no crash |
| Import XLSX with filtered chart series | XLSX with chart using filteredAreaSeries | Series filtering applied |
| Import PPTX with 3D model | PPTX with model3d element | 3D model present on slide |
| Export XLSX with extension data | XLSX with extLst data; re-export | extLst present in output |
| Round-trip DOCX with picture effect | Import → export → import | Picture effect preserved (or graceful degradation) |
| Import DOCX with SVG image | DOCX with svgBlip | SVG image rendered |

---

## 8. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| P1 classifications based on content loss severity | VERIFIED | Source analysis of extension impact + W.1 findings |
| No existing implementation for any P1/P2/P3 candidate | VERIFIED | Zero namespace references in AOO source |
| Shared infrastructure opportunities identified | PROBABLE | Based on codebase structure analysis |
| Complexity estimates | PROBABLE | Based on similar feature implementations in AOO |
| Runtime behavior not verified | UNKNOWN | No runtime testing performed |
