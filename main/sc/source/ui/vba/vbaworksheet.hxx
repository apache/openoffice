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


#ifndef SC_VBA_WORKSHEET_HXX
#define SC_VBA_WORKSHEET_HXX

#include <cppuhelper/implbase2.hxx>
#include <comphelper/unwrapargs.hxx>

#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/script/XInvocation.hpp>
#include <ooo/vba/excel/XWorksheet.hpp>
#include <ooo/vba/excel/XComments.hpp>
#include <ooo/vba/excel/XRange.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <ooo/vba/excel/XOutline.hpp>
#include <ooo/vba/excel/XPageSetup.hpp>
#include <ooo/vba/excel/XHPageBreaks.hpp>
#include <ooo/vba/excel/XVPageBreaks.hpp>

#include <vbahelper/vbahelperinterface.hxx>
#include "address.hxx"

namespace ooo { namespace vba { namespace excel {
    class XChartObjects;
    class XHyperlinks;
} } }

class ScVbaSheetObjectsBase;

typedef InheritedHelperInterfaceImpl1< ov::excel::XWorksheet >  WorksheetImpl_BASE;

class ScVbaWorksheet : public WorksheetImpl_BASE
{
	css::uno::Reference< css::sheet::XSpreadsheet > mxSheet;
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference< ov::excel::XChartObjects > mxCharts;
	css::uno::Reference< ov::excel::XHyperlinks > mxHlinks;
	::rtl::Reference< ScVbaSheetObjectsBase > mxButtons;
	bool mbVeryHidden;

// MS Excel VBA: Internal helper - gets a worksheet by offset.
// Status: internal helper
	css::uno::Reference< ov::excel::XWorksheet > getSheetAtOffset(SCTAB offset);
// MS Excel VBA: Internal helper - gets the entire used range of the sheet.
// Status: internal helper
	css::uno::Reference< ov::excel::XRange > getSheetRange();

// MS Excel VBA: Internal helper - gets form controls of the sheet.
// Status: internal helper
	css::uno::Reference< css::container::XNameAccess > getFormControls();
// MS Excel VBA: Internal helper - gets a control shape by name.
// Status: internal helper
	css::uno::Any getControlShape( const rtl::OUString& sName );

// MS Excel VBA: Internal helper - gets first database range properties.
// Status: internal helper
	css::uno::Reference< css::beans::XPropertySet > getFirstDBRangeProperties();

protected:

// MS Excel VBA: Protected constructor (internal use only).
// Status: internal helper
	ScVbaWorksheet( const css::uno::Reference< ov::XHelperInterface >& xParent,  const css::uno::Reference< css::uno::XComponentContext >& xContext );
public:
// MS Excel VBA: Constructor taking a spreadsheet and model.
// Status: implemented
	ScVbaWorksheet( const css::uno::Reference< ov::XHelperInterface >& xParent,
		const css::uno::Reference< css::uno::XComponentContext >& xContext,
		const css::uno::Reference< css::sheet::XSpreadsheet >& xSheet,
		const css::uno::Reference< css::frame::XModel >& xModel )  ;
// MS Excel VBA: Constructor taking arguments sequence (for UNO create).
// Status: implemented
	ScVbaWorksheet( css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );

// MS Excel VBA: Destructor.
// Status: implemented
	virtual ~ScVbaWorksheet();

// MS Excel VBA: Gets the underlying model object.
// Status: implemented
	virtual css::uno::Reference< css::frame::XModel > getModel()
	{ return mxModel; }
// MS Excel VBA: Gets the underlying spreadsheet object.
// Status: implemented
	virtual css::uno::Reference< css::sheet::XSpreadsheet > getSheet()
	{ return mxSheet; }

    // Attributes
// MS Excel VBA: Gets the name of the worksheet.
// Status: implemented
	virtual ::rtl::OUString SAL_CALL getName();
// MS Excel VBA: Sets the name of the worksheet.
// Status: implemented
	virtual void SAL_CALL setName( const ::rtl::OUString &rName );
// MS Excel VBA: Gets the visibility of the worksheet.
// Status: implemented
	virtual sal_Int32 SAL_CALL getVisible();
// MS Excel VBA: Sets the visibility of the worksheet.
// Status: implemented
	virtual void SAL_CALL setVisible( sal_Int32 nVisible );
// MS Excel VBA: Gets the standard column width.
// Status: implemented
	virtual ::sal_Int32 SAL_CALL getStandardWidth();
// MS Excel VBA: Gets the standard row height.
// Status: implemented
	virtual ::sal_Int32 SAL_CALL getStandardHeight();
// MS Excel VBA: Gets whether the worksheet is in protection mode.
// Status: implemented
	virtual ::sal_Bool SAL_CALL getProtectionMode();
// MS Excel VBA: Gets whether contents are protected.
// Status: implemented
	virtual ::sal_Bool SAL_CALL getProtectContents();
// MS Excel VBA: Gets whether drawing objects are protected.
// Status: implemented
	virtual ::sal_Bool SAL_CALL getProtectDrawingObjects();
// MS Excel VBA: Gets whether scenarios are protected.
// Status: implemented
	virtual ::sal_Bool SAL_CALL getProtectScenarios();
// MS Excel VBA: Returns the used range of the worksheet.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getUsedRange() ;
// MS Excel VBA: Returns a ChartObject by index.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Any SAL_CALL ChartObjects( const css::uno::Any& Index );
// MS Excel VBA: Returns the outline for the worksheet.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Reference< ov::excel::XOutline > SAL_CALL Outline( );
// MS Excel VBA: Returns the page setup for the worksheet.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Reference< ov::excel::XPageSetup > SAL_CALL PageSetup( );
// MS Excel VBA: Returns a HPageBreaks collection.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Any SAL_CALL HPageBreaks( const css::uno::Any& aIndex );
// MS Excel VBA: Returns a VPageBreaks collection.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Any SAL_CALL VPageBreaks( const css::uno::Any& aIndex );
// MS Excel VBA: Returns the next worksheet.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getNext();
// MS Excel VBA: Returns the previous worksheet.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getPrevious();
// MS Excel VBA: Returns the index of the worksheet.
// Status: implemented
 	virtual sal_Int16 SAL_CALL getIndex();
// MS Excel VBA: Gets the selection enabled state for the worksheet.
// Status: implemented
 	virtual sal_Int32 SAL_CALL getEnableSelection();
// MS Excel VBA: Sets the selection enabled state for the worksheet.
// Status: implemented
 	virtual void SAL_CALL setEnableSelection( sal_Int32 nSelection );
// MS Excel VBA: Gets whether AutoFilter mode is enabled.
// Status: implemented
	virtual sal_Bool SAL_CALL getAutoFilterMode();
// MS Excel VBA: Sets whether AutoFilter mode is enabled.
// Status: implemented
	virtual void SAL_CALL setAutoFilterMode( sal_Bool bAutoFilterMode );

    // Methods
// MS Excel VBA: Activates the worksheet.
// Status: implemented
	virtual void SAL_CALL Activate();
// MS Excel VBA: Selects the worksheet.
// Status: implemented
	virtual void SAL_CALL Select();
// MS Excel VBA: Returns a Range object representing a cell or range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Range( const css::uno::Any& Cell1, const css::uno::Any& Cell2 );
// MS Excel VBA: Moves the worksheet to a new location.
// Status: implemented
	virtual void SAL_CALL Move( const css::uno::Any& Before, const css::uno::Any& After ) ;
// MS Excel VBA: Copies the worksheet to a new location.
// Status: implemented
 	virtual void SAL_CALL Copy( const css::uno::Any& Before, const css::uno::Any& After );
// MS Excel VBA: Pastes clipboard contents to the destination.
// Status: implemented
	virtual void SAL_CALL Paste( const css::uno::Any& Destination, const css::uno::Any& Link );
// MS Excel VBA: Deletes the worksheet.
// Status: implemented
	virtual void SAL_CALL Delete(  );
// MS Excel VBA: Protects the worksheet with a password and options.
// Status: implemented - partial (subset of options supported)
	virtual void SAL_CALL Protect( const css::uno::Any& Password, const css::uno::Any& DrawingObjects, const css::uno::Any& Contents, const css::uno::Any& Scenarios, const css::uno::Any& UserInterfaceOnly );
// MS Excel VBA: Removes protection from the worksheet.
// Status: implemented
	virtual void SAL_CALL Unprotect( const css::uno::Any& Password );

// MS Excel VBA: Calculates all open workbooks and worksheets.
// Status: implemented
	virtual void SAL_CALL Calculate(  );
// MS Excel VBA: Checks spelling of the worksheet text.
// Status: implemented
	virtual void SAL_CALL CheckSpelling( const css::uno::Any& CustomDictionary,const css::uno::Any& IgnoreUppercase,const css::uno::Any& AlwaysSuggest, const css::uno::Any& SpellingLang );
	// Hacks (?)
// MS Excel VBA: Returns a single cell or cell range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Cells( const css::uno::Any &nRow, const css::uno::Any &nCol );
// MS Excel VBA: Returns a single row or row range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Rows(const css::uno::Any& aIndex );
// MS Excel VBA: Returns a single column or column range.
// Status: implemented
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Columns(const css::uno::Any& aIndex );

// MS Excel VBA: Evaluates an Excel formula expression.
// Status: implemented
	virtual css::uno::Any SAL_CALL Evaluate( const ::rtl::OUString& Name );
// MS Excel VBA: Returns a PivotTable by index.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Any SAL_CALL PivotTables( const css::uno::Any& Index );
// MS Excel VBA: Returns a Comment by index.
// Status: implemented
	virtual css::uno::Any SAL_CALL Comments( const css::uno::Any& Index );
// MS Excel VBA: Returns a Hyperlink by index.
// Status: implemented
	virtual css::uno::Any SAL_CALL Hyperlinks( const css::uno::Any& aIndex );
// MS Excel VBA: Returns a Name object by index.
// Status: implemented
    virtual css::uno::Any SAL_CALL Names( const css::uno::Any& aIndex );

// MS Excel VBA: Returns an OLEObject by index.
// Status: stubbed - not fully implemented in Calc
	virtual css::uno::Any SAL_CALL OLEObjects( const css::uno::Any& Index );
// MS Excel VBA: Returns a Shape object by index.
// Status: implemented
	virtual css::uno::Any SAL_CALL Shapes( const css::uno::Any& aIndex );

// MS Excel VBA: Returns Button controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL Buttons( const css::uno::Any& rIndex );
// MS Excel VBA: Returns CheckBox controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL CheckBoxes( const css::uno::Any& rIndex );
// MS Excel VBA: Returns DropDown controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL DropDowns( const css::uno::Any& rIndex );
// MS Excel VBA: Returns GroupBox controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL GroupBoxes( const css::uno::Any& rIndex );
// MS Excel VBA: Returns Label controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL Labels( const css::uno::Any& rIndex );
// MS Excel VBA: Returns ListBox controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL ListBoxes( const css::uno::Any& rIndex );
// MS Excel VBA: Returns OptionButton controls on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL OptionButtons( const css::uno::Any& rIndex );
// MS Excel VBA: Returns ScrollBars control objects on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL ScrollBars( const css::uno::Any& rIndex );
// MS Excel VBA: Returns Spinners control objects on the worksheet.
// Status: not implemented in Calc
    virtual css::uno::Any SAL_CALL Spinners( const css::uno::Any& rIndex );

// MS Excel VBA: Sets whether calculation is enabled on the worksheet.
// Status: implemented
	virtual void SAL_CALL setEnableCalculation( ::sal_Bool EnableCalculation );
// MS Excel VBA: Gets whether calculation is enabled on the worksheet.
// Status: implemented
	virtual ::sal_Bool SAL_CALL getEnableCalculation(  );
// MS Excel VBA: Displays the data form for entering/editing data.
// Status: implemented
	virtual void SAL_CALL ShowDataForm(  );
	// XInvocation
// MS Excel VBA: Returns introspection access (XInvocation).
// Status: implemented
	virtual css::uno::Reference< css::beans::XIntrospectionAccess > SAL_CALL getIntrospection(  );
// MS Excel VBA: Invokes a method by name (XInvocation).
// Status: implemented
	virtual css::uno::Any SAL_CALL invoke( const ::rtl::OUString& aFunctionName, const css::uno::Sequence< css::uno::Any >& aParams, css::uno::Sequence< ::sal_Int16 >& aOutParamIndex, css::uno::Sequence< css::uno::Any >& aOutParam );
// MS Excel VBA: Sets the value of a property by name.
// Status: implemented
	virtual void SAL_CALL setValue( const ::rtl::OUString& aPropertyName, const css::uno::Any& aValue );
// MS Excel VBA: Gets the value of a property by name.
// Status: implemented
	virtual css::uno::Any SAL_CALL getValue( const ::rtl::OUString& aPropertyName );
// MS Excel VBA: Indicates if a method exists on this object.
// Status: implemented
	virtual ::sal_Bool SAL_CALL hasMethod( const ::rtl::OUString& aName );
// MS Excel VBA: Indicates if a property exists on this object.
// Status: implemented
	virtual ::sal_Bool SAL_CALL hasProperty( const ::rtl::OUString& aName );
    // CodeName
// MS Excel VBA: Returns the code name of the worksheet.
// Status: implemented
    virtual rtl::OUString SAL_CALL getCodeName();
// MS Excel VBA: Returns the sheet ID for this worksheet.
// Status: implemented
    sal_Int16 getSheetID();

// MS Excel VBA: Prints the worksheet.
// Status: implemented
	virtual void SAL_CALL PrintOut( const css::uno::Any& From, const css::uno::Any& To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName, const css::uno::Any& IgnorePrintAreas );
	// XHelperInterface
// MS Excel VBA: Returns the implementation service name for UNO.
// Status: implemented
	virtual rtl::OUString& getServiceImplName();
// MS Excel VBA: Returns the list of supported service names for UNO.
// Status: implemented
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SC_VBA_WORKSHEET_HXX */
