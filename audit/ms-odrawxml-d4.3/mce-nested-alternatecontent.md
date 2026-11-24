# Native OOXML Export Capability Inventory — Milestone W.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Target:** trunk  
**Milestone:** W.1  
**Scope:** Native DOCX, XLSX and PPTX export implementation  
**Mode:** Source audit only — no production code changes

## 1. Objective

Produce a comprehensive, source-backed inventory of Apache OpenOffice's existing native OOXML export capabilities.

The objective is to determine:

1. What export functionality already exists.
2. Which code paths are actually reachable from normal document export.
3. Which serializers are complete, partial, unfinished or unused.
4. Which OOXML package parts are generated.
5. Which features are silently omitted or degraded.
6. Whether the export filters are correctly registered and exposed.
7. What work is required to achieve useful Transitional OOXML export.
8. What architectural work is required for Strict OOXML export.

**Important scope restrictions:**

- Apache POI is a reference for understanding OOXML writing behavior and package structure only.
- Do not integrate POI.
- Do not introduce a Java runtime dependency.
- Do not copy POI code.
- Do not use LibreOffice code as an implementation source.
- Prefer completing AOO's existing native exporters over introducing replacement architectures.
- Do not modify production code during this milestone.

## 3. XLSX — audit the existing native exporter

### 3.1 Trace the complete export path

Start with:

- `main/sc/source/filter/excel/excel.cxx`
- `main/sc/source/filter/excel/expop2.cxx`
- `main/sc/source/filter/excel/excdoc.cxx`
- `main/sc/source/filter/excel/xestream.cxx`

Trace the actual route from Calc document export through:

`ScDocShell::ConvertTo()` → `ScFormatFilter::ScExportExcel5()` → `lcl_ExportExcel2007Xml()` → `ExportXml2007::Write()` → `ExcDocument::ReadDoc()` → `ExcDocument::WriteXml()`.

Document every transition and the relevant conditions.

Do not mistake the generic `ExcelFilter::exportDocument()` or `XclExpXmlStream::exportDocument()` methods returning false for proof that XLSX export is unavailable. The direct Calc export route must be audited independently.

### 3.2 Inventory XML-specific serializers

Inspect at least:

- `xetable.cxx`
- `xestyle.cxx`
- `xecontent.cxx`
- `xepivot.cxx`
- `xeescher.cxx`
- `xepage.cxx`
- `xename.cxx`

Find every XML-specific serialization method, including `SaveXml()` implementations and methods called by them.

For each serializer, record:

- Source file and method.
- Actual caller.
- Whether it is reached during normal XLSX export.
- OOXML package part or XML element produced.
- Features serialized.
- Features omitted.
- TODO/FIXME markers.
- Whether output is conditional on document content or configuration.

### 3.3 Inspect `FillAsXmlTable()` in detail

The XML-specific branch in `ExcTable::FillAsTable()` is important:

`GetOutput() != EXC_OUTPUT_BINARY` → `FillAsXmlTable()`.

Compare `FillAsXmlTable()` with the binary path and establish:

- Which records are constructed exclusively for XML.
- Which binary-oriented records are intentionally omitted.
- Which XML records are missing.
- Whether there are features implemented in the binary exporter but not the XML exporter.
- Whether any XML-specific structures are built but never serialized.

In particular, investigate the commented/disabled DFF export path. Determine the practical consequences for charts, shapes, drawings and embedded graphics.

### 3.4 XLSX feature inventory

Cover at least:

- Workbook and worksheet structure.
- Cells and formulas.
- Shared strings and rich-text runs.
- Styles, number formats, fonts, fills and borders.
- Merged cells.
- Row and column properties.
- Hidden and grouped rows/columns.
- Hyperlinks.
- Comments.
- Named ranges.
- Data validation.
- Conditional formatting.
- Charts and drawings.
- Images.
- Shapes and text boxes.
- Pivot tables and pivot caches.
- External links.
- Tables and autofilters.
- Page setup, print areas and headers/footers.
- VBA projects and macro-enabled workbook preservation.
- Workbook calculation settings and formula metadata.

Do not classify a feature as missing merely because it is not found in one file. Search the complete export path.

### 3.5 Filter exposure

Inspect:

- `main/filter/source/config/fragments/filters/calc_MS_Excel_2007_XML.xcu`
- `main/sc/source/ui/docshell/docsh.cxx`
- Relevant filter registration and component files.

Determine whether the filter is exposed through normal Save As, UNO filter APIs and the filter registry.

The current configuration appears to identify the filter as import-only. Verify the full registration chain and identify the exact changes that would be required to expose export.

Do not change the configuration.

## 4. DOCX — audit the existing Writer exporter

### 4.1 Trace the complete export path

Inspect:

- `main/writerfilter/source/filter/ImportFilter.cxx`
- `main/writerfilter/util/writerfilter.component`
- `main/sw/source/filter/ww8/docxexportfilter.cxx`
- `main/sw/source/filter/ww8/docxexportfilter.hxx`
- `main/sw/source/filter/ww8/docxexport.cxx`
- `main/sw/source/filter/ww8/docxexport.hxx`

Trace the path from Writer's filter invocation to package creation and commit.

Document the relationship between `WriterFilter`, `DocxExportFilter` and `DocxExport`.

Check whether normal Save As and UNO export can reach this path. The filter configuration currently appears import-only; establish the actual consequences.

### 4.2 Audit `DocxAttributeOutput`

Inspect:

- `main/sw/source/filter/ww8/docxattributeoutput.cxx`
- `main/sw/source/filter/ww8/docxattributeoutput.hxx`

Create a method-level inventory of the attribute output interface.

For every relevant method, classify it as:

- Fully implemented.
- Partially implemented.
- Stub/no-op.
- TODO or FIXME.
- Not reached by the active export path.
- Unclear; requires runtime validation.

Map the method to its OOXML element or package part.

### 4.3 Audit document node serialization

Trace the handling of:

- Paragraphs and character formatting.
- Tables, rows and cells.
- Images and floating graphics.
- OLE objects.
- Text frames.
- Hyperlinks.
- Bookmarks.
- Fields and form fields.
- Lists and numbering.
- Footnotes and endnotes.
- Headers and footers.
- Sections and page breaks.
- Page styles.
- Comments and tracked changes.
- Ruby text and drop caps.

**Important:** Do not conclude that table export is missing because `DocxExport::OutputTableNode()` contains a TODO. Trace the actual `MSWordExportBase::WriteText()` and `mpTableInfo->processSwTable(...)` route, including the active `DocxAttributeOutput` table callbacks.

Similarly, distinguish the different graphic and OLE paths. A no-op for one node type does not establish that all graphics or objects are unsupported.

### 4.4 DOCX package inventory

Record the creation and relationship handling for:

- `word/document.xml`
- `word/styles.xml`
- `word/numbering.xml`
- `word/fontTable.xml`
- `word/footnotes.xml`
- `word/endnotes.xml`
- Headers and footers.
- Document properties.
- Media.
- Relationships.
- Content types.

Identify package parts that are generated, conditionally generated, incomplete or absent.

### 4.5 DOCX filter exposure

Inspect:

`main/filter/source/config/fragments/filters/MS_Word_2007_XML.xcu`

Trace the filter configuration to the component implementation and determine what is required for normal Save As and UNO export.

Do not change configuration.

## 5. PPTX — determine what can be reused

### 5.1 Establish whether a top-level exporter exists

Inspect:

- `main/oox/source/ppt/pptimport.cxx`
- `main/oox/source/export/`
- `main/oox/inc/oox/export/`
- `main/sd/source/filter/eppt/`

Establish whether a complete Impress-to-PresentationML export path exists.

Do not treat the legacy binary PPT exporter as PPTX export.

Do not treat an export service declaration or an individual serializer as evidence of a functioning package writer.

### 5.2 Inventory reusable DrawingML infrastructure

Inspect and trace actual callers of:

- `drawingml.cxx`
- `shapes.cxx`
- `vmlexport.cxx`
- `utils.cxx`
- Corresponding export headers.

Inventory support for:

- Shape geometry.
- Transformations.
- Fill and outline.
- Gradients and colors.
- Text runs and paragraphs.
- Images and media.
- Connectors and lines.
- Custom shapes.
- Relationships and package output.

For each capability, identify whether it is called from production code, merely declared, or effectively unused.

### 5.3 Identify missing PresentationML orchestration

Determine what is required to generate:

- `ppt/presentation.xml`
- Slide XML files.
- Slide masters.
- Slide layouts.
- Themes.
- Notes and notes masters.
- Relationships.
- Content types.
- Slide size and ordering.
- Transitions.
- Animations and timings.
- Embedded objects.

Identify existing AOO infrastructure that could be reused for OPC packaging, relationships, XML serialization and media handling.

Provide a proposed architecture for a native Impress-to-PPTX exporter. Do not implement it.

## 6. Transitional versus Strict export

Inspect all active serializers and identify hardcoded Transitional assumptions.

At minimum, audit:

- Namespace declarations.
- Relationship type URIs.
- Content types.
- Package-level relationships.
- XML element and attribute assumptions.
- Extension handling.

Determine whether any existing export path can emit Strict OOXML.

Propose a centralized namespace, relationship and content-type abstraction for future Strict support. Avoid recommending scattered conditionals throughout serializers.

Strict support should be treated as a separate future milestone unless the audit discovers meaningful existing infrastructure.

## 7. Testing and validation inventory

Identify existing AOO tests relevant to:

- DOCX export.
- XLSX export.
- PPTX export.
- OOXML package integrity.
- Filter registration.
- UNO filter invocation.

For each test, document its location, execution method, coverage and limitations.

Prepare a feature-oriented regression matrix that includes:

- AOO document → OOXML export → package inspection.
- OOXML export → reopen in AOO.
- Export → reopen in Microsoft Office-compatible software where available.
- Preservation of formulas, styles, objects, drawings and relationships.
- Negative tests for unsupported features and silent data loss.

Use independent OOXML package validation where practical.

Do not claim tests were executed unless they actually were.

## 8. Prioritization

Produce a ranked implementation recommendation using:

- Existing implementation maturity.
- Amount of missing work.
- User-visible impact.
- Risk of data loss.
- Reuse of existing AOO code.
- Testability.

The initial expected ordering is:

1. XLSX: complete and expose the existing native exporter.
2. DOCX: complete and expose the existing native exporter.
3. PPTX: implement the missing top-level exporter using existing AOO DrawingML and package infrastructure.
4. Strict OOXML export.
5. Broader interoperability and conformance work.

Challenge this ordering if the source audit provides concrete evidence for a better sequence.

## 9. Evidence and quality requirements

Every major conclusion must include:

- Exact source path.
- Function or method name.
- Relevant call chain.
- Relevant source evidence.
- Confidence level: confirmed, probable or requires runtime verification.

Clearly distinguish:

- Code that exists.
- Code that is called.
- Code that generates package output.
- Code that is exposed to users.
- Code that has been tested.

Do not infer feature support from class names, comments, TODO counts or service declarations alone.

Do not report a feature as absent until the complete relevant call path has been examined.

## 10. Completion criteria

W.1 is complete only when:

1. XLSX export has a documented, verified source call graph.
2. DOCX export has a documented, verified source call graph.
3. PPTX export status is established beyond doubt.
4. Every important package part is accounted for.
5. XML-specific serializers and their callers are inventoried.
6. Filter exposure issues are documented.
7. Strict versus Transitional assumptions are catalogued.
8. The feature inventory is machine-readable and traceable to source.
9. A prioritized implementation plan is delivered.
10. No production code was modified.

## 11. Final report

Conclude with a short decision document answering:

- Which exporter is closest to being production-usable?
- Which existing exporter gives the best return on implementation effort?
- Which features are the most serious causes of data loss?
- What is the minimum work required to expose DOCX and XLSX export?
- What is the minimum viable architecture for PPTX export?
- What should the next implementation milestone contain?

Do not mark W.1 complete based solely on producing the documents. The source evidence and call graphs must be complete and independently reviewable.
