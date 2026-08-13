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


#ifndef SW_VBA_PAGESETUP_HXX
#define SW_VBA_PAGESETUP_HXX

#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/word/XPageSetup.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbapagesetupbase.hxx>

typedef cppu::ImplInheritanceHelper1< VbaPageSetupBase, ooo::vba::word::XPageSetup > SwVbaPageSetup_BASE;

class SwVbaPageSetup :  public SwVbaPageSetup_BASE
{
private:
    rtl::OUString getStyleOfFirstPage();

public:
	SwVbaPageSetup( const css::uno::Reference< ooo::vba::XHelperInterface >& xParent,
                    const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::frame::XModel >& xModel,
                    const css::uno::Reference< css::beans::XPropertySet >& xProps );
	virtual ~SwVbaPageSetup(){}

    // Attributes
    virtual double SAL_CALL getGutter();
    virtual void SAL_CALL setGutter( double _gutter );
    virtual double SAL_CALL getHeaderDistance();
    virtual void SAL_CALL setHeaderDistance( double _headerdistance );
    virtual double SAL_CALL getFooterDistance();
    virtual void SAL_CALL setFooterDistance( double _footerdistance );
    virtual sal_Bool SAL_CALL getDifferentFirstPageHeaderFooter();
    virtual void SAL_CALL setDifferentFirstPageHeaderFooter( sal_Bool status );
    virtual ::sal_Int32 SAL_CALL getSectionStart();
    virtual void SAL_CALL setSectionStart( ::sal_Int32 _sectionstart );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif
