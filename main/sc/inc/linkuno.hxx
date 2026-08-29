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



#ifndef SC_LINKUNO_HXX
#define SC_LINKUNO_HXX

#include <svl/lstner.hxx>
#include <svl/itemprop.hxx>
#include <com/sun/star/sheet/XDDELink.hpp>
#include <com/sun/star/sheet/XDDELinkResults.hpp>
#include <com/sun/star/sheet/XDDELinks.hpp>
#include <com/sun/star/sheet/XExternalDocLink.hpp>
#include <com/sun/star/sheet/XExternalDocLinks.hpp>
#include <com/sun/star/sheet/XExternalSheetCache.hpp>
#include <com/sun/star/sheet/XAreaLink.hpp>
#include <com/sun/star/sheet/XAreaLinks.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>

#include "externalrefmgr.hxx"

#include <hash_map>
#include <vector>

class ScAreaLink;
class ScDocShell;
class ScTableLink;

typedef ::com::sun::star::uno::Reference<
			::com::sun::star::util::XRefreshListener >* XRefreshListenerPtr;
SV_DECL_PTRARR_DEL( XRefreshListenerArr_Impl, XRefreshListenerPtr, 4, 4 )



class ScSheetLinkObj : public cppu::WeakImplHelper4<
							com::sun::star::container::XNamed,
							com::sun::star::util::XRefreshable,
							com::sun::star::beans::XPropertySet,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	SfxItemPropertySet		aPropSet;
	ScDocShell*				pDocShell;
	String					aFileName;
	XRefreshListenerArr_Impl aRefreshListeners;

	ScTableLink*			GetLink_Impl() const;
	void					Refreshed_Impl();
	void					ModifyRefreshDelay_Impl( sal_Int32 nRefresh );

public:
							ScSheetLinkObj(ScDocShell* pDocSh, const String& rName);
	virtual					~ScSheetLinkObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XRefreshable
	virtual void SAL_CALL	refresh();
	virtual void SAL_CALL	addRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );
	virtual void SAL_CALL	removeRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
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

							// aus get/setPropertyValue gerufen:
	::rtl::OUString			getFileName(void) const;
	void					setFileName(const ::rtl::OUString& FileName);
	::rtl::OUString			getFilter(void) const;
	void					setFilter(const ::rtl::OUString& Filter);
	::rtl::OUString			getFilterOptions(void) const;
	void					setFilterOptions(const ::rtl::OUString& FilterOptions);
	sal_Int32				getRefreshDelay(void) const;
	void					setRefreshDelay(sal_Int32 nRefreshDelay);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScSheetLinksObj : public cppu::WeakImplHelper4<
							com::sun::star::container::XNameAccess,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::container::XIndexAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScSheetLinkObj*			GetObjectByIndex_Impl(sal_Int32 nIndex);
	ScSheetLinkObj*			GetObjectByName_Impl(const ::rtl::OUString& aName);

public:
							ScSheetLinksObj(ScDocShell* pDocSh);
	virtual					~ScSheetLinksObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

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


class ScAreaLinkObj : public cppu::WeakImplHelper4<
							com::sun::star::sheet::XAreaLink,
							com::sun::star::util::XRefreshable,
							com::sun::star::beans::XPropertySet,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	SfxItemPropertySet		aPropSet;
	ScDocShell*				pDocShell;
	sal_uInt16					nPos;
	XRefreshListenerArr_Impl aRefreshListeners;

	void	Modify_Impl( const ::rtl::OUString* pNewFile, const ::rtl::OUString* pNewFilter,
						 const ::rtl::OUString* pNewOptions, const ::rtl::OUString* pNewSource,
						 const com::sun::star::table::CellRangeAddress* pNewDest );
	void	ModifyRefreshDelay_Impl( sal_Int32 nRefresh );
	void	Refreshed_Impl();

public:
							ScAreaLinkObj(ScDocShell* pDocSh, sal_uInt16 nP);
	virtual					~ScAreaLinkObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XRefreshable
	virtual void SAL_CALL	refresh();
	virtual void SAL_CALL	addRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );
	virtual void SAL_CALL	removeRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
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

							// aus get/setPropertyValue gerufen:
	::rtl::OUString			getFileName(void) const;
	void					setFileName(const ::rtl::OUString& FileName);
	::rtl::OUString			getFilter(void) const;
	void					setFilter(const ::rtl::OUString& Filter);
	::rtl::OUString			getFilterOptions(void) const;
	void					setFilterOptions(const ::rtl::OUString& FilterOptions);
	sal_Int32				getRefreshDelay(void) const;
	void					setRefreshDelay(sal_Int32 nRefreshDelay);

							// XAreaLink
	virtual ::rtl::OUString SAL_CALL getSourceArea();
	virtual void SAL_CALL 	setSourceArea( const ::rtl::OUString& aSourceArea );
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getDestArea();
	virtual void SAL_CALL setDestArea( const ::com::sun::star::table::CellRangeAddress& aDestArea );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScAreaLinksObj : public cppu::WeakImplHelper3<
							com::sun::star::sheet::XAreaLinks,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScAreaLinkObj*			GetObjectByIndex_Impl(sal_Int32 nIndex);

public:
							ScAreaLinksObj(ScDocShell* pDocSh);
	virtual					~ScAreaLinksObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XAreaLinks
	virtual void SAL_CALL	insertAtPosition( const ::com::sun::star::table::CellAddress& aDestPos,
												const ::rtl::OUString& aFileName,
												const ::rtl::OUString& aSourceArea,
												const ::rtl::OUString& aFilter,
												const ::rtl::OUString& aFilterOptions );
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


//!	order of XNamed and DDELink changed to avoid "duplicate comdat" symbols

class ScDDELinkObj : public cppu::WeakImplHelper5<
							com::sun::star::sheet::XDDELink,
							com::sun::star::container::XNamed,
							com::sun::star::util::XRefreshable,
                            com::sun::star::sheet::XDDELinkResults,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	String					aAppl;
	String					aTopic;
	String					aItem;
	XRefreshListenerArr_Impl aRefreshListeners;

	void					Refreshed_Impl();

public:
							ScDDELinkObj(ScDocShell* pDocSh, const String& rA,
											const String& rT, const String& rI);
	virtual					~ScDDELinkObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XDDELink
	virtual ::rtl::OUString SAL_CALL getApplication();
	virtual ::rtl::OUString SAL_CALL getTopic();
	virtual ::rtl::OUString SAL_CALL getItem();

							// XRefreshable
	virtual void SAL_CALL	refresh();
	virtual void SAL_CALL	addRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );
	virtual void SAL_CALL	removeRefreshListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XRefreshListener >& l );

                            // XDDELinkResults
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > >
        SAL_CALL getResults(  );
    virtual void SAL_CALL setResults(
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > >& aResults );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScDDELinksObj : public cppu::WeakImplHelper4<
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::container::XIndexAccess,
                            com::sun::star::sheet::XDDELinks,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScDDELinkObj*			GetObjectByIndex_Impl(sal_Int32 nIndex);
	ScDDELinkObj*			GetObjectByName_Impl(const ::rtl::OUString& aName);

public:
							ScDDELinksObj(ScDocShell* pDocSh);
	virtual					~ScDDELinksObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

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

                            // XDDELinks
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDDELink > SAL_CALL addDDELink(
        const ::rtl::OUString& aApplication, const ::rtl::OUString& aTopic,
        const ::rtl::OUString& aItem, ::com::sun::star::sheet::DDELinkMode nMode );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

// ============================================================================

class ScExternalSheetCacheObj : public cppu::WeakImplHelper1< ::com::sun::star::sheet::XExternalSheetCache >
{
public:
    explicit ScExternalSheetCacheObj(ScExternalRefCache::TableTypeRef pTable, size_t nIndex);
    ~ScExternalSheetCacheObj();

                            // XExternalSheetCache
    virtual void SAL_CALL setCellValue(
        sal_Int32 nCol, sal_Int32 nRow, const ::com::sun::star::uno::Any& rAny);

    virtual ::com::sun::star::uno::Any SAL_CALL getCellValue(sal_Int32 nCol, sal_Int32 nRow);

    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getAllRows();

    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getAllColumns(sal_Int32 nRow);

    // Attributes
    virtual sal_Int32 SAL_CALL getTokenIndex();

private:
    ScExternalSheetCacheObj();
    ScExternalSheetCacheObj(const ScExternalSheetCacheObj&);

private:
    ScExternalRefCache::TableTypeRef mpTable;
    size_t mnIndex;
};

// ============================================================================

class ScExternalDocLinkObj : public cppu::WeakImplHelper1< ::com::sun::star::sheet::XExternalDocLink >
{
public:
    ScExternalDocLinkObj(ScExternalRefManager* pRefMgr, sal_uInt16 nFileId);
    ~ScExternalDocLinkObj();

                            // XExternalDocLink
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XExternalSheetCache >
        SAL_CALL addSheetCache( const ::rtl::OUString& aSheetName, sal_Bool bDynamicCache );

                            // XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

                            // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount();
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex );

                            // XEnumerationAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
                            createEnumeration();

                            // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

    // Attributes
    virtual sal_Int32 SAL_CALL getTokenIndex();

private:
    ScExternalRefManager*   mpRefMgr;
    sal_uInt16              mnFileId;
};

// ============================================================================

/** This is the UNO API equivalent of ScExternalRefManager. */
class ScExternalDocLinksObj : public cppu::WeakImplHelper1< ::com::sun::star::sheet::XExternalDocLinks >
{
public:
    ScExternalDocLinksObj(ScDocShell* pDocShell);
    ~ScExternalDocLinksObj();

                            // XExternalDocLinks
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XExternalDocLink >
        SAL_CALL addDocLink( const ::rtl::OUString& aDocName );

                            // XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

                            // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount();
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex );

                            // XEnumerationAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
                            createEnumeration();

                            // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

private:
    ScExternalDocLinksObj();
    ScExternalDocLinksObj(const ScExternalDocLinksObj&);

private:
    ScDocShell*                         mpDocShell;
    ScExternalRefManager*               mpRefMgr;
};

#endif
