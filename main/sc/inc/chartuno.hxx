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

#ifndef SC_CHARTUNO_HXX
#define SC_CHARTUNO_HXX

#include "address.hxx"
#include <svl/lstner.hxx>
#include <tools/string.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>

#include <com/sun/star/table/XTableChart.hpp>
#include <com/sun/star/table/XTableCharts.hpp>
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNamedEx.hpp>
#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/implbase4.hxx>


class ScDocShell;
class ScRangeListRef;
class ScChartObj;


class ScChartsObj : public cppu::WeakImplHelper4<
							com::sun::star::table::XTableCharts,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::container::XIndexAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	SCTAB					nTab;			// Charts sind pro Sheet

	ScChartObj*				GetObjectByIndex_Impl(long nIndex) const;
	ScChartObj*				GetObjectByName_Impl(const ::rtl::OUString& aName) const;

public:
							ScChartsObj(ScDocShell* pDocSh, SCTAB nT);
	virtual					~ScChartsObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XTableCharts
	virtual void SAL_CALL	addNewByName( const ::rtl::OUString& aName,
									const ::com::sun::star::awt::Rectangle& aRect,
									const ::com::sun::star::uno::Sequence<
										::com::sun::star::table::CellRangeAddress >& aRanges,
									sal_Bool bColumnHeaders, sal_Bool bRowHeaders );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& aName );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


typedef ::cppu::WeakComponentImplHelper4<
    ::com::sun::star::table::XTableChart,
    ::com::sun::star::document::XEmbeddedObjectSupplier,
    ::com::sun::star::container::XNamedEx,  // #i121178#: support displaying name
    ::com::sun::star::lang::XServiceInfo > ScChartObj_Base;

typedef ::comphelper::OPropertyContainer ScChartObj_PBase;
typedef ::comphelper::OPropertyArrayUsageHelper< ScChartObj > ScChartObj_PABase;

class ScChartObj : public ::comphelper::OBaseMutex
                  ,public ScChartObj_Base
                  ,public ScChartObj_PBase
                  ,public ScChartObj_PABase
                  ,public SfxListener
{
private:
	ScDocShell*				pDocShell;
	SCTAB					nTab;			// Charts sind pro Sheet
	String					aChartName;
	String					aObjectName;    // #i121178#: the OLE object's name(displaying name)

	void	Update_Impl( const ScRangeListRef& rRanges, bool bColHeaders, bool bRowHeaders );
	void	GetData_Impl( ScRangeListRef& rRanges, bool& rColHeaders, bool& rRowHeaders ) const;

protected:
    // ::comphelper::OPropertySetHelper
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
    virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );
    using ::cppu::OPropertySetHelper::getFastPropertyValue;
    virtual void SAL_CALL getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;

    // ::comphelper::OPropertyArrayUsageHelper
    virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

public:
							ScChartObj(ScDocShell* pDocSh, SCTAB nT, const String& rN);
	virtual					~ScChartObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    // XInterface
    DECLARE_XINTERFACE()

    // XTypeProvider
    DECLARE_XTYPEPROVIDER()

    // XComponent
    virtual void SAL_CALL disposing();

							// XTableChart
	virtual sal_Bool SAL_CALL getHasColumnHeaders();
	virtual void SAL_CALL	setHasColumnHeaders( sal_Bool bHasColumnHeaders );
	virtual sal_Bool SAL_CALL getHasRowHeaders();
	virtual void SAL_CALL	setHasRowHeaders( sal_Bool bHasRowHeaders );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress > SAL_CALL
							getRanges(  );
	virtual void SAL_CALL	setRanges( const ::com::sun::star::uno::Sequence<
									::com::sun::star::table::CellRangeAddress >& aRanges );

							// XEmbeddedObjectSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > SAL_CALL
							getEmbeddedObject();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XNamedEx
	virtual ::rtl::OUString SAL_CALL getDisplayName();
	virtual void SAL_CALL	setDisplayName( const ::rtl::OUString& aName );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
};

#endif
