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

#ifndef MYSQLC_SRESULTSET_HXX
#define MYSQLC_SRESULTSET_HXX

#include "mysqlc_preparedstatement.hxx"
#include "mysqlc_statement.hxx"
#include "mysqlc_subcomponent.hxx"

#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/XColumnLocate.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XResultSetUpdate.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XRowUpdate.hpp>
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#include <com/sun/star/sdbcx/XDeleteRows.hpp>
#include <com/sun/star/sdbcx/XRowLocate.hpp>
#include <com/sun/star/util/XCancellable.hpp>

#include <cppuhelper/compbase12.hxx>


namespace connectivity
{
	namespace mysqlc
	{
		using ::rtl::OUString;
		using ::com::sun::star::sdbc::SQLException;
		using ::com::sun::star::uno::RuntimeException;
		using ::com::sun::star::uno::Any;
		typedef ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > my_XInputStreamRef;
		typedef my_XNameAccessRef my_XNameAccessRef;

		/*
		**	OResultSet
		*/
        typedef ::cppu::WeakComponentImplHelper12<	::com::sun::star::sdbc::XResultSet,
													::com::sun::star::sdbc::XRow,
													::com::sun::star::sdbc::XResultSetMetaDataSupplier,
													::com::sun::star::util::XCancellable,
													::com::sun::star::sdbc::XWarningsSupplier,
													::com::sun::star::sdbc::XResultSetUpdate,
													::com::sun::star::sdbc::XRowUpdate,
													::com::sun::star::sdbcx::XRowLocate,
													::com::sun::star::sdbcx::XDeleteRows,
													::com::sun::star::sdbc::XCloseable,
													::com::sun::star::sdbc::XColumnLocate,
													::com::sun::star::lang::XServiceInfo> OResultSet_BASE;

		class OResultSet :	public	OBase_Mutex,
							public	OResultSet_BASE,
							public	::cppu::OPropertySetHelper,
							public	OPropertyArrayUsageHelper<OResultSet>
		{
		protected:
			::com::sun::star::uno::WeakReferenceHelper	m_aStatement;
			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData> m_xMetaData;
			sql::ResultSet		*m_result;
			unsigned int		fieldCount;
            rtl_TextEncoding    m_encoding;
			// OPropertyArrayUsageHelper
			::cppu::IPropertyArrayHelper* createArrayHelper() const;
			// OPropertySetHelper
			::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

			sal_Bool SAL_CALL convertFastPropertyValue(Any & rConvertedValue, Any & rOldValue, sal_Int32 nHandle, const Any& rValue);

			void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const Any& rValue);

			void SAL_CALL getFastPropertyValue(Any& rValue, sal_Int32 nHandle) const;

			// you can't delete objects of this type
			virtual ~OResultSet();

		public:
			DECLARE_SERVICE_INFO();

			OResultSet( OCommonStatement* pStmt, sql::ResultSet *result, rtl_TextEncoding _encoding );

			::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > operator *()
			{
				return ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(*(OResultSet_BASE*)this);
			}

			// ::cppu::OComponentHelper
			void SAL_CALL disposing();

			// XInterface
			Any SAL_CALL queryInterface(const ::com::sun::star::uno::Type & rType);

			void SAL_CALL acquire()										throw();

			void SAL_CALL release()										throw();

			//XTypeProvider
			::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();

			// XPropertySet
			::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();

			// XResultSet
			sal_Bool SAL_CALL next();

			sal_Bool SAL_CALL isBeforeFirst();

			sal_Bool SAL_CALL isAfterLast();

			sal_Bool SAL_CALL isFirst();

			sal_Bool SAL_CALL isLast();

			void SAL_CALL beforeFirst();

			void SAL_CALL afterLast();

			sal_Bool SAL_CALL first();

			sal_Bool SAL_CALL last();

			sal_Int32 SAL_CALL getRow();

			sal_Bool SAL_CALL absolute(sal_Int32 row);

			sal_Bool SAL_CALL relative(sal_Int32 rows);

			sal_Bool SAL_CALL previous();

			void SAL_CALL refreshRow();

			sal_Bool SAL_CALL rowUpdated();

			sal_Bool SAL_CALL rowInserted();

			sal_Bool SAL_CALL rowDeleted();

			::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getStatement();
			// XRow
			sal_Bool SAL_CALL wasNull();

			OUString SAL_CALL getString(sal_Int32 column);

			sal_Bool SAL_CALL getBoolean(sal_Int32 column);

			sal_Int8 SAL_CALL getByte(sal_Int32 column);

			sal_Int16 SAL_CALL getShort(sal_Int32 column);

			sal_Int32 SAL_CALL getInt(sal_Int32 column);

			sal_Int64 SAL_CALL getLong(sal_Int32 column);

			float SAL_CALL getFloat(sal_Int32 column);

			double SAL_CALL getDouble(sal_Int32 column);

			::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBytes(sal_Int32 column);

			::com::sun::star::util::Date SAL_CALL getDate(sal_Int32 column);

			::com::sun::star::util::Time SAL_CALL getTime(sal_Int32 column);

			::com::sun::star::util::DateTime SAL_CALL getTimestamp(sal_Int32 column);

			my_XInputStreamRef SAL_CALL getBinaryStream(sal_Int32 column);

			my_XInputStreamRef SAL_CALL getCharacterStream(sal_Int32 column);

			Any SAL_CALL getObject(sal_Int32 column, const my_XNameAccessRef& typeMap);

			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef > SAL_CALL getRef(sal_Int32 column);

			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob > SAL_CALL getBlob(sal_Int32 column);

			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob > SAL_CALL getClob(sal_Int32 column);

			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray > SAL_CALL getArray(sal_Int32 column);

			// XResultSetMetaDataSupplier
			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData();

			// XCancellable
			void SAL_CALL cancel();

			// XCloseable
			void SAL_CALL close();

			// XWarningsSupplier
			Any SAL_CALL getWarnings();

			void SAL_CALL clearWarnings();

			// XResultSetUpdate
			void SAL_CALL insertRow();

			void SAL_CALL updateRow();

			void SAL_CALL deleteRow();

			void SAL_CALL cancelRowUpdates();

			void SAL_CALL moveToInsertRow();

			void SAL_CALL moveToCurrentRow();

			// XRowUpdate
			void SAL_CALL updateNull(sal_Int32 column);

			void SAL_CALL updateBoolean(sal_Int32 column, sal_Bool x);

			void SAL_CALL updateByte(sal_Int32 column, sal_Int8 x);

			void SAL_CALL updateShort(sal_Int32 column, sal_Int16 x);

			void SAL_CALL updateInt(sal_Int32 column, sal_Int32 x);

			void SAL_CALL updateLong(sal_Int32 column, sal_Int64 x);

			void SAL_CALL updateFloat(sal_Int32 column, float x);

			void SAL_CALL updateDouble(sal_Int32 column, double x);

			void SAL_CALL updateString(sal_Int32 column, const OUString& x);

			void SAL_CALL updateBytes(sal_Int32 column, const ::com::sun::star::uno::Sequence< sal_Int8 >& x);

			void SAL_CALL updateDate(sal_Int32 column, const ::com::sun::star::util::Date& x);

			void SAL_CALL updateTime(sal_Int32 column, const ::com::sun::star::util::Time& x);

			void SAL_CALL updateTimestamp(sal_Int32 column, const ::com::sun::star::util::DateTime& x);

			void SAL_CALL updateBinaryStream(sal_Int32 column, const my_XInputStreamRef& x, sal_Int32 length);

			void SAL_CALL updateCharacterStream(sal_Int32 column, const my_XInputStreamRef& x, sal_Int32 length);

			void SAL_CALL updateObject(sal_Int32 column, const Any& x);

			void SAL_CALL updateNumericObject(sal_Int32 column, const Any& x, sal_Int32 scale);

			// XColumnLocate
			sal_Int32 SAL_CALL findColumn(const OUString& columnName);

			// XRowLocate
			Any SAL_CALL getBookmark();

			sal_Bool SAL_CALL moveToBookmark(const Any& bookmark);

			sal_Bool SAL_CALL moveRelativeToBookmark(const Any& bookmark, sal_Int32 rows);

			sal_Int32 SAL_CALL compareBookmarks(const Any& first, const Any& second);

			sal_Bool SAL_CALL hasOrderedBookmarks();

			sal_Int32 SAL_CALL hashBookmark(const Any& bookmark);

			// XDeleteRows
			::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL deleteRows(const ::com::sun::star::uno::Sequence< Any >& rows);

			void checkColumnIndex(sal_Int32 index);

		private:
			using ::cppu::OPropertySetHelper::getFastPropertyValue;
		};
	} /* mysqlc */
} /* connectivity */
#endif // CONNECTIVITY_SRESULTSET_HXX

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
