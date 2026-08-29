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



#ifndef DBACCESS_CORE_API_STATICSET_HXX
#define DBACCESS_CORE_API_STATICSET_HXX

#ifndef DBACCESS_CORE_API_CACHESET_HXX
#include "CacheSet.hxx"
#endif

namespace dbaccess
{
	// is used when nothing is supported by the driver
	// we use a snapshot
	class OStaticSet : public OCacheSet
	{
		ORowSetMatrix			m_aSet;
		ORowSetMatrix::iterator m_aSetIter;
		sal_Bool				m_bEnd;
		sal_Bool fetchRow();
		void fillAllRows();
	public:
        OStaticSet(sal_Int32 i_nMaxRows) : OCacheSet(i_nMaxRows)
			, m_aSetIter(m_aSet.end())
			, m_bEnd(sal_False)
		{
			m_aSet.push_back(NULL); // this is the beforefirst record
		}

		virtual void fillValueRow(ORowSetRow& _rRow,sal_Int32 _nPosition);
		// ::com::sun::star::sdbcx::XRowLocate
		virtual ::com::sun::star::uno::Any SAL_CALL getBookmark();
		virtual sal_Bool SAL_CALL moveToBookmark( const ::com::sun::star::uno::Any& bookmark );
		virtual sal_Bool SAL_CALL moveRelativeToBookmark( const ::com::sun::star::uno::Any& bookmark, sal_Int32 rows );
		virtual sal_Int32 SAL_CALL compareBookmarks( const ::com::sun::star::uno::Any& first, const ::com::sun::star::uno::Any& second );
		virtual sal_Bool SAL_CALL hasOrderedBookmarks(  );
		virtual sal_Int32 SAL_CALL hashBookmark( const ::com::sun::star::uno::Any& bookmark );

		// ::com::sun::star::sdbc::XResultSet
		virtual sal_Bool SAL_CALL next(  );
		virtual sal_Bool SAL_CALL isBeforeFirst(  );
		virtual sal_Bool SAL_CALL isAfterLast(  );
		virtual sal_Bool SAL_CALL isFirst(  );
		virtual sal_Bool SAL_CALL isLast(  );
		virtual void SAL_CALL beforeFirst(  );
		virtual void SAL_CALL afterLast(  );
		virtual sal_Bool SAL_CALL first(  );
		virtual sal_Bool SAL_CALL last(  );
		virtual sal_Int32 SAL_CALL getRow(  );
		virtual sal_Bool SAL_CALL absolute( sal_Int32 row );
		virtual sal_Bool SAL_CALL relative( sal_Int32 rows );
		virtual sal_Bool SAL_CALL previous(  );
		virtual void SAL_CALL refreshRow(  );
		virtual sal_Bool SAL_CALL rowUpdated(  );
		virtual sal_Bool SAL_CALL rowInserted(  );
		virtual sal_Bool SAL_CALL rowDeleted(  );
		// ::com::sun::star::sdbcx::XDeleteRows
		virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL deleteRows( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& rows,const connectivity::OSQLTable& _xTable );
		// ::com::sun::star::sdbc::XResultSetUpdate
		virtual void SAL_CALL insertRow( const ORowSetRow& _rInsertRow,const connectivity::OSQLTable& _xTable );
		virtual void SAL_CALL updateRow(const ORowSetRow& _rInsertRow,const ORowSetRow& _rOrginalRow,const connectivity::OSQLTable& _xTable  );
		virtual void SAL_CALL deleteRow(const ORowSetRow& _rInsertRow,const connectivity::OSQLTable& _xTable  );
		virtual void SAL_CALL cancelRowUpdates(  );
		virtual void SAL_CALL moveToInsertRow(  );
		virtual void SAL_CALL moveToCurrentRow(  );
	};
}
#endif //DBACCESS_CORE_API_STATICSET_HXX
