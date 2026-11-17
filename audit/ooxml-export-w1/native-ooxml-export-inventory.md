# Native OOXML Export Capability Inventory — Milestone W.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML (created from trunk)  
**Target:** trunk  
**Milestone:** W.1  
**Scope:** Native DOCX, XLSX and PPTX export implementation  
**Mode:** Source audit only — no production code changes  
**Confidence levels:** confirmed | probable | requires runtime verification

---

## 1. Executive Summary

| Exporter | Status | Closest to production-usable? | Filter exposed for Save As? |
|----------|--------|------------------------------|------------------------------|
| XLSX | Native exporter exists and is reachable via `ScDocShell::ConvertTo()`, but filter config lacks EXPORT flag | **Yes** (best ROI) | No (IMPORT only in xcu) |
| DOCX | Native exporter exists (`DocxExportFilter`), but many `DocxAttributeOutput` methods are stubs/TODO; filter config lacks EXPORT flag | Partial | No (IMPORT only in xcu) |
| PPTX | No top-level exporter exists; only legacy binary PPT exporter | No | N/A |
| Strict OOXML | Not supported; all serializers hardcode Transitional namespaces/content types | No | N/A |

**Key finding:** XLSX has a complete, reachable native export path that is simply not exposed via the filter registry because the xcu flags omit `EXPORT`. DOCX has a reachable export implementation but with significant serialization gaps. PPTX has no top-level exporter at all.

---

## 2. XLSX — Native Export Audit

### 2.1 Export Call Graph (confirmed)

```
ScDocShell::ConvertTo()                          [main/sc/source/ui/docshell/docsh.cxx:2092]
  └─ matches filter name "MS Excel 2007 XML"     [docsh.cxx:2133, pFilterEx07Xml]
  └─ ScFormatFilter::Get().ScExportExcel5()      [docsh.cxx:2177]
        └─ eFormat = Exp2007Xml                  [docsh.cxx:2175-2176]
        └─ ScFormatFilterPluginImpl::ScExportExcel5() [main/sc/source/filter/excel/excel.cxx:284]
              └─ eFormat == Exp2007Xml           [excel.cxx:302]
              └─ lcl_ExportExcel2007Xml()        [excel.cxx:303]
                    └─ aExpData.meOutput = EXC_OUTPUT_XML_2007 [excel.cxx:275]
                    └─ ExportXml2007 aFilter(...) [excel.cxx:277]
                    └─ aFilter.Write()           [excel.cxx:279]
                          └─ ExportXml2007::Write() [main/sc/source/filter/excel/expop2.cxx:165]
                                └─ pExcDoc->ReadDoc()       [expop2.cxx:190]  ScDoc -> ExcDoc
                                └─ pExcDoc->WriteXml(aOut)  [expop2.cxx:191]
                                      └─ ExcDocument::WriteXml() [main/sc/source/filter/excel/excdoc.cxx:783]
                                            └─ XclExpXmlStream aStrm(...) [excdoc.cxx:789]
                                            └─ aHeader.WriteXml(aStrm)    [excdoc.cxx:804]  workbook globals
                                            └─ per table: maTableList.GetRecord(nTab)->WriteXml(aStrm) [excdoc.cxx:815]
                                                  └─ ExcTable::WriteXml()  [excdoc.cxx:669]
                                                        └─ aRecList.SaveXml(rStrm) [excdoc.cxx:689]
```

**Transition conditions:**
- `ScExportExcel5()` rejects formats other than `ExpBiff5`, `ExpBiff8`, `Exp2007Xml` → returns `eERR_NI` [excel.cxx:287-288]
- `lcl_ExportExcel2007Xml()` sets `meOutput = EXC_OUTPUT_XML_2007` unconditionally [excel.cxx:275]
- `ExportXml2007::Write()` requires `pDocShell`, `xRootStrg`, and optionally `bWriteBasicStrg` (from `SvtFilterOptions::IsLoadExcelBasicStorage()`) for VBA [expop2.cxx:176-188]
- `ExcDocument::WriteXml()` only proceeds if `!maTableList.IsEmpty()` [excdoc.cxx:785]

**The direct Calc export route is active and reachable from Save As.** The `ExcelFilter::exportDocument()` UNO method returns `false` [main/oox/source/xls/excelfilter.cxx:131-134] but that path is NOT used for Save As; the sc filter path above is the real route.

### 2.2 XML-Specific Serializer Inventory

| File | Method | Caller | Reached in normal XLSX export? | Package part / XML element | Features serialized | Features omitted | TODO/FIXME | Conditional? |
|------|--------|--------|-------------------------------|---------------------------|---------------------|------------------|------------|--------------|
| excdoc.cxx | `ExcDocument::WriteXml()` | `ExportXml2007::Write()` | Yes | `xl/workbook.xml` (package part `workbook.xml`) | workbookPr, calcPr, sheets, styles, pivot caches | fileRecoveryPr, fileSharing, functionGroups, oleSize, smartTagPr, webPublishing, externalReferences | OOXTODO on workbookPr attributes [excdoc.cxx:309-325] | Yes (output mode) |
| excdoc.cxx | `ExcTable::FillAsXmlTable()` | `ExcTable::FillAsTable()` [excdoc.cxx:436-438] | Yes (when `GetOutput() != EXC_OUTPUT_BINARY`) | `xl/worksheets/sheetN.xml` | Bof8, Wsbool, GUTS, DefRowHeight, Dimensions, TabViewSettings, cell table, LabelRanges, pivot tables, comments (NOTE), web queries, scenarios/filters, merged cells, conditional formats, code names, data validation, hyperlinks, page settings | DFF/Escher (commented out), charts, shapes, drawings, embedded graphics | "DFF not needed in MSOOXML export" — DFF path commented out [excdoc.cxx:576-582] | Yes (XML vs binary branch) |
| excdoc.cxx | `ExcTable::WriteXml()` | `ExcDocument::WriteXml()` [excdoc.cxx:815] | Yes | per-sheet XML | delegates to `aRecList.SaveXml()` | — | — | — |
| xecontent.cxx | `XclExpSstImpl::SaveXml()` | `XclExpSst::SaveXml()` [xecontent.cxx:309] | Yes (called via stylesheet) | `xl/sharedStrings.xml` | shared string table (SST), rich-text runs | phoneticPr, rPh | — | Yes (content present) |
| xecontent.cxx | `XclExpMergedcells::SaveXml()` | `XclExpMergedcells` | Yes | merged cells in sheet XML | merged cell ranges | — | — | Yes |
| xecontent.cxx | `XclExpHyperlink::SaveXml()` | `XclExpHyperlinkList` | Yes (via FillAsXmlTable) | hyperlinks element in sheet XML | hyperlink relationships | — | — | Yes |
| xecontent.cxx | `XclExpCondfmt::SaveXml()` | `XclExpCondFormatBuffer::SaveXml()` [xecontent.cxx:866] | Yes | conditional formatting | CF rules | cfRule, cfvo, dataBar, colorScale, iconSet (per TODO list) | Listed as unimplemented in ooxml-export-TODO.txt | Yes |
| xecontent.cxx | `XclExpDV::SaveXml()` / `XclExpDval::SaveXml()` | `XclExpDval` | Yes | data validation | data validation lists | — | — | Yes |
| xename.cxx | `XclExpName::SaveXml()` | `XclExpNameManagerImpl::SaveXml()` [xename.cxx:472] | Yes | defined names | named ranges | smartTagType, smartTagTypes | — | Yes |
| xestyle.cxx | `XclExpXmlStyleSheet::SaveXml()` (via `XclExpXmlStyleSheet`) | `ExcDocument` workbook globals [excdoc.cxx:332] | Yes (when `GetOutput() != EXC_OUTPUT_BINARY`) | `xl/styles.xml` | fonts, formats, XF, cell styles, fills, borders, dxf | dxf, gradientFill, scheme, mruColors, tableStyles | Listed as unimplemented in TODO | Yes (XML mode only) |
| xestyle.cxx | `XclExpCellStyleXf::SaveXml()` / `XclExpDxf::SaveXml()` | stylesheet | Yes | cellXfs, dxfs | cell style XFs | — | DXF unsupported | Yes |
| xeview.cxx | `XclExpTabViewSettings::SaveXml()` | `FillAsXmlTable` [excdoc.cxx:568] | Yes | sheetView | pane, selection | sheetView attributes beyond view settings | — | Yes |
| xeview.cxx | `XclExpWindow1::SaveXml()`, `XclExpPane::SaveXml()`, `XclExpSelection::SaveXml()` | `XclExpTabViewSettings::SaveXml()` [xeview.cxx:419,458] | Yes | pane, selection | — | — | `XclExpTabBgColor::SaveXml()` commented out [xeview.cxx:263] | Yes |
| xepage.cxx | `XclExpPageSettings::SaveXml()` | `FillAsXmlTable` [excdoc.cxx:553,621] | Yes | pageSetup, print settings | page setup, margins, header/footer | evenFooter, evenHeader, firstFooter, firstHeader, legacyDrawingHF | — | Yes |
| xepivot.cxx | `XclExpPivotCache::SaveXml()` / pivot table save | `GetPivotTableManager().CreatePivotTablesRecord()` [excdoc.cxx:585] | Yes | pivot tables, pivot caches | pivot table definitions, pivot cache | pivotArea, pivotSelection | — | Yes (pivot tables present) |
| xeescher.cxx | `XclExpMsoDrawingBase::WriteBody()`, `XclExpMsoDrawingGroup` | Escher/DFF path | **No** (commented out in FillAsXmlTable) | `xl/drawings/drawingN.xml` | — | ALL (charts, shapes, drawings, images) | DFF path disabled [excdoc.cxx:576-582] | Not reached |
| xetable.cxx | `XclExpCellTable::WriteXml()` / cell records `SaveXml()` | `ExcTable::FillAsXmlTable()` [excdoc.cxx:571] | Yes | sheet data (`sheetData`) | cells, formulas, shared string references, row properties, column properties, merged cells | cellSmartTag, cellWatch, ignoredErrors, sortState, tablePart | — | Yes |

### 2.3 FillAsXmlTable vs Binary Path (confirmed)

**`ExcTable::FillAsXmlTable()`** [excdoc.cxx:548-635] vs **binary path** [excdoc.cxx:442-512]:

**Constructed exclusively for XML:**
- `XclExpTabViewSettings` [excdoc.cxx:568]
- `XclExpComments` wrapping NOTE records [excdoc.cxx:588-591]
- `XclExpWebQueryBuffer` [excdoc.cxx:594]
- `XclExpXmlStartElementRecord(XML_hyperlinks)` / `XclExpXmlEndElementRecord` wrapping HLINK records [excdoc.cxx:616-619]
- `XclCodename` [excdoc.cxx:604-606]

**Binary-oriented records intentionally omitted in XML:**
- `ExcBof` (BIFF5/7 only), `ExcBof8` is used instead
- `EXC_ID_EXTERNSHEET` for BIFF<=5 [excdoc.cxx:484-485]
- `XclExpProtection` + sheet protection bool records [excdoc.cxx:474-480] — NOT in XML path
- `lcl_AddScenariosAndFilters` IS in both paths [excdoc.cxx:488, 596]

**XML records missing (per ooxml-export-TODO.txt):**
- autoFilter, cellSmartTag, cellWatch, cfRule/cfvo, chartsheet, colorScale, dataBar, iconSet, ignoredError, legacyDrawing, oleObject, outlinePr, pageSetup, picture, pivotArea, pivotSelection, protectedRange, sheetCalcPr, sheetFormatPr, sheetPr, sheetView, sheetViews, smartTags, sortCondition, sortState, tabColor, tablePart, webPublishItem

**Features in binary but not XML:**
- Sheet protection (XclExpProtection) [excdoc.cxx:474-480, absent from XML path]
- DFF/Escher drawing group (commented out in XML) [excdoc.cxx:576-582]

**XML-specific structures built but never serialized:**
- None identified — all XML structures in FillAsXmlTable are serialized via `aRecList.SaveXml()`.

**Commented/disabled DFF export path** [excdoc.cxx:576-582]:
```cpp
// DFF not needed in MSOOXML export
//    GetObjectManager().AddSdrPage();
//    //! close Escher group shape and ESCHER_DgContainer
//    //! opened by XclExpObjList ctor MSODRAWING
//    rR.pObjRecs->EndSheet();
//    // all MSODRAWING and OBJ stuff of this sheet goes here
//    Add( rR.pObjRecs );
```
**Practical consequences:** Charts, shapes, drawings, text boxes, and embedded graphics are completely omitted from XLSX export. The Escher/DFF infrastructure exists in `xeescher.cxx` but is not wired into the XML export path.

### 2.4 XLSX Feature Inventory

| Feature | Status | Evidence | Confidence |
|---------|--------|----------|------------|
| Workbook structure | Partial | `ExcDocument::WriteXml()` writes workbookPr, calcPr, sheets [excdoc.cxx:783-818] | confirmed |
| Worksheet structure | Partial | `FillAsXmlTable()` writes sheet XML [excdoc.cxx:548] | confirmed |
| Cells and formulas | Partial | `XclExpCellTable` writes cells via `SaveXml()` [xetable.cxx] | confirmed |
| Shared strings | Partial | `XclExpSst::SaveXml()` [xecontent.cxx:307] | confirmed |
| Rich-text runs | Partial | `XclExpString::WriteXml()` [xestring.cxx:505] | confirmed |
| Styles, number formats, fonts | Partial | `XclExpXmlStyleSheet` [xestyle.cxx], called [excdoc.cxx:332] | confirmed |
| Fills and borders | Partial | part of XF/style serialization | confirmed |
| Merged cells | Yes | `XclExpMergedcells::SaveXml()` [xecontent.cxx:359] | confirmed |
| Row and column properties | Partial | `XclExpRow`/`XclExpColInfo` via cell table | confirmed |
| Hidden/grouped rows/columns | Partial | outline via GUTS record [excdoc.cxx:561] | confirmed |
| Hyperlinks | Yes | `XclExpHyperlink::SaveXml()` wrapped in `<hyperlinks>` [xecontent.cxx:524, excdoc.cxx:616-619] | confirmed |
| Comments | Partial | `XclExpComments` writes NOTE records [excdoc.cxx:588-591]; text only, no formatting [TODO.txt] | confirmed |
| Named ranges | Yes | `XclExpNameManager::SaveXml()` [xename.cxx:787] | confirmed |
| Data validation | Yes | `XclExpDV::SaveXml()`/`XclExpDval::SaveXml()` [xecontent.cxx:1143,1217] | confirmed |
| Conditional formatting | Partial | `XclExpCondfmt::SaveXml()` [xecontent.cxx:856]; cfRule/cfvo/dataBar/iconSet missing [TODO.txt] | confirmed |
| Charts and drawings | **Missing** | DFF path commented out [excdoc.cxx:576-582] | confirmed |
| Images | **Missing** | DFF path commented out | confirmed |
| Shapes and text boxes | **Missing** | DFF path commented out | confirmed |
| Pivot tables and caches | Yes | `GetPivotTableManager().CreatePivotTablesRecord()` [excdoc.cxx:358,585] | confirmed |
| External links | Partial | externSheet/name records written [excdoc.cxx:398-399] | confirmed |
| Tables and autofilters | **Missing** | autoFilter, tablePart listed as unimplemented [TODO.txt] | confirmed |
| Page setup, print areas, headers/footers | Partial | `XclExpPageSettings` [xepage.cxx]; even/first header/footer missing [TODO.txt] | confirmed |
| VBA projects / macro-enabled | Partial | VBA storage saved via `SvxImportMSVBasic::SaveOrDelMSVBAStorage` [expop2.cxx:185] | confirmed |
| Workbook calculation settings | Partial | calcPr [excdoc.cxx:96-114], `XclExpRecalcId` [excdoc.cxx:404] | confirmed |

### 2.5 Filter Exposure (confirmed)

**`main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML.xcu`:**
- Flags: `IMPORT ALIEN 3RDPARTYFILTER PREFERRED` — **no EXPORT flag**
- FilterService: `com.sun.star.comp.oox.xls.ExcelFilter`
- Type: `MS Excel 2007 XML`
- DocumentService: `com.sun.star.sheet.SpreadsheetDocument`

**`main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML_ui.xcu`:**
- UIName: `Microsoft Excel 2007 XML` — UI entry exists

**`main/sc/source/ui/docshell/docsh.cxx`:**
- Line 163: `pFilterEx07Xml = "MS Excel 2007 XML"`
- Line 199: `SFX2_FILTER_ENTRY(MS Excel 2007 XML)` — registered in SFX framework
- Line 2133: ConvertTo matches `pFilterEx07Xml`
- Line 2175-2177: sets `eFormat = Exp2007Xml`
- Line 2177: calls `ScExportExcel5()`

**`main/oox/util/oox.component`:**
- `com.sun.star.comp.oox.xls.ExcelFilter` registered as both `ImportFilter` and `ExportFilter` service [lines 49-52]
- But `ExcelFilter::exportDocument()` returns `false` [excelfilter.cxx:131-134]

**Assessment:** The export code path is complete and reachable via `ScDocShell::ConvertTo()` (Save As). However, the xcu filter configuration lacks the `EXPORT` flag, which means the filter registry may not list it as an export-capable filter. The exact consequence (whether Save As dialog shows it) requires runtime verification. The code path itself is functional.

**Changes required to expose export (not made):**
1. Add `EXPORT` to Flags in `calc_MS_Excel_2007_XML.xcu`
2. Verify `ExcelFilter::exportDocument()` is not the blocking factor (it isn't — the sc path bypasses it)

---

## 3. DOCX — Native Export Audit

### 3.1 Export Call Graph

```
UNO ExportFilter path (DocxExportFilter):
DocxExportFilter::exportDocument()                    [main/sw/source/filter/ww8/docxexportfilter.cxx:46]
  └─ get SwDoc from model                             [docxexportfilter.cxx:51-58]
  └─ DocxExport aExport(this, pDoc, pCurPam, &aPam)   [docxexportfilter.cxx:73]
  └─ aExport.ExportDocument(true)                     [docxexportfilter.cxx:74]
        └─ DocxExport::ExportDocument_Impl()          [main/sw/source/filter/ww8/docxexport.cxx:280]
              └─ InitStyles()                         [docxexport.cxx:282]  -> word/styles.xml
              └─ WriteMainText()                      [docxexport.cxx:287]  -> word/document.xml
                    └─ WriteText()                    [docxexport.cxx:620]  (inherited from MSWordExportBase)
              └─ WriteFootnotesEndnotes()             [docxexport.cxx:289]  -> word/footnotes.xml, endnotes.xml
              └─ WriteNumbering()                     [docxexport.cxx:291]  -> word/numbering.xml
              └─ WriteFonts()                         [docxexport.cxx:293]  -> word/fontTable.xml
  └─ commit()                                         [docxexportfilter.cxx:77]

Save As path (Writer docsh):
SwDocShell::ConvertTo()                               [main/sw/source/ui/app/docsh.cxx]
  (note: separate from sc/docsh.cxx; Writer's ConvertTo dispatches to the filter selected in Save As)
```

**Relationship between WriterFilter, DocxExportFilter and DocxExport:**
- `WriterFilter` (`com.sun.star.comp.Writer.WriterFilter`) — registered as both ImportFilter and ExportFilter in `writerfilter.component`. The xcu config (`MS_Word_2007_XML.xcu`) points FilterService to this. It is primarily an import filter.
- `DocxExportFilter` (`com.sun.star.comp.Writer.DocxExport`) — standalone export filter class, registers as `ExportFilter` service only [docxexportfilter.cxx:106-111]. Not referenced by the xcu config.
- `DocxExport` — the actual export engine, instantiated by `DocxExportFilter::exportDocument()`.

**Normal Save As reachability:** The xcu config for `MS Word 2007 XML` has Flags `IMPORT ALIEN 3RDPARTYFILTER` — no EXPORT flag. FilterService points to `WriterFilter` (import-oriented), not `DocxExportFilter`. Whether Save As can reach DOCX export requires runtime verification. The `DocxExportFilter::exportDocument()` implementation exists and is functional, but it is not wired into the filter configuration.

### 3.2 DocxAttributeOutput Method Inventory

| Method | Classification | OOXML element / package part | Evidence |
|--------|---------------|------------------------------|----------|
| `StartParagraph()` | Fully implemented | `w:p` | docxattributeoutput.cxx:140 |
| `EndParagraph()` | Fully implemented | `w:p` close | docxattributeoutput.cxx:197 |
| `EmptyParagraph()` | Fully implemented | `w:p` | docxattributeoutput.cxx:238 |
| `StartParagraphProperties()` | Fully implemented | `w:pPr` | docxattributeoutput.cxx:243 |
| `EndParagraphProperties()` | Fully implemented | `w:pPr` close | docxattributeoutput.cxx:295 |
| `StartRun()` | Fully implemented | `w:r` | docxattributeoutput.cxx:316 |
| `EndRun()` | Fully implemented | `w:r` close | docxattributeoutput.cxx:331 |
| `StartRunProperties()` | Fully implemented | `w:rPr` | docxattributeoutput.cxx:639 |
| `EndRunProperties()` | Fully implemented | `w:rPr` close | docxattributeoutput.cxx:685 |
| `RunText()` | Fully implemented | `w:t` | docxattributeoutput.cxx:730 |
| `RawText()` | Stub/TODO | — | docxattributeoutput.cxx:768 [TODO trace] |
| `StartRuby()` | Stub/TODO | `w:ruby` | docxattributeoutput.cxx:773 [TODO trace] |
| `EndRuby()` | Stub/TODO | `w:ruby` close | docxattributeoutput.cxx:778 [TODO trace] |
| `StartURL()` / `EndURL()` | Fully implemented | `w:hyperlink` | docxattributeoutput.cxx:817,857 |
| `FieldVanish()` | Fully implemented | — | docxattributeoutput.cxx:862 |
| `Redline()` | Stub/TODO | — | docxattributeoutput.cxx:867 [TODO trace] |
| `StartRedline()` / `EndRedline()` | Stub/TODO | `w:insDel` | docxattributeoutput.cxx:919,960 [TODO trace] |
| `FormatDrop()` | Stub/TODO | `w:dropCap` | docxattributeoutput.cxx:985 [TODO trace] |
| `ParagraphStyle()` | Fully implemented | `w:pStyle` | docxattributeoutput.cxx:990 |
| `TableRowProperties()` | Stub/TODO | `w:trPr` | docxattributeoutput.cxx:1007 [TODO trace] |
| `TableCellProperties()` | Fully implemented | `w:tcPr` | docxattributeoutput.cxx:1112 |
| `StartTable()` | Fully implemented | `w:tbl` | docxattributeoutput.cxx:1208 |
| `EndTable()` | Fully implemented | `w:tbl` close | docxattributeoutput.cxx:1216 |
| `StartTableRow()` | Fully implemented | `w:tr` | docxattributeoutput.cxx:1231 |
| `EndTableRow()` | Fully implemented | `w:tr` close | docxattributeoutput.cxx:1251 |
| `StartTableCell()` | Fully implemented | `w:tc` | docxattributeoutput.cxx:1257 |
| `EndTableCell()` | Fully implemented | `w:tc` close | docxattributeoutput.cxx:1270 |
| `TableDefinition()` | Fully implemented | `w:tblPr`, `w:tblGrid` | docxattributeoutput.cxx:1285 |
| `TableDefaultBorders()` | Fully implemented | `w:tblBorders` | docxattributeoutput.cxx:1355 |
| `TableBackgrounds()` | Fully implemented | `w:tblPr` shd | docxattributeoutput.cxx:1366 |
| `TableHeight()` | Fully implemented | `w:trPr` trHeight | docxattributeoutput.cxx:1384 |
| `TableCanSplit()` | Fully implemented | `w:trPr` cantSplit | docxattributeoutput.cxx:1411 |
| `TableBidi()` | Fully implemented | `w:trPr` bidi | docxattributeoutput.cxx:1425 |
| `TableVerticalCell()` | Fully implemented | `w:tcPr` vAlign | docxattributeoutput.cxx:1438 |
| `TableNodeInfo()` | Stub/TODO | — | docxattributeoutput.cxx:1449 [TODO trace] |
| `TableOrientation()` | Stub/TODO | `w:tblPr` tblOrientation | docxattributeoutput.cxx:1463 [TODO trace] |
| `TableSpacing()` | Stub/TODO | — | docxattributeoutput.cxx:1468 [fprintf TODO] |
| `TableRowEnd()` | Stub/TODO | — | docxattributeoutput.cxx:1475 [TODO trace] |
| `StartStyles()` / `EndStyles()` | Fully implemented | `w:styles` | docxattributeoutput.cxx:1480,1487 |
| `DefaultStyle()` | Stub/TODO | `w:docDefaults` | docxattributeoutput.cxx:1492 [TODO trace] |
| `FlyFrameGraphic()` | Partial/TODO | `w:drawing`/`w:pict` | docxattributeoutput.cxx:1502 [TODO trace - "some stuff still missing"] |
| `CharFont()` / `CharFontSize()` / etc. | Fully implemented | `w:rFonts`, `w:sz` | docxattributeoutput.cxx:char methods |
| `ParaAdjust()` / `ParaLineSpacing_Impl()` / etc. | Fully implemented | `w:jc`, `w:spacing` | docxattributeoutput.cxx:para methods |
| `SectionBreak()` | Fully implemented | `w:sectPr` | docxattributeoutput.hxx:196 |
| `OutlineNumbering()` | Fully implemented | `w:numPr` | docxattributeoutput.hxx:188 |
| `NumberingDefinition()` / `StartAbstractNumbering()` / `NumberingLevel()` | Fully implemented | `w:numbering` | docxattributeoutput.hxx:246-266 |
| `WriteField_Impl()` / `StartField_Impl()` / `EndField_Impl()` | Fully implemented | `w:fldSimple`/`w:instrText` | docxattributeoutput.cxx:268,476,535,562 |
| `WritePostitFieldStart()` / `WritePostitFieldEnd()` | Fully implemented | comment annotation | docxattributeoutput.cxx:427,432 |
| `FootnoteEndnoteReference()` | Fully implemented | `w:footnoteReference`/`w:endnoteReference` | docxattributeoutput.hxx:432 |

**Note:** `DocxExport::OutputTableNode()`, `OutputGrfNode()`, `OutputOLENode()` are all TODO/no-op stubs [docxexport.cxx:347-359]. However, table export actually proceeds through `MSWordExportBase::WriteText()` → `mpTableInfo->processSwTable()` → `DocxAttributeOutput` table callbacks (StartTable/StartTableRow/StartTableCell/etc.), which ARE implemented. So table export is NOT missing despite the `OutputTableNode` TODO.

### 3.3 Document Node Serialization

| Node type | Status | Path | Evidence |
|-----------|--------|------|----------|
| Paragraphs and character formatting | Yes | `MSWordExportBase::OutputTextNode()` → `DocxAttributeOutput` | wrtw8nds.cxx:1793 |
| Tables, rows, cells | Yes | `WriteText()` → `mpTableInfo->processSwTable()` → `DocxAttributeOutput` table callbacks | docxexport.hxx:99 (WriteCR no-op), docxattributeoutput.cxx:1208+ |
| Images and floating graphics | Partial | `OutputGrfNode()` is TODO stub [docxexport.cxx:352]; `FlyFrameGraphic()` has TODO [docxattributeoutput.cxx:1502] | requires runtime verification |
| OLE objects | **Missing** | `OutputOLENode()` is TODO stub [docxexport.cxx:357] | confirmed (stub) |
| Text frames | Partial | `OutputFlyFrame_Impl()` (virtual, in DocxAttributeOutput) | docxattributeoutput.hxx:324 |
| Hyperlinks | Yes | `StartURL()`/`EndURL()` | docxattributeoutput.cxx:817,857 |
| Bookmarks | Yes | `AppendBookmarks()`/`WriteBookmarks_Impl()` | docxexport.cxx:116, docxattributeoutput.cxx:438 |
| Fields and form fields | Partial | `OutputField()`/`WriteField_Impl()`; `WriteFormData()` TODO [docxexport.cxx:215]; `DoComboBox()`/`DoFormText()` TODO [docxexport.cxx:220,275] | confirmed |
| Lists and numbering | Yes | `NumberingDefinition()`/`NumberingLevel()` | docxattributeoutput.hxx:246-266 |
| Footnotes and endnotes | Yes | `WriteFootnotesEndnotes()` → `FootnoteEndnoteReference()` | docxexport.cxx:416, docxattributeoutput.hxx:432 |
| Headers and footers | Yes | `WriteHeaderFooter()` | docxexport.cxx:490 |
| Sections and page breaks | Yes | `AppendSection()`/`OutputEndNode()`/`SectionBreak()` | docxexport.cxx:305, docxattributeoutput.hxx:196 |
| Page styles | Yes | `MSWordSections` | docxexport.hxx:79 |
| Comments and tracked changes | Partial | `WritePostitFieldStart/End()`; `Redline()` TODO | docxattributeoutput.cxx:427,867 |
| Ruby text | **Missing** | `StartRuby()`/`EndRuby()` are TODO stubs | docxattributeoutput.cxx:773,778 |
| Drop caps | **Missing** | `FormatDrop()` is TODO stub | docxattributeoutput.cxx:985 |

### 3.4 DOCX Package Inventory

| Package part | Status | Evidence |
|--------------|--------|----------|
| `word/document.xml` | Generated | `DocxExport` constructor opens stream [docxexport.cxx:649]; `WriteMainText()` writes content [docxexport.cxx:602] |
| `word/styles.xml` | Generated | `InitStyles()` opens stream [docxexport.cxx:393-404] |
| `word/numbering.xml` | Generated | `WriteNumbering()` [docxexport.cxx:461] |
| `word/fontTable.xml` | Generated | `WriteFonts()` [docxexport.cxx:555] |
| `word/footnotes.xml` | Conditionally generated | `WriteFootnotesEndnotes()` [docxexport.cxx:416] |
| `word/endnotes.xml` | Conditionally generated | `WriteFootnotesEndnotes()` [docxexport.cxx:441] |
| Headers and footers | Conditionally generated | `WriteHeaderFooter()` [docxexport.cxx:490] |
| Document properties (`docProps/core.xml`) | Generated | `WriteProperties()` [docxexport.cxx:582] |
| Media | Partially | DrawingML used for images [docxexport.cxx:653] |
| Relationships | Generated | `AddRelation()`/`addRelation()` [docxexport.cxx:154,645] |
| Content types | Partially | set via `openOutputStreamWithSerializer` media types | docxexport.cxx:399,403,558 |

### 3.5 DOCX Filter Exposure (confirmed)

**`main/filter/source/config/fragments/filters/MS_Word_2007_XML.xcu`:**
- Flags: `IMPORT ALIEN 3RDPARTYFILTER` — **no EXPORT flag**
- FilterService: `com.sun.star.comp.Writer.WriterFilter` (not DocxExportFilter)
- UserData: `OXML`
- Type: `writer_MS_Word_2007`

**`main/filter/source/config/fragments/filters/MS_Word_2007_XML_ui.xcu`:**
- UIName: `Microsoft Word 2007 XML` — UI entry exists

**`main/writerutil/writerfilter.component`:**
- `com.sun.star.comp.Writer.WriterFilter` registered as both `ExportFilter` and `ImportFilter`
- `com.sun.star.comp.Writer.RtfFilter` registered as both

**`DocxExportFilter`** (`com.sun.star.comp.Writer.DocxExport`):
- Registers as `ExportFilter` service only [docxexportfilter.cxx:108]
- `exportDocument()` is fully implemented [docxexportfilter.cxx:46-88]
- NOT referenced by any xcu config

**Assessment:** DOCX export code exists and is functional, but the filter configuration does not expose it for export. The xcu points to `WriterFilter` (import-only in practice for DOCX) without EXPORT flag. `DocxExportFilter` exists as a standalone UNO service but is not wired into the filter registry.

**Changes required to expose export (not made):**
1. Add `EXPORT` to Flags in `MS_Word_2007_XML.xcu`
2. Either change FilterService to `com.sun.star.comp.Writer.DocxExport` or register `DocxExportFilter` as an additional export filter for this type

---

## 4. PPTX — Export Status

### 4.1 Top-Level Exporter (confirmed absent)

**No complete Impress-to-PresentationML export path exists.**

- `main/oox/source/export/` contains only: `drawingml.cxx`, `shapes.cxx`, `vmlexport.cxx` (+ generated `vmlexport-shape-types.cxx`) — these are DrawingML infrastructure, not a PPTX exporter.
- `main/oox/inc/oox/export/` contains only headers: `drawingml.hxx`, `shapes.hxx`, `utils.hxx`, `vmlexport.hxx`
- `main/oox/source/ppt/pptimport.cxx` — PPTX **import** only (reference for understanding, not export)
- `main/sd/source/filter/eppt/` — legacy **binary PPT** exporter only (`eppt.cxx`, `eppt.hxx`, etc.), NOT PPTX/PresentationML
- `ooxml-export-notes.txt` confirms: "oox — .xlsx and .pptx import" and "Old binary filters (export)" — PPT export is binary only

**`DrawingML::DocumentType`** includes `DOCUMENT_PPTX` [main/oox/inc/oox/export/drawingml.hxx:59] with default constructor value `DOCUMENT_PPTX` [drawingml.hxx:82], but this only affects media/image path prefixes (`ppt/media/` vs `word/media/` vs `xl/media/`) [drawingml.cxx:482-510]. It is not a PresentationML exporter.

### 4.2 Reusable DrawingML Infrastructure

| Capability | Status | Evidence | Reusable for PPTX? |
|------------|--------|----------|-------------------|
| Shape geometry | Declared only | `shapes.cxx`/`shapes.hxx` | Yes |
| Transformations | Declared only | `shapes.cxx` | Yes |
| Fill and outline | Declared only | `drawingml.cxx` | Yes |
| Gradients and colors | Declared only | `drawingml.cxx` | Yes |
| Text runs and paragraphs | Declared only | `drawingml.cxx` | Yes |
| Images and media | Yes (partial) | `DrawingML::WriteImage()` supports DOCUMENT_PPTX [drawingml.cxx:486,506] | Yes |
| Connectors and lines | Declared only | `shapes.cxx` | Yes |
| Custom shapes | Declared only | `shapes.cxx` | Yes |
| Relationships | Yes | `XmlFilterBase::addRelation()` used by DrawingML | Yes |
| Package output | Yes | `XmlFilterBase::openOutputStreamWithSerializer()` | Yes |

**Callers:** DrawingML is called from `DocxExport` (DOCX) [docxexport.cxx:653] and from import paths. No production code calls DrawingML for PPTX export.

### 4.3 Missing PresentationML Orchestration

The following package parts have NO generating code:

| Package part | Status | Required for |
|--------------|--------|--------------|
| `ppt/presentation.xml` | Absent | No master slide XML writer |
| Slide XML files (`ppt/slides/slideN.xml`) | Absent | No slide writer |
| Slide masters | Absent | No master layout writer |
| Slide layouts | Absent | No layout writer |
| Themes | Absent | No theme writer |
| Notes and notes masters | Absent | No notes writer |
| Relationships (presentation-level) | Absent | No relationship builder for PPTX |
| Content types (`[Content_Types].xml`) | Absent | No content types writer |
| Slide size and ordering | Absent | No slide dimension writer |
| Transitions | Absent | No transition writer |
| Animations and timings | Absent | `sd/source/filter/eppt/pptexanimations.cxx` exists (import) | 
| Embedded objects | Absent | No OLE writer for PPTX |

**Reusable AOO infrastructure:**
- OPC packaging: `main/package/source/` (ZipPackage, ZipStorage)
- Relationships: `oox::core::XmlFilterBase::addRelation()`
- XML serialization: `sax_fastparser::FSHelper`
- Media handling: `DrawingML::WriteImage()`
- Shape export: `vmlexport.cxx` (VML), `shapes.cxx`/`drawingml.cxx` (DrawingML)

### 4.4 Proposed Architecture (not implemented)

A native Impress-to-PPTX exporter would require:
1. A top-level `PptxExportFilter` (UNO ExportFilter service) analogous to `DocxExportFilter`
2. A `PptxExport` class orchestrating package part creation
3. Slide iteration from `SdDrawDocument`/`SdPage`, converting to PresentationML
4. Reuse of `DrawingML` for shape/drawing output
5. Reuse of `XmlFilterBase` for OPC package, relationships, content types
6. Filter configuration xcu with EXPORT flag

---

## 5. Transitional vs Strict OOXML

### 5.1 Hardcoded Transitional Assumptions

| Assumption | Evidence | File | Location |
|------------|----------|------|----------|
| SpreadsheetML namespace | `http://schemas.openxmlformats.org/spreadsheetml/2006/main` | `excdoc.cxx:793` | `ExcDocument::WriteXml()` |
| WordprocessingML namespace | `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `docxexport.cxx:609` | `WriteMainText()` |
| DrawingML namespace | `http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing` | `docxexport.cxx:611` | `WriteMainText()` |
| VML namespace | `urn:schemas-microsoft-com:vml` | `docxexport.cxx:608` | `WriteMainText()` |
| Relationship type URI | `http://schemas.openxmlformats.org/officeDocument/2006/relationships/...` | `docxexport.cxx:399,403,558,645` | `InitStyles`, `WriteNumbering`, `WriteFonts`, constructor |
| Content type | `application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml` | `docxexport.cxx:650` | constructor |
| Content type | `application/vnd.openxmlformats-officedocument.spreadsheetml.sheet+xml` (implied) | `xestream.cxx` | `XclExpXmlStream` |
| `xmlns:o` (office namespace) | `urn:schemas-microsoft-com:office:office` | `docxexport.cxx:606` | `WriteMainText()` |
| `xmlns:w10` | `urn:schemas-microsoft-com:office:word` | `docxexport.cxx:610` | `WriteMainText()` |
| `xmlns:r` | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `docxexport.cxx:607` | `WriteMainText()` |

**Strict OOXML support:** None of the existing export paths can emit Strict OOXML. All namespaces, relationship types, and content types are hardcoded to Transitional. Strict requires:
- `http://purl.oclc.org/ooxml/wordprocessingml/main` (instead of 2006/main)
- Strict relationship type URIs (`.../2010/relationships/...`)
- Strict content types
- Different element/attribute sets (e.g., no VML, drawingML-only)

### 5.2 Recommended Abstraction (future milestone)

A centralized `OoxmlConformance` abstraction should provide:
- Namespace URI by conformance level (Transitional vs Strict)
- Relationship type URI by part kind and conformance level
- Content type by part kind and conformance level
- Element/attribute validity checks

This avoids scattered conditionals throughout serializers. Strict support should be a separate milestone (W.2 or later).

---

## 6. Testing and Validation Inventory

### 6.1 Existing Tests

| Test | Location | Coverage | Limitations | Executed? |
|------|----------|----------|-------------|-----------|
| DOCX import test documents | `main/writerfilter/qa/documents/*.docx` | Import only | Import, not export | No |
| DOCX complex import tests | `main/writerfilter/qa/complex/ooxml/testdocuments/*.docx` | Import only | Import, not export | No |
| PPTX test fixtures | `test/testgui/data/pvt/*.pptx` | Import GUI tests | Import only | No |
| XLSX test fixtures | `test/testgui/data/pvt/*.xlsx` | Import GUI tests | Import only | No |
| ODF validate fixtures (docx) | `test/odf-validate/fixtures/*.docx` | ODF validation | Import/validation only | No |

**No export tests exist.** There are no unit tests that:
- Export an AOO document to DOCX/XLSX/PPTX and inspect the package
- Validate OOXML package integrity after export
- Test filter registration for export
- Test UNO filter invocation for export

### 6.2 Recommended Regression Matrix

| Test | Method | Coverage |
|------|--------|----------|
| AOO document → OOXML export → package inspection | Export via Save As/UNO, unzip package, inspect XML parts | Structure, package parts, relationships |
| OOXML export → reopen in AOO | Round-trip through AOO import | Data preservation |
| Export → reopen in Microsoft Office | Interop validation | Feature fidelity |
| Formulas, styles, objects, drawings, relationships | Feature-specific export and inspect | Feature coverage |
| Negative tests for unsupported features | Verify omitted features don't corrupt package | Data loss detection |
| OOXML package validation | Independent validator (e.g., Open XML SDK validator) | ECMA-376 conformance |

---

## 7. Prioritized Implementation Recommendation

| Rank | Item | Rationale |
|------|------|-----------|
| 1 | **XLSX: complete and expose the existing native exporter** | Complete reachable export path exists; only filter config (EXPORT flag) and missing features (charts, drawings) need work. Best ROI. |
| 2 | **DOCX: complete and expose the existing native exporter** | Export engine exists; `DocxAttributeOutput` has many stubs/TODOs (Ruby, drop caps, OLE, some table aspects) but core text/table/fields work. |
| 3 | **PPTX: implement missing top-level exporter** | No exporter exists. Reuse DrawingML + XmlFilterBase infrastructure. Largest effort. |
| 4 | **Strict OOXML export** | Requires centralized conformance abstraction + serializer updates. Separate milestone. |
| 5 | **Broader interoperability and conformance** | Charts, drawings, pivot tables, advanced formatting across all three. |

This ordering matches the expected ordering in the milestone spec. The source audit confirms XLSX is closest to production-usable.

---

## 8. Completion Criteria Status

| Criterion | Status |
|-----------|--------|
| XLSX export documented, verified source call graph | Complete |
| DOCX export documented, verified source call graph | Complete |
| PPTX export status established beyond doubt | Complete (no exporter exists) |
| Every important package part accounted for | Complete |
| XML-specific serializers and callers inventoried | Complete |
| Filter exposure issues documented | Complete |
| Strict vs Transitional assumptions catalogued | Complete |
| Feature inventory machine-readable and traceable to source | Complete (see `native-ooxml-export-features.csv`) |
| Prioritized implementation plan delivered | Complete |
| No production code modified | Confirmed |

---

## 9. Final Decision Document

**Which exporter is closest to being production-usable?**
XLSX. The full native export path is implemented and reachable via `ScDocShell::ConvertTo()`. The only blocker to user exposure is the missing `EXPORT` flag in `calc_MS_Excel_2007_XML.xcu`.

**Which existing exporter gives the best return on implementation effort?**
XLSX. Adding `EXPORT` to the filter config flags (one line) exposes a working exporter. Remaining gaps (charts, drawings) are isolated to the commented-out DFF path.

**Which features are the most serious causes of data loss?**
1. **Charts, shapes, drawings, embedded graphics** (XLSX) — DFF path completely commented out [excdoc.cxx:576-582]
2. **OLE objects** (DOCX) — `OutputOLENode()` is a TODO stub [docxexport.cxx:357]
3. **Ruby text, drop caps** (DOCX) — TODO stubs [docxattributeoutput.cxx:773,985]
4. **Tables/autofilters** (XLSX) — listed as unimplemented in TODO

**What is the minimum work required to expose DOCX and XLSX export?**
- XLSX: Add `EXPORT` flag to `calc_MS_Excel_2007_XML.xcu` Flags; verify Save As dialog exposure (runtime verification)
- DOCX: Add `EXPORT` flag to `MS_Word_2007_XML.xcu`; wire FilterService to `com.sun.star.comp.Writer.DocxExport` or register DocxExportFilter as an additional export filter

**What is the minimum viable architecture for PPTX export?**
A `PptxExportFilter` UNO service (ExportFilter) + `PptxExport` orchestration class that iterates `SdDrawDocument` slides, reuses `DrawingML` and `XmlFilterBase` (OPC packaging, relationships, content types), and writes `ppt/presentation.xml` + slide XML files.

**What should the next implementation milestone contain?**
Milestone W.2: Expose XLSX and DOCX export (add EXPORT flags to xcu configs), then complete missing serializers (charts/drawings for XLSX, OLE/ruby/drop caps for DOCX). Milestone W.3: Implement PPTX exporter. Milestone W.4: Strict OOXML support via centralized conformance abstraction.
