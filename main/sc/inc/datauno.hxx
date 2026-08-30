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



#ifndef SC_DATAUNO_HXX
#define SC_DATAUNO_HXX

#include "global.hxx"
#include "queryparam.hxx"

#include <com/sun/star/sheet/TableFilterField.hpp>
#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/XSheetFilterDescriptor.hpp>
#include <com/sun/star/sheet/XConsolidationDescriptor.hpp>
#include <com/sun/star/sheet/XDatabaseRanges.hpp>
#include <com/sun/star/sheet/XDatabaseRange.hpp>
#include <com/sun/star/sheet/XSubTotalDescriptor.hpp>
#include <com/sun/star/sheet/XSubTotalField.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/sheet/XSheetFilterDescriptor2.hpp>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/implbase6.hxx>
#include "svl/itemprop.hxx"
#include "svl/lstner.hxx"

class ScDBData;
class ScDocShell;

class ScSubTotalFieldObj;
class ScDatabaseRangeObj;
class ScDataPilotDescriptorBase;

struct ScSortParam;

typedef ::com::sun::star::uno::Reference<
			::com::sun::star::util::XRefreshListener >* XDBRefreshListenerPtr;
SV_DECL_PTRARR_DEL( XDBRefreshListenerArr_Impl, XDBRefreshListenerPtr, 4, 4 )

class ScDataUnoConversion
{
public:
	static ScSubTotalFunc	GeneralToSubTotal( com::sun::star::sheet::GeneralFunction eSummary );
	static com::sun::star::sheet::GeneralFunction SubTotalToGeneral( ScSubTotalFunc eSubTotal );
};


//	ImportDescriptor gibt's nicht mehr als Uno-Objekt, nur noch Property-Sequence

class ScImportDescriptor
{
public:
	static void	FillImportParam(
					ScImportParam& rParam,
					const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rSeq );
	static void	FillProperties(
					com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rSeq,
					const ScImportParam& rParam );
	static long GetPropertyCount();
};

//	SortDescriptor gibt's nicht mehr als Uno-Objekt, nur noch Property-Sequence

class ScSortDescriptor
{
public:
	static void	FillSortParam(
					ScSortParam& rParam,
					const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rSeq );
	static void	FillProperties(
					com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rSeq,
					const ScSortParam& rParam );
	static long GetPropertyCount();
};


//	ScSubTotalDescriptorBase - Basisklasse fuer SubTotalDescriptor alleine und im DB-Bereich

//	to uno, both look the same

class ScSubTotalDescriptorBase : public cppu::WeakImplHelper6<
										com::sun::star::sheet::XSubTotalDescriptor,
										com::sun::star::container::XEnumerationAccess,
										com::sun::star::container::XIndexAccess,
										com::sun::star::beans::XPropertySet,
										com::sun::star::lang::XUnoTunnel,
										com::sun::star::lang::XServiceInfo >
{
private:
	SfxItemPropertySet		aPropSet;

	ScSubTotalFieldObj*		GetObjectByIndex_Impl(sal_uInt16 nIndex);

public:
							ScSubTotalDescriptorBase();
	virtual					~ScSubTotalDescriptorBase();

							// in derived classes:
							// (Fields are within the range)
	virtual void			GetData( ScSubTotalParam& rParam ) const = 0;
	virtual void			PutData( const ScSubTotalParam& rParam ) = 0;

							// XSubTotalDescriptor
	virtual void SAL_CALL	addNew( const ::com::sun::star::uno::Sequence<
									::com::sun::star::sheet::SubTotalColumn >& aSubTotalColumns,
								sal_Int32 nGroupColumn );
	virtual void SAL_CALL	clear();

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScSubTotalDescriptorBase* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::sheet::XSubTotalDescriptor> xObj );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


//	ScSubTotalDescriptor - dummer Container zur Benutzung mit XImportTarget

class ScSubTotalDescriptor : public ScSubTotalDescriptorBase
{
private:
	ScSubTotalParam			aStoredParam;

public:
							ScSubTotalDescriptor();
	virtual					~ScSubTotalDescriptor();

							// von ScSubTotalDescriptorBase:
	virtual void			GetData( ScSubTotalParam& rParam ) const;
	virtual void			PutData( const ScSubTotalParam& rParam );

							// Zugriff von aussen:
	void					SetParam( const ScSubTotalParam& rNew );
//	const ScSubTotalParam&	GetParam() const	{ return aStoredParam; }
};


//	ScRangeSubTotalDescriptor - SubTotalDescriptor eines Datenbank-Bereichs

class ScRangeSubTotalDescriptor : public ScSubTotalDescriptorBase
{
private:
	ScDatabaseRangeObj*		pParent;

public:
							ScRangeSubTotalDescriptor(ScDatabaseRangeObj* pPar);
	virtual					~ScRangeSubTotalDescriptor();

							// von ScSubTotalDescriptorBase:
	virtual void			GetData( ScSubTotalParam& rParam ) const;
	virtual void			PutData( const ScSubTotalParam& rParam );
};


class ScSubTotalFieldObj : public cppu::WeakImplHelper2<
								com::sun::star::sheet::XSubTotalField,
								com::sun::star::lang::XServiceInfo >
{
private:
	com::sun::star::uno::Reference<com::sun::star::sheet::XSubTotalDescriptor> xRef;
	ScSubTotalDescriptorBase&	rParent;
	sal_uInt16						nPos;

public:
							ScSubTotalFieldObj( ScSubTotalDescriptorBase* pDesc, sal_uInt16 nP );
	virtual					~ScSubTotalFieldObj();

							// XSubTotalField
	virtual sal_Int32 SAL_CALL getGroupColumn();
	virtual void SAL_CALL	setGroupColumn( sal_Int32 nGroupColumn );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::SubTotalColumn > SAL_CALL
							getSubTotalColumns();
	virtual void SAL_CALL	setSubTotalColumns( const ::com::sun::star::uno::Sequence<
								::com::sun::star::sheet::SubTotalColumn >& aSubTotalColumns );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScConsolidationDescriptor : public cppu::WeakImplHelper2<
										com::sun::star::sheet::XConsolidationDescriptor,
										com::sun::star::lang::XServiceInfo >
{
private:
	ScConsolidateParam		aParam;

public:
							ScConsolidationDescriptor();
	virtual					~ScConsolidationDescriptor();

	void					SetParam( const ScConsolidateParam& rNew );
	const ScConsolidateParam& GetParam() const	{ return aParam; }

							// XConsolidationDescriptor
	virtual ::com::sun::star::sheet::GeneralFunction SAL_CALL getFunction();
	virtual void SAL_CALL	setFunction( ::com::sun::star::sheet::GeneralFunction nFunction );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress > SAL_CALL
							getSources(  );
	virtual void SAL_CALL	setSources( const ::com::sun::star::uno::Sequence<
								::com::sun::star::table::CellRangeAddress >& aSources );
	virtual ::com::sun::star::table::CellAddress SAL_CALL getStartOutputPosition();
	virtual void SAL_CALL	setStartOutputPosition(
								const ::com::sun::star::table::CellAddress& aStartOutputPosition );
	virtual sal_Bool SAL_CALL getUseColumnHeaders();
	virtual void SAL_CALL	setUseColumnHeaders( sal_Bool bUseColumnHeaders );
	virtual sal_Bool SAL_CALL getUseRowHeaders();
	virtual void SAL_CALL	setUseRowHeaders( sal_Bool bUseRowHeaders );
	virtual sal_Bool SAL_CALL getInsertLinks();
	virtual void SAL_CALL	setInsertLinks( sal_Bool bInsertLinks );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


//	ScFilterDescriptorBase - Basisklasse fuer FilterDescriptor
//							 alleine, im DB-Bereich und im DataPilot

//	to uno, all three look the same

class ScFilterDescriptorBase : public cppu::WeakImplHelper4<
									com::sun::star::sheet::XSheetFilterDescriptor,
                                    com::sun::star::sheet::XSheetFilterDescriptor2,
									com::sun::star::beans::XPropertySet,
									com::sun::star::lang::XServiceInfo >,
						       public SfxListener
{
private:
	SfxItemPropertySet		aPropSet;
    ScDocShell*             pDocSh;

public:
							ScFilterDescriptorBase(ScDocShell* pDocShell);
	virtual					~ScFilterDescriptorBase();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// in den Ableitungen:
							// (nField[] hier innerhalb des Bereichs)
	virtual void			GetData( ScQueryParam& rParam ) const = 0;
	virtual void			PutData( const ScQueryParam& rParam ) = 0;

							// XSheetFilterDescriptor
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::TableFilterField > SAL_CALL
							getFilterFields();
	virtual void SAL_CALL	setFilterFields( const ::com::sun::star::uno::Sequence<
								::com::sun::star::sheet::TableFilterField >& aFilterFields );

                            // XSheetFilterDescriptor2
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::TableFilterField2 > SAL_CALL
                            getFilterFields2();
    virtual void SAL_CALL	setFilterFields2( const ::com::sun::star::uno::Sequence<
                                ::com::sun::star::sheet::TableFilterField2 >& aFilterFields );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


//	ScFilterDescriptor - dummer Container zur Benutzung mit XFilterable

class ScFilterDescriptor : public ScFilterDescriptorBase
{
private:
	ScQueryParam			aStoredParam;		// nField[] hier innerhalb des Bereichs

public:
							ScFilterDescriptor(ScDocShell* pDocSh);
	virtual					~ScFilterDescriptor();

							// von ScFilterDescriptorBase:
	virtual void			GetData( ScQueryParam& rParam ) const;
	virtual void			PutData( const ScQueryParam& rParam );

							// Zugriff von aussen:
	void					SetParam( const ScQueryParam& rNew );
	const ScQueryParam&		GetParam() const	{ return aStoredParam; }
};


//	ScRangeFilterDescriptor - FilterDescriptor eines Datenbank-Bereichs

class ScRangeFilterDescriptor : public ScFilterDescriptorBase
{
private:
	ScDatabaseRangeObj*		pParent;

public:
							ScRangeFilterDescriptor(ScDocShell* pDocSh, ScDatabaseRangeObj* pPar);
	virtual					~ScRangeFilterDescriptor();

							// von ScFilterDescriptorBase:
	virtual void			GetData( ScQueryParam& rParam ) const;
	virtual void			PutData( const ScQueryParam& rParam );
};


//	ScDataPilotFilterDescriptor - FilterDescriptor eines DataPilotDescriptors

class ScDataPilotFilterDescriptor : public ScFilterDescriptorBase
{
private:
	ScDataPilotDescriptorBase*	pParent;

public:
							ScDataPilotFilterDescriptor(ScDocShell* pDocSh, ScDataPilotDescriptorBase* pPar);
	virtual					~ScDataPilotFilterDescriptor();

							// von ScFilterDescriptorBase:
	virtual void			GetData( ScQueryParam& rParam ) const;
	virtual void			PutData( const ScQueryParam& rParam );
};


class ScDatabaseRangeObj : public cppu::WeakImplHelper6<
								com::sun::star::sheet::XDatabaseRange,
    							com::sun::star::util::XRefreshable,
								com::sun::star::container::XNamed,
								com::sun::star::sheet::XCellRangeReferrer,
								com::sun::star::beans::XPropertySet,
								com::sun::star::lang::XServiceInfo >,
						   public SfxListener
{
private:
	ScDocShell*				pDocShell;
	String					aName;
	SfxItemPropertySet		aPropSet;
	XDBRefreshListenerArr_Impl aRefreshListeners;

private:
	ScDBData*				GetDBData_Impl() const;
	void					Refreshed_Impl();

public:
							ScDatabaseRangeObj(ScDocShell* pDocSh, const String& rNm);
	virtual					~ScDatabaseRangeObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// nField[] hier innerhalb des Bereichs:
	void					GetQueryParam(ScQueryParam& rQueryParam) const;
	void					SetQueryParam(const ScQueryParam& rQueryParam);
	void					GetSubTotalParam(ScSubTotalParam& rSubTotalParam) const;
	void					SetSubTotalParam(const ScSubTotalParam& rSubTotalParam);

                            // XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XDatabaseRange
    virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getDataArea();
    virtual void SAL_CALL	setDataArea( const ::com::sun::star::table::CellRangeAddress& aDataArea );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
    						getSortDescriptor();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetFilterDescriptor > SAL_CALL
    						getFilterDescriptor();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSubTotalDescriptor > SAL_CALL
    						getSubTotalDescriptor();
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
    						getImportDescriptor();
// implemented for the XRefreshable Interface
//    virtual void SAL_CALL	refresh() throw(::com::sun::star::uno::RuntimeException);

							// XRefreshable
	virtual void SAL_CALL	refresh();
	virtual void SAL_CALL	addRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );
	virtual void SAL_CALL	removeRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );

							// XCellRangeReferrer
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getReferredCells();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScDatabaseRangesObj : public cppu::WeakImplHelper4<
								com::sun::star::sheet::XDatabaseRanges,
								com::sun::star::container::XEnumerationAccess,
								com::sun::star::container::XIndexAccess,
								com::sun::star::lang::XServiceInfo >,
						    public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScDatabaseRangeObj*		GetObjectByIndex_Impl(sal_uInt16 nIndex);
	ScDatabaseRangeObj*		GetObjectByName_Impl(const ::rtl::OUString& aName);

public:
							ScDatabaseRangesObj(ScDocShell* pDocSh);
	virtual					~ScDatabaseRangesObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XDatabaseRanges
	virtual void SAL_CALL	addNewByName( const ::rtl::OUString& aName,
								const ::com::sun::star::table::CellRangeAddress& aRange );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& aName );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};



#endif
