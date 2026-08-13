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



#ifndef _SORTRESULT_HXX
#define _SORTRESULT_HXX

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XResultSetMetaData.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/ucb/NumberedSortingInfo.hpp>
#include <com/sun/star/ucb/XAnyCompareFactory.hpp>
#include <com/sun/star/ucb/ListAction.hpp>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>

#include <deque>
#include <ucbhelper/macros.hxx>

namespace cppu {
	class OInterfaceContainerHelper;
}

//-----------------------------------------------------------------------------
struct	SortInfo;
struct	SortListData;
class	SRSPropertySetInfo;
class	PropertyChangeListeners_Impl;

//-----------------------------------------------------------------------------
class SortedEntryList
{
	std::deque < SortListData* > maData;

public:
						 SortedEntryList(){}
						~SortedEntryList(){ Clear(); }

	sal_uInt32			Count() const { return (sal_uInt32) maData.size(); }

	void				Clear();
	void				Insert( SortListData *pEntry, long nPos );
	SortListData*		Remove( long nPos );
	SortListData*		GetData( long nPos );

	long				operator [] ( long nPos ) const;
};

//-----------------------------------------------------------------------------
#define LISTACTION	com::sun::star::ucb::ListAction

class EventList
{
	std::deque < LISTACTION* > maData;

public:
					 EventList(){}
					~EventList(){ Clear(); }

	sal_uInt32		Count() { return (sal_uInt32) maData.size(); }

	void			AddEvent( long nType, long nPos, long nCount );
	void			Insert( LISTACTION *pAction ) { maData.push_back( pAction ); }
	void			Clear();
	LISTACTION*		GetAction( long nIndex ) { return maData[ nIndex ]; }
};

//-----------------------------------------------------------------------------

class SimpleList
{
	std::deque < void* > maData;

public:
					 SimpleList(){}
					~SimpleList(){ Clear(); }

	sal_uInt32		Count() { return (sal_uInt32) maData.size(); }
	void			Clear() { maData.clear(); }

	void			Remove( sal_uInt32 nPos );
	void			Remove( void* pData );

	void			Append( void* pData )
						{ maData.push_back( pData ); }
	void			Insert( void* pData, sal_uInt32 nPos );
	void*			GetObject( sal_uInt32 nPos ) const;
	void			Replace( void* pData, sal_uInt32 nPos );
};

//-----------------------------------------------------------------------------

#define PROPERTYCHANGEEVENT		com::sun::star::beans::PropertyChangeEvent
#define RUNTIME_EXCEPTION		com::sun::star::uno::RuntimeException
#define REFERENCE				com::sun::star::uno::Reference
#define SEQUENCE				com::sun::star::uno::Sequence
#define XEVENTLISTENER			com::sun::star::lang::XEventListener
#define XRESULTSET				com::sun::star::sdbc::XResultSet
#define SQLEXCEPTION			com::sun::star::sdbc::SQLException
#define XRESULTSETMETADATA		com::sun::star::sdbc::XResultSetMetaData
#define NUMBERED_SORTINGINFO	com::sun::star::ucb::NumberedSortingInfo
#define XANYCOMPAREFACTORY		com::sun::star::ucb::XAnyCompareFactory

#define RESULTSET_SERVICE_NAME 	"com.sun.star.ucb.SortedResultSet"

//-----------------------------------------------------------------------------

class SortedResultSet:
				public cppu::OWeakObject,
				public com::sun::star::lang::XTypeProvider,
				public com::sun::star::lang::XServiceInfo,
				public com::sun::star::lang::XComponent,
				public com::sun::star::ucb::XContentAccess,
				public XRESULTSET,
				public com::sun::star::sdbc::XRow,
				public com::sun::star::sdbc::XCloseable,
				public com::sun::star::sdbc::XResultSetMetaDataSupplier,
				public com::sun::star::beans::XPropertySet
{
	cppu::OInterfaceContainerHelper	*mpDisposeEventListeners;
	PropertyChangeListeners_Impl	*mpPropChangeListeners;
	PropertyChangeListeners_Impl	*mpVetoChangeListeners;

	REFERENCE < XRESULTSET >			mxOriginal;
	REFERENCE < XRESULTSET >			mxOther;

	SRSPropertySetInfo*	mpPropSetInfo;
	SortInfo*			mpSortInfo;
	osl::Mutex			maMutex;
	SortedEntryList		maS2O;			// maps the sorted entries to the original ones
	SimpleList			maO2S;			// maps the original Entries to the sorted ones
	SimpleList			maModList;		// keeps track of modified entries
	long				mnLastSort;		// index of the last sorted entry;
	long				mnCurEntry;		// index of the current entry
	long				mnCount;		// total count of the elements
	sal_Bool			mbIsCopy;


private:

	long				FindPos( SortListData *pEntry, long nStart, long nEnd );
	long				Compare( SortListData *pOne,
								 SortListData *pTwo );
	void				BuildSortInfo( REFERENCE< XRESULTSET > aResult,
									   const SEQUENCE < NUMBERED_SORTINGINFO > &xSortInfo,
									   const REFERENCE< XANYCOMPAREFACTORY > &xCompFac );
	long				CompareImpl( REFERENCE < XRESULTSET > xResultOne,
									 REFERENCE < XRESULTSET > xResultTwo,
									 long nIndexOne, long nIndexTwo,
									 SortInfo* pSortInfo );
	long				CompareImpl( REFERENCE < XRESULTSET > xResultOne,
									 REFERENCE < XRESULTSET > xResultTwo,
									 long nIndexOne, long nIndexTwo );
	void				PropertyChanged( const PROPERTYCHANGEEVENT& rEvt );

public:
						SortedResultSet( REFERENCE< XRESULTSET > aResult );
						~SortedResultSet();

	const SortedEntryList*		GetS2OList() const { return &maS2O; }
	const SimpleList*			GetO2SList() const { return &maO2S; }
	REFERENCE < XRESULTSET >	GetResultSet() const { return mxOriginal; }
	SortInfo*					GetSortInfo() const { return mpSortInfo; }
	long						GetCount() const { return mnCount; }

	void				CopyData( SortedResultSet* pSource );
	void				Initialize( const SEQUENCE < NUMBERED_SORTINGINFO > &xSortInfo,
									const REFERENCE< XANYCOMPAREFACTORY > &xCompFac );
	void				CheckProperties( long nOldCount, sal_Bool bWasFinal );

	void				InsertNew( long nPos, long nCount );
	void				SetChanged( long nPos, long nCount );
	void				Remove( long nPos, long nCount, EventList *pList );
	void				Move( long nPos, long nCount, long nOffset );

	void				ResortModified( EventList* pList );
	void				ResortNew( EventList* pList );

	// XInterface
	XINTERFACE_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XServiceInfo
	XSERVICEINFO_NOFACTORY_DECL()

	// XComponent
	virtual void SAL_CALL
	dispose();

    virtual void SAL_CALL
	addEventListener( const REFERENCE< XEVENTLISTENER >& Listener );

    virtual void SAL_CALL
	removeEventListener( const REFERENCE< XEVENTLISTENER >& Listener );

	// XContentAccess
    virtual rtl::OUString SAL_CALL
	queryContentIdentifierString();
    virtual REFERENCE<
				com::sun::star::ucb::XContentIdentifier > SAL_CALL
	queryContentIdentifier();
    virtual REFERENCE<
				com::sun::star::ucb::XContent > SAL_CALL
	queryContent();

	// XResultSet
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
    virtual REFERENCE<
				com::sun::star::uno::XInterface > SAL_CALL
	getStatement();

	// XRow
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

	virtual REFERENCE<
				com::sun::star::io::XInputStream > SAL_CALL
	getBinaryStream( sal_Int32 columnIndex );

	virtual REFERENCE<
				com::sun::star::io::XInputStream > SAL_CALL
	getCharacterStream( sal_Int32 columnIndex );

	virtual com::sun::star::uno::Any SAL_CALL
	getObject( sal_Int32 columnIndex,
			   const REFERENCE<
			   	com::sun::star::container::XNameAccess >& typeMap );
    virtual REFERENCE<
				com::sun::star::sdbc::XRef > SAL_CALL
	getRef( sal_Int32 columnIndex );
    virtual REFERENCE<
				com::sun::star::sdbc::XBlob > SAL_CALL
	getBlob( sal_Int32 columnIndex );
    virtual REFERENCE<
				com::sun::star::sdbc::XClob > SAL_CALL
	getClob( sal_Int32 columnIndex );
    virtual REFERENCE<
				com::sun::star::sdbc::XArray > SAL_CALL
	getArray( sal_Int32 columnIndex );

	// XCloseable
    virtual void SAL_CALL
	close();

	// XResultSetMetaDataSupplier
	virtual REFERENCE< XRESULTSETMETADATA > SAL_CALL
	getMetaData();


	// XPropertySet
    virtual REFERENCE<
				com::sun::star::beans::XPropertySetInfo > SAL_CALL
	getPropertySetInfo();

    virtual void SAL_CALL
	setPropertyValue( const rtl::OUString& PropertyName,
					  const com::sun::star::uno::Any& Value );

    virtual com::sun::star::uno::Any SAL_CALL
	getPropertyValue( const rtl::OUString& PropertyName );

    virtual void SAL_CALL
	addPropertyChangeListener( const rtl::OUString& PropertyName,
							   const REFERENCE<
							   		com::sun::star::beans::XPropertyChangeListener >& Listener );

    virtual void SAL_CALL
	removePropertyChangeListener( const rtl::OUString& PropertyName,
								  const REFERENCE<
								  	com::sun::star::beans::XPropertyChangeListener >& Listener );

    virtual void SAL_CALL
	addVetoableChangeListener( const rtl::OUString& PropertyName,
							   const REFERENCE<
							   		com::sun::star::beans::XVetoableChangeListener >& Listener );

    virtual void SAL_CALL
	removeVetoableChangeListener( const rtl::OUString& PropertyName,
								  const REFERENCE<
								  	com::sun::star::beans::XVetoableChangeListener >& aListener );
};

#endif
