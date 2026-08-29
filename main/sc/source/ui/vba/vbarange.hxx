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

class ArrayVisitor
{
public:
	virtual void visitNode( sal_Int32 x, sal_Int32 y, const css::uno::Reference< css::table::XCell >& xCell ) = 0;
	virtual	~ArrayVisitor(){}
};

class ValueSetter : public ArrayVisitor
{
public:
	virtual bool processValue( const css::uno::Any& aValue, const css::uno::Reference< css::table::XCell >& xCell ) = 0;


};

class ValueGetter : public ArrayVisitor
{

public:
	virtual void processValue( sal_Int32 x, sal_Int32 y, const css::uno::Any& aValue ) = 0;
	virtual const css::uno::Any& getValue() const = 0;
};



class ScVbaRange : public ScVbaRange_BASE
{
	css::uno::Reference< ov::XCollection > m_Areas;
	css::uno::Reference< ov::XCollection > m_Borders;
	css::uno::Reference< css::table::XCellRange > mxRange;
	css::uno::Reference< css::sheet::XSheetCellRangeContainer > mxRanges;
	sal_Bool mbIsRows;
	sal_Bool mbIsColumns;
	css::uno::Reference< ov::excel::XValidation > m_xValidation;

	double getCalcColWidth( const css::table::CellRangeAddress& );
	double getCalcRowHeight( const css::table::CellRangeAddress& );
	void visitArray( ArrayVisitor& vistor );

	css::uno::Reference< ov::excel::XRange > getEntireColumnOrRow( bool bColumn = true );

	void fillSeries(  css::sheet::FillDirection nFillDirection, css::sheet::FillMode nFillMode, css::sheet::FillDateMode nFillDateMode, double fStep, double fEndValue );

	void ClearContents( sal_Int32 nFlags, bool bFireEvent );

	css::uno::Any getValue( ValueGetter& rValueGetter );
	void setValue( const css::uno::Any& aValue, ValueSetter& setter, bool bFireEvent );

	css::uno::Any getFormulaValue( formula::FormulaGrammar::Grammar );
	void setFormulaValue( const css::uno::Any& aValue, formula::FormulaGrammar::Grammar, bool bFireEvent );

	css::uno::Reference< ov::excel::XRange > getArea( sal_Int32 nIndex  );
	ScCellRangeObj* getCellRangeObj( );
    ScCellRangesObj* getCellRangesObj();
	css::uno::Reference< ov::XCollection >& getBorders();
	void groupUnGroup( bool bUnGroup = false );
 	css::uno::Reference< ov::excel::XRange > PreviousNext( bool bIsPrevious );
 	css::uno::Reference< ov::excel::XRange > SpecialCellsImpl( sal_Int32 nType, const css::uno::Any& _oValue);
	css::awt::Point getPosition();

    /** Fires a Worksheet_Change event for this range or range list. */
	void fireChangeEvent();

protected:
	virtual ScCellRangesBase* getCellRangesBase();
	virtual SfxItemSet* getCurrentDataSet( );
public:
	ScVbaRange( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::table::XCellRange >& xRange, sal_Bool bIsRows = false, sal_Bool bIsColumns = false );
	ScVbaRange( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::sheet::XSheetCellRangeContainer >& xRanges, sal_Bool bIsRows = false, sal_Bool bIsColumns = false );
	ScVbaRange( css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );

	ScDocument* getScDocument();
	ScDocShell* getScDocShell();

    /** Returns the ScVbaRange implementation object for the passed VBA Range object. */
	static ScVbaRange* getImplementation( const css::uno::Reference< ov::excel::XRange >& rxRange );

	css::uno::Reference< css::frame::XModel > getUnoModel();
	static css::uno::Reference< css::frame::XModel > getUnoModel( const css::uno::Reference< ov::excel::XRange >& rxRange );

	const ScRangeList& getScRangeList();
	static const ScRangeList& getScRangeList( const css::uno::Reference< ov::excel::XRange >& rxRange );

	virtual ~ScVbaRange();
 	virtual css::uno::Reference< ov::XHelperInterface > thisHelperIface() { return this; }
	bool isSingleCellRange();

    static css::uno::Reference< ov::excel::XRange > getRangeObjectForName(
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const rtl::OUString& sRangeName, ScDocShell* pDocSh,
        formula::FormulaGrammar::AddressConvention eConv = formula::FormulaGrammar::CONV_XL_A1  );

    static css::uno::Reference< ov::excel::XRange > CellsHelper(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::table::XCellRange >& xRange,
        const css::uno::Any &nRowIndex, const css::uno::Any &nColumnIndex );

    // Attributes
	virtual css::uno::Any SAL_CALL getValue();
	virtual void   SAL_CALL setValue( const css::uno::Any& aValue );
	virtual css::uno::Any SAL_CALL getFormula();
	virtual void   SAL_CALL setFormula( const css::uno::Any& rFormula );
	virtual css::uno::Any SAL_CALL getFormulaArray();
	virtual void   SAL_CALL setFormulaArray(const css::uno::Any& rFormula);
	virtual css::uno::Any SAL_CALL getFormulaR1C1();
	virtual void   SAL_CALL setFormulaR1C1( const css::uno::Any &rFormula );
	virtual ::sal_Int32 SAL_CALL getCount();
	virtual ::sal_Int32 SAL_CALL getRow();
	virtual ::sal_Int32 SAL_CALL getColumn();
	virtual ::rtl::OUString SAL_CALL getText();
    using ScVbaRange_BASE::setNumberFormat;
	virtual void SAL_CALL setNumberFormat( const css::uno::Any& rNumberFormat );
	virtual css::uno::Any SAL_CALL getNumberFormat();
	virtual void SAL_CALL setMergeCells( const css::uno::Any& bMerge );
	virtual css::uno::Any SAL_CALL getMergeCells();
	virtual void SAL_CALL setWrapText( const css::uno::Any& bIsWrapped );
	virtual css::uno::Any SAL_CALL getWrapText();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getEntireRow();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getEntireColumn();
	virtual css::uno::Reference< ov::excel::XComment > SAL_CALL getComment();
	virtual css::uno::Any SAL_CALL getHidden();
	virtual void SAL_CALL setHidden( const css::uno::Any& _hidden );
	virtual css::uno::Any SAL_CALL getColumnWidth();
	virtual void SAL_CALL setColumnWidth( const css::uno::Any& _columnwidth );
	virtual css::uno::Any SAL_CALL getRowHeight();
	virtual void SAL_CALL setRowHeight( const css::uno::Any& _rowheight );
	virtual css::uno::Any SAL_CALL getWidth();
	virtual css::uno::Any SAL_CALL getHeight();
	virtual css::uno::Any SAL_CALL getTop();
	virtual css::uno::Any SAL_CALL getLeft();

	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getWorksheet();
	virtual css::uno::Any SAL_CALL getPageBreak();
	virtual void SAL_CALL setPageBreak( const css::uno::Any& _pagebreak );
	virtual css::uno::Reference< ov::excel::XValidation > SAL_CALL getValidation();
	virtual css::uno::Any SAL_CALL getPrefixCharacter();
	virtual css::uno::Any SAL_CALL getShowDetail();
	virtual void SAL_CALL setShowDetail(const css::uno::Any& aShowDetail);
	// Methods
	sal_Bool IsRows() { return mbIsRows; }
	sal_Bool IsColumns() { return mbIsColumns; }
	virtual css::uno::Reference< ov::excel::XComment > SAL_CALL AddComment( const css::uno::Any& Text );
	virtual void SAL_CALL Clear();
	virtual void SAL_CALL ClearComments();
	virtual void SAL_CALL ClearContents();
	virtual void SAL_CALL ClearFormats();
	virtual css::uno::Any SAL_CALL HasFormula();
	virtual void SAL_CALL FillLeft();
	virtual void SAL_CALL FillRight();
	virtual void SAL_CALL FillUp();
	virtual void SAL_CALL FillDown();
	virtual	css::uno::Reference< ov::excel::XRange > SAL_CALL Offset( const css::uno::Any &nRowOffset, const css::uno::Any &nColOffset );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL CurrentRegion();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL CurrentArray();
	virtual ::rtl::OUString SAL_CALL Characters( const css::uno::Any& nIndex, const css::uno::Any& nCount );

	virtual ::rtl::OUString SAL_CALL Address( const css::uno::Any& RowAbsolute, const css::uno::Any& ColumnAbsolute, const css::uno::Any& ReferenceStyle, const css::uno::Any& External, const css::uno::Any& RelativeTo );

	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Cells( const css::uno::Any &nRow, const css::uno::Any &nCol );
	virtual void SAL_CALL Select();
	virtual void SAL_CALL Activate();
	virtual css::uno::Reference< ov::excel::XRange >  SAL_CALL Rows( const css::uno::Any& nIndex );
	virtual css::uno::Reference< ov::excel::XRange >  SAL_CALL Columns( const css::uno::Any &nIndex );
	virtual void SAL_CALL Copy( const css::uno::Any& Destination );
	virtual void SAL_CALL Cut( const css::uno::Any& Destination );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Resize( const css::uno::Any& RowSize, const css::uno::Any& ColumnSize );
	virtual css::uno::Reference< ov::excel::XFont > SAL_CALL Font();
	virtual css::uno::Reference< ov::excel::XInterior > SAL_CALL Interior(  ) ;
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Range( const css::uno::Any &Cell1, const css::uno::Any &Cell2 );
	virtual css::uno::Reference< ov::excel::XRange > Range( const css::uno::Any &Cell1, const css::uno::Any &Cell2, bool bForceUseInpuRangeTab );
	virtual css::uno::Any SAL_CALL getCellRange(  );
	static css::uno::Any getCellRange( const css::uno::Reference< ov::excel::XRange >& rxRange );
	virtual void SAL_CALL PasteSpecial( const css::uno::Any& Paste, const css::uno::Any& Operation, const css::uno::Any& SkipBlanks, const css::uno::Any& Transpose );
	virtual ::sal_Bool SAL_CALL Replace( const ::rtl::OUString& What, const ::rtl::OUString& Replacement, const css::uno::Any& LookAt, const css::uno::Any& SearchOrder, const css::uno::Any& MatchCase, const css::uno::Any& MatchByte, const css::uno::Any& SearchFormat, const css::uno::Any& ReplaceFormat );
    virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Find( const css::uno::Any& What, const css::uno::Any& After, const css::uno::Any& LookIn, const css::uno::Any& LookAt, const css::uno::Any& SearchOrder, const css::uno::Any& SearchDirection, const css::uno::Any& MatchCase, const css::uno::Any& MatchByte, const css::uno::Any& SearchFormat );
	virtual void SAL_CALL Sort( const css::uno::Any& Key1, const css::uno::Any& Order1, const css::uno::Any& Key2, const css::uno::Any& Type, const css::uno::Any& Order2, const css::uno::Any& Key3, const css::uno::Any& Order3, const css::uno::Any& Header, const css::uno::Any& OrderCustom, const css::uno::Any& MatchCase, const css::uno::Any& Orientation, const css::uno::Any& SortMethod,  const css::uno::Any& DataOption1, const css::uno::Any& DataOption2, const css::uno::Any& DataOption3 );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL End( ::sal_Int32 Direction );
	virtual css::uno::Reference< ov::excel::XCharacters > SAL_CALL characters( const css::uno::Any& Start, const css::uno::Any& Length );
	virtual void SAL_CALL Delete( const css::uno::Any& Shift );
	virtual css::uno::Any SAL_CALL Areas( const css::uno::Any& );
	virtual css::uno::Any SAL_CALL Borders( const css::uno::Any& );
	virtual css::uno::Any SAL_CALL BorderAround( const css::uno::Any& LineStyle,
                const css::uno::Any& Weight, const css::uno::Any& ColorIndex, const css::uno::Any& Color );
    virtual void SAL_CALL TextToColumns( const css::uno::Any& Destination, const css::uno::Any& DataType, const css::uno::Any& TextQualifier,
                const css::uno::Any& ConsecutinveDelimiter, const css::uno::Any& Tab, const css::uno::Any& Semicolon, const css::uno::Any& Comma,
                const css::uno::Any& Space, const css::uno::Any& Other, const css::uno::Any& OtherChar, const css::uno::Any& FieldInfo,
                const css::uno::Any& DecimalSeparator, const css::uno::Any& ThousandsSeparator, const css::uno::Any& TrailingMinusNumbers );
	virtual css::uno::Any SAL_CALL Hyperlinks( const css::uno::Any& aIndex );

	virtual void SAL_CALL AutoFilter( const css::uno::Any& Field, const css::uno::Any& Criteria1, const css::uno::Any& Operator, const css::uno::Any& Criteria2, const css::uno::Any& VisibleDropDown );
	virtual void SAL_CALL Insert( const css::uno::Any& Shift, const css::uno::Any& CopyOrigin );
	virtual void SAL_CALL Autofit();
	virtual void SAL_CALL PrintOut( const css::uno::Any& From, const css::uno::Any& To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName );
	virtual void SAL_CALL AutoFill( const css::uno::Reference< ov::excel::XRange >& Destination, const css::uno::Any& Type ) ;
	 void SAL_CALL Calculate(  );
	virtual void SAL_CALL AutoOutline(  );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Item( const ::css::uno::Any& row, const css::uno::Any& column );
	virtual void SAL_CALL ClearOutline(  );
	virtual void SAL_CALL Ungroup(  );
	virtual void SAL_CALL Group(  );
	virtual void SAL_CALL Merge( const css::uno::Any& Across );
	virtual void SAL_CALL UnMerge(  );
	virtual css::uno::Any SAL_CALL getStyle();
	virtual void SAL_CALL setStyle( const css::uno::Any& _style );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Next();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Previous();
	virtual void SAL_CALL RemoveSubtotal(  );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL MergeArea();
	virtual void SAL_CALL Subtotal( ::sal_Int32 GroupBy, ::sal_Int32 Function, const css::uno::Sequence< ::sal_Int32 >& TotalList, const css::uno::Any& Replace, const css::uno::Any& PageBreaks, const css::uno::Any& SummaryBelowData );
	// XEnumerationAccess
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration();
	// XElementAccess
	virtual css::uno::Type SAL_CALL getElementType()
	{
		return ov::excel::XRange::static_type(0);

	}
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
	static css::uno::Reference< ov::excel::XRange > ApplicationRange( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Any &Cell1, const css::uno::Any &Cell2 );
	virtual sal_Bool SAL_CALL GoalSeek( const css::uno::Any& Goal, const css::uno::Reference< ov::excel::XRange >& ChangingCell );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL SpecialCells( const css::uno::Any& _oType, const css::uno::Any& _oValue);
	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SC_VBA_RANGE_HXX */
