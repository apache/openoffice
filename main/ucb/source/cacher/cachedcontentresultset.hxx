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



#ifndef _CACHED_CONTENT_RESULTSET_HXX
#define _CACHED_CONTENT_RESULTSET_HXX

#include <contentresultsetwrapper.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ucb/XFetchProvider.hpp>
#include <com/sun/star/ucb/XFetchProviderForContentAccess.hpp>
#include <com/sun/star/ucb/FetchResult.hpp>
#include <com/sun/star/ucb/XContentIdentifierMapping.hpp>
#include <com/sun/star/ucb/XCachedContentResultSetFactory.hpp>

#define CACHED_CONTENT_RESULTSET_SERVICE_NAME "com.sun.star.ucb.CachedContentResultSet"
#define CACHED_CONTENT_RESULTSET_FACTORY_NAME "com.sun.star.ucb.CachedContentResultSetFactory"

//=========================================================================

namespace com { namespace sun { namespace star { namespace script {
	class XTypeConverter;
} } } }

class CCRS_PropertySetInfo;
class CachedContentResultSet
				: public ContentResultSetWrapper
				, public com::sun::star::lang::XTypeProvider
				, public com::sun::star::lang::XServiceInfo
{
	//--------------------------------------------------------------------------
	// class CCRS_Cache

	class CCRS_Cache
	{
	private:
		com::sun::star::ucb::FetchResult*			m_pResult;
		com::sun::star::uno::Reference<
			com::sun::star::ucb::XContentIdentifierMapping >
													m_xContentIdentifierMapping;
		com::sun::star::uno::Sequence< sal_Bool >*	m_pMappedReminder;

	private:
		com::sun::star::uno::Any& SAL_CALL
		getRowAny( sal_Int32 nRow );

		void SAL_CALL clear();


		void SAL_CALL remindMapped( sal_Int32 nRow );
		sal_Bool SAL_CALL isRowMapped( sal_Int32 nRow );
		void SAL_CALL clearMappedReminder();
		com::sun::star::uno::Sequence< sal_Bool >* SAL_CALL getMappedReminder();

	public:
		CCRS_Cache( const com::sun::star::uno::Reference<
				com::sun::star::ucb::XContentIdentifierMapping > & xMapping );
		~CCRS_Cache();

		void SAL_CALL loadData(
			const com::sun::star::ucb::FetchResult& rResult );

		sal_Bool SAL_CALL
		hasRow( sal_Int32 nRow );

		sal_Bool SAL_CALL
		hasCausedException( sal_Int32 nRow );

		sal_Int32 SAL_CALL
		getMaxRow();

		sal_Bool SAL_CALL
		hasKnownLast();

		//---
		const com::sun::star::uno::Any& SAL_CALL
		getAny( sal_Int32 nRow, sal_Int32 nColumnIndex );

		const rtl::OUString& SAL_CALL
		getContentIdentifierString( sal_Int32 nRow );

		const com::sun::star::uno::Reference<
				com::sun::star::ucb::XContentIdentifier >& SAL_CALL
		getContentIdentifier( sal_Int32 nRow );

		const com::sun::star::uno::Reference<
				com::sun::star::ucb::XContent >& SAL_CALL
		getContent( sal_Int32 nRow );
	};
	//-----------------------------------------------------------------
	//members

    com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
                            m_xSMgr;

	//different Interfaces from Origin:
	com::sun::star::uno::Reference< com::sun::star::ucb::XFetchProvider >
							m_xFetchProvider; //XFetchProvider-interface from m_xOrigin

	com::sun::star::uno::Reference< com::sun::star::ucb::XFetchProviderForContentAccess >
							m_xFetchProviderForContentAccess; //XFetchProviderForContentAccess-interface from m_xOrigin

	//my PropertySetInfo
	com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo >
							m_xMyPropertySetInfo;//holds m_pMyPropSetInfo alive
	CCRS_PropertySetInfo*	m_pMyPropSetInfo;


	//
	com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifierMapping >
							m_xContentIdentifierMapping;// can be used for remote optimized ContentAccess

	//some Properties and helping variables
	sal_Int32				m_nRow;
	sal_Bool				m_bAfterLast; // TRUE, if m_nRow is after final count; can be TRUE without knowing the exact final count

	sal_Int32				m_nLastAppliedPos;
	sal_Bool				m_bAfterLastApplied;

	sal_Int32				m_nKnownCount; // count we know from the Origin
	sal_Bool				m_bFinalCount; // TRUE if the Origin has reached final count and we got that count in m_nKnownCount

	sal_Int32				m_nFetchSize;
	sal_Int32				m_nFetchDirection;

	sal_Bool				m_bLastReadWasFromCache;
	sal_Bool				m_bLastCachedReadWasNull;

	//cache:
	CCRS_Cache				m_aCache;
	CCRS_Cache				m_aCacheContentIdentifierString;
	CCRS_Cache				m_aCacheContentIdentifier;
	CCRS_Cache				m_aCacheContent;


private:
	//-----------------------------------------------------------------
	//helping XPropertySet methods.
	virtual void SAL_CALL impl_initPropertySetInfo();


	//-----------------------------------------------------------------
	sal_Bool SAL_CALL
	applyPositionToOrigin( sal_Int32 nRow );

	void SAL_CALL
	impl_fetchData( sal_Int32 nRow, sal_Int32 nCount
					, sal_Int32 nFetchDirection );

	sal_Bool SAL_CALL
	impl_isKnownValidPosition( sal_Int32 nRow );

	sal_Bool SAL_CALL
	impl_isKnownInvalidPosition( sal_Int32 nRow );

	void SAL_CALL
	impl_changeRowCount( sal_Int32 nOld, sal_Int32 nNew );

	void SAL_CALL
	impl_changeIsRowCountFinal( sal_Bool bOld, sal_Bool bNew );

public:
    CachedContentResultSet(
                        const com::sun::star::uno::Reference<
                            com::sun::star::lang::XMultiServiceFactory > &
                                xSMgr,
                        const com::sun::star::uno::Reference<
                            com::sun::star::sdbc::XResultSet > & xOrigin,
                        const com::sun::star::uno::Reference<
                            com::sun::star::ucb::XContentIdentifierMapping > &
                                xContentIdentifierMapping );

	virtual ~CachedContentResultSet();

	//-----------------------------------------------------------------
	// XInterface inherited
	//-----------------------------------------------------------------
	XINTERFACE_DECL()
	//-----------------------------------------------------------------
	// XTypeProvider
	//-----------------------------------------------------------------
	XTYPEPROVIDER_DECL()
	//-----------------------------------------------------------------
    // XServiceInfo
	//-----------------------------------------------------------------
	XSERVICEINFO_NOFACTORY_DECL()

	//-----------------------------------------------------------------
	// XPropertySet inherited
	//-----------------------------------------------------------------

    virtual void SAL_CALL
	setPropertyValue( const rtl::OUString& aPropertyName,
					  const com::sun::star::uno::Any& aValue );

    virtual com::sun::star::uno::Any SAL_CALL
	getPropertyValue( const rtl::OUString& PropertyName );

	//-----------------------------------------------------------------
	// own inherited
	//-----------------------------------------------------------------
	virtual void SAL_CALL
	impl_disposing( const com::sun::star::lang::EventObject& Source );

	virtual void SAL_CALL
	impl_propertyChange( const com::sun::star::beans::PropertyChangeEvent& evt );

	virtual void SAL_CALL
	impl_vetoableChange( const com::sun::star::beans::PropertyChangeEvent& aEvent );

	//-----------------------------------------------------------------
	// XContentAccess inherited
	//-----------------------------------------------------------------
    virtual rtl::OUString SAL_CALL
	queryContentIdentifierString();

    virtual com::sun::star::uno::Reference<
				com::sun::star::ucb::XContentIdentifier > SAL_CALL
	queryContentIdentifier();

    virtual com::sun::star::uno::Reference<
				com::sun::star::ucb::XContent > SAL_CALL
	queryContent();

	//-----------------------------------------------------------------
	// XResultSet inherited
	//-----------------------------------------------------------------
    virtual sal_Bool SAL_CALL
	next();
    virtual sal_Bool SAL_CALL
	isBeforeFirst();
    virtual sal_Bool SAL_CALL
	isAfterLast();
    virtual sal_Bool SAL_CALL
	isFirst();
    virtual sal_Bool SAL_CALL
	isLast();
    virtual void SAL_CALL
	beforeFirst();
    virtual void SAL_CALL
	afterLast();
    virtual sal_Bool SAL_CALL
	first();
    virtual sal_Bool SAL_CALL
	last();
    virtual sal_Int32 SAL_CALL
	getRow();
    virtual sal_Bool SAL_CALL
	absolute( sal_Int32 row );
    virtual sal_Bool SAL_CALL
	relative( sal_Int32 rows );
    virtual sal_Bool SAL_CALL
	previous();
    virtual void SAL_CALL
	refreshRow();
    virtual sal_Bool SAL_CALL
	rowUpdated();
    virtual sal_Bool SAL_CALL
	rowInserted();
    virtual sal_Bool SAL_CALL
	rowDeleted();
    virtual com::sun::star::uno::Reference<
				com::sun::star::uno::XInterface > SAL_CALL
	getStatement();

	//-----------------------------------------------------------------
	// XRow inherited
	//-----------------------------------------------------------------
    virtual sal_Bool SAL_CALL
	wasNull();

    virtual rtl::OUString SAL_CALL
	getString( sal_Int32 columnIndex );

    virtual sal_Bool SAL_CALL
	getBoolean( sal_Int32 columnIndex );

    virtual sal_Int8 SAL_CALL
	getByte( sal_Int32 columnIndex );

    virtual sal_Int16 SAL_CALL
	getShort( sal_Int32 columnIndex );

    virtual sal_Int32 SAL_CALL
	getInt( sal_Int32 columnIndex );

    virtual sal_Int64 SAL_CALL
	getLong( sal_Int32 columnIndex );

    virtual float SAL_CALL
	getFloat( sal_Int32 columnIndex );

    virtual double SAL_CALL
	getDouble( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL
	getBytes( sal_Int32 columnIndex );

    virtual com::sun::star::util::Date SAL_CALL
	getDate( sal_Int32 columnIndex );

    virtual com::sun::star::util::Time SAL_CALL
	getTime( sal_Int32 columnIndex );

    virtual com::sun::star::util::DateTime SAL_CALL
	getTimestamp( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Reference<
				com::sun::star::io::XInputStream > SAL_CALL
	getBinaryStream( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Reference<
				com::sun::star::io::XInputStream > SAL_CALL
	getCharacterStream( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Any SAL_CALL
	getObject( sal_Int32 columnIndex,
			   const com::sun::star::uno::Reference<
			   	com::sun::star::container::XNameAccess >& typeMap );

    virtual com::sun::star::uno::Reference<
				com::sun::star::sdbc::XRef > SAL_CALL
	getRef( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Reference<
				com::sun::star::sdbc::XBlob > SAL_CALL
	getBlob( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Reference<
				com::sun::star::sdbc::XClob > SAL_CALL
	getClob( sal_Int32 columnIndex );

    virtual com::sun::star::uno::Reference<
				com::sun::star::sdbc::XArray > SAL_CALL
	getArray( sal_Int32 columnIndex );

	//-----------------------------------------------------------------
    // Type Converter support
	//-----------------------------------------------------------------

private:
    sal_Bool m_bTriedToGetTypeConverter;
    com::sun::star::uno::Reference<
        com::sun::star::script::XTypeConverter > m_xTypeConverter;

    const com::sun::star::uno::Reference<
        com::sun::star::script::XTypeConverter >& getTypeConverter();
};

//=========================================================================

class CachedContentResultSetFactory
				: public cppu::OWeakObject
				, public com::sun::star::lang::XTypeProvider
				, public com::sun::star::lang::XServiceInfo
				, public com::sun::star::ucb::XCachedContentResultSetFactory
{
protected:
	com::sun::star::uno::Reference<
		com::sun::star::lang::XMultiServiceFactory >	m_xSMgr;

public:

	CachedContentResultSetFactory(
		const com::sun::star::uno::Reference<
		com::sun::star::lang::XMultiServiceFactory > & rSMgr);

	virtual ~CachedContentResultSetFactory();

	//-----------------------------------------------------------------
	// XInterface
	XINTERFACE_DECL()

	//-----------------------------------------------------------------
	// XTypeProvider
	XTYPEPROVIDER_DECL()

	//-----------------------------------------------------------------
    // XServiceInfo
	XSERVICEINFO_DECL()

	//-----------------------------------------------------------------
    // XCachedContentResultSetFactory

	virtual com::sun::star::uno::Reference<
				com::sun::star::sdbc::XResultSet > SAL_CALL
	createCachedContentResultSet(
			const com::sun::star::uno::Reference<
				com::sun::star::sdbc::XResultSet > & xSource,
			const com::sun::star::uno::Reference<
				com::sun::star::ucb::XContentIdentifierMapping > & xMapping	);
};

#endif
