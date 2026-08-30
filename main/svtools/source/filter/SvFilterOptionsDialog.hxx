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



#ifndef _SV_FILTER_OPTIONS_DIALOG_HXX_
#define _SV_FILTER_OPTIONS_DIALOG_HXX_

#include <tools/fldunit.hxx>
#include <cppuhelper/implbase5.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#include <com/sun/star/document/XExporter.hpp>

class SvFilterOptionsDialog : public cppu::WeakImplHelper5
<
	com::sun::star::document::XExporter,
	com::sun::star::ui::dialogs::XExecutableDialog,
    com::sun::star::beans::XPropertyAccess,
	com::sun::star::lang::XInitialization,
	com::sun::star::lang::XServiceInfo
>
{
    const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
        mxMgr;
    com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >
        maMediaDescriptor;
    com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >
        maFilterDataSequence;
	com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >
		mxSourceDocument;

	rtl::OUString   maDialogTitle;
	FieldUnit		meFieldUnit;
	sal_Bool		mbExportSelection;

public:

	SvFilterOptionsDialog( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > _rxORB );
	~SvFilterOptionsDialog();

	// XInterface
    virtual void SAL_CALL acquire() throw();
    virtual void SAL_CALL release() throw();

	// XInitialization
    virtual void SAL_CALL initialize( const com::sun::star::uno::Sequence< com::sun::star::uno::Any > & aArguments );

	// XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName );
    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XPropertyAccess
    virtual com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPropertyValues();
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > & aProps );

	// XExecuteDialog
    virtual sal_Int16 SAL_CALL execute();
    virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle );

	// XExporter
    virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc );

};


#endif // _SV_FILTER_OPTIONS_DIALOG_HXX_
