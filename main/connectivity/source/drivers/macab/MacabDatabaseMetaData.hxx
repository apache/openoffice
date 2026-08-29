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



#ifndef _CONNECTIVITY_MACAB_DATABASEMETADATA_HXX_
#define _CONNECTIVITY_MACAB_DATABASEMETADATA_HXX_

#include "MacabConnection.hxx"
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#include <cppuhelper/implbase1.hxx>

namespace connectivity
{
	namespace macab
	{
		//**************************************************************
		//************ Class: MacabDatabaseMetaData
		//**************************************************************

		typedef ::cppu::WeakImplHelper1< ::com::sun::star::sdbc::XDatabaseMetaData> MacabDatabaseMetaData_BASE;

		class MacabDatabaseMetaData : public MacabDatabaseMetaData_BASE
		{
			::com::sun::star::uno::Reference< MacabConnection >	m_xConnection;
			sal_Bool											m_bUseCatalog;

		public:

			inline MacabConnection* getOwnConnection() const { return m_xConnection.get(); }

			MacabDatabaseMetaData(MacabConnection* _pCon);
			virtual ~MacabDatabaseMetaData();

			// this interface is really BIG
			// XDatabaseMetaData
			virtual sal_Bool SAL_CALL allProceduresAreCallable(  );
			virtual sal_Bool SAL_CALL allTablesAreSelectable(  );
			virtual ::rtl::OUString SAL_CALL getURL(  );
			virtual ::rtl::OUString SAL_CALL getUserName(  );
			virtual sal_Bool SAL_CALL isReadOnly(  );
			virtual sal_Bool SAL_CALL nullsAreSortedHigh(  );
			virtual sal_Bool SAL_CALL nullsAreSortedLow(  );
			virtual sal_Bool SAL_CALL nullsAreSortedAtStart(  );
			virtual sal_Bool SAL_CALL nullsAreSortedAtEnd(  );
			virtual ::rtl::OUString SAL_CALL getDatabaseProductName(  );
			virtual ::rtl::OUString SAL_CALL getDatabaseProductVersion(  );
			virtual ::rtl::OUString SAL_CALL getDriverName(  );
			virtual ::rtl::OUString SAL_CALL getDriverVersion(  );
			virtual sal_Int32 SAL_CALL getDriverMajorVersion(  );
			virtual sal_Int32 SAL_CALL getDriverMinorVersion(  );
			virtual sal_Bool SAL_CALL usesLocalFiles(  );
			virtual sal_Bool SAL_CALL usesLocalFilePerTable(  );
			virtual sal_Bool SAL_CALL supportsMixedCaseIdentifiers(  );
			virtual sal_Bool SAL_CALL storesUpperCaseIdentifiers(  );
			virtual sal_Bool SAL_CALL storesLowerCaseIdentifiers(  );
			virtual sal_Bool SAL_CALL storesMixedCaseIdentifiers(  );
			virtual sal_Bool SAL_CALL supportsMixedCaseQuotedIdentifiers(  );
			virtual sal_Bool SAL_CALL storesUpperCaseQuotedIdentifiers(  );
			virtual sal_Bool SAL_CALL storesLowerCaseQuotedIdentifiers(  );
			virtual sal_Bool SAL_CALL storesMixedCaseQuotedIdentifiers(  );
			virtual ::rtl::OUString SAL_CALL getIdentifierQuoteString(  );
			virtual ::rtl::OUString SAL_CALL getSQLKeywords(  );
			virtual ::rtl::OUString SAL_CALL getNumericFunctions(  );
			virtual ::rtl::OUString SAL_CALL getStringFunctions(  );
			virtual ::rtl::OUString SAL_CALL getSystemFunctions(  );
			virtual ::rtl::OUString SAL_CALL getTimeDateFunctions(  );
			virtual ::rtl::OUString SAL_CALL getSearchStringEscape(  );
			virtual ::rtl::OUString SAL_CALL getExtraNameCharacters(  );
			virtual sal_Bool SAL_CALL supportsAlterTableWithAddColumn(  );
			virtual sal_Bool SAL_CALL supportsAlterTableWithDropColumn(  );
			virtual sal_Bool SAL_CALL supportsColumnAliasing(  );
			virtual sal_Bool SAL_CALL nullPlusNonNullIsNull(  );
			virtual sal_Bool SAL_CALL supportsTypeConversion(  );
			virtual sal_Bool SAL_CALL supportsConvert( sal_Int32 fromType, sal_Int32 toType );
			virtual sal_Bool SAL_CALL supportsTableCorrelationNames(  );
			virtual sal_Bool SAL_CALL supportsDifferentTableCorrelationNames(  );
			virtual sal_Bool SAL_CALL supportsExpressionsInOrderBy(  );
			virtual sal_Bool SAL_CALL supportsOrderByUnrelated(  );
			virtual sal_Bool SAL_CALL supportsGroupBy(  );
			virtual sal_Bool SAL_CALL supportsGroupByUnrelated(  );
			virtual sal_Bool SAL_CALL supportsGroupByBeyondSelect(  );
			virtual sal_Bool SAL_CALL supportsLikeEscapeClause(  );
			virtual sal_Bool SAL_CALL supportsMultipleResultSets(  );
			virtual sal_Bool SAL_CALL supportsMultipleTransactions(  );
			virtual sal_Bool SAL_CALL supportsNonNullableColumns(  );
			virtual sal_Bool SAL_CALL supportsMinimumSQLGrammar(  );
			virtual sal_Bool SAL_CALL supportsCoreSQLGrammar(  );
			virtual sal_Bool SAL_CALL supportsExtendedSQLGrammar(  );
			virtual sal_Bool SAL_CALL supportsANSI92EntryLevelSQL(  );
			virtual sal_Bool SAL_CALL supportsANSI92IntermediateSQL(  );
			virtual sal_Bool SAL_CALL supportsANSI92FullSQL(  );
			virtual sal_Bool SAL_CALL supportsIntegrityEnhancementFacility(  );
			virtual sal_Bool SAL_CALL supportsOuterJoins(  );
			virtual sal_Bool SAL_CALL supportsFullOuterJoins(  );
			virtual sal_Bool SAL_CALL supportsLimitedOuterJoins(  );
			virtual ::rtl::OUString SAL_CALL getSchemaTerm(  );
			virtual ::rtl::OUString SAL_CALL getProcedureTerm(  );
			virtual ::rtl::OUString SAL_CALL getCatalogTerm(  );
			virtual sal_Bool SAL_CALL isCatalogAtStart(  );
			virtual ::rtl::OUString SAL_CALL getCatalogSeparator(  );
			virtual sal_Bool SAL_CALL supportsSchemasInDataManipulation(  );
			virtual sal_Bool SAL_CALL supportsSchemasInProcedureCalls(  );
			virtual sal_Bool SAL_CALL supportsSchemasInTableDefinitions(  );
			virtual sal_Bool SAL_CALL supportsSchemasInIndexDefinitions(  );
			virtual sal_Bool SAL_CALL supportsSchemasInPrivilegeDefinitions(  );
			virtual sal_Bool SAL_CALL supportsCatalogsInDataManipulation(  );
			virtual sal_Bool SAL_CALL supportsCatalogsInProcedureCalls(  );
			virtual sal_Bool SAL_CALL supportsCatalogsInTableDefinitions(  );
			virtual sal_Bool SAL_CALL supportsCatalogsInIndexDefinitions(  );
			virtual sal_Bool SAL_CALL supportsCatalogsInPrivilegeDefinitions(  );
			virtual sal_Bool SAL_CALL supportsPositionedDelete(  );
			virtual sal_Bool SAL_CALL supportsPositionedUpdate(  );
			virtual sal_Bool SAL_CALL supportsSelectForUpdate(  );
			virtual sal_Bool SAL_CALL supportsStoredProcedures(  );
			virtual sal_Bool SAL_CALL supportsSubqueriesInComparisons(  );
			virtual sal_Bool SAL_CALL supportsSubqueriesInExists(  );
			virtual sal_Bool SAL_CALL supportsSubqueriesInIns(  );
			virtual sal_Bool SAL_CALL supportsSubqueriesInQuantifieds(  );
			virtual sal_Bool SAL_CALL supportsCorrelatedSubqueries(  );
			virtual sal_Bool SAL_CALL supportsUnion(  );
			virtual sal_Bool SAL_CALL supportsUnionAll(  );
			virtual sal_Bool SAL_CALL supportsOpenCursorsAcrossCommit(  );
			virtual sal_Bool SAL_CALL supportsOpenCursorsAcrossRollback(  );
			virtual sal_Bool SAL_CALL supportsOpenStatementsAcrossCommit(  );
			virtual sal_Bool SAL_CALL supportsOpenStatementsAcrossRollback(  );
			virtual sal_Int32 SAL_CALL getMaxBinaryLiteralLength(  );
			virtual sal_Int32 SAL_CALL getMaxCharLiteralLength(  );
			virtual sal_Int32 SAL_CALL getMaxColumnNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxColumnsInGroupBy(  );
			virtual sal_Int32 SAL_CALL getMaxColumnsInIndex(  );
			virtual sal_Int32 SAL_CALL getMaxColumnsInOrderBy(  );
			virtual sal_Int32 SAL_CALL getMaxColumnsInSelect(  );
			virtual sal_Int32 SAL_CALL getMaxColumnsInTable(  );
			virtual sal_Int32 SAL_CALL getMaxConnections(  );
			virtual sal_Int32 SAL_CALL getMaxCursorNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxIndexLength(  );
			virtual sal_Int32 SAL_CALL getMaxSchemaNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxProcedureNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxCatalogNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxRowSize(  );
			virtual sal_Bool SAL_CALL doesMaxRowSizeIncludeBlobs(  );
			virtual sal_Int32 SAL_CALL getMaxStatementLength(  );
			virtual sal_Int32 SAL_CALL getMaxStatements(  );
			virtual sal_Int32 SAL_CALL getMaxTableNameLength(  );
			virtual sal_Int32 SAL_CALL getMaxTablesInSelect(  );
			virtual sal_Int32 SAL_CALL getMaxUserNameLength(  );
			virtual sal_Int32 SAL_CALL getDefaultTransactionIsolation(  );
			virtual sal_Bool SAL_CALL supportsTransactions(  );
			virtual sal_Bool SAL_CALL supportsTransactionIsolationLevel( sal_Int32 level );
			virtual sal_Bool SAL_CALL supportsDataDefinitionAndDataManipulationTransactions(  );
			virtual sal_Bool SAL_CALL supportsDataManipulationTransactionsOnly(  );
			virtual sal_Bool SAL_CALL dataDefinitionCausesTransactionCommit(  );
			virtual sal_Bool SAL_CALL dataDefinitionIgnoredInTransactions(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getProcedures( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getProcedureColumns( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern, const ::rtl::OUString& columnNamePattern );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTables( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& types );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getSchemas(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getCatalogs(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTableTypes(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getColumns( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern, const ::rtl::OUString& columnNamePattern );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getColumnPrivileges( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, const ::rtl::OUString& columnNamePattern );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTablePrivileges( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getBestRowIdentifier( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, sal_Int32 scope, sal_Bool nullable );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getVersionColumns( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getPrimaryKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getImportedKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getExportedKeys( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getCrossReference( const ::com::sun::star::uno::Any& primaryCatalog, const ::rtl::OUString& primarySchema, const ::rtl::OUString& primaryTable, const ::com::sun::star::uno::Any& foreignCatalog, const ::rtl::OUString& foreignSchema, const ::rtl::OUString& foreignTable );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getTypeInfo(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getIndexInfo( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, sal_Bool unique, sal_Bool approximate );
			virtual sal_Bool SAL_CALL supportsResultSetType( sal_Int32 setType );
			virtual sal_Bool SAL_CALL supportsResultSetConcurrency( sal_Int32 setType, sal_Int32 concurrency );
			virtual sal_Bool SAL_CALL ownUpdatesAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL ownDeletesAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL ownInsertsAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL othersUpdatesAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL othersDeletesAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL othersInsertsAreVisible( sal_Int32 setType );
			virtual sal_Bool SAL_CALL updatesAreDetected( sal_Int32 setType );
			virtual sal_Bool SAL_CALL deletesAreDetected( sal_Int32 setType );
			virtual sal_Bool SAL_CALL insertsAreDetected( sal_Int32 setType );
			virtual sal_Bool SAL_CALL supportsBatchUpdates(  );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getUDTs( const ::com::sun::star::uno::Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& typeNamePattern, const ::com::sun::star::uno::Sequence< sal_Int32 >& types );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection(  );
		};
	}
}

#endif // _CONNECTIVITY_MACAB_DATABASEMETADATA_HXX_
