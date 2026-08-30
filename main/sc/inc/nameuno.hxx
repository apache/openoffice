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



#ifndef SC_NAMEUNO_HXX
#define SC_NAMEUNO_HXX

#include <svl/lstner.hxx>
#include "address.hxx"
#include "formula/grammar.hxx"
#include <com/sun/star/sheet/XLabelRange.hpp>
#include <com/sun/star/sheet/XLabelRanges.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/XNamedRange2.hpp>
#include <com/sun/star/sheet/XFormulaTokens.hpp>
#include <com/sun/star/sheet/XNamedRanges2.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sheet/RangeScopeName.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/document/XActionLockable.hpp>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/implbase6.hxx>

#include <global.hxx> //for EMPTY_STRING

class ScDocShell;
class ScRangeData;
class ScTokenArray;


class ScNamedRangeObj : public ::cppu::WeakImplHelper6<
							::com::sun::star::sheet::XNamedRange2,
                            ::com::sun::star::sheet::XFormulaTokens,
							::com::sun::star::sheet::XCellRangeReferrer,
							::com::sun::star::beans::XPropertySet,
							::com::sun::star::lang::XUnoTunnel,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	String					aName;
	String					aScopeName;

private:
	ScRangeData*			GetRangeData_Impl();
	void					Modify_Impl( const String* pNewName,
                                        const ScTokenArray* pNewTokens, const String* pNewContent,
										const ScAddress* pNewPos, const sal_uInt16* pNewType,
                                        const formula::FormulaGrammar::Grammar eGrammar, const String* pNewScopeName = NULL );

public:
							ScNamedRangeObj(ScDocShell* pDocSh, const String& rNm, const String& rScopeName = EMPTY_STRING);
	virtual					~ScNamedRangeObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XNamedRange
	virtual ::rtl::OUString SAL_CALL getContent();
	virtual void SAL_CALL	setContent( const ::rtl::OUString& aContent );
	virtual ::com::sun::star::table::CellAddress SAL_CALL getReferencePosition();
	virtual void SAL_CALL	setReferencePosition(
								const ::com::sun::star::table::CellAddress& aReferencePosition );
	virtual sal_Int32		SAL_CALL getType();
	virtual void SAL_CALL	setType( sal_Int32 nType );
	virtual ::rtl::OUString SAL_CALL getScopeName();
	virtual void SAL_CALL	setScopeAndRangeName( const ::rtl::OUString& aScopeName, const ::rtl::OUString& aRangeName );

                            // XFormulaTokens
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > SAL_CALL getTokens();
    virtual void SAL_CALL   setTokens( const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::sheet::FormulaToken >& aTokens );

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

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

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScNamedRangeObj* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::uno::XInterface> xObj );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

                            // methods accessible via getImplementation()
            void            SetContentWithGrammar( const ::rtl::OUString& aContent,
                                    const formula::FormulaGrammar::Grammar eGrammar );
};


class ScNamedRangesObj : public ::cppu::WeakImplHelper5<
							::com::sun::star::sheet::XNamedRanges2,
							::com::sun::star::container::XEnumerationAccess,
							::com::sun::star::container::XIndexAccess,
                            ::com::sun::star::document::XActionLockable,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScNamedRangeObj*		GetObjectByIndex_Impl(sal_uInt16 nIndex);
	ScNamedRangeObj*		GetObjectByName_Impl(const ::rtl::OUString& aName);
	ScNamedRangeObj*		GetObjectByScopeName_Impl(const ::rtl::OUString& aScopeName, const ::rtl::OUString& aRangeName);

	void ImplAddNewByScopeAndName(SCTAB aScope,const ::rtl::OUString& aRangeName, const ::rtl::OUString& aContent,
								const ::com::sun::star::table::CellAddress& aPosition, sal_Int32 nType);

	void ImplRemoveByScopeAndName(SCTAB aScope,const ::rtl::OUString& aRangeName);

protected:
    /** called from the XActionLockable interface methods on initial locking */
    virtual void            lock();

    /** called from the XActionLockable interface methods on final unlock */
    virtual void            unlock();

public:
							ScNamedRangesObj(ScDocShell* pDocSh);
	virtual					~ScNamedRangesObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XNamedRanges
	virtual void SAL_CALL	addNewByName( const ::rtl::OUString& aName, const ::rtl::OUString& aContent,
								const ::com::sun::star::table::CellAddress& aPosition, sal_Int32 nType );

	virtual void SAL_CALL	addNewByScopeName( const ::rtl::OUString& aScopeName,const ::rtl::OUString& aRangeName, const ::rtl::OUString& aContent,
								const ::com::sun::star::table::CellAddress& aPosition, sal_Int32 nType );
	virtual void SAL_CALL	addNewFromTitles( const ::com::sun::star::table::CellRangeAddress& aSource,
								::com::sun::star::sheet::Border aBorder );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& aName );
	virtual void SAL_CALL	removeByScopeName( const ::rtl::OUString& aScopeName, const ::rtl::OUString& aRangeName );
	virtual void SAL_CALL	outputList( const ::com::sun::star::table::CellAddress& aOutputPosition );
	virtual sal_Bool SAL_CALL hasByScopeName( const ::rtl::OUString& aScopeName, const ::rtl::OUString& aRangeName );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::RangeScopeName > SAL_CALL getElementScopeNames();
	virtual ::com::sun::star::uno::Any SAL_CALL getByScopeName( const ::rtl::OUString& aScopeName, const ::rtl::OUString& aRangeName );

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

                            // XActionLockable
    virtual sal_Bool SAL_CALL isActionLocked();
    virtual void SAL_CALL addActionLock();
    virtual void SAL_CALL removeActionLock();
    virtual void SAL_CALL setActionLocks( sal_Int16 nLock );
    virtual sal_Int16 SAL_CALL resetActionLocks();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScLabelRangeObj : public ::cppu::WeakImplHelper2<
							::com::sun::star::sheet::XLabelRange,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	sal_Bool				bColumn;
	ScRange					aRange;			// Kriterium um Bereich zu finden

private:
	ScRangePair*			GetData_Impl();
	void					Modify_Impl( const ScRange* pLabel, const ScRange* pData );

public:
							ScLabelRangeObj(ScDocShell* pDocSh, sal_Bool bCol, const ScRange& rR);
	virtual					~ScLabelRangeObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XLabelRange
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getLabelArea();
	virtual void SAL_CALL	setLabelArea( const ::com::sun::star::table::CellRangeAddress& aLabelArea );
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getDataArea();
	virtual void SAL_CALL	setDataArea( const ::com::sun::star::table::CellRangeAddress& aDataArea );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScLabelRangesObj : public ::cppu::WeakImplHelper3<
							::com::sun::star::sheet::XLabelRanges,
							::com::sun::star::container::XEnumerationAccess,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	sal_Bool					bColumn;

	ScLabelRangeObj*		GetObjectByIndex_Impl(sal_uInt16 nIndex);

public:
							ScLabelRangesObj(ScDocShell* pDocSh, sal_Bool bCol);
	virtual					~ScLabelRangesObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XLabelRanges
	virtual void SAL_CALL	addNew( const ::com::sun::star::table::CellRangeAddress& aLabelArea,
								const ::com::sun::star::table::CellRangeAddress& aDataArea );
	virtual void SAL_CALL	removeByIndex( sal_Int32 nIndex );

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




#endif
