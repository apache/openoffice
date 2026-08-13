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


#ifndef SC_VBA_PAGESETUP_HXX
#define SC_VBA_PAGESETUP_HXX

#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/excel/XPageSetup.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbapagesetupbase.hxx>

typedef cppu::ImplInheritanceHelper1< VbaPageSetupBase, ov::excel::XPageSetup > ScVbaPageSetup_BASE;

class ScVbaPageSetup :  public ScVbaPageSetup_BASE
{
    css::uno::Reference< css::sheet::XSpreadsheet > mxSheet;
public:
	ScVbaPageSetup( const css::uno::Reference< ov::XHelperInterface >& xParent,
                    const css::uno::Reference< css::uno::XComponentContext >& xContext,
		            const css::uno::Reference< css::sheet::XSpreadsheet>& xSheet,
                    const css::uno::Reference< css::frame::XModel >& xModel);
	virtual ~ScVbaPageSetup(){}

    // Attribute
	virtual rtl::OUString SAL_CALL getPrintArea();
	virtual void SAL_CALL setPrintArea( const rtl::OUString& rAreas );
    virtual double SAL_CALL getHeaderMargin();
    void SAL_CALL setHeaderMargin( double margin );
    double SAL_CALL getFooterMargin();
    void SAL_CALL setFooterMargin( double margin );
	virtual css::uno::Any SAL_CALL getFitToPagesTall();
	virtual void SAL_CALL setFitToPagesTall( const css::uno::Any& fitToPagesTall );
	virtual css::uno::Any SAL_CALL getFitToPagesWide();
	virtual void SAL_CALL setFitToPagesWide( const css::uno::Any& fitToPagesWide );
	virtual css::uno::Any SAL_CALL getZoom();
	virtual void SAL_CALL setZoom( const css::uno::Any& zoom );
	virtual rtl::OUString SAL_CALL getLeftHeader();
	virtual void SAL_CALL setLeftHeader( const rtl::OUString& leftHeader );
	virtual rtl::OUString SAL_CALL getCenterHeader();
	virtual void SAL_CALL setCenterHeader( const rtl::OUString& centerHeader );
	virtual rtl::OUString SAL_CALL getRightHeader();
	virtual void SAL_CALL setRightHeader( const rtl::OUString& rightHeader );
	virtual rtl::OUString SAL_CALL getLeftFooter();
	virtual void SAL_CALL setLeftFooter( const rtl::OUString& leftFooter );
	virtual rtl::OUString SAL_CALL getCenterFooter();
	virtual void SAL_CALL setCenterFooter( const rtl::OUString& centerFooter );
	virtual rtl::OUString SAL_CALL getRightFooter();
	virtual void SAL_CALL setRightFooter( const rtl::OUString& rightFooter );
	virtual sal_Int32 SAL_CALL getOrder();
	virtual void SAL_CALL setOrder( sal_Int32 order );
	virtual sal_Int32 SAL_CALL getFirstPageNumber();
	virtual void SAL_CALL setFirstPageNumber( sal_Int32 firstPageNumber );
	virtual sal_Bool SAL_CALL getCenterVertically();
	virtual void SAL_CALL setCenterVertically( sal_Bool centerVertically );
	virtual sal_Bool SAL_CALL getCenterHorizontally();
	virtual void SAL_CALL setCenterHorizontally( sal_Bool centerHorizontally );
	virtual sal_Bool SAL_CALL getPrintHeadings();
	virtual void SAL_CALL setPrintHeadings( sal_Bool printHeadings );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif
