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



#ifndef _CONNECTIVITY_ODATABASEMETADATABASE_HXX_
#define _CONNECTIVITY_ODATABASEMETADATABASE_HXX_

#include <com/sun/star/sdbc/XDatabaseMetaData2.hpp>
#include <cppuhelper/implbase2.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <com/sun/star/lang/XEventListener.hpp>
#include "FDatabaseMetaDataResultSet.hxx"
#include <functional>
#include "connectivity/dbtoolsdllapi.hxx"

namespace connectivity
{
        typedef ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >  TConditions;
		class OOO_DLLPUBLIC_DBTOOLS ODatabaseMetaDataBase :
                                        public	comphelper::OBaseMutex,
										public ::cppu::WeakImplHelper2< ::com::sun::star::sdbc::XDatabaseMetaData2,
																		::com::sun::star::lang::XEventListener>
		{
        private:
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >   m_aConnectionInfo;
            ::connectivity::ODatabaseMetaDataResultSet::ORows                           m_aTypeInfoRows;

            // cached database information
            ::std::pair<bool,sal_Bool>          m_isCatalogAtStart;
            ::std::pair<bool,::rtl::OUString>   m_sCatalogSeparator;
            ::std::pair<bool,::rtl::OUString>   m_sIdentifierQuoteString;
            ::std::pair<bool,sal_Bool>          m_supportsCatalogsInTableDefinitions;
            ::std::pair<bool,sal_Bool>          m_supportsSchemasInTableDefinitions;
            ::std::pair<bool,sal_Bool>          m_supportsCatalogsInDataManipulation;
            ::std::pair<bool,sal_Bool>          m_supportsSchemasInDataManipulation;
            ::std::pair<bool,sal_Bool>          m_supportsMixedCaseQuotedIdentifiers;
            ::std::pair<bool,sal_Bool>          m_supportsAlterTableWithAddColumn;
            ::std::pair<bool,sal_Bool>          m_supportsAlterTableWithDropColumn;
            ::std::pair<bool,sal_Int32>         m_MaxStatements;
            ::std::pair<bool,sal_Int32>         m_MaxTablesInSelect;
            ::std::pair<bool,sal_Bool>          m_storesMixedCaseQuotedIdentifiers;

            template <typename T> T callImplMethod(::std::pair<bool,T>& _rCache,const ::std::mem_fun_t<T,ODatabaseMetaDataBase>& _pImplMethod)
            {
                ::osl::MutexGuard aGuard( m_aMutex );
                if ( !_rCache.first )
                {
                    _rCache.second = _pImplMethod(this);
                    _rCache.first = true;
                }
                return _rCache.second;
            }
		protected:
			::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >		m_xConnection;
			::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener>	m_xListenerHelper; // forward the calls from the connection to me

			virtual ~ODatabaseMetaDataBase();

        protected:
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > impl_getTypeInfo_throw() = 0;
            // cached database information
            virtual ::rtl::OUString impl_getIdentifierQuoteString_throw(  )             = 0;
            virtual sal_Bool        impl_isCatalogAtStart_throw(  )                     = 0;
            virtual ::rtl::OUString impl_getCatalogSeparator_throw(  )                  = 0;
            virtual sal_Bool        impl_supportsCatalogsInTableDefinitions_throw(  )   = 0;
            virtual sal_Bool        impl_supportsSchemasInTableDefinitions_throw(  )    = 0;
            virtual sal_Bool        impl_supportsCatalogsInDataManipulation_throw(  )   = 0;
            virtual sal_Bool        impl_supportsSchemasInDataManipulation_throw(  )    = 0;
            virtual sal_Bool        impl_supportsMixedCaseQuotedIdentifiers_throw(  )   = 0;
            virtual sal_Bool        impl_supportsAlterTableWithAddColumn_throw(  )      = 0;
            virtual sal_Bool        impl_supportsAlterTableWithDropColumn_throw(  )     = 0;
            virtual sal_Int32       impl_getMaxStatements_throw(  )                     = 0;
            virtual sal_Int32       impl_getMaxTablesInSelect_throw(  )                 = 0;
            virtual sal_Bool        impl_storesMixedCaseQuotedIdentifiers_throw(  )     = 0;


		public:

			ODatabaseMetaDataBase(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rInfo);

            // XDatabaseMetaData2
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getConnectionInfo(  );

			// XEventListener
			virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTypeInfo(  );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getProcedures( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getProcedureColumns( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern, const ::rtl::OUString& columnNamePattern );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getSchemas(  );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getCatalogs(  );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getColumnPrivileges( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, const ::rtl::OUString& columnNamePattern );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTablePrivileges( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getBestRowIdentifier( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, sal_Int32 scope, sal_Bool nullable );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getVersionColumns( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getPrimaryKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getImportedKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getExportedKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getCrossReference( const ::com::sun::star::uno::Any& primaryCatalog, const ::rtl::OUString& primarySchema, const ::rtl::OUString& primaryTable, const ::com::sun::star::uno::Any& foreignCatalog, const ::rtl::OUString& foreignSchema, const ::rtl::OUString& foreignTable );
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getIndexInfo( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, sal_Bool unique, sal_Bool approximate );

            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection(  );
            // cached database information
            virtual ::rtl::OUString SAL_CALL getIdentifierQuoteString(  );
            virtual sal_Bool SAL_CALL isCatalogAtStart(  );
            virtual ::rtl::OUString SAL_CALL getCatalogSeparator(  );
            virtual sal_Bool SAL_CALL supportsCatalogsInTableDefinitions(  );
            virtual sal_Bool SAL_CALL supportsSchemasInTableDefinitions(  );
            virtual sal_Bool SAL_CALL supportsCatalogsInDataManipulation(  );
            virtual sal_Bool SAL_CALL supportsSchemasInDataManipulation(  );
            virtual sal_Bool SAL_CALL supportsMixedCaseQuotedIdentifiers(  );
            virtual sal_Bool SAL_CALL supportsAlterTableWithAddColumn(  );
            virtual sal_Bool SAL_CALL supportsAlterTableWithDropColumn(  );
            virtual sal_Int32 SAL_CALL getMaxStatements(  );
            virtual sal_Int32 SAL_CALL getMaxTablesInSelect(  );
            virtual sal_Bool SAL_CALL storesMixedCaseQuotedIdentifiers(  );
		};
}
#endif // _CONNECTIVITY_ODATABASEMETADATABASE_HXX_
