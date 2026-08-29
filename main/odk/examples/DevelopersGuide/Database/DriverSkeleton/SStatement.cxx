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



#include <stdio.h>
#include <osl/diagnose.h>
#include "SStatement.hxx"
#include "SConnection.hxx"
#include "SResultSet.hxx"
#include <osl/thread.h>
#include <com/sun/star/sdbc/ResultSetConcurrency.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <com/sun/star/sdbc/FetchDirection.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <cppuhelper/typeprovider.hxx>
#include "propertyids.hxx"

using namespace connectivity::skeleton;
//------------------------------------------------------------------------------
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::container;
using namespace com::sun::star::io;
using namespace com::sun::star::util;
//------------------------------------------------------------------------------
OStatement_Base::OStatement_Base(OConnection* _pConnection )
	: OStatement_BASE(m_aMutex),
	OPropertySetHelper(OStatement_BASE::rBHelper),
	rBHelper(OStatement_BASE::rBHelper),
	m_pConnection(_pConnection)
{
	m_pConnection->acquire();
}
// -----------------------------------------------------------------------------
OStatement_Base::~OStatement_Base()
{
}
//------------------------------------------------------------------------------
void OStatement_Base::disposeResultSet()
{
	// free the cursor if alive
	Reference< XComponent > xComp(m_xResultSet.get(), UNO_QUERY);
	if (xComp.is())
		xComp->dispose();
	m_xResultSet = Reference< XResultSet>();
}
//------------------------------------------------------------------------------
void OStatement_BASE2::disposing()
{
	::osl::MutexGuard aGuard(m_aMutex);

	disposeResultSet();

	if (m_pConnection)
		m_pConnection->release();
	m_pConnection = NULL;

	dispose_ChildImpl();
	OStatement_Base::disposing();
}
//-----------------------------------------------------------------------------
void SAL_CALL OStatement_BASE2::release() throw()
{
	relase_ChildImpl();
}
//-----------------------------------------------------------------------------
Any SAL_CALL OStatement_Base::queryInterface( const Type & rType )
{
	Any aRet = OStatement_BASE::queryInterface(rType);
	if(!aRet.hasValue())
		aRet = OPropertySetHelper::queryInterface(rType);
	return aRet;
}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL OStatement_Base::getTypes(  )
{
	::cppu::OTypeCollection aTypes(
        ::cppu::UnoType< Reference< XMultiPropertySet > >::get(),
        ::cppu::UnoType< Reference< XFastPropertySet > >::get(),
        ::cppu::UnoType< Reference< XPropertySet > >::get());

	return concatSequences(aTypes.getTypes(),OStatement_BASE::getTypes());
}
// -------------------------------------------------------------------------

void SAL_CALL OStatement_Base::cancel(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);
	// cancel the current sql statement
}
// -------------------------------------------------------------------------

void SAL_CALL OStatement_Base::close(  )
{
	{
		::osl::MutexGuard aGuard( m_aMutex );
		checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	}
	dispose();
}
// -------------------------------------------------------------------------

void SAL_CALL OStatement::clearBatch(  )
{
	// if you support batches clear it here
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OStatement_Base::execute( const ::rtl::OUString& sql )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	// returns true when a resultset is available
	return sal_False;
}
// -------------------------------------------------------------------------

Reference< XResultSet > SAL_CALL OStatement_Base::executeQuery( const ::rtl::OUString& sql )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);


	Reference< XResultSet > xRS = NULL;
	// create a resultset as result of executing the sql statement
	// you have to here something :-)
	m_xResultSet = xRS; // we nedd a reference to it for later use
	return xRS;
}
// -------------------------------------------------------------------------

Reference< XConnection > SAL_CALL OStatement_Base::getConnection(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	// just return our connection here
	return (Reference< XConnection >)m_pConnection;
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL OStatement_Base::getUpdateCount(  )
{
	return 0;
}
// -------------------------------------------------------------------------

Any SAL_CALL OStatement::queryInterface( const Type & rType )
{
	Any aRet = ::cppu::queryInterface(rType,static_cast< XBatchExecution*> (this));
	if(!aRet.hasValue())
		aRet = OStatement_Base::queryInterface(rType);
	return aRet;
}
// -------------------------------------------------------------------------

void SAL_CALL OStatement::addBatch( const ::rtl::OUString& sql )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);


	m_aBatchList.push_back(sql);
}
// -------------------------------------------------------------------------
Sequence< sal_Int32 > SAL_CALL OStatement::executeBatch(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	return Sequence< sal_Int32 >();
}
// -------------------------------------------------------------------------


sal_Int32 SAL_CALL OStatement_Base::executeUpdate( const ::rtl::OUString& sql )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	// the return values gives information about how many rows are affected by executing the sql statement
	return 0;

}
// -------------------------------------------------------------------------

Reference< XResultSet > SAL_CALL OStatement_Base::getResultSet(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

//	return our save resultset here
	return m_xResultSet;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OStatement_Base::getMoreResults(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);

	// if your driver supports more than only one resultset
	// and has one more at this moment return true
	return sal_False;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Any SAL_CALL OStatement_Base::getWarnings(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);


	return makeAny(m_aLastWarning);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void SAL_CALL OStatement_Base::clearWarnings(  )
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(OStatement_BASE::rBHelper.bDisposed);


	m_aLastWarning = SQLWarning();
}
// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper* OStatement_Base::createArrayHelper( ) const
{
	// this properties are define by the service statement
	// they must in alphabetic order
	Sequence< Property > aProps(10);
	Property* pProperties = aProps.getArray();
	sal_Int32 nPos = 0;
	DECL_PROP0(CURSORNAME,	::rtl::OUString);
	DECL_BOOL_PROP0(ESCAPEPROCESSING);
	DECL_PROP0(FETCHDIRECTION,sal_Int32);
	DECL_PROP0(FETCHSIZE,	sal_Int32);
	DECL_PROP0(MAXFIELDSIZE,sal_Int32);
	DECL_PROP0(MAXROWS,		sal_Int32);
	DECL_PROP0(QUERYTIMEOUT,sal_Int32);
	DECL_PROP0(RESULTSETCONCURRENCY,sal_Int32);
	DECL_PROP0(RESULTSETTYPE,sal_Int32);
	DECL_BOOL_PROP0(USEBOOKMARKS);

	return new ::cppu::OPropertyArrayHelper(aProps);
}

// -------------------------------------------------------------------------
::cppu::IPropertyArrayHelper & OStatement_Base::getInfoHelper()
{
	return *const_cast<OStatement_Base*>(this)->getArrayHelper();
}
// -------------------------------------------------------------------------
sal_Bool OStatement_Base::convertFastPropertyValue(
							Any & rConvertedValue,
							Any & rOldValue,
							sal_Int32 nHandle,
							const Any& rValue )
{
	sal_Bool bConverted = sal_False;
	// here we have to try to convert
	return bConverted;
}
// -------------------------------------------------------------------------
void OStatement_Base::setFastPropertyValue_NoBroadcast(sal_Int32 nHandle,const Any& rValue)
{
	// set the value to what ever is necessary
	switch(nHandle)
	{
		case PROPERTY_ID_QUERYTIMEOUT:
		case PROPERTY_ID_MAXFIELDSIZE:
		case PROPERTY_ID_MAXROWS:
		case PROPERTY_ID_CURSORNAME:
		case PROPERTY_ID_RESULTSETCONCURRENCY:
		case PROPERTY_ID_RESULTSETTYPE:
		case PROPERTY_ID_FETCHDIRECTION:
		case PROPERTY_ID_FETCHSIZE:
		case PROPERTY_ID_ESCAPEPROCESSING:
		case PROPERTY_ID_USEBOOKMARKS:
		default:
			;
	}
}
// -------------------------------------------------------------------------
void OStatement_Base::getFastPropertyValue(Any& rValue,sal_Int32 nHandle) const
{
	switch(nHandle)
	{
		case PROPERTY_ID_QUERYTIMEOUT:
		case PROPERTY_ID_MAXFIELDSIZE:
		case PROPERTY_ID_MAXROWS:
		case PROPERTY_ID_CURSORNAME:
		case PROPERTY_ID_RESULTSETCONCURRENCY:
		case PROPERTY_ID_RESULTSETTYPE:
		case PROPERTY_ID_FETCHDIRECTION:
		case PROPERTY_ID_FETCHSIZE:
		case PROPERTY_ID_ESCAPEPROCESSING:
		case PROPERTY_ID_USEBOOKMARKS:
		default:
			;
	}
}
// -------------------------------------------------------------------------
IMPLEMENT_SERVICE_INFO(OStatement,"com.sun.star.sdbcx.OStatement","com.sun.star.sdbc.Statement");
// -----------------------------------------------------------------------------
void SAL_CALL OStatement_Base::acquire() throw()
{
	OStatement_BASE::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OStatement_Base::release() throw()
{
	OStatement_BASE::release();
}
// -----------------------------------------------------------------------------
void SAL_CALL OStatement::acquire() throw()
{
	OStatement_BASE2::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OStatement::release() throw()
{
	OStatement_BASE2::release();
}
// -----------------------------------------------------------------------------
Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL OStatement_Base::getPropertySetInfo(  )
{
	return ::cppu::OPropertySetHelper::createPropertySetInfo(getInfoHelper());
}
// -----------------------------------------------------------------------------
