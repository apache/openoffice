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



#include "SResultSet.hxx"
#include "SResultSetMetaData.hxx"
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/sdbcx/CompareBookmark.hpp>
#include <cppuhelper/typeprovider.hxx>
#include <com/sun/star/lang/DisposedException.hpp>
#include "propertyids.hxx"

using namespace connectivity::skeleton;
using namespace cppu;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::container;
using namespace com::sun::star::io;
using namespace com::sun::star::util;

//------------------------------------------------------------------------------
//	IMPLEMENT_SERVICE_INFO(OResultSet,"com.sun.star.sdbcx.OResultSet","com.sun.star.sdbc.ResultSet");
::rtl::OUString SAL_CALL OResultSet::getImplementationName(  )	\
{
	return ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.skeleton.ResultSet");
}
// -------------------------------------------------------------------------
 Sequence< ::rtl::OUString > SAL_CALL OResultSet::getSupportedServiceNames(  )
{
	 Sequence< ::rtl::OUString > aSupported(2);
	aSupported[0] = ::rtl::OUString::createFromAscii("com.sun.star.sdbc.ResultSet");
	aSupported[1] = ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.ResultSet");
	return aSupported;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::supportsService( const ::rtl::OUString& _rServiceName )
{
	Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
	const ::rtl::OUString* pSupported = aSupported.getConstArray();
	const ::rtl::OUString* pEnd = pSupported + aSupported.getLength();
	for (;pSupported != pEnd && !pSupported->equals(_rServiceName); ++pSupported)
		;

	return pSupported != pEnd;
}

// -------------------------------------------------------------------------
OResultSet::OResultSet(OStatement_Base* pStmt)
	: OResultSet_BASE(m_aMutex)
	,OPropertySetHelper(OResultSet_BASE::rBHelper)
	,m_aStatement((OWeakObject*)pStmt)
	,m_xMetaData(NULL)
	,m_nTextEncoding(pStmt->getOwnConnection()->getTextEncoding())
	,m_pStatement(pStmt)
	,m_bWasNull(sal_True)
{
}
// -------------------------------------------------------------------------
OResultSet::~OResultSet()
{
}
// -------------------------------------------------------------------------
void OResultSet::disposing(void)
{
	OPropertySetHelper::disposing();

	::osl::MutexGuard aGuard(m_aMutex);

	m_aStatement	= NULL;
	m_xMetaData		= NULL;
}
// -------------------------------------------------------------------------
Any SAL_CALL OResultSet::queryInterface( const Type & rType )
{
	Any aRet = OPropertySetHelper::queryInterface(rType);
	if(!aRet.hasValue())
		aRet = OResultSet_BASE::queryInterface(rType);
	return aRet;
}
// -------------------------------------------------------------------------
 Sequence<  Type > SAL_CALL OResultSet::getTypes(  )
{
	OTypeCollection aTypes(
        ::cppu::UnoType< Reference< ::com::sun::star::beans::XMultiPropertySet > >::get(),
        ::cppu::UnoType< Reference< ::com::sun::star::beans::XFastPropertySet > >::get(),
        ::cppu::UnoType< Reference< ::com::sun::star::beans::XPropertySet > >::get());

	return concatSequences(aTypes.getTypes(),OResultSet_BASE::getTypes());
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSet::findColumn( const ::rtl::OUString& columnName )
{

	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	// find the first column with the name columnName

	::osl::MutexGuard aGuard( m_aMutex );

	Reference< XResultSetMetaData > xMeta = getMetaData();
	sal_Int32 nLen = xMeta->getColumnCount();
	sal_Int32 i = 1;
	for(;i<=nLen;++i)
		if(xMeta->isCaseSensitive(i) ? columnName == xMeta->getColumnName(i) :
				columnName.equalsIgnoreAsciiCase(xMeta->getColumnName(i)))
			break;
	return i;
}
// -------------------------------------------------------------------------
Reference< XInputStream > SAL_CALL OResultSet::getBinaryStream( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return NULL;
}
// -------------------------------------------------------------------------
Reference< XInputStream > SAL_CALL OResultSet::getCharacterStream( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return NULL;
}

// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::getBoolean( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------

sal_Int8 SAL_CALL OResultSet::getByte( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	sal_Int8 nRet = 0;
	return nRet;
}
// -------------------------------------------------------------------------

Sequence< sal_Int8 > SAL_CALL OResultSet::getBytes( sal_Int32 columnIndex )
{

	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
	::osl::MutexGuard aGuard( m_aMutex );

	return Sequence< sal_Int8 >();
}
// -------------------------------------------------------------------------

Date SAL_CALL OResultSet::getDate( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	Date nRet;
	return nRet;
}
// -------------------------------------------------------------------------

double SAL_CALL OResultSet::getDouble( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	double nRet = 0;
	return nRet;
}
// -------------------------------------------------------------------------

float SAL_CALL OResultSet::getFloat( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	float nVal(0);
	return nVal;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSet::getInt( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	sal_Int32 nRet=0;
	return nRet;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSet::getRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	sal_Int32 nValue = 0;
	return nValue;
}
// -------------------------------------------------------------------------

sal_Int64 SAL_CALL OResultSet::getLong( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_Int64();
}
// -------------------------------------------------------------------------

Reference< XResultSetMetaData > SAL_CALL OResultSet::getMetaData(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	if(!m_xMetaData.is())
		m_xMetaData = new OResultSetMetaData(m_pStatement->getOwnConnection());
	return m_xMetaData;
}
// -------------------------------------------------------------------------
Reference< XArray > SAL_CALL OResultSet::getArray( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return NULL;
}

// -------------------------------------------------------------------------

Reference< XClob > SAL_CALL OResultSet::getClob( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return NULL;
}
// -------------------------------------------------------------------------
Reference< XBlob > SAL_CALL OResultSet::getBlob( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return NULL;
}
// -------------------------------------------------------------------------

Reference< XRef > SAL_CALL OResultSet::getRef( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return NULL;
}
// -------------------------------------------------------------------------

Any SAL_CALL OResultSet::getObject( sal_Int32 columnIndex, const Reference< ::com::sun::star::container::XNameAccess >& typeMap )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return Any();
}
// -------------------------------------------------------------------------

sal_Int16 SAL_CALL OResultSet::getShort( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	sal_Int16 nRet=0;
	return nRet;
}
// -------------------------------------------------------------------------


::rtl::OUString SAL_CALL OResultSet::getString( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	::rtl::OUString nRet;
	return nRet;
}
// -------------------------------------------------------------------------

Time SAL_CALL OResultSet::getTime( sal_Int32 columnIndex )
{
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	::osl::MutexGuard aGuard( m_aMutex );

	Time nRet;
	return nRet;
}
// -------------------------------------------------------------------------


DateTime SAL_CALL OResultSet::getTimestamp( sal_Int32 columnIndex )
{
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	::osl::MutexGuard aGuard( m_aMutex );

	DateTime nRet;
	return nRet;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::isBeforeFirst(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	// here you have to implement your movements
	// return true means there is no data
	return sal_True;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::isAfterLast(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_True;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::isFirst(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::isLast(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::beforeFirst(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	// move before the first row so that isBeforeFirst returns false
	// the same for other movement methods
}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::afterLast(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::close(  )
{
	{
		::osl::MutexGuard aGuard( m_aMutex );
		checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	}
	dispose();
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::first(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::last(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::absolute( sal_Int32 row )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::relative( sal_Int32 row )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::previous(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OResultSet::getStatement(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return m_aStatement.get();
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::rowDeleted(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::rowInserted(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::rowUpdated(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::next(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return sal_False;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSet::wasNull(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return m_bWasNull;
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::cancel(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::clearWarnings(  )
{
}
// -------------------------------------------------------------------------
Any SAL_CALL OResultSet::getWarnings(  )
{
	return Any();
}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::insertRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	// you only have to implement this if you want to insert new rows
}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	// only when you allow updates
}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::deleteRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::cancelRowUpdates(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::moveToInsertRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	// only when you allow insert's
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::moveToCurrentRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateNull( sal_Int32 columnIndex )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateBoolean( sal_Int32 columnIndex, sal_Bool x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateByte( sal_Int32 columnIndex, sal_Int8 x )
{
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
	::osl::MutexGuard aGuard( m_aMutex );

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateShort( sal_Int32 columnIndex, sal_Int16 x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateInt( sal_Int32 columnIndex, sal_Int32 x )
{
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);
	::osl::MutexGuard aGuard( m_aMutex );

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateLong( sal_Int32 columnIndex, sal_Int64 x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -----------------------------------------------------------------------
void SAL_CALL OResultSet::updateFloat( sal_Int32 columnIndex, float x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateDouble( sal_Int32 columnIndex, double x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateString( sal_Int32 columnIndex, const ::rtl::OUString& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateBytes( sal_Int32 columnIndex, const Sequence< sal_Int8 >& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateDate( sal_Int32 columnIndex, const Date& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateTime( sal_Int32 columnIndex, const Time& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateTimestamp( sal_Int32 columnIndex, const DateTime& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateBinaryStream( sal_Int32 columnIndex, const Reference< XInputStream >& x, sal_Int32 length )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateCharacterStream( sal_Int32 columnIndex, const Reference< XInputStream >& x, sal_Int32 length )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::refreshRow(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
void SAL_CALL OResultSet::updateObject( sal_Int32 columnIndex, const Any& x )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------

void SAL_CALL OResultSet::updateNumericObject( sal_Int32 columnIndex, const Any& x, sal_Int32 scale )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

}
// -------------------------------------------------------------------------
// XRowLocate
Any SAL_CALL OResultSet::getBookmark(  )
{
	 ::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	// if you don't want to support bookmark you must remove the XRowLocate interface

	 return Any();
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::moveToBookmark( const  Any& bookmark )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::moveRelativeToBookmark( const  Any& bookmark, sal_Int32 rows )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return sal_False;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OResultSet::compareBookmarks( const  Any& first, const  Any& second )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);


	return CompareBookmark::NOT_EQUAL;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSet::hasOrderedBookmarks(  )
{
	return sal_False;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OResultSet::hashBookmark( const  Any& bookmark )
{
	throw SQLException();
}
// -------------------------------------------------------------------------
// XDeleteRows
Sequence< sal_Int32 > SAL_CALL OResultSet::deleteRows( const  Sequence<  Any >& rows )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OResultSet_BASE::rBHelper.bDisposed);

	return Sequence< sal_Int32 >();
}
// -------------------------------------------------------------------------
IPropertyArrayHelper* OResultSet::createArrayHelper( ) const
{
	Sequence< Property > aProps(6);
	Property* pProperties = aProps.getArray();
	sal_Int32 nPos = 0;
	DECL_PROP1IMPL(CURSORNAME,			::rtl::OUString) PropertyAttribute::READONLY);
	DECL_PROP0(FETCHDIRECTION,			sal_Int32);
	DECL_PROP0(FETCHSIZE,				sal_Int32);
	DECL_BOOL_PROP1IMPL(ISBOOKMARKABLE) PropertyAttribute::READONLY);
	DECL_PROP1IMPL(RESULTSETCONCURRENCY,sal_Int32) PropertyAttribute::READONLY);
	DECL_PROP1IMPL(RESULTSETTYPE,		sal_Int32) PropertyAttribute::READONLY);

	return new OPropertyArrayHelper(aProps);
}
// -------------------------------------------------------------------------
IPropertyArrayHelper & OResultSet::getInfoHelper()
{
	return *const_cast<OResultSet*>(this)->getArrayHelper();
}
// -------------------------------------------------------------------------
sal_Bool OResultSet::convertFastPropertyValue(
							Any & rConvertedValue,
							Any & rOldValue,
							sal_Int32 nHandle,
							const Any& rValue )
{
	switch(nHandle)
	{
		case PROPERTY_ID_ISBOOKMARKABLE:
		case PROPERTY_ID_CURSORNAME:
		case PROPERTY_ID_RESULTSETCONCURRENCY:
		case PROPERTY_ID_RESULTSETTYPE:
			throw ::com::sun::star::lang::IllegalArgumentException();
			break;
		case PROPERTY_ID_FETCHDIRECTION:
		case PROPERTY_ID_FETCHSIZE:
		default:
			;
	}
	return sal_False;
}
// -------------------------------------------------------------------------
void OResultSet::setFastPropertyValue_NoBroadcast(
								sal_Int32 nHandle,
								const Any& rValue
												 )
{
	switch(nHandle)
	{
		case PROPERTY_ID_ISBOOKMARKABLE:
		case PROPERTY_ID_CURSORNAME:
		case PROPERTY_ID_RESULTSETCONCURRENCY:
		case PROPERTY_ID_RESULTSETTYPE:
			throw Exception();
			break;
		case PROPERTY_ID_FETCHDIRECTION:
			break;
		case PROPERTY_ID_FETCHSIZE:
			break;
		default:
			;
	}
}
// -------------------------------------------------------------------------
void OResultSet::getFastPropertyValue(
								Any& rValue,
								sal_Int32 nHandle
									 ) const
{
	switch(nHandle)
	{
		case PROPERTY_ID_ISBOOKMARKABLE:
		case PROPERTY_ID_CURSORNAME:
		case PROPERTY_ID_RESULTSETCONCURRENCY:
		case PROPERTY_ID_RESULTSETTYPE:
		case PROPERTY_ID_FETCHDIRECTION:
		case PROPERTY_ID_FETCHSIZE:
			;
	}
}
// -----------------------------------------------------------------------------
void SAL_CALL OResultSet::acquire() throw()
{
	OResultSet_BASE::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OResultSet::release() throw()
{
	OResultSet_BASE::release();
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL OResultSet::getPropertySetInfo(  )
{
	return ::cppu::OPropertySetHelper::createPropertySetInfo(getInfoHelper());
}
// -----------------------------------------------------------------------------
