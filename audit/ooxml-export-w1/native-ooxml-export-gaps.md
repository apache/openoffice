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

# Native OOXML Export Gaps — Milestone W.1

**Source:** Source audit only. Every gap includes file:line evidence.  
**Confidence:** confirmed | probable | requires runtime verification

---

## 1. XLSX Gaps

### 1.1 Graphics/Drawings — DFF path completely disabled (confirmed)

**Gap:** Charts, shapes, drawings, text boxes, and embedded graphics are not exported to XLSX.

**Evidence:** `main/sc/source/filter/excel/excdoc.cxx:576-582` — the entire DFF/Escher drawing export block in `FillAsXmlTable()` is commented out:

```cpp
// DFF not needed in MSOOXML export
//    GetObjectManager().AddSdrPage();
//    //! close Escher group shape and ESCHER_DgContainer
//    //! opened by XclExpObjList ctor MSODRAWING
//    rR.pObjRecs->EndSheet();
//    // all MSODRAWING and OBJ stuff of this sheet goes here
//    Add( rR.pObjRecs );
```

**Consequence:** Any drawing object, chart, or image in a Calc document is silently dropped on XLSX export. The Escher/DFF infrastructure exists in `xeescher.cxx` but is not wired into the XML export path.

**Required work:** Re-enable and adapt the DFF/Escher export path for XML output, or implement DrawingML-based chart/drawing export.

### 1.2 Tables and Autofilters — missing (confirmed)

**Gap:** Excel tables (`<table>`) and autofilters (`<autoFilter>`) are not exported.

**Evidence:** `main/sc/source/filter/excel/ooxml-export-TODO.txt` lists `autoFilter` [3.3.1.1] and `tablePart` [3.3.1.91] as unsupported, mapped to `[ ??? ]` records.

**Consequence:** Filter settings and structured table definitions are lost.

### 1.3 Conditional formatting — partial (confirmed)

**Gap:** `cfRule`, `cfvo`, `dataBar`, `colorScale`, `iconSet` elements not serialized.

**Evidence:** `ooxml-export-TODO.txt` lists these under Worksheets §3.3.1 as unsupported. `XclExpCondfmt::SaveXml()` exists [xecontent.cxx:856] but does not emit these sub-elements.

**Consequence:** Conditional formatting rules are partially or fully lost.

### 1.4 Comments — partial (confirmed)

**Gap:** Comment text formatting not written; no associated drawing object.

**Evidence:** `ooxml-export-TODO.txt` Comments §3.7: "Currently we only write unformatted text into comments?.xml" and "Excel requires that there be a drawing object associated with the comment before it will show it. If you just generate the <comments/> XML part... Excel will NOT display the comment." Since drawings are not exported (gap 1.1), comments will not display in Excel.

**Consequence:** Comments exported as plain text only, may not display in Excel.

### 1.5 Sheet protection — absent from XML path (confirmed)

**Gap:** Sheet-level protection (`XclExpProtection`) is written in the binary path but not the XML path.

**Evidence:** `excdoc.cxx:474-480` writes `XclExpProtection` in `FillAsTable()` binary branch [lines 474-480], but `FillAsXmlTable()` [lines 548-635] has no equivalent.

**Consequence:** Sheet protection settings lost on XLSX export.

### 1.6 Workbook properties — several attributes not mapped (confirmed)

**Gap:** Multiple `workbookPr` attributes not written.

**Evidence:** `excdoc.cxx:309-325` OOXTODO block lists: allowRefreshQuery, autoCompressPictures, checkCompatibility, codeName, defaultThemeVersion, filterPrivacy, hidePivotFieldList, promptedSolutions, publishItems, saveExternalLinkValues, showBorderUnselectedTables, showInkAnnotation, showPivotChart, updateLinks — all mapped to unsupported BIFF records.

**Consequence:** Workbook-level settings partially lost.

### 1.7 Even/first page headers and footers — missing (confirmed)

**Gap:** `evenFooter`, `evenHeader`, `firstFooter`, `firstHeader` not supported.

**Evidence:** `ooxml-export-TODO.txt` Worksheets §3.3.1 lists these as `[ HeaderFooter 89Ch ]` but unsupported.

**Consequence:** Differentiated headers/footers lost.

---

## 2. DOCX Gaps

### 2.1 OLE objects — missing (confirmed)

**Gap:** `DocxExport::OutputOLENode()` is a no-op stub.

**Evidence:** `main/sw/source/filter/ww8/docxexport.cxx:357-359`:

```cpp
void DocxExport::OutputOLENode( const SwOLENode& )
{
    OSL_TRACE( "TODO DocxExport::OutputOLENode( const SwOLENode& )\n" );
}
```

**Consequence:** Embedded OLE objects are silently dropped.

### 2.2 Ruby text — missing (confirmed)

**Gap:** `StartRuby()` and `EndRuby()` are no-op stubs.

**Evidence:** `main/sw/source/filter/ww8/docxattributeoutput.cxx:773,778`:

```cpp
void DocxAttributeOutput::StartRuby( const SwTxtNode& /*rNode*/, const SwFmtRuby& /*rRuby*/ )
{
    OSL_TRACE("TODO DocxAttributeOutput::StartRuby(...)\n");
}
```

**Consequence:** Ruby (furigana) text lost.

### 2.3 Drop caps — missing (confirmed)

**Gap:** `FormatDrop()` is a no-op stub.

**Evidence:** `main/sw/source/filter/ww8/docxattributeoutput.cxx:985-987`:

```cpp
void DocxAttributeOutput::FormatDrop( const SwTxtNode& /*rNode*/, const SwFmtDrop& /*rSwFmtDrop*/, ... )
{
    OSL_TRACE( "TODO DocxAttributeOutput::FormatDrop(...)\n" );
}
```

**Consequence:** Drop caps formatting lost.

### 2.4 Tracked changes (redlines) — partial (confirmed)

**Gap:** `Redline()`, `StartRedline()`, `EndRedline()` are stubs with TODO traces.

**Evidence:** `main/sw/source/filter/ww8/docxattributeoutput.cxx:867,919,960`.

**Consequence:** Tracked changes/redlines lost or partially represented.

### 2.5 Form fields — partial (confirmed)

**Gap:** `WriteFormData()`, `DoComboBox()`, `DoFormText()` are stubs.

**Evidence:** `main/sw/source/filter/ww8/docxexport.cxx:215,220,275`:

```cpp
void DocxExport::WriteFormData( const ::sw::mark::IFieldmark& /*rFieldmark*/ )
{
    OSL_TRACE( "TODO DocxExport::WriteFormData()\n" );
}
```

**Consequence:** Form field data lost.

### 2.6 Images and floating graphics — partial (confirmed)

**Gap:** `OutputGrfNode()` is a stub; `FlyFrameGraphic()` has TODO trace "some stuff still missing".

**Evidence:** `main/sw/source/filter/ww8/docxexport.cxx:352-354`; `docxattributeoutput.cxx:1502`.

**Consequence:** Some images/floating graphics may be lost or malformed.

### 2.7 Default style — stub (confirmed)

**Gap:** `DefaultStyle()` has TODO trace.

**Evidence:** `main/sw/source/filter/ww8/docxattributeoutput.cxx:1496`.

**Consequence:** `w:docDefaults` may be incomplete.

### 2.8 Various table attributes — stubs (confirmed)

**Gap:** `TableRowProperties()`, `TableNodeInfo()`, `TableOrientation()`, `TableSpacing()`, `TableRowEnd()` are stubs.

**Evidence:** `docxattributeoutput.cxx:1007,1449,1463,1468,1475`.

**Consequence:** Some table properties may be incomplete.

---

## 3. PPTX Gaps

### 3.1 No top-level exporter exists (confirmed)

**Gap:** There is no Impress-to-PresentationML export path at all.

**Evidence:**

- `main/oox/source/export/` contains only `drawingml.cxx`, `shapes.cxx`, `vmlexport.cxx` — DrawingML infrastructure, no PresentationML writer
- `main/oox/source/ppt/pptimport.cxx` — PPTX **import** only
- `main/sd/source/filter/eppt/` — legacy **binary PPT** exporter only (`eppt.cxx:268` `PPTWriter`), NOT PPTX
- `ooxml-export-notes.txt` confirms: oox module handles ".xlsx and .pptx import"; PPT export is binary only

**Consequence:** PPTX export is completely unavailable. Saving an Impress presentation as PPTX is not possible.

### 3.2 Missing PresentationML package parts (confirmed)

All of the following have zero generating code:

| Package part | Required |
|--------------|----------|
| `ppt/presentation.xml` | Presentation root |
| `ppt/slides/slideN.xml` | Slide content |
| `ppt/slideMasters/slideMasterN.xml` | Slide masters |
| `ppt/slideLayouts/slideLayoutN.xml` | Slide layouts |
| `ppt/theme/themeN.xml` | Themes |
| `ppt/notesSlides/notesN.xml` | Notes |
| `[Content_Types].xml` | Content types |
| `ppt/presProps.xml` (optional) | Presentation properties |
| `ppt/tableStyles.xml` (optional) | Table styles |

**Consequence:** No PPTX package can be produced.

---

## 4. Filter Exposure Gaps

### 4.1 XLSX — EXPORT flag missing (confirmed)

**Gap:** `calc_MS_Excel_2007_XML.xcu` Flags lacks `EXPORT`.

**Evidence:** `main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML.xcu:22`:

```xml
<prop oor:name="Flags"><value>IMPORT ALIEN 3RDPARTYFILTER PREFERRED</value></prop>
```

**Consequence:** The filter registry may not list XLSX as an export-capable filter, even though the export code path is complete and reachable via `ScDocShell::ConvertTo()`.

### 4.2 DOCX — EXPORT flag missing + wrong FilterService (confirmed)

**Gap:** `MS_Word_2007_XML.xcu` Flags lacks `EXPORT`; FilterService points to `WriterFilter` (import-oriented), not `DocxExportFilter`.

**Evidence:** `main/filter/source/config/fragments/filters/MS_Word_2007_XML.xcu:22-28`:

```xml
<prop oor:name="Flags"><value>IMPORT ALIEN 3RDPARTYFILTER</value></prop>
<prop oor:name="FilterService"><value>com.sun.star.comp.Writer.WriterFilter</value></prop>
```

**Consequence:** DOCX export may not be exposed via Save As or UNO export APIs. `DocxExportFilter` (a complete export implementation) exists as a standalone UNO service but is not wired into the filter configuration.

---

## 5. Strict OOXML Gaps

### 5.1 No Strict support (confirmed)

**Gap:** All serializers hardcode Transitional namespaces, relationship types, and content types.

**Evidence:**

- SpreadsheetML: `http://schemas.openxmlformats.org/spreadsheetml/2006/main` [excdoc.cxx:793]
- WordprocessingML: `http://schemas.openxmlformats.org/wordprocessingml/2006/main` [docxexport.cxx:609]
- Relationship types: `.../officeDocument/2006/relationships/...` [docxexport.cxx:399,403,558,645]
- Content types: `...wordprocessingml.document.main+xml` [docxexport.cxx:650]
- VML namespace used in DOCX: `urn:schemas-microsoft-com:vml` [docxexport.cxx:608] — Strict forbids VML

**Consequence:** Strict OOXML export is impossible without a conformance abstraction layer.

### 5.2 No centralized conformance abstraction (confirmed)

**Gap:** No `OoxmlConformance` or similar abstraction exists for namespace/relationship/content-type selection by conformance level.

**Consequence:** Strict support would require scattered conditionals throughout serializers unless a centralized abstraction is introduced first.

---

## 6. Testing Gaps

### 6.1 No export tests exist (confirmed)

**Gap:** No unit tests exercise DOCX/XLSX/PPTX export, package inspection, or filter registration for export.

**Evidence:** Search of `main/qa/` found no export tests for docxfilter, excelexport, or pptxexport. Existing test documents in `main/writerfilter/qa/documents/` and `test/testgui/data/pvt/` are import fixtures only.

**Consequence:** No automated regression protection for export functionality.

### 6.2 No OOXML package validation tests (confirmed)

**Gap:** No tests validate exported OOXML packages against ECMA-376 schema or with independent validators.

**Consequence:** Structural validity of exported packages is unverified.
