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


#ifndef CONNECTIVITY_ADABAS_RESULTSET_HXX
#define CONNECTIVITY_ADABAS_RESULTSET_HXX

#include "odbc/OResultSet.hxx"

namespace connectivity
{
	namespace adabas
	{
		class OAdabasResultSet :	public	::connectivity::odbc::OResultSet
		{
			::vos::ORef<OSQLColumns>	m_aSelectColumns;
		public:
			OAdabasResultSet(SQLHANDLE _pStatementHandle,::connectivity::odbc::OStatement_Base* pStmt,const ::vos::ORef<OSQLColumns>& _rSelectColumns)
			 : ::connectivity::odbc::OResultSet( _pStatementHandle,pStmt)
			 ,m_aSelectColumns(_rSelectColumns)
			{}

			// XResultSetMetaDataSupplier
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData(  );

			virtual sal_Bool SAL_CALL next(  );
            virtual sal_Bool SAL_CALL first(  );
            virtual sal_Bool SAL_CALL last(  );
            virtual sal_Bool SAL_CALL absolute( sal_Int32 row );
            virtual sal_Bool SAL_CALL relative( sal_Int32 rows );
            virtual sal_Bool SAL_CALL previous(  );
			virtual void SAL_CALL refreshRow(  );
		};
	}
}

#endif // CONNECTIVITY_ADABAS_RESULTSET_HXX
