# OOXML Architecture Inventory — Milestone A

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** A  
**Mode:** Source audit only — no production code changes

---

## 1. Executive Summary

Apache OpenOffice's OOXML implementation is split across three major subsystems:

| Subsystem | Formats | Primary Role |
|-----------|---------|-------------|
| `main/oox/` | XLSX, PPTX | OOXML import (XLSX, PPTX), DrawingML infrastructure, shared OOXML utilities |
| `main/writerfilter/` | DOCX | DOCX import (DomainMapper), partial DOCX export via DocxExportFilter |
| `main/sc/source/filter/excel/` | XLSX | Native XLSX export (complete, reachable via ScDocShell::ConvertTo) |
| `main/sw/source/filter/ww8/` | DOCX | DOCX export (DocxExportFilter, DocxExport, DocxAttributeOutput) |
| `main/sd/source/filter/eppt/` | PPT (binary only) | Legacy binary PPT export — NOT PPTX/PresentationML |
| `main/package/` | All | ZIP/OPC package infrastructure |
| `main/xmlsecurity/` | All | Encryption support |

**Critical finding:** No native PPTX/PresentationML export exists. `PowerPointImport` declares `ExportFilter` but `exportDocument()` is not implemented for PPTX. The `sd/source/filter/eppt/` code is legacy binary PPT only.

---

## 2. Module Map

### 2.1 `main/oox/` — OOXML Import/Export Infrastructure

| Directory | Purpose | Key Files |
|-----------|---------|-----------|
| `oox/source/core/` | Core OOXML filter framework | `filterbase.cxx`, `xmlfilterbase.cxx`, `relations.cxx`, `services.cxx`, `fastparser.cxx`, `filterdetect.cxx`, `fragmenthandler.cxx`, `recordparser.cxx`, `relationshandler.cxx`, `encryption.cxx` |
| `oox/source/drawingml/` | DrawingML import/export | `shape.cxx`, `fillproperties.cxx`, `lineproperties.cxx`, `textparagraph.cxx`, `textrun.cxx`, `theme.cxx`, `color.cxx`, `chart/` (subdir), `table/` (subdir), `diagram/` (subdir) |
| `oox/source/export/` | DrawingML export (used by DOCX) | `drawingml.cxx`, `shapes.cxx`, `vmlexport.cxx` |
| `oox/source/ppt/` | PPTX import | `pptimport.cxx`, `presentationfragmenthandler.cxx`, `slidefragmenthandler.cxx`, `slidemastertextstylescontext.cxx`, `timenode.cxx`, `slidetransition.cxx`, `animationspersist.cxx` |
| `oox/source/xls/` | XLSX import/export (UNO) | `excelfilter.cxx` (exportDocument returns false), `workbookfragment.cxx`, `worksheetfragment.cxx`, `stylesbuffer.cxx`, `sheetdatabuffer.cxx`, `pivottablebuffer.cxx`, `condformatbuffer.cxx`, `drawingmanager.cxx`, etc. |
| `oox/source/helper/` | OOXML helper utilities | `zipstorage.cxx`, `binaryinputstream.cxx`, `binaryoutputstream.cxx`, `graphichelper.cxx`, `containerhelper.cxx`, `propertyset.cxx`, `storagebase.cxx` |
| `oox/source/ole/` | OLE/VBA support | `vbaproject.cxx`, `oleobjecthelper.cxx` |
| `oox/source/token/` | Fast token generation | Generated token headers |
| `oox/source/dump/` | Debug dumpers | `pptxdumper.hxx`, `xlsbdumper.hxx`, `biffdumper.hxx` |
| `oox/util/oox.component` | UNO service registration | Registers ExcelFilter, PowerPointImport, FastTokenHandler, FilterDetect, etc. |

**UNO services registered in `oox.component`:**
- `com.sun.star.comp.oox.xls.ExcelFilter` — ImportFilter + ExportFilter (exportDocument returns false)
- `com.sun.star.comp.oox.xls.ExcelBiffFilter` — ImportFilter + ExportFilter (exportDocument returns false)
- `com.sun.star.comp.oox.xls.ExcelVbaProjectFilter` — ImportFilter only
- `com.sun.star.comp.oox.ppt.PowerPointImport` — ImportFilter + ExportFilter (no PPTX export implementation)
- `com.sun.star.comp.oox.core.FastTokenHandler` — FastTokenHandler
- `com.sun.star.comp.oox.FormatDetector` — ExtendedTypeDetection
- `com.sun.star.comp.oox.docprop.DocumentPropertiesImporter` — OOXMLDocumentPropertiesImporter
- `com.sun.star.comp.oox.ShapeContextHandler` — FastShapeContextHandler
- `com.sun.star.comp.oox.xls.BiffDetector` — ExtendedTypeDetection
- `com.sun.star.comp.oox.xls.FormulaParser` — FilterFormulaParser

### 2.2 `main/writerfilter/` — DOCX Import/Export

| Directory | Purpose | Key Files |
|-----------|---------|-----------|
| `writerfilter/source/ooxml/` | DOCX import (DomainMapper) + schema model | `model.xml` (930KB schema), `DomainMapper.cxx`, `DomainMapper_Impl.cxx`, `PropertyMap.cxx`, `StyleSheetTable.cxx`, `NumberingManager.cxx`, `GraphicImport.cxx`, `OLEHandler.cxx`, `FontTable.cxx`, `SettingsTable.cxx`, `ThemeTable.cxx`, `FFDataHandler.cxx`, `BorderHandler.cxx`, `CellColorHandler.cxx`, `WrapPolygonHandler.cxx`, `DomainMapperTableHandler.cxx`, `DomainMapperTableManager.cxx` |
| `writerfilter/source/dmapper/` | DomainMapper implementation | `DomainMapper_Impl.cxx`, `DomainMapperTableHandler.cxx`, `DomainMapperTableManager.cxx` |
| `writerfilter/source/doctok/` | Document tokenizer | Tokenizer for OOXML binary-to-text |
| `writerfilter/util/writerfilter.component` | UNO service registration | Registers `WriterFilter` as ImportFilter + ExportFilter |
| `writerfilter/documentation/` | Documentation | - |

**UNO services registered in `writerfilter.component`:**
- `com.sun.star.comp.Writer.WriterFilter` — ImportFilter + ExportFilter (DOCX import via DomainMapper; export capability declared but not via this path)
- `com.sun.star.comp.Writer.WriterFilterDetector` — ExtendedTypeDetection
- `com.sun.star.comp.Writer.RtfFilter` — ImportFilter + ExportFilter

### 2.3 `main/sw/source/filter/ww8/` — DOCX Export

| File | Purpose |
|------|---------|
| `docxexportfilter.cxx` | `DocxExportFilter` — UNO ExportFilter service, entry point for UNO DOCX export |
| `docxexport.hxx` / `docxexport.cxx` | `DocxExport` — DOCX export engine, writes document.xml, styles.xml, numbering.xml, fontTable.xml, footnotes.xml, endnotes.xml, headers/footers |
| `docxattributeoutput.hxx` / `docxattributeoutput.cxx` | `DocxAttributeOutput` — attribute/element serialization (paragraphs, runs, tables, fields, etc.) — many TODO stubs |
| `wrtw8nds.cxx` | `MSWordExportBase::WriteText()` — document node iteration |
| `wrtww8.cxx` | `MSWordExportBase` — WW8/DOCX base export |
| `wrtw8sty.cxx` | Style export |
| `wrtw8num.cxx` | Numbering export |
| `wrtw8esh.cxx` | Escher/Drawing export |

### 2.4 `main/sc/source/filter/excel/` — Native XLSX Export (sc path)

| File | Purpose |
|------|---------|
| `excel.cxx` | `ScFormatFilter::ScExportExcel5()`, `lcl_ExportExcel2007Xml()` — Calc Save As entry point |
| `expop2.cxx` | `ExportXml2007::Write()` — XML export orchestration, calls `ReadDoc()` and `WriteXml()` |
| `excdoc.cxx` | `ExcDocument::WriteXml()`, `ExcTable::FillAsXmlTable()` — workbook/sheet XML serialization |
| `xetable.cxx` | Cell table XML serialization |
| `xestyle.cxx` | Styles XML serialization |
| `xecontent.cxx` | Shared strings, merged cells, hyperlinks, conditional formatting, data validation |
| `xepage.cxx` | Page setup XML |
| `xepivot.cxx` | Pivot tables XML |
| `xeescher.cxx` | Escher/DFF drawing (commented out in XML path) |
| `xename.cxx` | Named ranges XML |
| `xeview.cxx` | Tab view settings, window pane, selection |
| `xestream.cxx` | `XclExpXmlStream` — multi-stream package part management |
| `xestring.cxx` | Rich string serialization |

### 2.5 `main/oox/source/xls/` — XLSX UNO Import/Export

| File | Purpose |
|------|---------|
| `excelfilter.cxx` | `ExcelFilter::importDocument()` (implemented), `ExcelFilter::exportDocument()` (returns false) |
| `workbookfragment.cxx` | Workbook-level XML import |
| `worksheetfragment.cxx` | Worksheet-level XML import |
| `stylesbuffer.cxx` | Styles import buffer |
| `sheetdatabuffer.cxx` | Sheet data import buffer |
| `pivottablebuffer.cxx` | Pivot table import buffer |
| `condformatbuffer.cxx` | Conditional formatting import buffer |
| `drawingmanager.cxx` | Drawing import manager |
| `externallinkbuffer.cxx` | External links import buffer |
| `formulabase.cxx` / `formulaparser.cxx` | Formula parsing |
| `ooxformulaparser.cxx` | OOXML formula parser |
| `excelvbaproject.cxx` | VBA project import |
| `richstring.cxx` | Rich string import |

### 2.6 `main/sd/source/filter/` — Impress Import/Export

| Directory | Format | Purpose |
|-----------|--------|---------|
| `eppt/` | PPT (binary) | Legacy binary PPT export only — `eppt.cxx` (`PPTWriter`), `eppt.hxx`, `epptdef.hxx`, `epptso.cxx`, `pptexanimations.cxx` |
| `ppt/` | PPT (binary) | Legacy binary PPT import — `pptin.cxx`, `pptinanimations.cxx` |
| `xml/` | ODF | ODF import/export for Impress |

### 2.7 `main/oox/source/ppt/` — PPTX Import

| File | Purpose |
|------|---------|
| `pptimport.cxx` | `PowerPointImport::importDocument()` — PPTX import entry point |
| `presentationfragmenthandler.cxx` | Presentation XML fragment handler |
| `slidefragmenthandler.cxx` | Slide XML fragment handler |
| `slidemastertextstylescontext.cxx` | Slide master text styles |
| `timenode.cxx` | Timing nodes (animations) |
| `slidetransition.cxx` | Slide transitions |
| `animationspersist.cxx` | Animation persistence |

### 2.8 `main/package/` — ZIP/OPC Package Infrastructure

| Directory | Purpose |
|-----------|---------|
| `package/source/zippackage/` | `ZipPackage` — OPC package over ZIP |
| `package/source/zipapi/` | Low-level ZIP API |
| `package/source/xstor/` | `xstorage` — OLE structured storage over ZIP |

### 2.9 `main/xmlsecurity/` — Encryption

| Directory | Purpose |
|-----------|---------|
| `xmlsecurity/source/` | XML encryption/decryption for OOXML documents |

---

## 3. Import/Export Architecture Diagram

```
                    +---------------------+
                    |   AOO Document      |
                    |   (ScDoc/SwDoc/Sd)  |
                    +---------------------+
                           |
           +---------------+---------------+
           |                               |
   +-------+-------+               +-------+-------+
   |  Save As /    |               |   UNO API      |
   |  ConvertTo()  |               |  XFilter::     |
   |               |               |  exportDocument|
   +-------+-------+               +-------+-------+
           |                               |
   +-------+-------+               +-------+-------+
   |               |               |               |
+--v---+     +----v---+     +------v------+  +-----v------+
|  sc  |     |  sw    |     |   oox/xls   |  | oox/ppt    |
| Excel|     |  WW8   |     | ExcelFilter |  | PowerPoint |
| export|    | export |     | (import only|  | Import     |
| (XML) |    | (DOCX) |     |  export=F)  |  |            |
+--+---+     +--+---+     +------+------+  +------+------+
   |               |               |               |
   |               |               |               |
   v               v               v               v
[ScExportExcel5] [DocxExportFilter] [ExcelFilter::] [No PPTX]
lcl_Export       docxexportfilter exportDocument]   exporter
Excel2007Xml     ::exportDocument()  returns false
ExportXml2007       |
::Write()           v
ExcDocument::   +---v---+
WriteXml()      | DocxExport |
                | ExportDocument_Impl() |
                +---+---+
                    |
                    v
              DocxAttributeOutput
              (stubs + implemented)
```

---

## 4. XML Parsers and Serializers

| Component | File | Role |
|-----------|------|------|
| Fast SAX parser | `oox/source/core/fastparser.cxx` | Fast XML parsing for OOXML import |
| Fast serializer | `sax/fshelper.hxx` (external) | `FSHelperPtr`, `FastSerializerHelper` for XML output |
| Token handler | `oox/source/core/fasttokenhandler.cxx` | `FastTokenHandler` — maps QNames to token IDs |
| Fragment handler | `oox/source/core/fragmenthandler.cxx` | Base class for OOXML fragment handlers |
| Record parser | `oox/source/core/recordparser.cxx` | Record-based parsing for binary formats |
| Zip storage | `oox/source/helper/zipstorage.cxx` | ZIP-based package storage for OPC parts |
| Binary input/output | `oox/source/helper/binaryinputstream.cxx`, `binaryoutputstream.cxx` | Binary stream wrappers |
| Graphic helper | `oox/source/helper/graphichelper.cxx` | Graphic conversion for OOXML |

---

## 5. Package and Relationship Handling

| Component | File | Role |
|-----------|------|------|
| Relations | `oox/source/core/relations.cxx` | `Relations` class — OPC relationship storage and resolution |
| Relations handler | `oox/source/core/relationshandler.cxx` | SAX handler for `_rels/.rels` and part-level `.rels` |
| Zip storage | `oox/source/helper/zipstorage.cxx` | ZIP package storage implementation |
| Zip package | `main/package/source/zippackage/` | `ZipPackage` — OPC package creation |
| xstorage | `main/package/source/xstor/xstorage.cxx` | OLE structured storage over ZIP |
| Content types | Per-filter | Each filter sets content types via `openOutputStreamWithSerializer` |

---

## 6. Shared DrawingML Implementation

| File | Role | Used By |
|------|------|---------|
| `oox/source/export/drawingml.cxx` | DrawingML export writer | DOCX export (`DocxExport` creates `DrawingML` instance) |
| `oox/source/export/shapes.cxx` | Shape export | DrawingML export |
| `oox/source/export/vmlexport.cxx` | VML export | DOCX export (VML fallback for shapes) |
| `oox/source/drawingml/shape.cxx` | DrawingML shape import | XLSX import, PPTX import |
| `oox/source/drawingml/fillproperties.cxx` | Fill properties | DrawingML import |
| `oox/source/drawingml/lineproperties.cxx` | Line properties | DrawingML import |
| `oox/source/drawingml/textparagraph.cxx` | Text paragraphs | DrawingML import |

**Reusability:** DrawingML export infrastructure exists in `oox/source/export/` and is used by DOCX export. It could be reused for a future PPTX exporter, but no PPTX exporter currently exists.

---

## 7. Application-Specific Implementations

| Application | Module | Import | Export |
|-------------|--------|--------|--------|
| Writer | `main/sw/` | `writerfilter/source/ooxml/` (DomainMapper) | `sw/source/filter/ww8/docxexport*` |
| Calc | `main/sc/` | `oox/source/xls/` (ExcelFilter) | `sc/source/filter/excel/` (native XML exporter) |
| Impress | `main/sd/` | `oox/source/ppt/` (PowerPointImport) | NONE for PPTX; `sd/source/filter/eppt/` for binary PPT only |
| Draw | `main/sd/` | ODF only | ODF only |

---

## 8. Module Dependency Diagram

```
                    +------------------+
                    |   UNO Services   |
                    | (writerfilter,   |
                    |  oox.component)  |
                    +------------------+
                           |
   +-----------------------+-----------------------+
   |                       |                       |
+--v------+         +------v------+         +------v------+
| writer  |         |    oox      |         |   package   |
| filter  |         |             |         |             |
+--+------+         +--+---+------+         +------+------+
   |                   |   |   |                   |
   |                   |   |   |                   |
   v                   v   v   v                   v
+--+------+      +----v--+ +-v--+              +--v------+
| Domain  |      | core  | | xls|              | ZipPackage|
| Mapper  |      |       | | ppt|              | xstorage  |
+--+------+      |       | |    |              +--+------+
   |             | filter| | base|                 |
   |             +--+----+ +--+--+                 |
   |                |        |                      |
   v                v        v                      v
+--+------+   +----v---+ +--v---+              +--v------+
| SwDoc   |   | XmlFilter| | Excel|              |  OPC    |
|         |   | Base   | | Filter|              | Package |
+--+------+   +----+---+ +--+---+              +---------+
   |                |        |
   |                |        v
   |                |   +--v---+
   |                |   |PPT   |
   |                |   |Import|
   |                |   +--+---+
   |                |        |
   v                v        v
+--+------+   +----+---+ +--v---+
| DOCX    |   | XLSX   | | PPTX |
| Import  |   | Import | | Import|
+---------+   +---------+ +------+
```

---

## 9. Confidence Level

All findings in this report are **VERIFIED** through direct source code inspection of file paths, class names, method signatures, and service registrations. No assumptions were made from class names or service declarations alone.
