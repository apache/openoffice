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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_macabdrv1.hxx"

#include "MacabPreparedStatement.hxx"
#include "MacabAddressBook.hxx"
#include "propertyids.hxx"
#include <connectivity/dbexception.hxx>
#include "connectivity/dbtools.hxx"
#include "resource/macab_res.hrc"
#include "resource/sharedresources.hxx"

using namespace connectivity::macab;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::util;

IMPLEMENT_SERVICE_INFO(MacabPreparedStatement, "com.sun.star.sdbc.drivers.MacabPreparedStatement", "com.sun.star.sdbc.PreparedStatement");
// -------------------------------------------------------------------------
void MacabPreparedStatement::checkAndResizeParameters(sal_Int32 nParams)
{
	if ( !m_aParameterRow.isValid() )
		m_aParameterRow = new OValueVector();

	if (nParams < 1)
		::dbtools::throwInvalidIndexException(*(MacabPreparedStatement *) this,Any());

	if (nParams >= (sal_Int32) (m_aParameterRow->get()).size())
		(m_aParameterRow->get()).resize(nParams);
}
// -------------------------------------------------------------------------
void MacabPreparedStatement::setMacabFields() const
{
	::vos::ORef<connectivity::OSQLColumns> xColumns;	// selected columns

	xColumns = m_aSQLIterator.getSelectColumns();
	if (!xColumns.isValid())
	{
		::connectivity::SharedResources aResources;
        const ::rtl::OUString sError( aResources.getResourceString(
                STR_INVALID_COLUMN_SELECTION
             ) );
	    ::dbtools::throwGenericSQLException(sError,NULL);
	}
	m_xMetaData->setMacabFields(xColumns);
}
// -------------------------------------------------------------------------
void MacabPreparedStatement::resetParameters() const
{
	m_nParameterIndex = 0;
}
// -------------------------------------------------------------------------
void MacabPreparedStatement::getNextParameter(::rtl::OUString &rParameter) const
{
	if (m_nParameterIndex >= (sal_Int32) (m_aParameterRow->get()).size())
    {
        ::connectivity::SharedResources aResources;
        const ::rtl::OUString sError( aResources.getResourceString(
                STR_INVALID_PARA_COUNT
             ) );
	    ::dbtools::throwGenericSQLException(sError,*(MacabPreparedStatement *) this);
    }

	rParameter = (m_aParameterRow->get())[m_nParameterIndex];

	m_nParameterIndex++;
}
// -------------------------------------------------------------------------
MacabPreparedStatement::MacabPreparedStatement(
	MacabConnection* _pConnection,
	const ::rtl::OUString& sql)
	: MacabPreparedStatement_BASE(_pConnection),
	  m_sSqlStatement(sql),
	  m_bPrepared(sal_False),
	  m_nParameterIndex(0),
	  m_aParameterRow()
{

}
// -------------------------------------------------------------------------
MacabPreparedStatement::~MacabPreparedStatement()
{
}
// -------------------------------------------------------------------------
void MacabPreparedStatement::disposing()
{
	MacabPreparedStatement_BASE::disposing();

	if (m_aParameterRow.isValid())
	{
		m_aParameterRow->get().clear();
		m_aParameterRow = NULL;
	}
}
// -------------------------------------------------------------------------
Reference< XResultSetMetaData > SAL_CALL MacabPreparedStatement::getMetaData()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	if (!m_xMetaData.is())
	{
		const OSQLTables& xTabs = m_aSQLIterator.getTables();
		::rtl::OUString sTableName = MacabAddressBook::getDefaultTableName();

		if(! xTabs.empty() )
		{

			// can only deal with one table at a time
			if(xTabs.size() == 1 && !m_aSQLIterator.hasErrors() )
				sTableName = xTabs.begin()->first;

		}
		m_xMetaData = new MacabResultSetMetaData(getOwnConnection(),sTableName);
		setMacabFields();
	}
	Reference< XResultSetMetaData > xMetaData = m_xMetaData.get();
	return xMetaData;
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::close()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	// Reset last warning message
	try {
		clearWarnings ();
		MacabCommonStatement::close();
	}
	catch (SQLException &) {
		// If we get an error, ignore
	}

	// Remove this Statement object from the Connection object's
	// list
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL MacabPreparedStatement::execute()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	Reference< XResultSet> xRS = MacabCommonStatement::executeQuery(m_sSqlStatement);

	return xRS.is();
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL MacabPreparedStatement::executeUpdate()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	// same as in statement with the difference that this statement also can contain parameter
	return 0;
}
// -------------------------------------------------------------------------
Reference< XConnection > SAL_CALL MacabPreparedStatement::getConnection()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	return (Reference< XConnection >) m_pConnection;
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL MacabPreparedStatement::executeQuery()
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	Reference< XResultSet > rs = MacabCommonStatement::executeQuery(m_sSqlStatement);

	return rs;
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setNull(sal_Int32 parameterIndex, sal_Int32)
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	checkAndResizeParameters(parameterIndex);

	(m_aParameterRow->get())[parameterIndex - 1].setNull();
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setObjectNull(sal_Int32, sal_Int32, const ::rtl::OUString&)
{



::dbtools::throwFunctionNotSupportedException("setObjectNull", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setBoolean(sal_Int32, sal_Bool)
{



::dbtools::throwFunctionNotSupportedException("setBoolean", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setByte(sal_Int32, sal_Int8)
{



::dbtools::throwFunctionNotSupportedException("setByte", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setShort(sal_Int32, sal_Int16)
{



::dbtools::throwFunctionNotSupportedException("setShort", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setInt(sal_Int32, sal_Int32)
{



::dbtools::throwFunctionNotSupportedException("setInt", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setLong(sal_Int32, sal_Int64)
{



::dbtools::throwFunctionNotSupportedException("setLong", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setFloat(sal_Int32, float)
{



::dbtools::throwFunctionNotSupportedException("setFloat", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setDouble(sal_Int32, double)
{



::dbtools::throwFunctionNotSupportedException("setDouble", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setString(sal_Int32 parameterIndex, const ::rtl::OUString &x)
{
	::osl::MutexGuard aGuard( m_aMutex );
	checkDisposed(MacabCommonStatement_BASE::rBHelper.bDisposed);

	checkAndResizeParameters(parameterIndex);

	(m_aParameterRow->get())[parameterIndex - 1] = x;
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setBytes(sal_Int32, const Sequence< sal_Int8 >&)
{



::dbtools::throwFunctionNotSupportedException("setBytes", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setDate(sal_Int32, const Date&)
{



::dbtools::throwFunctionNotSupportedException("setDate", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setTime(sal_Int32, const Time&)
{



::dbtools::throwFunctionNotSupportedException("setTime", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setTimestamp(sal_Int32, const DateTime&)
{



::dbtools::throwFunctionNotSupportedException("setTimestamp", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setBinaryStream(sal_Int32, const Reference< ::com::sun::star::io::XInputStream >&, sal_Int32)
{



::dbtools::throwFunctionNotSupportedException("setBinaryStream", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setCharacterStream(sal_Int32, const Reference< ::com::sun::star::io::XInputStream >&, sal_Int32)
{



::dbtools::throwFunctionNotSupportedException("setCharacterStream", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setObject(sal_Int32 parameterIndex, const Any& x)
{
	if(!::dbtools::implSetObject(this,parameterIndex,x))
	{
        const ::rtl::OUString sError( m_pConnection->getResources().getResourceStringWithSubstitution(
                STR_UNKNOWN_PARA_TYPE,
                "$position$", ::rtl::OUString::valueOf(parameterIndex)
             ) );
		::dbtools::throwGenericSQLException(sError,*this);
	}
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setObjectWithInfo(sal_Int32, const Any&, sal_Int32, sal_Int32)
{



::dbtools::throwFunctionNotSupportedException("setObjectWithInfo", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setRef(sal_Int32, const Reference< XRef >&)
{



::dbtools::throwFunctionNotSupportedException("setRef", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setBlob(sal_Int32, const Reference< XBlob >&)
{



::dbtools::throwFunctionNotSupportedException("setBlob", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setClob(sal_Int32, const Reference< XClob >&)
{



::dbtools::throwFunctionNotSupportedException("setClob", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::setArray(sal_Int32, const Reference< XArray >&)
{



::dbtools::throwFunctionNotSupportedException("setArray", NULL);
}
// -------------------------------------------------------------------------
void SAL_CALL MacabPreparedStatement::clearParameters()
{
::dbtools::throwFunctionNotSupportedException("clearParameters", NULL);
}
// -------------------------------------------------------------------------
void MacabPreparedStatement::setFastPropertyValue_NoBroadcast(sal_Int32 nHandle,const Any& rValue)
{
	switch (nHandle)
	{
		case PROPERTY_ID_RESULTSETCONCURRENCY:
			break;
		case PROPERTY_ID_RESULTSETTYPE:
			break;
		case PROPERTY_ID_FETCHDIRECTION:
			break;
		case PROPERTY_ID_USEBOOKMARKS:
			break;
		default:
			MacabCommonStatement::setFastPropertyValue_NoBroadcast(nHandle,rValue);
	}
}
