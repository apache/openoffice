<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# Native OOXML Export Call Graphs — Milestone W.1

**Source:** Source audit only. Every edge traced to actual source file:line.  
**Confidence:** confirmed | probable | requires runtime verification

---

## XLSX — Complete Export Call Graph

### Call Graph 1: Calc Save As → XLSX package

```
ScDocShell::ConvertTo(SfxMedium& rMed)
  File: main/sc/source/ui/docshell/docsh.cxx:2092
  Confidence: confirmed

  Condition: rMed.GetFilter()->GetFilterName() == "MS Excel 2007 XML"
    [docsh.cxx:2109, 2133; pFilterEx07Xml defined at docsh.cxx:163]
  Condition: Filter registered via SFX2_FILTER_ENTRY(MS Excel 2007 XML)
    [docsh.cxx:199]

  ├─ ScExtDocOptions handling, password warning [docsh.cxx:2138-2168]
  ├─ ExportFormatExcel eFormat = Exp2007Xml [docsh.cxx:2175-2176]
  └─ ScFormatFilter::Get().ScExportExcel5(rMed, &aDocument, eFormat, RTL_TEXTENCODING_MS_1252)
       File: docsh.cxx:2177
       Confidence: confirmed

ScFormatFilterPluginImpl::ScExportExcel5(SfxMedium&, ScDocument*, ExportFormatExcel, CharSet)
  File: main/sc/source/filter/excel/excel.cxx:284
  Confidence: confirmed

  Condition: eFormat == Exp2007Xml → lcl_ExportExcel2007Xml()
    [excel.cxx:287-303]
  Condition: eFormat == ExpBiff5/ExpBiff8 → lcl_ExportExcelBiff() [binary path, not audited]

  └─ lcl_ExportExcel2007Xml(SfxMedium&, ScDocument*, SvStream*, CharSet)
       File: excel.cxx:269
       Confidence: confirmed

       Action: aExpData.meOutput = EXC_OUTPUT_XML_2007 [excel.cxx:275]
       Action: ExportXml2007 aFilter(aExpData, *pMedStrm) [excel.cxx:277]
       Action: FltError eRet = aFilter.Write() [excel.cxx:279]

ExportXml2007::Write()
  File: main/sc/source/filter/excel/expop2.cxx:165
  Confidence: confirmed

  Condition: bWriteBasicStrg from SvtFilterOptions::IsLoadExcelBasicStorage()
    [expop2.cxx:176-180] — controls VBA project preservation
  Action: if bWriteBasicStrg → SvxImportMSVBasic::SaveOrDelMSVBAStorage(sal_True)
    [expop2.cxx:184-185] — writes xl/vbaProject.bin
  Action: pExcDoc->ReadDoc() [expop2.cxx:190] — ScDoc → ExcDoc conversion
  Action: pExcDoc->WriteXml(aOut) [expop2.cxx:191] — XML serialization

ExcDocument::WriteXml(SvStream& rStrm)
  File: main/sc/source/filter/excel/excdoc.cxx:783
  Confidence: confirmed

  Condition: if (!maTableList.IsEmpty()) [excdoc.cxx:785]
  Action: XclExpXmlStream aStrm(compContext, rStrm, GetRoot()) [excdoc.cxx:789]
  Action: rWorkbook->startElement(XML_workbook, xmlns=spreadsheetml/2006/main, xmlns:r=officeDocument/2006/relationships)
    [excdoc.cxx:792-795] — Transitional namespace hardcoded
  Action: rWorkbook->singleElement(XML_fileVersion, appName="Calc") [excdoc.cxx:96-97]
    OOXTODO: XML_codeName, XML_lastEdited, XML_lowestEdited, XML_rupBuild [excdoc.cxx:798-801]
  Action: aHeader.WriteXml(aStrm) [excdoc.cxx:804] — workbook globals (Pr, calcPr, sheets, styles)
  Action: per table → maTableList.GetRecord(nTab)->WriteXml(aStrm) [excdoc.cxx:815]
  Action: rWorkbook->endElement(XML_workbook) [excdoc.cxx:818]
  Action: aStrm.commitStorage() [excdoc.cxx:820]

ExcTable::WriteXml(XclExpXmlStream& rStrm)
  File: main/sc/source/filter/excel/excdoc.cxx:669
  Confidence: confirmed

  Action: aRecList.SaveXml(rStrm) [excdoc.cxx:689]

ExcTable::FillAsTable(SCTAB nCodeNameIdx)
  File: main/sc/source/filter/excel/excdoc.cxx:418
  Confidence: confirmed

  Condition: if (GetOutput() != EXC_OUTPUT_BINARY) → FillAsXmlTable(nCodeNameIdx); return;
    [excdoc.cxx:436-439] — XML branch
  Condition: else → binary BIFF path [excdoc.cxx:442-512]

ExcTable::FillAsXmlTable(SCTAB nCodeNameIdx)
  File: main/sc/source/filter/excel/excdoc.cxx:548
  Confidence: confirmed

  Action: XclExpPageSettings [excdoc.cxx:553,621] — page setup
  Action: ExcBof8 [excdoc.cxx:556]
  Action: XclExpWsbool [excdoc.cxx:558]
  Action: mxCellTable->CreateRecord(EXC_ID_GUTS) [excdoc.cxx:561] — outline
  Action: mxCellTable->CreateRecord(EXC_ID2_DEFROWHEIGHT) [excdoc.cxx:563]
  Action: mxCellTable->CreateRecord(EXC_ID3_DIMENSIONS) [excdoc.cxx:565]
  Action: XclExpTabViewSettings [excdoc.cxx:568] — sheet view
  Action: mxCellTable (cell records) [excdoc.cxx:571]
  Action: XclExpLabelranges [excdoc.cxx:574]
  // DFF path commented out [excdoc.cxx:576-582]:
  //   GetObjectManager().AddSdrPage();
  //   rR.pObjRecs->EndSheet();
  //   Add( rR.pObjRecs );
  Action: GetPivotTableManager().CreatePivotTablesRecord(mnScTab) [excdoc.cxx:585]
  Action: XclExpComments from NOTE records [excdoc.cxx:588-591]
  Action: XClExpWebQueryBuffer [excdoc.cxx:594]
  Action: lcl_AddScenariosAndFilters [excdoc.cxx:596]
  Action: mxCellTable->CreateRecord(EXC_ID_MERGEDCELLS) [excdoc.cxx:599]
  Action: XclExpCondFormatBuffer [excdoc.cxx:602]
  Action: XClCodename [excdoc.cxx:604-606]
  Action: mxCellTable->CreateRecord(EXC_ID_DVAL) [excdoc.cxx:609]
  Action: HLINK records wrapped in XML_hyperlinks [excdoc.cxx:612-619]
  Action: xPageSett [excdoc.cxx:621]
  Action: XclExpUsersViewBegin/End [excdoc.cxx:624-631]
  Action: ExcEof [excdoc.cxx:634]

XclExpXmlStream
  File: main/sc/source/filter/excel/xestream.cxx:820
  Confidence: confirmed

  Manages multiple output streams (workbook, sheets, styles, shared strings, etc.)
  GetStreamForPath() maps package paths to sax_fastparser::FSHelperPtr streams [xestream.cxx:870]
  CreateOutputStream() creates a new stream for a package part [xestream.cxx:971]

### Call Graph 2: UNO ExportFilter path (NOT used for Save As)

ExcelFilter::exportDocument()
  File: main/oox/source/xls/excelfilter.cxx:131
  Confidence: confirmed

  Action: return false [excelfilter.cxx:133] — STUB, not implemented
  Note: This path is NOT invoked by ScDocShell::ConvertTo(). The sc filter path above is used.

---

## DOCX — Complete Export Call Graph

### Call Graph 3: UNO ExportFilter → DOCX package

DocxExportFilter::exportDocument()
  File: main/sw/source/filter/ww8/docxexportfilter.cxx:46
  Confidence: confirmed

  Action: get SwDoc from model via SwXTextDocument [docxexportfilter.cxx:51-58]
  Action: SwPaM aPam for entire document [docxexportfilter.cxx:64-65]
  Action: DocxExport aExport(this, pDoc, pCurPam, &aPam) [docxexportfilter.cxx:73]
  Action: aExport.ExportDocument(true) [docxexportfilter.cxx:74]
  Action: commit() [docxexportfilter.cxx:77] — commits OPC package

DocxExport::ExportDocument_Impl()
  File: main/sw/source/filter/ww8/docxexport.cxx:280
  Confidence: confirmed

  Action: InitStyles() [docxexport.cxx:282] — word/styles.xml
  Action: m_pSections = new MSWordSections(*this) [docxexport.cxx:285]
  Action: WriteMainText() [docxexport.cxx:287] — word/document.xml
  Action: WriteFootnotesEndnotes() [docxexport.cxx:289] — word/footnotes.xml, endnotes.xml
  Action: WriteNumbering() [docxexport.cxx:291] — word/numbering.xml
  Action: WriteFonts() [docxexport.cxx:293] — word/fontTable.xml

DocxExport::DocxExport(DocxExportFilter*, SwDoc*, SwPaM*, SwPaM*)
  File: main/sw/source/filter/ww8/docxexport.cxx:632
  Confidence: confirmed

  Action: WriteProperties() [docxexport.cxx:642] — docProps/core.xml
  Action: m_pFilter->addRelation(officeDocument → word/document.xml) [docxexport.cxx:645-646]
  Action: m_pDocumentFS = m_pFilter->openOutputStreamWithSerializer("word/document.xml", ...)
    [docxexport.cxx:649-650]
  Action: m_pDrawingML = new oox::drawingml::DrawingML(m_pDocumentFS, m_pFilter, DOCUMENT_DOCX)
    [docxexport.cxx:653]
  Action: m_pAttrOutput = new DocxAttributeOutput(*this, m_pDocumentFS, m_pDrawingML)
    [docxexport.cxx:656]
  Action: m_pVMLExport = new VMLExport(m_pDocumentFS) [docxexport.cxx:659]

DocxExport::WriteMainText()
  File: main/sw/source/filter/ww8/docxexport.cxx:602
  Confidence: confirmed

  Action: startElementNS(w:document, namespaces o, r, v, w, w10, wp) [docxexport.cxx:605-612]
    — ALL Transitional namespaces hardcoded
  Action: startElementNS(w:body) [docxexport.cxx:615]
  Action: WriteText() [docxexport.cxx:620] — inherited from MSWordExportBase
  Action: SectionProperties(section info) [docxexport.cxx:623-625]
  Action: endElementNS(w:body), endElementNS(w:document) [docxexport.cxx:628-629]

MSWordExportBase::WriteText()
  File: main/sw/source/filter/ww8/wrtw8nds.cxx (inherited base class)
  Confidence: probable — implementation in wrtw8nds.cxx

  Iterates document nodes:
  ├─ OutputTextNode(SwTxtNode) [wrtw8nds.cxx:1793] → DocxAttributeOutput paragraph/run callbacks
  ├─ mpTableInfo->processSwTable() → DocxAttributeOutput table callbacks (StartTable, StartTableRow, StartTableCell, etc.)
  ├─ OutputSectionNode(SwSectionNode) [wrtw8nds.cxx:2580]
  └─ OutputEndNode(SwEndNode) [wrtw8nds.cxx:311] → MSWordExportBase::OutputEndNode

DocxExport::OutputTableNode(const SwTableNode&)
  File: main/sw/source/filter/ww8/docxexport.cxx:347
  Confidence: confirmed

  Action: OSL_TRACE("TODO DocxExport::OutputTableNode()") — STUB/no-op
  Note: Table export actually proceeds via MSWordExportBase::WriteText() → mpTableInfo→processSwTable() →
        DocxAttributeOutput table callbacks, which ARE implemented.

DocxExport::OutputGrfNode(const SwGrfNode&)
  File: main/sw/source/filter/ww8/docxexport.cxx:352
  Confidence: confirmed

  Action: OSL_TRACE("TODO DocxExport::OutputGrfNode()") — STUB/no-op

DocxExport::OutputOLENode(const SwOLENode&)
  File: main/sw/source/filter/ww8/docxexport.cxx:357
  Confidence: confirmed

  Action: OSL_TRACE("TODO DocxExport::OutputOLENode()") — STUB/no-op

DocxExport::InitStyles()
  File: main/sw/source/filter/ww8/docxexport.cxx:393
  Confidence: confirmed

  Action: addRelation(styles → styles.xml) [docxexport.cxx:398-399]
  Action: openOutputStreamWithSerializer("word/styles.xml", styles+xml) [docxexport.cxx:403-404]

DocxExport::WriteNumbering()
  File: main/sw/source/filter/ww8/docxexport.cxx:461
  Confidence: confirmed

  Action: addRelation(numbering → numbering.xml) [docxexport.cxx:466-467]
  Action: openOutputStreamWithSerializer("word/numbering.xml", numbering+xml)

DocxExport::WriteFonts()
  File: main/sw/source/filter/ww8/docxexport.cxx:555
  Confidence: confirmed

  Action: addRelation(fontTable → fontTable.xml) [docxexport.cxx:558-559]
  Action: openOutputStreamWithSerializer("word/fontTable.xml", fontTable+xml)

DocxExport::WriteFootnotesEndnotes()
  File: main/sw/source/filter/ww8/docxexport.cxx:416
  Confidence: confirmed

  Action: addRelation(footnotes → footnotes.xml) [docxexport.cxx:421-422]
  Action: addRelation(endnotes → endnotes.xml) [docxexport.cxx:442-443]

DocxExport::WriteHeaderFooter(const SwFmt&, bool, const char*)
  File: main/sw/source/filter/ww8/docxexport.cxx:490
  Confidence: confirmed

  Action: addRelation(header/footer → headerN.xml / footerN.xml) [docxexport.cxx:500,515]

DocxExport::WriteProperties()
  File: main/sw/source/filter/ww8/docxexport.cxx:582
  Confidence: confirmed

  Action: writes docProps/core.xml (document properties)

DocxAttributeOutput table callbacks (implemented)
  File: main/sw/source/filter/ww8/docxattributeoutput.cxx
  Confidence: confirmed

  StartTable() [line 1208] → w:tbl
  StartTableRow() [line 1231] → w:tr
  StartTableCell() [line 1257] → w:tc
  TableCellProperties() [line 1112] → w:tcPr
  EndTableCell() [line 1270] → w:tc close
  EndTableRow() [line 1251] → w:tr close
  EndTable() [line 1216] → w:tbl close
  TableDefinition() [line 1285] → w:tblPr, w:tblGrid

DocxAttributeOutput stubs/TODOs
  File: main/sw/source/filter/ww8/docxattributeoutput.cxx
  Confidence: confirmed

  RawText() [line 768] — TODO trace
  StartRuby()/EndRuby() [lines 773,778] — TODO trace
  Redline()/StartRedline()/EndRedline() [lines 867,919,960] — TODO trace
  FormatDrop() [line 985] — TODO trace
  TableRowProperties() [line 1007] — TODO trace
  TableNodeInfo() [line 1449] — TODO trace
  TableOrientation() [line 1463] — TODO trace
  TableSpacing() [line 1468] — fprintf TODO
  TableRowEnd() [line 1475] — TODO trace
  DefaultStyle() [line 1492] — TODO trace
  FlyFrameGraphic() [line 1502] — TODO trace ("some stuff still missing")

### Call Graph 4: Writer Save As path (reachability unclear)

SwDocShell::ConvertTo()
  File: main/sw/source/ui/app/docsh.cxx
  Confidence: requires runtime verification

  Note: Writer's ConvertTo dispatches to the filter selected in Save As.
  The MS_Word_2007_XML xcu config has Flags="IMPORT ALIEN 3RDPARTYFILTER" (no EXPORT)
  and FilterService="com.sun.star.comp.Writer.WriterFilter" (not DocxExportFilter).
  Whether Save As can reach DocxExportFilter::exportDocument() requires runtime verification.

---

## PPTX — Call Graph Status

No top-level PPTX export call graph exists.

Confirmed absent:
- main/oox/source/export/ contains only drawingml.cxx, shapes.cxx, vmlexport.cxx (DrawingML infrastructure, no PresentationML writer)
- main/sd/source/filter/eppt/ contains only legacy binary PPT exporter (eppt.cxx), NOT PPTX
- DrawingML::DOCUMENT_PPTX only affects media path prefixes [drawingml.cxx:486,506], not slide XML generation

Reusable infrastructure for future PPTX exporter:
- oox::core::XmlFilterBase::addRelation() — OPC relationships
- oox::core::XmlFilterBase::openOutputStreamWithSerializer() — package part output
- oox::drawingml::DrawingML::WriteImage() — media/images with DOCUMENT_PPTX support [drawingml.cxx:486,506]
- sax_fastparser::FSHelper — XML serialization
- main/package/source/ (ZipPackage, ZipStorage) — OPC packaging
