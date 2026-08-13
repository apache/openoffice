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


#ifndef SC_VBA_VALIDATION_HXX
#define SC_VBA_VALIDATION_HXX
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <ooo/vba/excel/XValidation.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1<ov::excel::XValidation > ValidationImpl_BASE;

class ScVbaValidation : public ValidationImpl_BASE
{
	css::uno::Reference< css::table::XCellRange > m_xRange;

public:
	ScVbaValidation( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::table::XCellRange >& xRange ) : ValidationImpl_BASE( xParent, xContext ), m_xRange( xRange) {}
    // Attributes
    virtual ::sal_Bool SAL_CALL getIgnoreBlank();
    virtual void SAL_CALL setIgnoreBlank( ::sal_Bool _ignoreblank );
    virtual ::sal_Bool SAL_CALL getInCellDropdown();
    virtual void SAL_CALL setInCellDropdown( ::sal_Bool _incelldropdown );
    virtual ::sal_Bool SAL_CALL getShowInput();
    virtual void SAL_CALL setShowInput( ::sal_Bool _showinput );
    virtual ::sal_Bool SAL_CALL getShowError();
    virtual void SAL_CALL setShowError( ::sal_Bool _showerror );
    virtual ::rtl::OUString SAL_CALL getInputTitle();
    virtual void SAL_CALL setInputTitle( const ::rtl::OUString& _inputtitle );
    virtual ::rtl::OUString SAL_CALL getErrorTitle();
    virtual void SAL_CALL setErrorTitle( const ::rtl::OUString& _errortitle );
    virtual ::rtl::OUString SAL_CALL getInputMessage();
    virtual void SAL_CALL setInputMessage( const ::rtl::OUString& _inputmessage );
    virtual ::rtl::OUString SAL_CALL getErrorMessage();
    virtual void SAL_CALL setErrorMessage( const ::rtl::OUString& _errormessage );
    virtual ::rtl::OUString SAL_CALL getFormula1() ;
    virtual ::rtl::OUString SAL_CALL getFormula2();
    // Methods
    virtual void SAL_CALL Delete(  );
    virtual void SAL_CALL Add( const css::uno::Any& Type, const css::uno::Any& AlertStyle, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2 );
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

};

#endif
