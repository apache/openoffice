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

#ifndef CONNECTIVITY_SPREPAREDSTATEMENT_HXX
#define CONNECTIVITY_SPREPAREDSTATEMENT_HXX
#include "mysqlc_statement.hxx"
#include "mysqlc_resultset.hxx"

#ifndef _COM_SUN_STAR_SDBC_XPREPAREDSTATEMENT_HPP_
#include <com/sun/star/sdbc/XPreparedStatement.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XPARAMETERS_HPP_
#include <com/sun/star/sdbc/XParameters.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATASUPPLIER_HPP_
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XPREPAREDBATCHEXECUTION_HPP_
#include <com/sun/star/sdbc/XPreparedBatchExecution.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif

#include <preextstl.h>
#include <cppconn/prepared_statement.h>
#include <postextstl.h>

namespace connectivity
{
	namespace mysqlc
	{
		using ::com::sun::star::uno::Reference;
		using ::com::sun::star::uno::Any;
		using ::com::sun::star::uno::Type;
		using ::com::sun::star::sdbc::SQLException;
		using ::com::sun::star::uno::RuntimeException;
		using ::com::sun::star::sdbc::XResultSetMetaData;

		class OBoundParam;
		typedef ::cppu::ImplHelper5<	::com::sun::star::sdbc::XPreparedStatement,
										::com::sun::star::sdbc::XParameters,
										::com::sun::star::sdbc::XPreparedBatchExecution,
										::com::sun::star::sdbc::XResultSetMetaDataSupplier,
										::com::sun::star::lang::XServiceInfo> OPreparedStatement_BASE;

		class OPreparedStatement :	public OCommonStatement,
									public OPreparedStatement_BASE
		{
		protected:
			unsigned int		m_paramCount;	// number of placeholders
			sal_Int32			m_nNumParams;	// Number of parameter markers for the prepared statement
			Reference< XResultSetMetaData > m_xMetaData;

			void checkParameterIndex(sal_Int32 parameter);

		protected:
			void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const Any& rValue);
			virtual ~OPreparedStatement();
		public:
			DECLARE_SERVICE_INFO();
			OPreparedStatement(OConnection* _pConnection, sql::PreparedStatement * cppPrepStmt);

			//XInterface
			Any SAL_CALL queryInterface(const Type & rType);
			void SAL_CALL acquire() throw();
			void SAL_CALL release() throw();

			//XTypeProvider
			::com::sun::star::uno::Sequence< Type > SAL_CALL getTypes();

			// XPreparedStatement
			Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery();
			sal_Int32 SAL_CALL executeUpdate();
			sal_Bool SAL_CALL execute();
			Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection();

            // XStatement
			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery(const OUString& sql);
			sal_Int32 SAL_CALL executeUpdate(const OUString& sql);
			sal_Bool SAL_CALL execute( const OUString& sql );

			// XParameters
			void SAL_CALL setNull(sal_Int32 parameter, sal_Int32 sqlType);

			void SAL_CALL setObjectNull(sal_Int32 parameter, sal_Int32 sqlType, const OUString& typeName);

			void SAL_CALL setBoolean(sal_Int32 parameter, sal_Bool x);

			void SAL_CALL setByte(sal_Int32 parameter, sal_Int8 x);

			void SAL_CALL setShort(sal_Int32 parameter, sal_Int16 x);

			void SAL_CALL setInt(sal_Int32 parameter, sal_Int32 x);

			void SAL_CALL setLong(sal_Int32 parameter, sal_Int64 x);

			void SAL_CALL setFloat(sal_Int32 parameter, float x);

			void SAL_CALL setDouble(sal_Int32 parameter, double x);

			void SAL_CALL setString(sal_Int32 parameter, const OUString& x);

			void SAL_CALL setBytes(sal_Int32 parameter, const ::com::sun::star::uno::Sequence< sal_Int8 >& x);

			void SAL_CALL setDate(sal_Int32 parameter, const ::com::sun::star::util::Date& x);

			void SAL_CALL setTime(sal_Int32 parameter, const ::com::sun::star::util::Time& x);
			void SAL_CALL setTimestamp(sal_Int32 parameter, const ::com::sun::star::util::DateTime& x);

			void SAL_CALL setBinaryStream(sal_Int32 parameter, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);

			void SAL_CALL setCharacterStream(sal_Int32 parameter, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);

			void SAL_CALL setObject(sal_Int32 parameter, const Any& x);

			void SAL_CALL setObjectWithInfo(sal_Int32 parameter, const Any& x, sal_Int32 targetSqlType, sal_Int32 scale);

			void SAL_CALL setRef(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XRef >& x);

			void SAL_CALL setBlob(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XBlob >& x);

			void SAL_CALL setClob(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XClob >& x);

			void SAL_CALL setArray(sal_Int32 parameter, const Reference< ::com::sun::star::sdbc::XArray >& x);

			void SAL_CALL clearParameters();

			// XPreparedBatchExecution
			void SAL_CALL addBatch();
			void SAL_CALL clearBatch();
			::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL executeBatch();

			// XCloseable
			void SAL_CALL close();

			// XResultSetMetaDataSupplier
			Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData();
		};
	} /* mysqlc */
} /* connectivity */
#endif // CONNECTIVITY_SPREPAREDSTATEMENT_HXX

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
