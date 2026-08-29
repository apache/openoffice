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

#ifndef CONNECTIVITY_SDATABASEMETADATA_HXX
#define CONNECTIVITY_SDATABASEMETADATA_HXX

#include "mysqlc_connection.hxx"

#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#include <preextstl.h>
#include <cppconn/metadata.h>
#include <postextstl.h>

namespace connectivity
{
	namespace mysqlc
	{
		typedef ::com::sun::star::sdbc::SQLException my_SQLException;
		typedef ::com::sun::star::uno::RuntimeException my_RuntimeException;
		typedef ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > my_XResultSetRef;
		using ::com::sun::star::uno::Any;
		using ::rtl::OUString;

		//**************************************************************
		//************ Class: ODatabaseMetaData
		//**************************************************************

		typedef ::cppu::WeakImplHelper1< ::com::sun::star::sdbc::XDatabaseMetaData> ODatabaseMetaData_BASE;

        class ODatabaseMetaData : public ODatabaseMetaData_BASE
		{
			OConnection&	m_rConnection;
			sal_Bool		m_bUseCatalog;
		protected:
			sql::DatabaseMetaData * meta;
			OUString		identifier_quote_string;
			bool			identifier_quote_string_set;

        private:
            OUString impl_getStringMetaData( const sal_Char* _methodName, const ext_std::string& (sql::DatabaseMetaData::*_Method)() );
            OUString impl_getStringMetaData( const sal_Char* _methodName, ext_std::string (sql::DatabaseMetaData::*_Method)() );
            OUString impl_getStringMetaData( const sal_Char* _methodName, const sql::SQLString& (sql::DatabaseMetaData::*_Method)() );
            OUString impl_getStringMetaData( const sal_Char* _methodName, sql::SQLString (sql::DatabaseMetaData::*_Method)() );
            sal_Int32 impl_getInt32MetaData( const sal_Char* _methodName, unsigned int (sql::DatabaseMetaData::*_Method)() );
            sal_Bool impl_getBoolMetaData( const sal_Char* _methodName, bool (sql::DatabaseMetaData::*_Method)() );
            sal_Bool impl_getBoolMetaData( const sal_Char* _methodName, bool (sql::DatabaseMetaData::*_Method)(int), sal_Int32 _arg );
            sal_Bool impl_getRSTypeMetaData( const sal_Char* _methodName, bool (sql::DatabaseMetaData::*_Method)(int), sal_Int32 _resultSetType );

        public:
			inline const OConnection& getOwnConnection() const { return m_rConnection; }

			ODatabaseMetaData(OConnection& _rCon);
			virtual ~ODatabaseMetaData();

			// as I mentioned before this interface is really BIG
			// XDatabaseMetaData
            sal_Bool SAL_CALL allProceduresAreCallable();
            sal_Bool SAL_CALL allTablesAreSelectable();
            OUString SAL_CALL getURL();
            OUString SAL_CALL getUserName();
            sal_Bool SAL_CALL isReadOnly();
            sal_Bool SAL_CALL nullsAreSortedHigh();
            sal_Bool SAL_CALL nullsAreSortedLow();
            sal_Bool SAL_CALL nullsAreSortedAtStart();
            sal_Bool SAL_CALL nullsAreSortedAtEnd();
            OUString SAL_CALL getDatabaseProductName();
            OUString SAL_CALL getDatabaseProductVersion();
            OUString SAL_CALL getDriverName();
            OUString SAL_CALL getDriverVersion();
            sal_Int32 SAL_CALL getDriverMajorVersion();
            sal_Int32 SAL_CALL getDriverMinorVersion();
            sal_Bool SAL_CALL usesLocalFiles();
            sal_Bool SAL_CALL usesLocalFilePerTable();
            sal_Bool SAL_CALL supportsMixedCaseIdentifiers();
            sal_Bool SAL_CALL storesUpperCaseIdentifiers();
            sal_Bool SAL_CALL storesLowerCaseIdentifiers();
            sal_Bool SAL_CALL storesMixedCaseIdentifiers();
            sal_Bool SAL_CALL supportsMixedCaseQuotedIdentifiers();
            sal_Bool SAL_CALL storesUpperCaseQuotedIdentifiers();
            sal_Bool SAL_CALL storesLowerCaseQuotedIdentifiers();
            sal_Bool SAL_CALL storesMixedCaseQuotedIdentifiers();
            OUString SAL_CALL getIdentifierQuoteString();
            OUString SAL_CALL getSQLKeywords();
            OUString SAL_CALL getNumericFunctions();
            OUString SAL_CALL getStringFunctions();
            OUString SAL_CALL getSystemFunctions();
            OUString SAL_CALL getTimeDateFunctions();
            OUString SAL_CALL getSearchStringEscape();
            OUString SAL_CALL getExtraNameCharacters();
            sal_Bool SAL_CALL supportsAlterTableWithAddColumn();
            sal_Bool SAL_CALL supportsAlterTableWithDropColumn();
            sal_Bool SAL_CALL supportsColumnAliasing();
            sal_Bool SAL_CALL nullPlusNonNullIsNull();
            sal_Bool SAL_CALL supportsTypeConversion();
            sal_Bool SAL_CALL supportsConvert(sal_Int32 fromType, sal_Int32 toType);
            sal_Bool SAL_CALL supportsTableCorrelationNames();
            sal_Bool SAL_CALL supportsDifferentTableCorrelationNames();
            sal_Bool SAL_CALL supportsExpressionsInOrderBy();
            sal_Bool SAL_CALL supportsOrderByUnrelated();
            sal_Bool SAL_CALL supportsGroupBy();
            sal_Bool SAL_CALL supportsGroupByUnrelated();
            sal_Bool SAL_CALL supportsGroupByBeyondSelect();
            sal_Bool SAL_CALL supportsLikeEscapeClause();
            sal_Bool SAL_CALL supportsMultipleResultSets();
            sal_Bool SAL_CALL supportsMultipleTransactions();
            sal_Bool SAL_CALL supportsNonNullableColumns();
            sal_Bool SAL_CALL supportsMinimumSQLGrammar();
            sal_Bool SAL_CALL supportsCoreSQLGrammar();
            sal_Bool SAL_CALL supportsExtendedSQLGrammar();
            sal_Bool SAL_CALL supportsANSI92EntryLevelSQL();
            sal_Bool SAL_CALL supportsANSI92IntermediateSQL();
            sal_Bool SAL_CALL supportsANSI92FullSQL();
            sal_Bool SAL_CALL supportsIntegrityEnhancementFacility();
            sal_Bool SAL_CALL supportsOuterJoins();
            sal_Bool SAL_CALL supportsFullOuterJoins();
            sal_Bool SAL_CALL supportsLimitedOuterJoins();
            OUString SAL_CALL getSchemaTerm();
            OUString SAL_CALL getProcedureTerm();
            OUString SAL_CALL getCatalogTerm();
            sal_Bool SAL_CALL isCatalogAtStart();
            OUString SAL_CALL getCatalogSeparator();
            sal_Bool SAL_CALL supportsSchemasInDataManipulation();
            sal_Bool SAL_CALL supportsSchemasInProcedureCalls();
            sal_Bool SAL_CALL supportsSchemasInTableDefinitions();
            sal_Bool SAL_CALL supportsSchemasInIndexDefinitions();
            sal_Bool SAL_CALL supportsSchemasInPrivilegeDefinitions();
            sal_Bool SAL_CALL supportsCatalogsInDataManipulation();
            sal_Bool SAL_CALL supportsCatalogsInProcedureCalls();
            sal_Bool SAL_CALL supportsCatalogsInTableDefinitions();
            sal_Bool SAL_CALL supportsCatalogsInIndexDefinitions();
            sal_Bool SAL_CALL supportsCatalogsInPrivilegeDefinitions();
            sal_Bool SAL_CALL supportsPositionedDelete();
            sal_Bool SAL_CALL supportsPositionedUpdate();
            sal_Bool SAL_CALL supportsSelectForUpdate();
            sal_Bool SAL_CALL supportsStoredProcedures();
            sal_Bool SAL_CALL supportsSubqueriesInComparisons();
            sal_Bool SAL_CALL supportsSubqueriesInExists();
            sal_Bool SAL_CALL supportsSubqueriesInIns();
            sal_Bool SAL_CALL supportsSubqueriesInQuantifieds();
            sal_Bool SAL_CALL supportsCorrelatedSubqueries();
            sal_Bool SAL_CALL supportsUnion();
            sal_Bool SAL_CALL supportsUnionAll();
            sal_Bool SAL_CALL supportsOpenCursorsAcrossCommit();
            sal_Bool SAL_CALL supportsOpenCursorsAcrossRollback();
            sal_Bool SAL_CALL supportsOpenStatementsAcrossCommit();
            sal_Bool SAL_CALL supportsOpenStatementsAcrossRollback();
            sal_Int32 SAL_CALL getMaxBinaryLiteralLength();
            sal_Int32 SAL_CALL getMaxCharLiteralLength();
            sal_Int32 SAL_CALL getMaxColumnNameLength();
            sal_Int32 SAL_CALL getMaxColumnsInGroupBy();
            sal_Int32 SAL_CALL getMaxColumnsInIndex();
            sal_Int32 SAL_CALL getMaxColumnsInOrderBy();
            sal_Int32 SAL_CALL getMaxColumnsInSelect();
            sal_Int32 SAL_CALL getMaxColumnsInTable();
            sal_Int32 SAL_CALL getMaxConnections();
            sal_Int32 SAL_CALL getMaxCursorNameLength();
            sal_Int32 SAL_CALL getMaxIndexLength();
            sal_Int32 SAL_CALL getMaxSchemaNameLength();
            sal_Int32 SAL_CALL getMaxProcedureNameLength();
            sal_Int32 SAL_CALL getMaxCatalogNameLength();
            sal_Int32 SAL_CALL getMaxRowSize();
            sal_Bool SAL_CALL doesMaxRowSizeIncludeBlobs();
            sal_Int32 SAL_CALL getMaxStatementLength();
            sal_Int32 SAL_CALL getMaxStatements();
            sal_Int32 SAL_CALL getMaxTableNameLength();
            sal_Int32 SAL_CALL getMaxTablesInSelect();
            sal_Int32 SAL_CALL getMaxUserNameLength();
            sal_Int32 SAL_CALL getDefaultTransactionIsolation();
            sal_Bool SAL_CALL supportsTransactions();
            sal_Bool SAL_CALL supportsTransactionIsolationLevel(sal_Int32 level);
            sal_Bool SAL_CALL supportsDataDefinitionAndDataManipulationTransactions();
            sal_Bool SAL_CALL supportsDataManipulationTransactionsOnly();
            sal_Bool SAL_CALL dataDefinitionCausesTransactionCommit();
            sal_Bool SAL_CALL dataDefinitionIgnoredInTransactions();
            my_XResultSetRef SAL_CALL getProcedures(const Any& catalog, const OUString& schemaPattern, const OUString& procedureNamePattern);
            my_XResultSetRef SAL_CALL getProcedureColumns(const Any& catalog, const OUString& schemaPattern, const OUString& procedureNamePattern, const OUString& columnNamePattern);
            my_XResultSetRef SAL_CALL getTables(const Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern, const ::com::sun::star::uno::Sequence< OUString >& types);
            my_XResultSetRef SAL_CALL getSchemas();
            my_XResultSetRef SAL_CALL getCatalogs();
            my_XResultSetRef SAL_CALL getTableTypes();
            my_XResultSetRef SAL_CALL getColumns(const Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern, const OUString& columnNamePattern);
            my_XResultSetRef SAL_CALL getColumnPrivileges(const Any& catalog, const OUString& schema, const OUString& table, const OUString& columnNamePattern);
            my_XResultSetRef SAL_CALL getTablePrivileges(const Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern);
            my_XResultSetRef SAL_CALL getBestRowIdentifier(const Any& catalog, const OUString& schema, const OUString& table, sal_Int32 scope, sal_Bool nullable);
            my_XResultSetRef SAL_CALL getVersionColumns(const Any& catalog, const OUString& schema, const OUString& table);
            my_XResultSetRef SAL_CALL getPrimaryKeys(const Any& catalog, const OUString& schema, const OUString& table);
            my_XResultSetRef SAL_CALL getImportedKeys(const Any& catalog, const OUString& schema, const OUString& table);
            my_XResultSetRef SAL_CALL getExportedKeys(const Any& catalog, const OUString& schema, const OUString& table);
            my_XResultSetRef SAL_CALL getCrossReference(const Any& primaryCatalog, const OUString& primarySchema, const OUString& primaryTable, const Any& foreignCatalog, const OUString& foreignSchema, const OUString& foreignTable);
            my_XResultSetRef SAL_CALL getTypeInfo();
            my_XResultSetRef SAL_CALL getIndexInfo(const Any& catalog, const OUString& schema, const OUString& table, sal_Bool unique, sal_Bool approximate);
            sal_Bool SAL_CALL supportsResultSetType(sal_Int32 setType);
            sal_Bool SAL_CALL supportsResultSetConcurrency(sal_Int32 setType, sal_Int32 concurrency);
            sal_Bool SAL_CALL ownUpdatesAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL ownDeletesAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL ownInsertsAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL othersUpdatesAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL othersDeletesAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL othersInsertsAreVisible(sal_Int32 setType);
            sal_Bool SAL_CALL updatesAreDetected(sal_Int32 setType);
            sal_Bool SAL_CALL deletesAreDetected(sal_Int32 setType);
            sal_Bool SAL_CALL insertsAreDetected(sal_Int32 setType);
            sal_Bool SAL_CALL supportsBatchUpdates();
            my_XResultSetRef SAL_CALL getUDTs(const Any& catalog, const OUString& schemaPattern, const OUString& typeNamePattern, const ::com::sun::star::uno::Sequence< sal_Int32 >& types);
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection();
		};
	}
}

#endif // CONNECTIVITY_SDATABASEMETADATA_HXX
