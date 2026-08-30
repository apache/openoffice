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



#ifndef SCRIPTING_DLGPROV_HXX
#define SCRIPTING_DLGPROV_HXX

#ifndef _COM_SUN_STAR_AWT_XSTRINGRESOURCEWITHSTORAGE_HPP_
#include <com/sun/star/resource/XStringResourceWithStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XSTRINGRESOURCEWITHSTORAGE_HPP_
#include <com/sun/star/resource/XStringResourceWithLocation.hpp>
#endif
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <osl/mutex.hxx>

#include <vector>
#include <hash_map>

//.........................................................................
namespace stringresource
{
//.........................................................................

// =============================================================================
// mutex
// =============================================================================

::osl::Mutex& getMutex();


// =============================================================================
// class stringresourceImpl
// =============================================================================

// Hashtable to map string ids to string
struct hashName_Impl
{
	size_t operator()(const ::rtl::OUString Str) const
	{
		return (size_t)Str.hashCode();
	}
};

struct eqName_Impl
{
	sal_Bool operator()(const ::rtl::OUString Str1, const ::rtl::OUString Str2) const
	{
		return ( Str1 == Str2 );
	}
};

typedef std::hash_map
<
	::rtl::OUString,
	::rtl::OUString,
	hashName_Impl,
	eqName_Impl
>
IdToStringMap;

typedef std::hash_map
<
	::rtl::OUString,
	sal_Int32,
	hashName_Impl,
	eqName_Impl
>
IdToIndexMap;


struct LocaleItem
{
	::com::sun::star::lang::Locale		m_locale;
	IdToStringMap						m_aIdToStringMap;
	IdToIndexMap						m_aIdToIndexMap;
	sal_Int32							m_nNextIndex;
	bool								m_bLoaded;
	bool								m_bModified;

	LocaleItem( ::com::sun::star::lang::Locale locale, bool bLoaded=true )
		: m_locale( locale )
		, m_nNextIndex( 0 )
		, m_bLoaded( bLoaded )
		, m_bModified( false )
	{}
};

typedef std::vector< LocaleItem* > LocaleItemVector;
typedef std::vector< LocaleItem* >::iterator LocaleItemVectorIt;
typedef std::vector< LocaleItem* >::const_iterator LocaleItemVectorConstIt;

typedef ::cppu::WeakImplHelper2<
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::resource::XStringResourceManager > StringResourceImpl_BASE;

class StringResourceImpl : public StringResourceImpl_BASE
{
protected:
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >        m_xContext;
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiComponentFactory >	m_xMCF;

	LocaleItem*																			m_pCurrentLocaleItem;
	LocaleItem*																			m_pDefaultLocaleItem;
	bool																				m_bDefaultModified;

	::cppu::OInterfaceContainerHelper													m_aListenerContainer;

	LocaleItemVector																	m_aLocaleItemVector;
	LocaleItemVector																	m_aDeletedLocaleItemVector;
	LocaleItemVector																	m_aChangedDefaultLocaleVector;

	bool																				m_bModified;
	bool																				m_bReadOnly;

	sal_Int32																			m_nNextUniqueNumericId;

	// Scans ResourceID to start with number and adapt m_nNextUniqueNumericId
	void implScanIdForNumber( const ::rtl::OUString& ResourceID );
	const static sal_Int32 UNIQUE_NUMBER_NEEDS_INITIALISATION = -1;

	// Checks read only status and throws exception if it's true
	void implCheckReadOnly( const sal_Char* pExceptionMsg );

	// Return the context's MultiComponentFactory
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiComponentFactory >
		getMultiComponentFactory( void );

	// Returns the LocalItem for a given locale, if it exists, otherwise NULL
	// This method compares the locales exactly, no closest match search is performed
	LocaleItem* getItemForLocale( const ::com::sun::star::lang::Locale& locale, sal_Bool bException );

	// Returns the LocalItem for a given locale, if it exists, otherwise NULL
	// This method performs a closest match search, at least the language must match
	LocaleItem* getClosestMatchItemForLocale( const ::com::sun::star::lang::Locale& locale );
	void implSetCurrentLocale( const ::com::sun::star::lang::Locale& locale,
		sal_Bool FindClosestMatch, sal_Bool bUseDefaultIfNoMatch );

	void implModified( void );
	void implNotifyListeners( void );

	//=== Impl methods for ...ForLocale methods ===
    ::rtl::OUString SAL_CALL implResolveString( const ::rtl::OUString& ResourceID, LocaleItem* pLocaleItem );
    ::sal_Bool implHasEntryForId( const ::rtl::OUString& ResourceID, LocaleItem* pLocaleItem );
	::com::sun::star::uno::Sequence< ::rtl::OUString > implGetResourceIDs( LocaleItem* pLocaleItem );
	void implSetString( const ::rtl::OUString& ResourceID,
		const ::rtl::OUString& Str, LocaleItem* pLocaleItem );
    void implRemoveId( const ::rtl::OUString& ResourceID, LocaleItem* pLocaleItem );

	// Method to load a locale if necessary, returns true if loading was
	// successful. Default implementation in base class just returns true.
	virtual bool loadLocale( LocaleItem* pLocaleItem );

	virtual void implLoadAllLocales( void );

public:
    StringResourceImpl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
    virtual ~StringResourceImpl();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// XModifyBroadcaster
	virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
	virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

 	// XStringResourceResolver
	virtual ::rtl::OUString SAL_CALL resolveString( const ::rtl::OUString& ResourceID );
    virtual ::rtl::OUString SAL_CALL resolveStringForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Bool SAL_CALL hasEntryForId( const ::rtl::OUString& ResourceID );
    virtual ::sal_Bool SAL_CALL hasEntryForIdAndLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDs(  );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDsForLocale
		( const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::lang::Locale SAL_CALL getCurrentLocale(  );
	virtual ::com::sun::star::lang::Locale SAL_CALL getDefaultLocale(  );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getLocales(  );

	// XStringResourceManager
	virtual ::sal_Bool SAL_CALL isReadOnly();
	virtual void SAL_CALL setCurrentLocale( const ::com::sun::star::lang::Locale& locale, ::sal_Bool FindClosestMatch );
	virtual void SAL_CALL setDefaultLocale( const ::com::sun::star::lang::Locale& locale );
    virtual void SAL_CALL setString( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str );
    virtual void SAL_CALL setStringForLocale( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeId( const ::rtl::OUString& ResourceID );
    virtual void SAL_CALL removeIdForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL newLocale( const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeLocale( const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Int32 SAL_CALL getUniqueNumericId(  );
 };

typedef ::cppu::ImplInheritanceHelper1<
		StringResourceImpl,
		::com::sun::star::resource::XStringResourcePersistence > StringResourcePersistenceImpl_BASE;

class BinaryOutput;
class BinaryInput;

class StringResourcePersistenceImpl : public StringResourcePersistenceImpl_BASE
{
protected:
	::rtl::OUString																m_aNameBase;
	::rtl::OUString																m_aComment;

    void SAL_CALL implInitializeCommonParameters
		( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// Scan locale properties files
	virtual void implScanLocales( void );

	// Method to load a locale if necessary, returns true if loading was successful
	virtual bool loadLocale( LocaleItem* pLocaleItem );

	// does the actual loading
	virtual bool implLoadLocale( LocaleItem* pLocaleItem );

	virtual void implLoadAllLocales( void );

	void implScanLocaleNames( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aContentSeq );
	::rtl::OUString implGetFileNameForLocaleItem( LocaleItem* pLocaleItem, const ::rtl::OUString& aNameBase );
	::rtl::OUString implGetPathForLocaleItem( LocaleItem* pLocaleItem, const ::rtl::OUString& aNameBase,
		const ::rtl::OUString& aLocation, bool bDefaultFile=false );

	bool implReadPropertiesFile( LocaleItem* pLocaleItem,
		const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xInput );

	bool implWritePropertiesFile( LocaleItem* pLocaleItem, const ::com::sun::star::uno::Reference
		< ::com::sun::star::io::XOutputStream >& xOutputStream, const ::rtl::OUString& aComment );

	void implWriteLocaleBinary( LocaleItem* pLocaleItem, BinaryOutput& rOut );

	void implStoreAtStorage
	(
		const ::rtl::OUString& aNameBase,
		const ::rtl::OUString& aComment,
		const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage,
		bool bUsedForStore,
		bool bStoreAll
	);

	void implKillRemovedLocaleFiles
	(
		const ::rtl::OUString& Location,
		const ::rtl::OUString& aNameBase,
		const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess
	);

	void implKillChangedDefaultFiles
	(
		const ::rtl::OUString& Location,
		const ::rtl::OUString& aNameBase,
		const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess
	);

	void implStoreAtLocation
	(
		const ::rtl::OUString& Location,
		const ::rtl::OUString& aNameBase,
		const ::rtl::OUString& aComment,
		const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess,
		bool bUsedForStore,
		bool bStoreAll,
		bool bKillAll = false
	);

public:
    StringResourcePersistenceImpl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
    virtual ~StringResourcePersistenceImpl();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// XModifyBroadcaster
	virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
	virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

 	// XStringResourceResolver
	virtual ::rtl::OUString SAL_CALL resolveString( const ::rtl::OUString& ResourceID );
    virtual ::rtl::OUString SAL_CALL resolveStringForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Bool SAL_CALL hasEntryForId( const ::rtl::OUString& ResourceID );
    virtual ::sal_Bool SAL_CALL hasEntryForIdAndLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDs(  );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDsForLocale
		( const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::lang::Locale SAL_CALL getCurrentLocale(  );
	virtual ::com::sun::star::lang::Locale SAL_CALL getDefaultLocale(  );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getLocales(  );

	// XStringResourceManager
	virtual ::sal_Bool SAL_CALL isReadOnly();
	virtual void SAL_CALL setCurrentLocale( const ::com::sun::star::lang::Locale& locale, ::sal_Bool FindClosestMatch );
	virtual void SAL_CALL setDefaultLocale( const ::com::sun::star::lang::Locale& locale );
    virtual void SAL_CALL setString( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str );
    virtual void SAL_CALL setStringForLocale( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeId( const ::rtl::OUString& ResourceID );
    virtual void SAL_CALL removeIdForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL newLocale( const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeLocale( const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Int32 SAL_CALL getUniqueNumericId(  );

	// XStringResourcePersistence
    virtual void SAL_CALL store(  );
    virtual ::sal_Bool SAL_CALL isModified(  );
    virtual void SAL_CALL setComment( const ::rtl::OUString& Comment );
	virtual void SAL_CALL storeToStorage
		( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage,
		  const ::rtl::OUString& NameBase, const ::rtl::OUString& Comment );
    virtual void SAL_CALL storeToURL( const ::rtl::OUString& URL, const ::rtl::OUString& NameBase,
		const ::rtl::OUString& Comment,	const ::com::sun::star::uno::Reference
		< ::com::sun::star::task::XInteractionHandler >& Handler );
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL exportBinary(  );
    virtual void SAL_CALL importBinary( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& Data );
};


typedef ::cppu::ImplInheritanceHelper2<
		StringResourcePersistenceImpl,
	    ::com::sun::star::lang::XInitialization,
		::com::sun::star::resource::XStringResourceWithStorage > StringResourceWithStorageImpl_BASE;

class StringResourceWithStorageImpl : public StringResourceWithStorageImpl_BASE
{
	::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >		m_xStorage;
	bool																		m_bStorageChanged;

	virtual void implScanLocales( void );
	virtual bool implLoadLocale( LocaleItem* pLocaleItem );

public:
    StringResourceWithStorageImpl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
    virtual ~StringResourceWithStorageImpl();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XModifyBroadcaster
	virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
	virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

 	// XStringResourceResolver
	virtual ::rtl::OUString SAL_CALL resolveString( const ::rtl::OUString& ResourceID );
    virtual ::rtl::OUString SAL_CALL resolveStringForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Bool SAL_CALL hasEntryForId( const ::rtl::OUString& ResourceID );
    virtual ::sal_Bool SAL_CALL hasEntryForIdAndLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDs(  );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDsForLocale
		( const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::lang::Locale SAL_CALL getCurrentLocale(  );
	virtual ::com::sun::star::lang::Locale SAL_CALL getDefaultLocale(  );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getLocales(  );

	// XStringResourceManager
	virtual ::sal_Bool SAL_CALL isReadOnly();
	virtual void SAL_CALL setCurrentLocale( const ::com::sun::star::lang::Locale& locale, ::sal_Bool FindClosestMatch );
	virtual void SAL_CALL setDefaultLocale( const ::com::sun::star::lang::Locale& locale );
    virtual void SAL_CALL setString( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str );
    virtual void SAL_CALL setStringForLocale( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeId( const ::rtl::OUString& ResourceID );
    virtual void SAL_CALL removeIdForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL newLocale( const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeLocale( const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Int32 SAL_CALL getUniqueNumericId(  );

	// XStringResourcePersistence
    virtual void SAL_CALL store(  );
    virtual ::sal_Bool SAL_CALL isModified(  );
    virtual void SAL_CALL setComment( const ::rtl::OUString& Comment );
    virtual void SAL_CALL storeToStorage
		( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage,
		  const ::rtl::OUString& NameBase, const ::rtl::OUString& Comment );
    virtual void SAL_CALL storeToURL( const ::rtl::OUString& URL, const ::rtl::OUString& NameBase,
		const ::rtl::OUString& Comment, const ::com::sun::star::uno::Reference
		< ::com::sun::star::task::XInteractionHandler >& Handler );
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL exportBinary(  );
    virtual void SAL_CALL importBinary( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& Data );

	// XStringResourceWithStorage
    virtual void SAL_CALL storeAsStorage
		( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage );
    virtual void SAL_CALL setStorage
		( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage );
};


typedef ::cppu::ImplInheritanceHelper2<
		StringResourcePersistenceImpl,
	    ::com::sun::star::lang::XInitialization,
		::com::sun::star::resource::XStringResourceWithLocation > StringResourceWithLocationImpl_BASE;

class StringResourceWithLocationImpl : public StringResourceWithLocationImpl_BASE
{
	::rtl::OUString																m_aLocation;
	bool																		m_bLocationChanged;
	com::sun::star::uno::Reference< com::sun::star::ucb::XSimpleFileAccess >	m_xSFI;
	com::sun::star::uno::Reference< com::sun::star::task::XInteractionHandler >	m_xInteractionHandler;

	const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess > getFileAccess( void );

	virtual void implScanLocales( void );
	virtual bool implLoadLocale( LocaleItem* pLocaleItem );

public:
    StringResourceWithLocationImpl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
    virtual ~StringResourceWithLocationImpl();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XModifyBroadcaster
	virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
	virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

 	// XStringResourceResolver
	virtual ::rtl::OUString SAL_CALL resolveString( const ::rtl::OUString& ResourceID );
    virtual ::rtl::OUString SAL_CALL resolveStringForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Bool SAL_CALL hasEntryForId( const ::rtl::OUString& ResourceID );
    virtual ::sal_Bool SAL_CALL hasEntryForIdAndLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDs(  );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getResourceIDsForLocale
		( const ::com::sun::star::lang::Locale& locale );
	virtual ::com::sun::star::lang::Locale SAL_CALL getCurrentLocale(  );
	virtual ::com::sun::star::lang::Locale SAL_CALL getDefaultLocale(  );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getLocales(  );

	// XStringResourceManager
	virtual ::sal_Bool SAL_CALL isReadOnly();
	virtual void SAL_CALL setCurrentLocale( const ::com::sun::star::lang::Locale& locale, ::sal_Bool FindClosestMatch );
	virtual void SAL_CALL setDefaultLocale( const ::com::sun::star::lang::Locale& locale );
    virtual void SAL_CALL setString( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str );
    virtual void SAL_CALL setStringForLocale( const ::rtl::OUString& ResourceID, const ::rtl::OUString& Str,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeId( const ::rtl::OUString& ResourceID );
    virtual void SAL_CALL removeIdForLocale( const ::rtl::OUString& ResourceID,
		const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL newLocale( const ::com::sun::star::lang::Locale& locale );
	virtual void SAL_CALL removeLocale( const ::com::sun::star::lang::Locale& locale );
    virtual ::sal_Int32 SAL_CALL getUniqueNumericId(  );

	// XStringResourcePersistence
    virtual void SAL_CALL store(  );
    virtual ::sal_Bool SAL_CALL isModified(  );
    virtual void SAL_CALL setComment( const ::rtl::OUString& Comment );
    virtual void SAL_CALL storeToStorage
		( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage,
		  const ::rtl::OUString& NameBase, const ::rtl::OUString& Comment );
    virtual void SAL_CALL storeToURL( const ::rtl::OUString& URL, const ::rtl::OUString& NameBase,
		const ::rtl::OUString& Comment, const ::com::sun::star::uno::Reference
		< ::com::sun::star::task::XInteractionHandler >& Handler );
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL exportBinary(  );
    virtual void SAL_CALL importBinary( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& Data );

	// XStringResourceWithLocation
    virtual void SAL_CALL storeAsURL( const ::rtl::OUString& URL );
    virtual void SAL_CALL setURL( const ::rtl::OUString& URL );
};

//.........................................................................
}	// namespace stringtable
//.........................................................................

#endif // SCRIPTING_DLGPROV_HXX
