# AOO DrawingML Source Map — Milestone D.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.1  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Source Map

| Subsystem | Role | Key Directories |
|-----------|------|-----------------|
| `main/oox/source/core/` | OOXML filter framework core | XmlFilterBase, Relations, FastTokenHandler, FragmentHandler, RecordParser |
| `main/oox/source/drawingml/` | DrawingML import (all formats) | shape, fill, line, text, theme, color, chart, table, diagram |
| `main/oox/source/export/` | DrawingML export (DOCX only) | drawingml.cxx, shapes.cxx, vmlexport.cxx |
| `main/oox/source/xls/` | XLSX import/export (UNO) | ExcelFilter (import OK, export returns false) |
| `main/oox/source/ppt/` | PPTX import | PowerPointImport (import OK, no export) |
| `main/oox/source/helper/` | OOXML helpers | ZipStorage, BinaryInput/OutputStream, GraphicHelper, ContainerHelper |
| `main/oox/source/ole/` | OLE/VBA support | VbaProject, OleObjectHelper |
| `main/oox/source/token/` | Generated fast tokens | Token headers for all OOXML namespaces |
| `main/writerfilter/source/ooxml/` | DOCX import (DomainMapper) | model.xml, DomainMapper, PropertyMap, GraphicImport, OLEHandler |
| `main/writerfilter/source/dmapper/` | DomainMapper implementation | DomainMapper_Impl, table handlers, style handlers |
| `main/sw/source/filter/ww8/` | DOCX export | DocxExportFilter, DocxExport, DocxAttributeOutput |
| `main/sc/source/filter/excel/` | Native XLSX export (sc path) | ScExportExcel5, ExportXml2007, ExcDocument, serializers |
| `main/sd/source/filter/eppt/` | Legacy binary PPT export | PPTWriter (binary PPT only, NOT PPTX) |
| `main/drawinglayer/` | Drawing layer (2D/3D model) | PRIMITIVE2D, PRIMITIVE3D, scene, objects |
| `main/svx/` | SVX drawing components | Escher/DFF handling, shape properties |
| `main/package/` | ZIP/OPC package | ZipPackage, xstorage |
| `main/xmlsecurity/` | Encryption | XML encryption/decryption |

---

## 2. MS-ODRAWXML Namespace Handling in AOO Source

### 2.1 Direct Namespace URI References

**Finding: ZERO direct references to any MS-ODRAWXML namespace URI** exist in the AOO source code.

Searched across:
- `main/oox/` — no matches for `schemas.microsoft.com/office/drawing`
- `main/writerfilter/` — no matches
- `main/sw/` — no matches
- `main/sc/` — no matches
- `main/sd/` — no matches
- `main/drawinglayer/` — no matches
- `main/svx/` — no matches

**Implication:** None of the 43 MS-ODRAWXML extension namespaces are explicitly recognized or handled by AOO's DrawingML import/export code.

### 2.2 extLst (Extension List) Handling

`extLst` (`CT_OfficeArtExtensionList` per ISO/IEC 29500-1:2016 section A.4.1) is recognized and **skipped** (not interpreted) in the following DrawingML import contexts:

| File | Line | Context | Handling |
|------|------|---------|----------|
| `main/oox/source/drawingml/themefragmenthandler.cxx` | 71 | `A_TOKEN(ext)` — CT_OfficeArtExtension in theme | Returns 0 (skip, no context created) |
| `main/oox/source/drawingml/transform2dcontext.cxx` | 59 | `A_TOKEN(ext)` — horz/vert size extension | Case label, no action shown |
| `main/oox/source/drawingml/hyperlinkcontext.cxx` | 158 | `A_TOKEN(extLst)` | Case label, no action shown |
| `main/oox/source/drawingml/table/tablerowcontext.cxx` | 61 | `A_TOKEN(extLst)` — CT_OfficeArtExtensionList | Case label, no action shown |
| `main/oox/source/drawingml/table/tablestylecontext.cxx` | 100 | `A_TOKEN(extLst)` | Case label, no action shown |
| `main/oox/source/drawingml/table/tablestyletextstylecontext.cxx` | 94 | `A_TOKEN(extLst)` | Case label, no action shown |
| `main/oox/source/drawingml/table/tablecellcontext.cxx` | 105 | `A_TOKEN(extLst)` | Case label, no action shown |
| `main/oox/source/drawingml/diagram/datamodelcontext.cxx` | 68, 138, 154 | `DGM_TOKEN(extLst)` | Case label, no action shown |
| `main/oox/source/drawingml/diagram/layoutnodecontext.cxx` | 279 | `DGM_TOKEN(extLst)` | Case label, no action shown |

**Pattern:** `extLst` is recognized as a known token but its **contents are not parsed or interpreted**. The parser skips over extension list elements without extracting any extension data.

### 2.3 XML_extLst in XLSX Export (Native sc Path)

The native XLSX XML exporter has explicit OOXTODO markers for `XML_extLst` (extension list not serialized in XML output):

| File | Line | Comment |
|------|------|---------|
| `main/sc/source/filter/excel/xestyle.cxx` | 2138 | `// OOXTODO: XML_extLst` |
| `main/sc/source/filter/excel/xestyle.cxx` | 2282 | `// OOXTODO: XML_extLst` |
| `main/sc/source/filter/excel/xecontent.cxx` | 867 | `// OOXTODO: XML_extLst` |
| `main/sc/source/filter/excel/xeview.cxx` | 465 | `// OOXTODO: XML_extLst` |
| `main/sc/source/filter/excel/xetable.cxx` | 2089 | `// OOXTODO: XML_extLst` |
| `main/sc/source/filter/excel/excrecds.cxx` | 825 | `// XML_extLst, XML_filters, XML_iconFilter, XML_top10` |
| `main/sc/source/filter/excel/excrecds.cxx` | 999 | `// OOXTODO: XML_extLst, XML_sortState` |

**Implication:** The XLSX XML exporter does not serialize extension lists. Any extension data in the source document is lost on export.

---

## 3. DrawingML Import Paths (Reader Side)

### 3.1 DOCX DrawingML Import

```
WordprocessingML document.xml
  └─ w:drawing / w:pict
       └─ DomainMapper (writerfilter/source/ooxml/DomainMapper.cxx)
            └─ PropertyMap-based property mapping
                 └─ GraphicImport.cxx — graphic import
                 └─ OLEHandler.cxx — OLE object import
```

**Files:**
- `main/writerfilter/source/ooxml/DomainMapper.cxx` — Main DOCX import entry point
- `main/writerfilter/source/ooxml/DomainMapper_Impl.cxx` — Implementation
- `main/writerfilter/source/ooxml/GraphicImport.cxx` — Graphic/DrawingML import
- `main/writerfilter/source/ooxml/OLEHandler.cxx` — OLE object import
- `main/writerfilter/source/ooxml/PropertyMap.cxx` — Property mapping table
- `main/writerfilter/source/dmapper/DomainMapperTableHandler.cxx` — Table handling
- `main/writerfilter/source/dmapper/StyleSheetTable.cxx` — Style handling

**MS-ODRAWXML extension support:** None. No extension namespaces are registered. `extLst` elements are not explicitly handled in DomainMapper (no grep matches for extLst in writerfilter/source/ooxml/).

### 3.2 XLSX DrawingML Import

```
XLSX package → xl/worksheets/sheetN.xml, xl/drawings/drawingN.xml
  └─ ExcelFilter::importDocument() (oox/source/xls/excelfilter.cxx:112)
       └─ WorkbookFragment → WorksheetFragment
            └─ DrawingManager (oox/source/xls/drawingmanager.cxx)
                 └─ DrawingML import via oox/source/drawingml/
```

**Files:**
- `main/oox/source/xls/excelfilter.cxx` — XLSX import entry point (exportDocument returns false)
- `main/oox/source/xls/worksheetfragment.cxx` — Worksheet XML fragment handler
- `main/oox/source/xls/drawingmanager.cxx` — Drawing import manager
- `main/oox/source/drawingml/` — DrawingML element parsers (shape, fill, line, text, etc.)

**MS-ODRAWXML extension support:** None. `extLst` is recognized but skipped. No extension namespace handlers exist.

### 3.3 PPTX DrawingML Import

```
PPTX package → ppt/slides/slideN.xml
  └─ PowerPointImport::importDocument() (oox/source/ppt/pptimport.cxx:67)
       └─ PresentationFragmentHandler → SlideFragmentHandler
            └─ DrawingML import via oox/source/drawingml/
```

**Files:**
- `main/oox/source/ppt/pptimport.cxx` — PPTX import entry point
- `main/oox/source/ppt/presentationfragmenthandler.cxx` — Presentation fragment handler
- `main/oox/source/ppt/slidefragmenthandler.cxx` — Slide fragment handler
- `main/oox/source/drawingml/` — DrawingML element parsers

**MS-ODRAWXML extension support:** None. No extension namespace handlers exist.

---

## 4. DrawingML Export Paths (Writer Side)

### 4.1 DOCX DrawingML Export

```
SwDoc → DocxExportFilter::exportDocument() (sw/source/filter/ww8/docxexportfilter.cxx:46)
       └─ DocxExport::ExportDocument_Impl() (docxexport.cxx:280)
            └─ WriteMainText() (docxexport.cxx:602)
                 └─ MSWordExportBase::WriteText() (wrtw8nds.cxx)
                      └─ DocxAttributeOutput paragraph/run/table callbacks
                           └─ DrawingML (oox/source/export/drawingml.cxx) — for images
                           └─ VMLExport (oox/source/export/vmlexport.cxx) — for shapes
```

**Files:**
- `main/sw/source/filter/ww8/docxexportfilter.cxx` — UNO ExportFilter entry point
- `main/sw/source/filter/ww8/docxexport.cxx` — DOCX export engine
- `main/sw/source/filter/ww8/docxexport.hxx` — Header
- `main/sw/source/filter/ww8/docxattributeoutput.cxx` — Attribute/element serialization
- `main/sw/source/filter/ww8/docxattributeoutput.hxx` — Header
- `main/oox/source/export/drawingml.cxx` — DrawingML export writer
- `main/oox/source/export/shapes.cxx` — Shape export
- `main/oox/source/export/vmlexport.cxx` — VML export (legacy fallback)

**MS-ODRAWXML extension support:** None. No extension serialization. `XML_extLst` TODOs exist in the XLSX exporter (sc path), but DOCX exporter (DocxAttributeOutput) has no extension list handling at all.

### 4.2 XLSX DrawingML Export (Native sc Path)

```
ScDocShell::ConvertTo() → ScExportExcel5() (excel.cxx:284)
       └─ lcl_ExportExcel2007Xml() (excel.cxx:269)
            └─ ExportXml2007::Write() (expop2.cxx:165)
                 └─ ExcDocument::ReadDoc() → WriteXml() (excdoc.cxx)
                      └─ Per-table serializers (xetable.cxx, xestyle.cxx, xecontent.cxx, etc.)
```

**Key serialization files:**
- `main/sc/source/filter/excel/excdoc.cxx` — Workbook and table XML
- `main/sc/source/filter/excel/xetable.cxx` — Cell table serialization
- `main/sc/source/filter/excel/xestyle.cxx` — Styles serialization (XML_extLst TODO at lines 2138, 2282)
- `main/sc/source/filter/excel/xecontent.cxx` — Shared strings, hyperlinks, conditional formatting (XML_extLst TODO at line 867)
- `main/sc/source/filter/excel/xeview.cxx` — Tab view settings (XML_extLst TODO at line 465)
- `main/sc/source/filter/excel/xepage.cxx` — Page setup
- `main/sc/source/filter/excel/xepivot.cxx` — Pivot tables
- `main/sc/source/filter/excel/xeescher.cxx` — Escher/DFF (commented out in XML path)
- `main/sc/source/filter/excel/xename.cxx` — Named ranges
- `main/sc/source/filter/excel/xestream.cxx` — Multi-stream package management

**MS-ODRAWXML extension support:** None. Extension lists are explicitly TODO'd (not serialized). No extension namespace elements are written.

### 4.3 PPTX DrawingML Export

**No PPTX export path exists.** The `sd/source/filter/eppt/` code handles legacy binary PPT only. There is no PresentationML export code anywhere in the codebase.

---

## 5. Drawing Layer Model (main/drawinglayer/)

| Directory | Purpose | Relevance to MS-ODRAWXML |
|-----------|---------|--------------------------|
| `drawinglayer/primitive2d/` | 2D rendering primitives | Represents drawn content internally |
| `drawinglayer/primitive3d/` | 3D rendering primitives | Could support 3D model extensions |
| `drawinglayer/processor/` | Processing pipelines | Converts DrawingML to internal model |
| `drawinglayer/geometry/` | Geometry types | Shape geometry representation |
| `drawinglayer/attribute/` | Attribute handling | Shape attribute storage |

**Relevance:** The drawing layer provides an internal model for 2D/3D content but has no knowledge of MS-ODRAWXML extension namespaces. No extension-specific primitives or attributes exist.

---

## 6. Token and Namespace Definitions

### 6.1 Fast Token Generation

| File | Purpose |
|------|---------|
| `main/oox/source/token/` | Generated fast token headers |
| `main/oox/inc/oox/` | Token header declarations |

**Finding:** The token system includes standard DrawingML tokens (a: namespace) but no MS-ODRAWXML extension namespace tokens (e.g., no `http://schemas.microsoft.com/office/drawing/2017/model3d` tokens).

### 6.2 DrawingML Namespace Handling in oox/core

| File | Role |
|------|------|
| `main/oox/source/core/xmlfilterbase.cxx` | Base filter class with relation management |
| `main/oox/source/core/fasttokenhandler.cxx` | Token-to-QName mapping |
| `main/oox/source/core/fragmenthandler.cxx` | Base fragment handler |
| `main/oox/inc/oox/core/relations.hxx` | Relationship type macros (CREATE_OFFICEDOC_RELATION_TYPE, etc.) |

**Relationship macros** (all hardcoded to Transitional 2006 URIs):
- `CREATE_OFFICEDOC_RELATION_TYPE` — `.../officeDocument/2006/relationships/`
- `CREATE_PACKAGE_RELATION_TYPE` — `.../package/2006/relationships/`
- `CREATE_MSOFFICE_RELATION_TYPE` — `http://schemas.microsoft.com/office/2006/relationships/`

No MS-ODRAWXML-specific relationship types are defined.

---

## 7. Summary of Extension Handling Gaps

| Aspect | Status | Evidence |
|--------|--------|----------|
| MS-ODRAWXML namespace recognition | MISSING | Zero namespace URI references in source |
| mc:AlternateContent handling | MISSING | Zero references (only UCB/ODMA unrelated usage) |
| mc:Ignorable handling | MISSING | No references found |
| extLst recognition | PARTIAL | Recognized and skipped in 9 locations, contents not interpreted |
| extLst serialization (XLSX export) | MISSING | 7 OOXTODO markers for XML_extLst |
| Extension-specific shape properties | MISSING | No extension property handling |
| 3D model support | MISSING | No model3d namespace handling |
| Ink extension support | MISSING | No ink namespace handling |
| Diagram extension support | MISSING | No diagram extension namespace handling |
| Chart extension support | MISSING | No chart extension namespace handling |
| Picture effect extensions | MISSING | No picture effect extension handling |
| OEmbed support | MISSING | No oEmbed namespace handling |
| Live feed support | MISSING | No livefeed namespace handling |
| Classification shape support | MISSING | No classification namespace handling |
| Script link support | MISSING | No scriptlink namespace handling |
| Image formula support | MISSING | No imageformula namespace handling |
| Sketchy shapes support | MISSING | No sketchyshapes namespace handling |
| Animation support | MISSING | No animation namespace handling |
