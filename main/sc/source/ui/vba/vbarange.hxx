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


#ifndef SC_VBA_RANGE_HXX
#define SC_VBA_RANGE_HXX

#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/container/XEnumerationAccess.hpp>

#include <ooo/vba/excel/XRange.hpp>
#include <ooo/vba/excel/XQueryTable.hpp>
#include <ooo/vba/excel/XFormatConditions.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <ooo/vba/excel/XFont.hpp>
#include <ooo/vba/excel/XComment.hpp>
#include <ooo/vba/XCollection.hpp>
#include <ooo/vba/excel/XlPasteType.hdl>
#include <ooo/vba/excel/XlPasteSpecialOperation.hdl>

#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/script/XDefaultMethod.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <com/sun/star/sheet/FillDateMode.hpp>
#include <com/sun/star/sheet/FillMode.hpp>
#include <com/sun/star/sheet/FillDirection.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSheetCellRangeContainer.hpp>

//#include <vbahelper/vbahelperinterface.hxx>
#include "vbaformat.hxx"
#include <formula/grammar.hxx>

class ScTableSheetsObj;
class ScCellRangesBase;
class ScCellRangeObj;
class ScCellRangesObj;
class ScDocShell;
class ScDocument;
class ScRangeList;

//typedef InheritedHelperInterfaceImpl1< ov::excel::XRange >  ScVbaRange_BASE;
typedef ScVbaFormat< ov::excel::XRange > ScVbaRange_BASE;

// MS Excel VBA: Internal helper class for visiting array nodes.
// Status: internal helper
class ArrayVisitor
{
public:
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const css::uno::Reference< css::table::XCell >& xCell ) = 0;
	virtual	~ArrayVisitor(){}
};

// MS Excel VBA: Internal helper class for setting values in an array.
// Status: internal helper
class ValueSetter : public ArrayVisitor
{
public:
	virtual bool processValue( const css::uno::Any& aValue, const css::uno::Reference< css::table::XCell >& xCell ) = 0;


};

// MS Excel VBA: Internal helper class for getting values from an array.
// Status: internal helper
class ValueGetter : public ArrayVisitor
{

public:
	virtual void processValue( sal_Int32 x, sal_Int32 y, const css::uno::Any& aValue ) = 0;
	virtual const css::uno::Any& getValue() const = 0;
};



// MS Excel VBA: Represents a cell or range of cells in an Excel worksheet.
// Status: implemented - primary Range implementation
class ScVbaRange : public ScVbaRange_BASE
{
	css::uno::Reference< ov::XCollection > m_Areas;
	css::uno::Reference< ov::XCollection > m_Borders;
	css::uno::Reference< css::table::XCellRange > mxRange;
	css::uno::Reference< css::sheet::XSheetCellRangeContainer > mxRanges;
	sal_Bool mbIsRows;
	sal_Bool mbIsColumns;
	css::uno::Reference< ov::excel::XValidation > m_xValidation;

// MS Excel VBA: Internal helper - calculates column width for a cell range address.
// Status: internal helper
	double getCalcColWidth( const css::table::CellRangeAddress& );
// MS Excel VBA: Internal helper - calculates row height for a cell range address.
// Status: internal helper
	double getCalcRowHeight( const css::table::CellRangeAddress& );
// MS Excel VBA: Internal helper - visits all cells in an array for processing.
// Status: internal helper
	void visitArray( ArrayVisitor& vistor );

// MS Excel VBA: Internal helper - gets the entire column or row of the range.
// Status: internal helper
	css::uno::Reference< ov::excel::XRange > getEntireColumnOrRow( bool bColumn = true );

// MS Excel VBA: Internal helper - fills cells with a series of values.
// Status: internal helper
	void fillSeries(  css::sheet::FillDirection nFillDirection, css::sheet::FillMode nFillMode, css::sheet::FillDateMode nFillDateMode, double fStep, double fEndValue );

// MS Excel VBA: Internal helper - clears contents with specified flags.
// Status: internal helper
	void ClearContents( sal_Int32 nFlags, bool bFireEvent );

// MS Excel VBA: Internal helper - gets values using a ValueGetter.
// Status: internal helper
	css::uno::Any getValue( ValueGetter& rValueGetter );
// MS Excel VBA: Internal helper - sets values using a ValueSetter.
// Status: internal helper
	void setValue( const css::uno::Any& aValue, ValueSetter& setter, bool bFireEvent );

// MS Excel VBA: Internal helper - gets formula values from cells.
// Status: internal helper
	css::uno::Any getFormulaValue( formula::FormulaGrammar::Grammar );
// MS Excel VBA: Internal helper - sets formula values in cells.
// Status: internal helper
	void setFormulaValue( const css::uno::Any& aValue, formula::FormulaGrammar::Grammar, bool bFireEvent );

// MS Excel VBA: Internal helper - gets a sub-range by index.
// Status: internal helper
	css::uno::Reference< ov::excel::XRange > getArea( sal_Int32 nIndex  );
	// MS Excel VBA: Internal helper - gets the ScCellRangeObj implementation.
	// Status: internal helper
	ScCellRangeObj* getCellRangeObj( );
// MS Excel VBA: Internal helper - gets the ScCellRangesObj implementation.
// Status: internal helper
    ScCellRangesObj* getCellRangesObj();
// MS Excel VBA: Internal helper - gets or creates the Borders collection.
// Status: internal helper
	css::uno::Reference< ov::XCollection >& getBorders();
// MS Excel VBA: Internal helper - groups or ungroups the range.
// Status: internal helper
	void groupUnGroup( bool bUnGroup = false );
// MS Excel VBA: Internal helper - returns previous or next cell.
// Status: internal helper
 	css::uno::Reference< ov::excel::XRange > PreviousNext( bool bIsPrevious );
// MS Excel VBA: Internal helper - implementation for SpecialCells method.
// Status: internal helper
 	css::uno::Reference< ov::excel::XRange > SpecialCellsImpl( sal_Int32 nType, const css::uno::Any& _oValue);
// MS Excel VBA: Internal helper - gets the position of the range.
// Status: internal helper
	css::awt::Point getPosition();

    /** Fires a Worksheet_Change event for this range or range list. */
// MS Excel VBA: Internal helper - fires a Worksheet_Change event for this range.
// Status: internal helper
	void fireChangeEvent();

protected:
// MS Excel VBA: Internal helper - gets the ScCellRangesBase interface.
// Status: internal helper
	virtual ScCellRangesBase* getCellRangesBase();
// MS Excel VBA: Internal helper - gets the current data set.
// Status: internal helper
	virtual SfxItemSet* getCurrentDataSet( );
public:
// MS Excel VBA: Constructor from a cell range.
// Status: implemented
	ScVbaRange( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::table::XCellRange >& xRange, sal_Bool bIsRows = false, sal_Bool bIsColumns = false );
// MS Excel VBA: Constructor from a cell range.
// Status: implemented
	ScVbaRange( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::sheet::XSheetCellRangeContainer >& xRanges, sal_Bool bIsRows = false, sal_Bool bIsColumns = false );
// MS Excel VBA: Constructor taking arguments sequence (for UNO create).
// Status: implemented
	ScVbaRange( css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );

	// MS Excel VBA: Gets the underlying ScDocument for this range.
	// Status: implemented
	ScDocument* getScDocument();
	// MS Excel VBA: Gets the underlying ScDocShell for this range.
	// Status: implemented
	ScDocShell* getScDocShell();

    /** Returns the ScVbaRange implementation object for the passed VBA Range object. */
// MS Excel VBA: Returns the ScVbaRange implementation object for a VBA Range object.
// Status: implemented
	static ScVbaRange* getImplementation( const css::uno::Reference< ov::excel::XRange >& rxRange );

	// MS Excel VBA: Gets the underlying model object.
	// Status: implemented
	css::uno::Reference< css::frame::XModel > getUnoModel();
// MS Excel VBA: Static method to get the underlying model from a Range object.
// Status: implemented
	static css::uno::Reference< css::frame::XModel > getUnoModel( const css::uno::Reference< ov::excel::XRange >& rxRange );

// MS Excel VBA: Gets the ScRangeList for this range.
// Status: implemented
	const ScRangeList& getScRangeList();
	// MS Excel VBA: Static method to get the underlying model from a Range object.
	// Status: implemented
	static const ScRangeList& getScRangeList( const css::uno::Reference< ov::excel::XRange >& rxRange );

// MS Excel VBA: Destructor.
// Status: implemented
	virtual ~ScVbaRange();
// MS Excel VBA: Returns this object as XHelperInterface.
// Status: implemented
 	virtual css::uno::Reference< ov::XHelperInterface > thisHelperIface() { return this; }
// MS Excel VBA: Checks if this range is a single cell.
// Status: implemented
	bool isSingleCellRange();

// MS Excel VBA: Creates a Range object from a range name string.
// Status: implemented
    static css::uno::Reference< ov::excel::XRange > getRangeObjectForName(
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const rtl::OUString& sRangeName, ScDocShell* pDocSh,
        formula::FormulaGrammar::AddressConvention eConv = formula::FormulaGrammar::CONV_XL_A1  );

// MS Excel VBA: Creates a Range object from row and column indices.
// Status: implemented
    static css::uno::Reference< ov::excel::XRange > CellsHelper(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::table::XCellRange >& xRange,
        const css::uno::Any &nRowIndex, const css::uno::Any &nColumnIndex );

    // Attributes
// MS Excel VBA: Gets the value of the range as a Variant.
// Status: implemented
	virtual css::uno::Any SAL_CALL getValue();
	virtual void   SAL_CALL setValue( const css::uno::Any& aValue );
	// MS Excel VBA: Gets the value of the range without date coercion.
	// Status: implemented
	virtual css::uno::Any SAL_CALL getValue2();
	// MS Excel VBA: Sets the value of the range without date coercion.
	// Status: implemented
	virtual void   SAL_CALL setValue2( const css::uno::Any& aValue );
	// MS Excel VBA: Gets the formula of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getFormula();
	// MS Excel VBA: Sets the formula of the range.
// Status: implemented
	virtual void   SAL_CALL setFormula( const css::uno::Any& rFormula );
// MS Excel VBA: Gets the formula using Formula2 syntax (array-enabled).
// Status: implemented
	virtual css::uno::Any SAL_CALL getFormula2();
// MS Excel VBA: Sets the formula using Formula2 syntax (array-enabled).
// Status: implemented
	virtual void   SAL_CALL setFormula2( const css::uno::Any& rFormula );
// MS Excel VBA: Gets the formula as an array.
// Status: implemented
	virtual css::uno::Any SAL_CALL getFormulaArray();
// MS Excel VBA: Sets the formula as an array.
// Status: implemented
	virtual void   SAL_CALL setFormulaArray(const css::uno::Any& rFormula);
// MS Excel VBA: Gets the formula in R1C1 notation.
// Status: implemented
	virtual css::uno::Any SAL_CALL getFormulaR1C1();
// MS Excel VBA: Sets the formula in R1C1 notation.
// Status: implemented
	virtual void   SAL_CALL setFormulaR1C1( const css::uno::Any &rFormula );
// MS Excel VBA: Gets the number of cells in the range.
// Status: implemented
	virtual ::sal_Int32 SAL_CALL getCount();
// MS Excel VBA: Gets the row number of the first cell.
// Status: implemented
	virtual ::sal_Int32 SAL_CALL getRow();
// MS Excel VBA: Gets the column number of the first cell.
// Status: implemented
	virtual ::sal_Int32 SAL_CALL getColumn();
// MS Excel VBA: Gets the text content of the range.
// Status: implemented
	virtual ::rtl::OUString SAL_CALL getText();
// MS Excel VBA: Uses the base class setNumberFormat method.
// Status: inherited
    using ScVbaRange_BASE::setNumberFormat;
// MS Excel VBA: Sets the number format for the range.
// Status: implemented
	virtual void SAL_CALL setNumberFormat( const css::uno::Any& rNumberFormat );
// MS Excel VBA: Gets the number format for the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getNumberFormat();
// MS Excel VBA: Sets whether cells are merged in the range.
// Status: implemented
	virtual void SAL_CALL setMergeCells( const css::uno::Any& bMerge );
// MS Excel VBA: Gets whether cells are merged in the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getMergeCells();
// MS Excel VBA: Sets whether text is wrapped in the range.
// Status: implemented
	virtual void SAL_CALL setWrapText( const css::uno::Any& bIsWrapped );
// MS Excel VBA: Gets whether text is wrapped in the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getWrapText();
// MS Excel VBA: Returns the entire row(s) of the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getEntireRow();
// MS Excel VBA: Returns the entire column(s) of the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getEntireColumn();
// MS Excel VBA: Gets or sets the comment for the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XComment > SAL_CALL getComment();
// MS Excel VBA: Gets whether the range is hidden.
// Status: implemented
	virtual css::uno::Any SAL_CALL getHidden();
// MS Excel VBA: Sets whether the range is hidden.
// Status: implemented
	virtual void SAL_CALL setHidden( const css::uno::Any& _hidden );
// MS Excel VBA: Gets the column width of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getColumnWidth();
// MS Excel VBA: Sets the column width of the range.
// Status: implemented
	virtual void SAL_CALL setColumnWidth( const css::uno::Any& _columnwidth );
// MS Excel VBA: Gets the row height of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getRowHeight();
// MS Excel VBA: Sets the row height of the range.
// Status: implemented
	virtual void SAL_CALL setRowHeight( const css::uno::Any& _rowheight );
// MS Excel VBA: Gets the width of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getWidth();
// MS Excel VBA: Gets the height of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getHeight();
// MS Excel VBA: Gets the top position of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getTop();
// MS Excel VBA: Gets the left position of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getLeft();

// MS Excel VBA: Returns the parent worksheet.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getWorksheet();
// MS Excel VBA: Gets/sets the page break for the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getPageBreak();
// MS Excel VBA: Sets the page break for the range.
// Status: implemented
	virtual void SAL_CALL setPageBreak( const css::uno::Any& _pagebreak );
// MS Excel VBA: Gets the data validation for the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XValidation > SAL_CALL getValidation();
// MS Excel VBA: Gets the prefix character for the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getPrefixCharacter();
// MS Excel VBA: Gets the show detail setting for the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getShowDetail();
// MS Excel VBA: Sets the show detail setting for the range.
// Status: implemented
	virtual void SAL_CALL setShowDetail(const css::uno::Any& aShowDetail);
	// Methods
// MS Excel VBA: Returns true if the range represents rows.
// Status: implemented - inline
	sal_Bool IsRows() { return mbIsRows; }
// MS Excel VBA: Returns true if the range represents columns.
// Status: implemented - inline
	sal_Bool IsColumns() { return mbIsColumns; }
// MS Excel VBA: Adds a comment to the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XComment > SAL_CALL AddComment( const css::uno::Any& Text );
// MS Excel VBA: Clears the entire range.
// Status: implemented
	virtual void SAL_CALL Clear();
// MS Excel VBA: Clears all comments in the range.
// Status: implemented
	virtual void SAL_CALL ClearComments();
// MS Excel VBA: Clears the contents of the range.
// Status: implemented
	virtual void SAL_CALL ClearContents();
// MS Excel VBA: Clears the formatting of the range.
// Status: implemented
	virtual void SAL_CALL ClearFormats();
// MS Excel VBA: Checks if the range contains any formulas.
// Status: implemented
	virtual css::uno::Any SAL_CALL HasFormula();
// MS Excel VBA: Fills left from the source range.
// Status: implemented
	virtual void SAL_CALL FillLeft();
// MS Excel VBA: Fills right from the source range.
// Status: implemented
	virtual void SAL_CALL FillRight();
// MS Excel VBA: Fills up from the source range.
// Status: implemented
	virtual void SAL_CALL FillUp();
// MS Excel VBA: Fills down from the source range.
// Status: implemented
	virtual void SAL_CALL FillDown();
// MS Excel VBA: Returns a range offset from the current range by row and column.
// Status: implemented
	virtual	css::uno::Reference< ov::excel::XRange > SAL_CALL Offset( const css::uno::Any &nRowOffset, const css::uno::Any &nColOffset );
// MS Excel VBA: Returns the current region (data area) around the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL CurrentRegion();
// MS Excel VBA: Returns the current array for the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL CurrentArray();
// MS Excel VBA: Returns a Characters object for the range.
// Status: implemented
	virtual ::rtl::OUString SAL_CALL Characters( const css::uno::Any& nIndex, const css::uno::Any& nCount );

// MS Excel VBA: Returns the address of the range.
// Status: implemented
	virtual ::rtl::OUString SAL_CALL Address( const css::uno::Any& RowAbsolute, const css::uno::Any& ColumnAbsolute, const css::uno::Any& ReferenceStyle, const css::uno::Any& External, const css::uno::Any& RelativeTo );

// MS Excel VBA: Returns a cell or cells from the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Cells( const css::uno::Any &nRow, const css::uno::Any &nCol );
// MS Excel VBA: Selects the range.
// Status: implemented
	virtual void SAL_CALL Select();
// MS Excel VBA: Activates the range.
// Status: implemented
	virtual void SAL_CALL Activate();
// MS Excel VBA: Returns a row or rows from the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange >  SAL_CALL Rows( const css::uno::Any& nIndex );
// MS Excel VBA: Returns a column or columns from the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange >  SAL_CALL Columns( const css::uno::Any &nIndex );
// MS Excel VBA: Copies the range to the clipboard or destination.
// Status: implemented
	virtual void SAL_CALL Copy( const css::uno::Any& Destination );
// MS Excel VBA: Cuts the range to the clipboard or destination.
// Status: implemented
	virtual void SAL_CALL Cut( const css::uno::Any& Destination );
// MS Excel VBA: Resizes the range to specified rows and columns.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Resize( const css::uno::Any& RowSize, const css::uno::Any& ColumnSize );
// MS Excel VBA: Returns the Font object for the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XFont > SAL_CALL Font();
	virtual css::uno::Reference< ov::excel::XInterior > SAL_CALL Interior(  ) ;
// MS Excel VBA: Returns a subrange within the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Range( const css::uno::Any &Cell1, const css::uno::Any &Cell2 );
	virtual css::uno::Reference< ov::excel::XRange > Range( const css::uno::Any &Cell1, const css::uno::Any &Cell2, bool bForceUseInpuRangeTab );
// MS Excel VBA: Gets the cell range object.
// Status: implemented
	virtual css::uno::Any SAL_CALL getCellRange(  );
	static css::uno::Any getCellRange( const css::uno::Reference< ov::excel::XRange >& rxRange );
// MS Excel VBA: Pastes from clipboard with specified paste options.
// Status: implemented
	virtual void SAL_CALL PasteSpecial( const css::uno::Any& Paste, const css::uno::Any& Operation, const css::uno::Any& SkipBlanks, const css::uno::Any& Transpose );
// MS Excel VBA: Replaces values matching What with Replacement.
// Status: implemented
	virtual ::sal_Bool SAL_CALL Replace( const ::rtl::OUString& What, const ::rtl::OUString& Replacement, const css::uno::Any& LookAt, const css::uno::Any& SearchOrder, const css::uno::Any& MatchCase, const css::uno::Any& MatchByte, const css::uno::Any& SearchFormat, const css::uno::Any& ReplaceFormat );
// MS Excel VBA: Finds a value matching the search criteria.
// Status: implemented
    virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Find( const css::uno::Any& What, const css::uno::Any& After, const css::uno::Any& LookIn, const css::uno::Any& LookAt, const css::uno::Any& SearchOrder, const css::uno::Any& SearchDirection, const css::uno::Any& MatchCase, const css::uno::Any& MatchByte, const css::uno::Any& SearchFormat );
// MS Excel VBA: Sorts the range by specified keys.
// Status: implemented
	virtual void SAL_CALL Sort( const css::uno::Any& Key1, const css::uno::Any& Order1, const css::uno::Any& Key2, const css::uno::Any& Type, const css::uno::Any& Order2, const css::uno::Any& Key3, const css::uno::Any& Order3, const css::uno::Any& Header, const css::uno::Any& OrderCustom, const css::uno::Any& MatchCase, const css::uno::Any& Orientation, const css::uno::Any& SortMethod,  const css::uno::Any& DataOption1, const css::uno::Any& DataOption2, const css::uno::Any& DataOption3 );
// MS Excel VBA: Returns the last cell in the specified direction.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL End( ::sal_Int32 Direction );
// MS Excel VBA: Returns a Characters object for a substring.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XCharacters > SAL_CALL characters( const css::uno::Any& Start, const css::uno::Any& Length );
// MS Excel VBA: Deletes the range with specified shift.
// Status: implemented
	virtual void SAL_CALL Delete( const css::uno::Any& Shift );
// MS Excel VBA: Returns the Areas collection of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL Areas( const css::uno::Any& );
// MS Excel VBA: Returns a Border object or Border collection.
// Status: implemented
	virtual css::uno::Any SAL_CALL Borders( const css::uno::Any& );
// MS Excel VBA: Sets border around the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL BorderAround( const css::uno::Any& LineStyle,
                const css::uno::Any& Weight, const css::uno::Any& ColorIndex, const css::uno::Any& Color );
// MS Excel VBA: Splits column data into separate columns.
// Status: stubbed - not fully implemented in Calc
    virtual void SAL_CALL TextToColumns( const css::uno::Any& Destination, const css::uno::Any& DataType, const css::uno::Any& TextQualifier,
                const css::uno::Any& ConsecutinveDelimiter, const css::uno::Any& Tab, const css::uno::Any& Semicolon, const css::uno::Any& Comma,
                const css::uno::Any& Space, const css::uno::Any& Other, const css::uno::Any& OtherChar, const css::uno::Any& FieldInfo,
                const css::uno::Any& DecimalSeparator, const css::uno::Any& ThousandsSeparator, const css::uno::Any& TrailingMinusNumbers );
// MS Excel VBA: Returns a Hyperlink by index.
// Status: implemented
	virtual css::uno::Any SAL_CALL Hyperlinks( const css::uno::Any& aIndex );

// MS Excel VBA: Filters the range using AutoFilter criteria.
// Status: implemented
	virtual void SAL_CALL AutoFilter( const css::uno::Any& Field, const css::uno::Any& Criteria1, const css::uno::Any& Operator, const css::uno::Any& Criteria2, const css::uno::Any& VisibleDropDown );
// MS Excel VBA: Inserts cells with specified shift.
// Status: implemented
	virtual void SAL_CALL Insert( const css::uno::Any& Shift, const css::uno::Any& CopyOrigin );
// MS Excel VBA: Automatically fits row height or column width.
// Status: implemented
	virtual void SAL_CALL Autofit();
// MS Excel VBA: Prints the range.
// Status: implemented
	virtual void SAL_CALL PrintOut( const css::uno::Any& From, const css::uno::Any& To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName );
// MS Excel VBA: Fills the range with a series using AutoFill.
// Status: implemented
	virtual void SAL_CALL AutoFill( const css::uno::Reference< ov::excel::XRange >& Destination, const css::uno::Any& Type ) ;
	 void SAL_CALL Calculate(  );
	virtual void SAL_CALL AutoOutline(  );
// MS Excel VBA: Returns a subrange at the specified row and column.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Item( const ::css::uno::Any& row, const css::uno::Any& column );
	virtual void SAL_CALL ClearOutline(  );
	virtual void SAL_CALL Ungroup(  );
	virtual void SAL_CALL Group(  );
// MS Excel VBA: Merges cells in the range.
// Status: implemented
	virtual void SAL_CALL Merge( const css::uno::Any& Across );
	virtual void SAL_CALL UnMerge(  );
// MS Excel VBA: Gets the style of the range.
// Status: implemented
	virtual css::uno::Any SAL_CALL getStyle();
// MS Excel VBA: Sets the style of the range.
// Status: implemented
	virtual void SAL_CALL setStyle( const css::uno::Any& _style );
// MS Excel VBA: Returns the FormatConditions collection.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Reference< ooo::vba::excel::XFormatConditions > SAL_CALL getFormatConditions();
// MS Excel VBA: Returns the QueryTable for the range.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Reference< ooo::vba::excel::XQueryTable > SAL_CALL getQueryTable();
// MS Excel VBA: Returns the next cell in the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Next();
// MS Excel VBA: Returns the previous cell in the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Previous();
	virtual void SAL_CALL RemoveSubtotal(  );
// MS Excel VBA: Returns the MergeArea for the range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL MergeArea();
// MS Excel VBA: Creates subtotals in the range.
// Status: stubbed - not fully implemented in Calc
	virtual void SAL_CALL Subtotal( ::sal_Int32 GroupBy, ::sal_Int32 Function, const css::uno::Sequence< ::sal_Int32 >& TotalList, const css::uno::Any& Replace, const css::uno::Any& PageBreaks, const css::uno::Any& SummaryBelowData );
	// XEnumerationAccess
// MS Excel VBA: Creates an enumeration over cells in the range.
// Status: implemented
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration();
	// XElementAccess
	virtual css::uno::Type SAL_CALL getElementType()
	{
		return ov::excel::XRange::static_type(0);

	}
// MS Excel VBA: Checks if the range has elements.
// Status: implemented
	virtual sal_Bool SAL_CALL hasElements();
	// XDefaultMethod
	::rtl::OUString SAL_CALL getDefaultMethodName(  );
        // XDefaultProperty
        ::rtl::OUString SAL_CALL getDefaultPropertyName(  ) { return ::rtl::OUString::createFromAscii("Value"); }


// #TODO completely rewrite ScVbaRange, its become a hackfest
// it needs to be closer to ScCellRangeBase in that the underlying
// object model should probably be a ScRangelst.
//     * would be nice to be able to construct a range from an address only
//     * or a list of address ( multi-area )
//     * object should be a lightweight as possible
//     * we shouldn't need hacks like this below
// MS Excel VBA: Static helper - creates a Range object from cell references.
// Status: internal helper
	static css::uno::Reference< ov::excel::XRange > ApplicationRange( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Any &Cell1, const css::uno::Any &Cell2 );
// MS Excel VBA: Performs goal seek to find input value for a goal.
// Status: implemented
	virtual sal_Bool SAL_CALL GoalSeek( const css::uno::Any& Goal, const css::uno::Reference< ov::excel::XRange >& ChangingCell );
// MS Excel VBA: Returns a range of cells with specified type.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL SpecialCells( const css::uno::Any& _oType, const css::uno::Any& _oValue);
	// XHelperInterface
// MS Excel VBA: Returns the implementation service name for UNO.
// Status: implemented
	virtual rtl::OUString& getServiceImplName();
// MS Excel VBA: Returns the list of supported service names for UNO.
// Status: implemented
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SC_VBA_RANGE_HXX */
