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



#ifndef _CONNECTIVITY_EVOAB_RESULTSET_HXX_
#define _CONNECTIVITY_EVOAB_RESULTSET_HXX_

#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/XColumnLocate.hpp>
#include <com/sun/star/util/XCancellable.hpp>
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#include <com/sun/star/sdbc/XResultSetUpdate.hpp>
#include <com/sun/star/sdbc/XRowUpdate.hpp>
#include <com/sun/star/sdbcx/XRowLocate.hpp>
#include <com/sun/star/sdbcx/XDeleteRows.hpp>
#include <cppuhelper/compbase8.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include "connectivity/CommonTools.hxx"
#include "connectivity/FValue.hxx"
#include "connectivity/warningscontainer.hxx"
#include "NStatement.hxx"
#include "OSubComponent.hxx"
#include "NResultSetMetaData.hxx"

namespace connectivity
{
	namespace evoab
	{
        typedef ::cppu::WeakComponentImplHelper8    <   ::com::sun::star::sdbc::XResultSet
                                                    ,   ::com::sun::star::sdbc::XRow
                                                    ,   ::com::sun::star::sdbc::XResultSetMetaDataSupplier
                                                    ,   ::com::sun::star::util::XCancellable
                                                    ,   ::com::sun::star::sdbc::XWarningsSupplier
                                                    ,   ::com::sun::star::sdbc::XCloseable
                                                    ,   ::com::sun::star::sdbc::XColumnLocate
                                                    ,   ::com::sun::star::lang::XServiceInfo
                                                    >   OResultSet_BASE;


		class OEvoabResultSet   :public comphelper::OBaseMutex
					            ,public OResultSet_BASE
                                ,public ::comphelper::OPropertyContainer
					            ,public ::comphelper::OPropertyArrayUsageHelper<OEvoabResultSet>
		{

		protected:

            OCommonStatement*                                                               m_pStatement;
            OEvoabConnection*                                                               m_pConnection;
            ::com::sun::star::uno::WeakReferenceHelper	                                    m_aStatement;
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData >  m_xMetaData;
            ::dbtools::WarningsContainer                                                    m_aWarnings;

            bool                                        m_bWasNull;
            // <properties>
			sal_Int32                                   m_nFetchSize;
			sal_Int32                                   m_nResultSetType;
			sal_Int32                                   m_nFetchDirection;
			sal_Int32                                   m_nResultSetConcurrency;
            // </properties>

			// Data & iteration
			GList	 *m_pContacts;
			sal_Int32 m_nIndex;
			sal_Int32 m_nLength;
			EContact *getCur()
			{
				gpointer pData = g_list_nth_data (m_pContacts, m_nIndex);
				return pData ? E_CONTACT (pData) : NULL;
			}

			// OPropertyArrayUsageHelper
			virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;
			// OPropertySetHelper
			virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

			// you can't delete objects of this type
			virtual ~OEvoabResultSet();
		public:
			DECLARE_SERVICE_INFO();

			OEvoabResultSet( OCommonStatement *pStmt, OEvoabConnection *pConnection );
            void construct( const QueryData& _rData );

			OEvoabConnection * getConnection() { return m_pConnection; }

			::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > operator *()
			{
				return ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >(*(OResultSet_BASE*)this);
			}

			// ::cppu::OComponentHelper
			virtual void SAL_CALL disposing(void);
			// XInterface
			virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
			virtual void SAL_CALL acquire() throw();
			virtual void SAL_CALL release() throw();
			//XTypeProvider
			virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
			// XPropertySet
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
			// XResultSet
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
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getStatement(  );
			// XRow
			virtual sal_Bool SAL_CALL wasNull(  );
			virtual ::rtl::OUString SAL_CALL getString( sal_Int32 columnIndex );
			virtual sal_Bool SAL_CALL getBoolean( sal_Int32 columnIndex );
			virtual sal_Int8 SAL_CALL getByte( sal_Int32 columnIndex );
			virtual sal_Int16 SAL_CALL getShort( sal_Int32 columnIndex );
			virtual sal_Int32 SAL_CALL getInt( sal_Int32 columnIndex );
			virtual sal_Int64 SAL_CALL getLong( sal_Int32 columnIndex );
			virtual float SAL_CALL getFloat( sal_Int32 columnIndex );
			virtual double SAL_CALL getDouble( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBytes( sal_Int32 columnIndex );
			virtual ::com::sun::star::util::Date SAL_CALL getDate( sal_Int32 columnIndex );
			virtual ::com::sun::star::util::Time SAL_CALL getTime( sal_Int32 columnIndex );
			virtual ::com::sun::star::util::DateTime SAL_CALL getTimestamp( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getBinaryStream( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getCharacterStream( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Any SAL_CALL getObject( sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef > SAL_CALL getRef( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob > SAL_CALL getBlob( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob > SAL_CALL getClob( sal_Int32 columnIndex );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray > SAL_CALL getArray( sal_Int32 columnIndex );
			// XCancellable
             virtual void SAL_CALL cancel(  );
			// XCloseable
            virtual void SAL_CALL close(  );
			// XResultSetMetaDataSupplier
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData(  );
			// XWarningsSupplier
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  );
            virtual void SAL_CALL clearWarnings(  );
			// XColumnLocate
			virtual sal_Int32 SAL_CALL findColumn( const ::rtl::OUString& columnName );
		};
	}
}
#endif // CONNECTIVITY_SRESULTSET_HXX
