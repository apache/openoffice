# DrawingML Loss and Fallback Analysis — Milestone D.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes

---

## 1. Markup Compatibility and Extensibility (MCE) Analysis

### 1.1 MCE Mechanisms in MS-ODRAWXML

Per MS-ODRAWXML Section 1.3 (Overview):

> "The extensions specified in this format are integrated into Office Open XML file formats by means of the Markup Compatibility and Extensibility features described in [ISO/IEC29500-3:2015]. Specifically, the **Ignorable attribute, the AlternateContent element, and application-defined extension elements** maintain compatibility with Office Open XML file formats implementations when integrating the extensions from this format."

The following MCE constructs are used:

| Construct | Purpose | Spec Reference |
|-----------|---------|----------------|
| `mc:Ignorable` | Declares extension namespaces that consumers may safely ignore | ISO/IEC 29500-3 |
| `mc:AlternateContent` | Wrapper containing Choice/Fallback pairs | ISO/IEC 29500-3 |
| `mc:Choice` | Contains the preferred (extension-specific) content | ISO/IEC 29500-3 |
| `mc:Fallback` | Contains the fallback (standard DrawingML) content | ISO/IEC 29500-3 |
| `mc:ProcessContent` | Controls content processing within ignorable regions | ISO/IEC 29500-3 |
| `mc:PreserveElements` | Preserves specific elements within ignorable regions | ISO/IEC 29500-3 |
| `mc:PreserveAttributes` | Preserves specific attributes within ignorable regions | ISO/IEC 29500-3 |

### 1.2 AOO MCE Handling — Finding: NONE EXISTS

**Zero references** to `mc:AlternateContent`, `mc:Ignorable`, `mc:Choice`, `mc:Fallback`, `mc:ProcessContent`, `mc:PreserveElements`, or `mc:PreserveAttributes` exist in any OOXML import or export code path.

Searched across:
- `main/oox/` — no matches
- `main/writerfilter/` — no matches
- `main/sw/source/filter/ww8/` — no matches
- `main/sc/source/filter/excel/` — no matches
- `main/sd/` — no matches

The only "AlternateContent" matches in the entire repository are in `main/ucb/source/ucp/odma/` (lines 62-63, 119-120, 176-182, 229-230, 261-262), which relate to Open Document Management API (ODMA) callbacks, not OOXML Markup Compatibility.

**Implication:** When AOO encounters an OOXML document containing `mc:AlternateContent` with MS-ODRAWXML extensions in the `mc:Choice` element and standard DrawingML in the `mc:Fallback` element:

1. **Import:** AOO's XML parser will encounter unknown elements in the extension namespace. Without `mc:Ignorable` handling, the parser may:
   - Skip unknown elements (if using lax validation)
   - Fail to parse (if using strict validation)
   - The `mc:Fallback` content will NOT be automatically selected — AOO has no logic to process `mc:AlternateContent` choice/fallback pairs

2. **Export:** AOO cannot produce `mc:AlternateContent` output. Any extension data that AOO cannot represent will simply be omitted, with no fallback content generated.

---

## 2. Extension Loss Analysis by Mechanism

### 2.1 Import Loss Mechanisms

When opening an OOXML document containing MS-ODRAWXML extensions:

| Loss Mechanism | Description | Affected Extensions | Severity |
|---------------|-------------|---------------------|----------|
| **Namespace not recognized** | Extension elements in unknown namespaces are skipped by the XML parser without creating any internal representation | All 43 MS-ODRAWXML namespaces | High — content silently dropped |
| **extLst contents ignored** | Extension list elements are recognized as tokens but their child elements are not parsed | All elements using extLst | Medium — extension metadata lost |
| **No mc:AlternateContent processing** | Choice/Fallback pairs are not evaluated; fallback content is not used as substitute | All extensions wrapped in AlternateContent | High — graceful degradation impossible |
| **Relationship not resolved** | Content part relationships referencing extension parts are not followed | `contentPart` elements | Medium — linked content lost |

### 2.2 Export Loss Mechanisms

When saving an AOO document to OOXML format:

| Loss Mechanism | Description | Affected Extensions | Severity |
|---------------|-------------|---------------------|----------|
| **No extension serialization** | AOO's internal model has no representation for MS-ODRAWXML extensions, so nothing is written | All extensions | High — all extension data lost |
| **XML_extLst OOXTODO** | XLSX exporter has explicit TODOs for XML_extLst serialization | All XLSX extensions using extLst | High — extension lists not written |
| **No mc:AlternateContent generation** | AOO cannot produce Choice/Fallback pairs for extensions it cannot represent | All extensions | N/A (nothing to wrap) |
| **DFF/Escher path commented out** | XLSX XML export has drawing export disabled; MS-ODRAWXML drawing extensions cannot be serialized | Chart, picture, shape extensions | Critical — all visual drawing data lost in XLSX XML export |

### 2.3 Round-Trip Loss

| Round-Trip Direction | Loss Description | Severity |
|---------------------|-----------------|----------|
| DOCX → AOO → DOCX | MS-ODRAWXML extensions dropped on import; nothing re-exported | Complete loss of extension data |
| XLSX → AOO → XLSX | MS-ODRAWXML chart extensions dropped; XML_extLst not serialized; charts/drawings already missing from export | Complete loss of extension data + pre-existing chart/drawing gaps |
| PPTX → AOO → PPTX | MS-ODRAWXML extensions dropped on import; no PPTX export exists | Complete loss (export not possible) |

---

## 3. Document Damage Classification

### 3.1 P1 — Critical Content Loss (Visible/Functional Damage)

| Extension Category | Specific Extensions | Application | Damage Description | Severity |
|-------------------|---------------------|-------------|-------------------|----------|
| **3D Models** | `model3d`, CT_Model3D, CT_Model3DCamera, CT_Model3DTransform, lights, viewports | PPTX | 3D model objects completely missing; slides with 3D content appear empty or broken | Critical |
| **Charts (filtered series)** | `filteredAreaSeries`, `filteredBarSeries`, `filteredPieSeries`, `filteredLineSeries`, etc. | XLSX | Chart data filtering lost; charts show all data instead of filtered subsets | Critical |
| **Chart data labels** | `datalabelsRange`, `showDataLabelsRange`, `dlblFieldTable` | XLSX | Chart data labels missing or incorrect | High |
| **Pictures with effects** | CT_PictureEffect* (37 types: BackgroundRemoval, Blur, Artistic effects, etc.) | All | Picture effects not applied; images appear unprocessed | High |
| **Ink annotations** | CT_CtxNode, CT_Property, ink monikers | PPTX | Ink drawings and annotations missing | High |
| **Diagram extensions** | Diagram data model extensions, group shapes, auto-bullets | PPTX | SmartArt/diagrams may render incorrectly or not at all | High |
| **Wordprocessing shapes** | `wsp`, CT_WordprocessingShape, text box linking | DOCX | Linked text boxes and wordprocessing shapes lost | High |
| **OEmbed content** | `oembed`, CT_OEmbed | DOCX | Online video/audio embeddings missing | High |

### 3.2 P2 — Moderate Content Loss (Editing Limitations)

| Extension Category | Specific Extensions | Application | Damage Description | Severity |
|-------------------|---------------------|-------------|-------------------|----------|
| **Picture properties** | `imgProps`, `useLocalDpi`, `shadowObscured`, `isCanvas` | All | Image properties lost; images may render at wrong DPI or with wrong shadow behavior | Medium |
| **Content parts** | `contentPart`, CT_GvmlContentPart, CT_ContentPartLocking | All | Linked content parts not resolved; embedded content may be missing | Medium |
| **Chart layout** | `layout`, `leaderLines`, `levelRef`, `numFmt`, `pivotSource` | XLSX | Chart formatting and layout degraded | Medium |
| **Animation** | `animPr`, `posterFrame`, `embedAnim` | PPTX | Slide animations missing or degraded | Medium |
| **Hyperlink color** | `hlinkClr`, CT_HyperlinkColor | All | Hyperlink colors not preserved | Low-Medium |
| **Sketchy shapes** | `lineSketchStyleProps` | All | Hand-drawn shape styles lost | Low-Medium |
| **Classification** | `classification`, CT_ClassificationOutcome | All | Document classification metadata lost | Low-Medium |

### 3.3 P3 — Minor/Specialized Loss

| Extension Category | Specific Extensions | Application | Damage Description | Severity |
|-------------------|---------------------|-------------|-------------------|----------|
| **Camera tool** | `cameraTool`, CT_CameraTool | All | 3D camera orientation for pictures lost | Low |
| **Compatibility extension** | `compatExt`, CT_CompatExt | All | Compatibility hints lost | Low |
| **Theme family** | `CT_ThemeFamily` | All | Theme family info lost | Low |
| **SVG blip** | `svgBlip`, CT_SVGBlip | All | SVG images not rendered | Low |
| **Picture attribution** | `picAttrSrcUrl`, CT_PictureAttributionSourceURL | All | Image attribution metadata lost | Low |
| **Live feed backgrounds** | Background blur/custom/normal/removed | PPTX | Video call background effects lost | Low |
| **Script link** | `scriptLink`, CT_ScriptLink | All | Shape script behavior lost | Low |
| **Image formula** | `imageFormula`, CT_ImageFormula | All | Dynamic image formulas lost | Low |

---

## 4. Fallback and Preservation Analysis

### 4.1 What Happens When AOO Encounters MS-ODRAWXML Content

**Import flow:**
1. OOXML package is opened via `ZipStorage` or `ZipPackage`
2. XML parts are parsed using `FastTokenizer`/`FSHelper`
3. DrawingML elements are dispatched to context handlers in `oox/source/drawingml/`
4. Unknown elements in unknown namespaces are **skipped** (no context created, no error raised)
5. `extLst` elements are recognized as tokens but their children are **not parsed**
6. No `mc:AlternateContent` evaluation occurs — fallback content is never selected as substitute

**Result:** Extension content is silently lost. The document opens without errors, but graphical objects, chart features, pictures with effects, and other extension content are missing or degraded.

### 4.2 What Happens When AOO Saves OOXML Content

**Export flow (DOCX):**
1. `DocxExportFilter::exportDocument()` creates `DocxExport`
2. `DocxExport::ExportDocument_Impl()` writes document.xml, styles.xml, etc.
3. `DocxAttributeOutput` serializes paragraph/run/table properties
4. `DrawingML` is used for image export
5. **No extension list serialization occurs**
6. **No mc:AlternateContent is generated**

**Export flow (XLSX — native sc path):**
1. `ScExportExcel5()` → `lcl_ExportExcel2007Xml()` → `ExportXml2007::Write()`
2. `ExcDocument::WriteXml()` serializes workbook and sheets
3. Per-table serializers write cell data, styles, etc.
4. `XML_extLst` is explicitly OOXTODO'd in 7 locations — **not serialized**
5. DFF/Escher drawing path is **commented out** — charts, shapes, drawings not exported

**Result:** No MS-ODRAWXML extension data is written to the output file. The package is valid OOXML but lacks any extension content.

### 4.3 Preservation Without Interpretation

AOO does **not** preserve MS-ODRAWXML extensions in any form:
- Not as raw XML (no passthrough mechanism)
- Not as extension list data (extLst contents skipped)
- Not as alternate content (no mc:AlternateContent handling)
- Not as relationships (extension part relationships not followed)

This contrasts with a "preserve without interpretation" approach where unknown XML would be retained in the package for round-trip fidelity. AOO does not implement such a mechanism for OOXML.

---

## 5. Interaction Between MS-ODRAWXML Extensions and Standard DrawingML

MS-ODRAWXML extensions extend standard DrawingML in the following ways:

| Extension Type | Interaction with Standard DrawingML | AOO Handling |
|---------------|-------------------------------------|--------------|
| Picture effects (CT_PictureEffect*) | Extends `a:effectLst` with additional effect types | Standard effects partially supported; MS-ODRAWXML effects completely unsupported |
| 3D models (model3d) | Extends `a:graphicData` with 3D model content | No 3D support at all in AOO DrawingML |
| Chart extensions (filtered series, etc.) | Extends `c:chart` elements with filtering and layout | Standard chart import exists; MS-ODRAWXML chart extensions unsupported |
| Wordprocessing shapes (wsp) | Extends `w:drawing` with wordprocessing-specific shapes | Standard shapes partially supported; MS-ODRAWXML wsp unsupported |
| Ink (CT_CtxNode) | Extends drawing with ink annotation data | No ink support in AOO |
| Diagrams (data model extensions) | Extends diagram `drawing` elements with data model | Basic diagram import exists; extensions unsupported |

**Key finding:** AOO's standard DrawingML support is itself incomplete (per W.1: charts/drawings missing from XLSX export, stubs in DOCX export). MS-ODRAWXML extensions build on top of this incomplete foundation, compounding the loss.

---

## 6. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| No mc:AlternateContent handling exists | VERIFIED | Zero references across all OOXML source code |
| No mc:Ignorable handling exists | VERIFIED | Zero references across all OOXML source code |
| extLst contents skipped during import | VERIFIED | 9 source locations with extLst token recognition but no child parsing |
| XML_extLst not serialized in XLSX export | VERIFIED | 7 OOXTODO markers in sc exporter |
| DFF/Escher drawing path commented out in XLSX export | VERIFIED | `excdoc.cxx:576-582` (from W.1 audit) |
| No extension preservation mechanism | VERIFIED | No raw XML passthrough, no extension list storage, no alternate content handling |
| Runtime behavior not verified | UNKNOWN | No runtime testing performed per milestone restrictions |
