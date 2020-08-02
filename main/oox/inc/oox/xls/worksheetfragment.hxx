/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef OOX_XLS_WORKSHEETFRAGMENT_HXX
#define OOX_XLS_WORKSHEETFRAGMENT_HXX

#include "oox/xls/excelhandlers.hxx"

namespace oox {
namespace xls {

// ============================================================================

class DataValidationsContext : public WorksheetContextBase
{
public:
    explicit            DataValidationsContext( WorksheetFragmentBase& rFragment );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onCharacters( const ::rtl::OUString& rChars );
    virtual void        onEndElement();

    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );

private:
    /** Imports the dataValidation element containing data validation settings. */
    void                importDataValidation( const AttributeList& rAttribs );
    /** Imports the DATAVALIDATION record containing data validation settings. */
    void                importDataValidation( SequenceInputStream& rStrm );

private:
    ::std::auto_ptr< ValidationModel > mxValModel;
};

// ============================================================================

class WorksheetFragment : public WorksheetFragmentBase
{
public:
    explicit            WorksheetFragment(
                            const WorksheetHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onCharacters( const ::rtl::OUString& rChars );

    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );

    virtual const ::oox::core::RecordInfo* getRecordInfos() const;
    virtual void        initializeImport();
    virtual void        finalizeImport();

private:
    /** Imports page settings from a pageSetUpPr element. */
    void                importPageSetUpPr( const AttributeList& rAttribs );
    /** Imports the dimension element containing the used area of the sheet. */
    void                importDimension( const AttributeList& rAttribs );
    /** Imports sheet format properties from a sheetFormatPr element. */
    void                importSheetFormatPr( const AttributeList& rAttribs );
    /** Imports column settings from a col element. */
    void                importCol( const AttributeList& rAttribs );
    /** Imports a merged cell range from a mergeCell element. */
    void                importMergeCell( const AttributeList& rAttribs );
    /** Imports the hyperlink element containing a hyperlink for a cell range. */
    void                importHyperlink( const AttributeList& rAttribs );
    /** Imports individual break that is either within row or column break context. */
    void                importBrk( const AttributeList& rAttribs, bool bRowBreak );
    /** Imports the relation identifier for the DrawingML part. */
    void                importDrawing( const AttributeList& rAttribs );
    /** Imports the relation identifier for the legacy VML drawing part. */
    void                importLegacyDrawing( const AttributeList& rAttribs );
    /** Imports additional data for an OLE object. */
    void                importOleObject( const AttributeList& rAttribs );
    /** Imports additional data for an OCX form control. */
    void                importControl( const AttributeList& rAttribs );

    /** Imports the DIMENSION record containing the used area of the sheet. */
    void                importDimension( SequenceInputStream& rStrm );
    /** Imports sheet format properties from a SHEETFORMATPR record. */
    void                importSheetFormatPr( SequenceInputStream& rStrm );
    /** Imports column settings from a COL record. */
    void                importCol( SequenceInputStream& rStrm );
    /** Imports a merged cell range from a MERGECELL record. */
    void                importMergeCell( SequenceInputStream& rStrm );
    /** Imports a hyperlink for a cell range from a HYPERLINK record. */
    void                importHyperlink( SequenceInputStream& rStrm );
    /** Imports the BRK record for an individual row or column page break. */
    void                importBrk( SequenceInputStream& rStrm, bool bRowBreak );
    /** Imports the DRAWING record containing the relation identifier for the DrawingML part. */
    void                importDrawing( SequenceInputStream& rStrm );
    /** Imports the LEGACYDRAWING record containing the relation identifier for the VML drawing part. */
    void                importLegacyDrawing( SequenceInputStream& rStrm );
    /** Imports additional data for an OLE object. */
    void                importOleObject( SequenceInputStream& rStrm );
    /** Imports additional data for an OCX form control. */
    void                importControl( SequenceInputStream& rStrm );

    /** Imports the binary data of an embedded OLE object from the fragment with the passed ID. */
    void                importEmbeddedOleData( StreamDataSequence& orEmbeddedData, const ::rtl::OUString& rRelId );
};

// ============================================================================

class BiffWorksheetFragment : public BiffWorksheetFragmentBase
{
public:
    explicit            BiffWorksheetFragment(
                            const WorksheetHelper& rHelper,
                            const BiffWorkbookFragmentBase& rParent );
    virtual             ~BiffWorksheetFragment();

    /** Imports the entire worksheet fragment, returns true, if EOF record has been reached. */
    virtual bool        importFragment();

private:
    /** Imports the AUTOFILTER and following records with auto filter settings. */
    void                importAutoFilter( BiffInputStream& rStrm );
    /** Imports the COLINFO record and sets column properties and formatting. */
    void                importColInfo( BiffInputStream& rStrm );
    /** Imports the BIFF2 COLUMNDEFAULT record and sets column default formatting. */
    void                importColumnDefault( BiffInputStream& rStrm );
    /** Imports the BIFF2 COLWIDTH record and sets column width. */
    void                importColWidth( BiffInputStream& rStrm );
    /** Imports the DATAVALIDATION record containing cell ranges with data validation settings. */
    void                importDataValidation( BiffInputStream& rStrm );
    /** Imports the DATAVALIDATIONS record containing global data validation settings. */
    void                importDataValidations( BiffInputStream& rStrm );
    /** Imports the DEFCOLWIDTH record and sets default column width. */
    void                importDefColWidth( BiffInputStream& rStrm );
    /** Imports the DEFROWHEIGHT record and sets default row height and properties. */
    void                importDefRowHeight( BiffInputStream& rStrm );
    /** Imports the DIMENSION record containing the used area of the sheet. */
    void                importDimension( BiffInputStream& rStrm );
    /** Imports the HYPERLINK record and sets a cell hyperlink. */
    void                importHyperlink( BiffInputStream& rStrm );
    /** Imports the LABELRANGES record and sets the imported label ranges. */
    void                importLabelRanges( BiffInputStream& rStrm );
    /** Imports the MEREDCELLS record and merges all cells in the document. */
    void                importMergedCells( BiffInputStream& rStrm );
    /** Imports the NOTE record containing a cell note. */
    void                importNote( BiffInputStream& rStrm );
    /** Imports the HORPAGEBREAKS or VERPAGEBREAKS record and inserts page breaks. */
    void                importPageBreaks( BiffInputStream& rStrm, bool bRowBreak );
    /** Imports a pivot table. */
    void                importPTDefinition( BiffInputStream& rStrm );
    /** Imports the QUERYTABLE and following records and inserts a web query. */
    void                importQueryTable( BiffInputStream& rStrm );
    /** Imports the SCENARIOS record and the following scenarios. */
    void                importScenarios( BiffInputStream& rStrm );
    /** Imports the SHAREDFEATHEAD record. */
    void                importSharedFeatHead( BiffInputStream& rStrm );
    /** Imports the STANDARDWIDTH record and sets standard column width. */
    void                importStandardWidth( BiffInputStream& rStrm );

private:
    ::boost::shared_ptr< BiffWorksheetContextBase > mxContext;
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif
