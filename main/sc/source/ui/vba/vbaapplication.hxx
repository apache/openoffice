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


#ifndef SC_VBA_APPLICATION_HXX
#define SC_VBA_APPLICATION_HXX


#include <ooo/vba/excel/XWorksheetFunction.hpp>
#include <ooo/vba/excel/XApplication.hpp>
#include <ooo/vba/XFileDialog.hpp>
#include <ooo/vba/XFileSearch.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbaapplicationbase.hxx>
#include <cppuhelper/implbase1.hxx>

//typedef InheritedHelperInterfaceImpl1< ov::excel::XApplication > ScVbaApplication_BASE;
typedef cppu::ImplInheritanceHelper1< VbaApplicationBase, ov::excel::XApplication > ScVbaApplication_BASE;

struct ScVbaAppSettings;

class ScVbaApplication : public ScVbaApplication_BASE
{
private:
    // note: member variables moved to struct "ScVbaAppSettings", see cxx file, to be shared by all application instances
    ScVbaAppSettings& mrAppSettings;

	// MS Excel VBA: Returns the system path for the given path type.
	// Status: implemented - uses UNO PathSettings service
	rtl::OUString getOfficePath( const rtl::OUString& sPath );

protected:
    // MS Excel VBA: Returns the current Excel document model.
    // Status: implemented - delegates to excel::getCurrentExcelDoc
    virtual css::uno::Reference< css::frame::XModel > getCurrentDocument();

public:
    // MS Excel VBA: Constructs the Application object with the component context.
    // Status: implemented
	ScVbaApplication( const css::uno::Reference< css::uno::XComponentContext >& m_xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
	virtual ~ScVbaApplication();

    /** Returns true, if VBA document events are enabled. */
    // MS Excel VBA: Returns whether VBA document events are enabled.
    // Status: implemented - checks shared settings
    static bool getDocumentEventsEnabled();

    // MS Excel VBA: Returns the document shell for the given model.
    // Status: implemented - delegates to excel::getDocShell
    virtual SfxObjectShell* GetDocShell( const css::uno::Reference< css::frame::XModel >& xModel );

    // XExactName
    // MS Excel VBA: Returns the exact name of the specified object.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual ::rtl::OUString SAL_CALL getExactName( const ::rtl::OUString& aApproximateName );

    // XInvocation
    // MS Excel VBA: Returns an object that provides introspection of the Application.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual css::uno::Reference< css::beans::XIntrospectionAccess >  SAL_CALL getIntrospection(void);
    // MS Excel VBA: Invokes a method on the Application.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual css::uno::Any  SAL_CALL invoke(const rtl::OUString& FunctionName, const css::uno::Sequence< css::uno::Any >& Params, css::uno::Sequence< sal_Int16 >& OutParamIndex, css::uno::Sequence< css::uno::Any >& OutParam);
    // MS Excel VBA: Sets a property by name.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual void  SAL_CALL setValue(const rtl::OUString& PropertyName, const css::uno::Any& Value);
    // MS Excel VBA: Gets a property by name.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual css::uno::Any  SAL_CALL getValue(const rtl::OUString& PropertyName);
    // MS Excel VBA: Returns true if the method exists.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual sal_Bool  SAL_CALL hasMethod(const rtl::OUString& Name);
    // MS Excel VBA: Returns true if the property exists.
    // Status: implemented - delegates to ScVbaWSFunction
	virtual sal_Bool  SAL_CALL hasProperty(const rtl::OUString& Name);

	// XApplication
    // MS Excel VBA: Sets the default file path for saving files.
    // Status: implemented - uses UNO PathSettings service
	virtual void SAL_CALL setDefaultFilePath( const ::rtl::OUString& DefaultFilePath );
    // MS Excel VBA: Returns the default file path for saving files.
    // Status: implemented - uses UNO PathSettings service
	virtual ::rtl::OUString SAL_CALL getDefaultFilePath();
    // MS Excel VBA: Returns the platform-specific path separator character.
    // Status: implemented
	virtual ::rtl::OUString SAL_CALL getPathSeparator();
    // MS Excel VBA: Returns the path to the VBA library.
    // Status: implemented - uses UNO PathSettings (Basic)
	virtual ::rtl::OUString SAL_CALL getLibraryPath();
    // MS Excel VBA: Returns the path to the templates folder.
    // Status: implemented - uses UNO PathSettings (Template)
	virtual ::rtl::OUString SAL_CALL getTemplatesPath();

    // MS Excel VBA: Returns the application name ("Microsoft Excel").
    // Status: implemented - returns hardcoded "Microsoft Excel"
	virtual rtl::OUString SAL_CALL getName();
    // MS Excel VBA: Returns whether status alerts are displayed.
    // Status: implemented - stored in shared settings
	virtual sal_Bool SAL_CALL getDisplayAlerts();
    // MS Excel VBA: Sets whether status alerts are displayed.
    // Status: implemented - stored in shared settings
	virtual void SAL_CALL setDisplayAlerts( sal_Bool displayAlerts );
    // MS Excel VBA: Returns the calculation mode (automatic, manual, etc.).
    // Status: implemented - delegates to UNO XCalculatable
	virtual ::sal_Int32 SAL_CALL getCalculation();
    // MS Excel VBA: Sets the calculation mode (automatic, manual, etc.).
    // Status: implemented - delegates to UNO XCalculatable
	virtual void SAL_CALL setCalculation( ::sal_Int32 _calculation );
    // MS Excel VBA: Returns the current selection.
    // Status: implemented - supports shapes, cell ranges
	virtual css::uno::Any SAL_CALL getSelection();
    // MS Excel VBA: Returns the active workbook.
    // Status: implemented - creates ScVbaWorkbook if not already a VBA document
	virtual css::uno::Reference< ov::excel::XWorkbook > SAL_CALL getActiveWorkbook();
    // MS Excel VBA: Returns the active cell.
    // Status: implemented - uses view cursor position
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getActiveCell();
    // MS Excel VBA: Returns the active window.
    // Status: implemented - wraps XController
    virtual css::uno::Reference< ov::excel::XWindow > SAL_CALL getActiveWindow();
    // MS Excel VBA: Returns the active sheet.
    // Status: implemented - throws if no active sheet
    virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getActiveSheet();
    // MS Excel VBA: Returns whether the formula bar is displayed.
    // Status: implemented - queries view shell state
	virtual ::sal_Bool SAL_CALL getDisplayFormulaBar();
    // MS Excel VBA: Sets whether the formula bar is displayed.
    // Status: implemented - toggles via SfxRequest
	virtual void SAL_CALL setDisplayFormulaBar( ::sal_Bool _displayformulabar );

    // MS Excel VBA: Returns the Assistant (Office Assistant) object.
    // Status: implemented - creates ScVbaAssistant
    virtual css::uno::Reference< ov::XAssistant > SAL_CALL getAssistant();
    // MS Excel VBA: Returns the workbook where the VBA code is running.
    // Status: implemented - creates ScVbaWorkbook if not already a VBA document
	virtual css::uno::Reference< ov::excel::XWorkbook > SAL_CALL getThisWorkbook();
    // MS Excel VBA: Returns the Workbooks collection or a single workbook by index.
    // Status: implemented - delegates to ScVbaWorkbooks collection
	virtual css::uno::Any SAL_CALL Workbooks( const css::uno::Any& aIndex );
    // MS Excel VBA: Returns the Worksheets collection or a single worksheet by index.
    // Status: implemented - delegates to active workbook's Worksheets
	virtual css::uno::Any SAL_CALL Worksheets( const css::uno::Any& aIndex );
    // MS Excel VBA: Returns the WorksheetFunction object for built-in functions.
    // Status: implemented - creates ScVbaWSFunction
	virtual css::uno::Any SAL_CALL WorksheetFunction( );
    // MS Excel VBA: Evaluates an expression or range reference.
    // Status: partial - only supports range names, not arbitrary expressions
	virtual css::uno::Any SAL_CALL Evaluate( const ::rtl::OUString& Name );
    // MS Excel VBA: Returns the Dialogs collection or a single dialog by index.
    // Status: implemented - delegates to ScVbaDialogs
	virtual css::uno::Any SAL_CALL Dialogs( const css::uno::Any& DialogIndex );
    // MS Excel VBA: Returns the cut/copy mode status.
    // Status: stubbed - always returns False
	virtual css::uno::Any SAL_CALL getCutCopyMode();
    // MS Excel VBA: Sets the cut/copy mode status.
    // Status: stubbed - no-op
	virtual void SAL_CALL setCutCopyMode( const css::uno::Any& _cutcopymode );
    // MS Excel VBA: Returns or sets the status bar text.
    // Status: implemented - delegates to UNO XStatusIndicator
	virtual css::uno::Any SAL_CALL getStatusBar();
    // MS Excel VBA: Sets the status bar text or restores default.
    // Status: implemented - uses UNO XStatusIndicator
	virtual void SAL_CALL setStatusBar( const css::uno::Any& _statusbar );
    // MS Excel VBA: Returns the current mouse cursor pointer style.
    // Status: implemented - maps VCL pointer to XlMousePointer
	virtual ::sal_Int32 SAL_CALL getCursor();
    // MS Excel VBA: Sets the mouse cursor pointer style.
    // Status: implemented - maps XlMousePointer to VCL pointer
	virtual void SAL_CALL setCursor( ::sal_Int32 _cursor );

    // MS Excel VBA: Returns whether events are enabled.
    // Status: implemented - stored in shared settings
	virtual sal_Bool SAL_CALL getEnableEvents();
    // MS Excel VBA: Sets whether events are enabled.
    // Status: implemented - stored in shared settings
	virtual void SAL_CALL setEnableEvents( sal_Bool bEnable );

    // MS Excel VBA: Returns the Windows collection or a single window by index.
    // Status: implemented - delegates to ScVbaWindows
	virtual css::uno::Any SAL_CALL Windows( const css::uno::Any& aIndex );
    // MS Excel VBA: Pauses execution for the specified time (in seconds).
    // Status: implemented - uses WaitUntil BASIC function
	virtual void SAL_CALL wait( double time );
    // MS Excel VBA: Returns the Range object for given cell references.
    // Status: implemented - delegates to ScVbaRange::ApplicationRange
	virtual css::uno::Any SAL_CALL Range( const css::uno::Any& Cell1, const css::uno::Any& Cell2 );
    // MS Excel VBA: Returns the Names collection or a single name by index.
    // Status: implemented - delegates to ScVbaNames
	virtual css::uno::Any SAL_CALL Names( const css::uno::Any& aIndex );
    // MS Excel VBA: Navigates to a range or procedure reference.
    // Status: implemented - supports Range objects and R1C1 strings
	virtual void SAL_CALL GoTo( const css::uno::Any& Reference, const css::uno::Any& Scroll );
    // MS Excel VBA: Recalculates all open workbooks.
    // Status: implemented - calls UNO XCalculatable::calculateAll
	virtual void SAL_CALL Calculate();
    // MS Excel VBA: Returns the intersection of two or more ranges.
    // Status: implemented - handles up to 30 range arguments
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Intersect( const css::uno::Reference< ov::excel::XRange >& Arg1, const css::uno::Reference< ov::excel::XRange >& Arg2, const css::uno::Any& Arg3, const css::uno::Any& Arg4, const css::uno::Any& Arg5, const css::uno::Any& Arg6, const css::uno::Any& Arg7, const css::uno::Any& Arg8, const css::uno::Any& Arg9, const css::uno::Any& Arg10, const css::uno::Any& Arg11, const css::uno::Any& Arg12, const css::uno::Any& Arg13, const css::uno::Any& Arg14, const css::uno::Any& Arg15, const css::uno::Any& Arg16, const css::uno::Any& Arg17, const css::uno::Any& Arg18, const css::uno::Any& Arg19, const css::uno::Any& Arg20, const css::uno::Any& Arg21, const css::uno::Any& Arg22, const css::uno::Any& Arg23, const css::uno::Any& Arg24, const css::uno::Any& Arg25, const css::uno::Any& Arg26, const css::uno::Any& Arg27, const css::uno::Any& Arg28, const css::uno::Any& Arg29, const css::uno::Any& Arg30 );
    // MS Excel VBA: Returns the union of two or more ranges.
    // Status: implemented - handles up to 30 range arguments
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Union( const css::uno::Reference< ov::excel::XRange >& Arg1, const css::uno::Reference< ov::excel::XRange >& Arg2, const css::uno::Any& Arg3, const css::uno::Any& Arg4, const css::uno::Any& Arg5, const css::uno::Any& Arg6, const css::uno::Any& Arg7, const css::uno::Any& Arg8, const css::uno::Any& Arg9, const css::uno::Any& Arg10, const css::uno::Any& Arg11, const css::uno::Any& Arg12, const css::uno::Any& Arg13, const css::uno::Any& Arg14, const css::uno::Any& Arg15, const css::uno::Any& Arg16, const css::uno::Any& Arg17, const css::uno::Any& Arg18, const css::uno::Any& Arg19, const css::uno::Any& Arg20, const css::uno::Any& Arg21, const css::uno::Any& Arg22, const css::uno::Any& Arg23, const css::uno::Any& Arg24, const css::uno::Any& Arg25, const css::uno::Any& Arg26, const css::uno::Any& Arg27, const css::uno::Any& Arg28, const css::uno::Any& Arg29, const css::uno::Any& Arg30 );
    // MS Excel VBA: Marks a cell as volatile (recalculated on every recalc).
    // Status: implemented - accepts any value (ignored in implementation)
	virtual void SAL_CALL Volatile( const css::uno::Any& Volatile );
    // MS Excel VBA: Yields control to the operating system for event processing.
    // Status: implemented - currently a no-op
	virtual void SAL_CALL DoEvents();
    // MS Excel VBA: Returns the caller of a macro.
    // Status: implemented - delegates to FuncCaller BASIC function
	virtual css::uno::Any SAL_CALL Caller( const css::uno::Any& aIndex );
    // MS Excel VBA: Displays a dialog for opening files and returns selected filename(s).
    // Status: implemented - uses ooo.vba.OpenFilePicker service
	virtual css::uno::Any SAL_CALL GetOpenFilename( const css::uno::Any& rFileFilter, const css::uno::Any& rFilterIndex, const css::uno::Any& rTitle, const css::uno::Any& rButtonText, const css::uno::Any& rMultiSelect );
    // MS Excel VBA: Displays a dialog for saving files and returns selected filename.
    // Status: implemented - uses ooo.vba.SaveAsFilePicker service
	virtual css::uno::Any SAL_CALL GetSaveAsFilename( const css::uno::Any& rInitialFileName, const css::uno::Any& rFileFilter, const css::uno::Any& rFilterIndex, const css::uno::Any& rTitle, const css::uno::Any& rButtonText );

    // MS Excel VBA: Returns the FileDialog object for the specified type.
    // Status: implemented - creates ScVbaFileDialog
	virtual css::uno::Reference< ooo::vba::XFileDialog > SAL_CALL FileDialog( sal_Int32 nFileDialogType );
    // MS Excel VBA: Returns the FileSearch object for searching files.
    // Status: implemented - creates ScVbaFileSearch
	virtual css::uno::Reference< ooo::vba::XFileSearch > SAL_CALL getFileSearch();

    // MS Excel VBA: Counts non-empty cells in the argument (delegates to WorksheetFunction.CountA).
    // Status: implemented - wraps WorksheetFunction.CountA
	virtual double SAL_CALL CountA( const css::uno::Any& aArg1 );
    // MS Excel VBA: Returns the MenuBars collection.
    // Status: stubbed - returns empty Any
    virtual css::uno::Any SAL_CALL MenuBars( const css::uno::Any& aIndex );
    // MS Excel VBA: Returns locale-specific information for the application.
    // Status: stubbed - returns empty Any
    virtual css::uno::Any SAL_CALL International( sal_Int32 nIndex );
    // MS Excel VBA: Undoes the last user action.
    // Status: stubbed - no-op
	virtual void SAL_CALL Undo();
    // MS Excel VBA: Converts inches to points (1 inch = 72 points).
    // Status: implemented - simple multiplication
	virtual double SAL_CALL InchesToPoints( double nInches );
    // MS Excel VBA: Sets the number of sheets in new workbooks.
    // Status: implemented - uses ScAppOptions
	virtual void SAL_CALL setSheetsInNewWorkbook( sal_Int32 nSheets );
    // MS Excel VBA: Returns the number of sheets in new workbooks.
    // Status: implemented - uses ScAppOptions
	virtual sal_Int32 SAL_CALL getSheetsInNewWorkbook();

	// XHelperInterface
    // MS Excel VBA: Returns the implementation name of this service.
    // Status: implemented - returns "ScVbaApplication"
	virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names supported by this implementation.
    // Status: implemented - returns "ooo.vba.excel.Application"
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SC_VBA_APPLICATION_HXX */
