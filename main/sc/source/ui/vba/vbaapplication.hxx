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

	rtl::OUString getOfficePath( const rtl::OUString& sPath );

protected:
    virtual css::uno::Reference< css::frame::XModel > getCurrentDocument();

public:
	ScVbaApplication( const css::uno::Reference< css::uno::XComponentContext >& m_xContext );
	virtual ~ScVbaApplication();

    /** Returns true, if VBA document events are enabled. */
    static bool getDocumentEventsEnabled();

    virtual SfxObjectShell* GetDocShell( const css::uno::Reference< css::frame::XModel >& xModel );

    // XExactName
	virtual ::rtl::OUString SAL_CALL getExactName( const ::rtl::OUString& aApproximateName );

    // XInvocation
	virtual css::uno::Reference< css::beans::XIntrospectionAccess >  SAL_CALL getIntrospection(void);
	virtual css::uno::Any  SAL_CALL invoke(const rtl::OUString& FunctionName, const css::uno::Sequence< css::uno::Any >& Params, css::uno::Sequence< sal_Int16 >& OutParamIndex, css::uno::Sequence< css::uno::Any >& OutParam);
	virtual void  SAL_CALL setValue(const rtl::OUString& PropertyName, const css::uno::Any& Value);
	virtual css::uno::Any  SAL_CALL getValue(const rtl::OUString& PropertyName);
	virtual sal_Bool  SAL_CALL hasMethod(const rtl::OUString& Name);
	virtual sal_Bool  SAL_CALL hasProperty(const rtl::OUString& Name);

	// XApplication
	virtual void SAL_CALL setDefaultFilePath( const ::rtl::OUString& DefaultFilePath );
	virtual ::rtl::OUString SAL_CALL getDefaultFilePath();
	virtual ::rtl::OUString SAL_CALL getPathSeparator();
	virtual ::rtl::OUString SAL_CALL getLibraryPath();
	virtual ::rtl::OUString SAL_CALL getTemplatesPath();

	virtual rtl::OUString SAL_CALL getName();
	virtual sal_Bool SAL_CALL getDisplayAlerts();
	virtual void SAL_CALL setDisplayAlerts( sal_Bool displayAlerts );
	virtual ::sal_Int32 SAL_CALL getCalculation();
	virtual void SAL_CALL setCalculation( ::sal_Int32 _calculation );
	virtual css::uno::Any SAL_CALL getSelection();
	virtual css::uno::Reference< ov::excel::XWorkbook > SAL_CALL getActiveWorkbook();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getActiveCell();
    virtual css::uno::Reference< ov::excel::XWindow > SAL_CALL getActiveWindow();
    virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getActiveSheet();
	virtual ::sal_Bool SAL_CALL getDisplayFormulaBar();
	virtual void SAL_CALL setDisplayFormulaBar( ::sal_Bool _displayformulabar );

    virtual css::uno::Reference< ov::XAssistant > SAL_CALL getAssistant();
	virtual css::uno::Reference< ov::excel::XWorkbook > SAL_CALL getThisWorkbook();
	virtual css::uno::Any SAL_CALL Workbooks( const css::uno::Any& aIndex );
	virtual css::uno::Any SAL_CALL Worksheets( const css::uno::Any& aIndex );
	virtual css::uno::Any SAL_CALL WorksheetFunction( );
	virtual css::uno::Any SAL_CALL Evaluate( const ::rtl::OUString& Name );
	virtual css::uno::Any SAL_CALL Dialogs( const css::uno::Any& DialogIndex );
	virtual css::uno::Any SAL_CALL getCutCopyMode();
	virtual void SAL_CALL setCutCopyMode( const css::uno::Any& _cutcopymode );
	virtual css::uno::Any SAL_CALL getStatusBar();
	virtual void SAL_CALL setStatusBar( const css::uno::Any& _statusbar );
	virtual ::sal_Int32 SAL_CALL getCursor();
	virtual void SAL_CALL setCursor( ::sal_Int32 _cursor );

	virtual sal_Bool SAL_CALL getEnableEvents();
	virtual void SAL_CALL setEnableEvents( sal_Bool bEnable );

	virtual css::uno::Any SAL_CALL Windows( const css::uno::Any& aIndex );
	virtual void SAL_CALL wait( double time );
	virtual css::uno::Any SAL_CALL Range( const css::uno::Any& Cell1, const css::uno::Any& Cell2 );
	virtual css::uno::Any SAL_CALL Names( const css::uno::Any& aIndex );
	virtual void SAL_CALL GoTo( const css::uno::Any& Reference, const css::uno::Any& Scroll );
	virtual void SAL_CALL Calculate();
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Intersect( const css::uno::Reference< ov::excel::XRange >& Arg1, const css::uno::Reference< ov::excel::XRange >& Arg2, const css::uno::Any& Arg3, const css::uno::Any& Arg4, const css::uno::Any& Arg5, const css::uno::Any& Arg6, const css::uno::Any& Arg7, const css::uno::Any& Arg8, const css::uno::Any& Arg9, const css::uno::Any& Arg10, const css::uno::Any& Arg11, const css::uno::Any& Arg12, const css::uno::Any& Arg13, const css::uno::Any& Arg14, const css::uno::Any& Arg15, const css::uno::Any& Arg16, const css::uno::Any& Arg17, const css::uno::Any& Arg18, const css::uno::Any& Arg19, const css::uno::Any& Arg20, const css::uno::Any& Arg21, const css::uno::Any& Arg22, const css::uno::Any& Arg23, const css::uno::Any& Arg24, const css::uno::Any& Arg25, const css::uno::Any& Arg26, const css::uno::Any& Arg27, const css::uno::Any& Arg28, const css::uno::Any& Arg29, const css::uno::Any& Arg30 );
	virtual css::uno::Reference< ov::excel::XRange > SAL_CALL Union( const css::uno::Reference< ov::excel::XRange >& Arg1, const css::uno::Reference< ov::excel::XRange >& Arg2, const css::uno::Any& Arg3, const css::uno::Any& Arg4, const css::uno::Any& Arg5, const css::uno::Any& Arg6, const css::uno::Any& Arg7, const css::uno::Any& Arg8, const css::uno::Any& Arg9, const css::uno::Any& Arg10, const css::uno::Any& Arg11, const css::uno::Any& Arg12, const css::uno::Any& Arg13, const css::uno::Any& Arg14, const css::uno::Any& Arg15, const css::uno::Any& Arg16, const css::uno::Any& Arg17, const css::uno::Any& Arg18, const css::uno::Any& Arg19, const css::uno::Any& Arg20, const css::uno::Any& Arg21, const css::uno::Any& Arg22, const css::uno::Any& Arg23, const css::uno::Any& Arg24, const css::uno::Any& Arg25, const css::uno::Any& Arg26, const css::uno::Any& Arg27, const css::uno::Any& Arg28, const css::uno::Any& Arg29, const css::uno::Any& Arg30 );
	virtual void SAL_CALL Volatile( const css::uno::Any& Volatile );
	virtual void SAL_CALL DoEvents();
	virtual css::uno::Any SAL_CALL Caller( const css::uno::Any& aIndex );
	virtual css::uno::Any SAL_CALL GetOpenFilename( const css::uno::Any& rFileFilter, const css::uno::Any& rFilterIndex, const css::uno::Any& rTitle, const css::uno::Any& rButtonText, const css::uno::Any& rMultiSelect );
	virtual css::uno::Any SAL_CALL GetSaveAsFilename( const css::uno::Any& rInitialFileName, const css::uno::Any& rFileFilter, const css::uno::Any& rFilterIndex, const css::uno::Any& rTitle, const css::uno::Any& rButtonText );
	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SC_VBA_APPLICATION_HXX */
