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



#ifndef _SAMPLEADDIN_HXX_
#define _SAMPLEADDIN_HXX_

#include <cppuhelper/implbase9.hxx>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/chart/XDiagram.hpp>
#include <com/sun/star/chart/XAxisXSupplier.hpp>
#include <com/sun/star/chart/XAxisYSupplier.hpp>
#include <com/sun/star/chart/XStatisticDisplay.hpp>

#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/lang/XLocalizable.hpp>

#include <com/sun/star/chart/XChartDocument.hpp>

com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL
	SampleAddIn_CreateInstance(
		const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& );

class SampleAddIn : public cppu::WeakImplHelper9<
	com::sun::star::lang::XInitialization,
	com::sun::star::chart::XDiagram,
	com::sun::star::chart::XAxisXSupplier,
	com::sun::star::chart::XAxisYSupplier,
	com::sun::star::chart::XStatisticDisplay,
	com::sun::star::lang::XServiceName,
	com::sun::star::lang::XServiceInfo,
	com::sun::star::util::XRefreshable,
	com::sun::star::lang::XLocalizable  >
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDocument > mxChartDoc;
	::com::sun::star::lang::Locale maLocale;

	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxMyRedLine;
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxMyText;

public:
	SampleAddIn();
	virtual ~SampleAddIn();

	// class specific code
	static ::rtl::OUString	getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();

	sal_Bool getLogicalPosition( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xAxis,
								 double fValue,
								 sal_Bool bVertical,
								 ::com::sun::star::awt::Point& aOutPosition );

	// XInitialization
	virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XDiagram
    virtual ::rtl::OUString SAL_CALL getDiagramType();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getDataRowProperties( sal_Int32 nRow );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getDataPointProperties( sal_Int32 nCol, sal_Int32 nRow );

	// XShape ( ::XDiagram )
	virtual ::com::sun::star::awt::Size SAL_CALL getSize();
	virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& );
	virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
	virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& );

	// XShapeDescriptor ( ::XShape ::XDiagram )
    virtual rtl::OUString SAL_CALL getShapeType();

	// XAxisXSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > SAL_CALL getXAxisTitle();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getXAxis();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getXMainGrid();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getXHelpGrid();

	// XAxisYSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > SAL_CALL getYAxisTitle();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getYAxis();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getYHelpGrid();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getYMainGrid();

	// XStatisticDisplay
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getUpBar();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getDownBar();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getMinMaxLine();

	// XServiceName
	virtual ::rtl::OUString SAL_CALL getServiceName();

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XRefreshable
	virtual void SAL_CALL refresh();
    virtual void SAL_CALL addRefreshListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshListener >& l );
    virtual void SAL_CALL removeRefreshListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshListener >& l );

	// XLocalizable
	virtual void SAL_CALL setLocale( const ::com::sun::star::lang::Locale& eLocale );
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale();
};

#endif	// _SAMPLEADDIN_HXX_
