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



#ifndef _CONNECTIVITY_MACAB_STATEMENT_HXX_
#define _CONNECTIVITY_MACAB_STATEMENT_HXX_

#include "MacabConnection.hxx"
#include "MacabHeader.hxx"
#include <list>
#include "connectivity/sqliterator.hxx"
#ifndef _CONNECTIVITY_PARSE_SQLPARSE_HXX_
#include "connectivity/sqlparse.hxx"
#endif
#include <com/sun/star/sdbc/XStatement.hpp>
#include <com/sun/star/util/XCancellable.hpp>
#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/proparrhlp.hxx>

namespace connectivity
{
	namespace macab
	{
		typedef ::cppu::WeakComponentImplHelper4<	::com::sun::star::sdbc::XStatement,
                                                    ::com::sun::star::sdbc::XWarningsSupplier,
                                                    ::com::sun::star::util::XCancellable,
                                                    ::com::sun::star::sdbc::XCloseable> MacabCommonStatement_BASE;

		//**************************************************************
		// Class MacabCommonStatement
		// is a base class for the normal statement and for the prepared statement
		//**************************************************************
		class MacabCommonStatement :	public comphelper::OBaseMutex,
						public	MacabCommonStatement_BASE,
						public	::cppu::OPropertySetHelper,
						public	comphelper::OPropertyArrayUsageHelper<MacabCommonStatement>

		{
			::com::sun::star::sdbc::SQLWarning	m_aLastWarning;

		protected:
			::std::list< ::rtl::OUString>		m_aBatchList;
			connectivity::OSQLParser			m_aParser;
			connectivity::OSQLParseTreeIterator	m_aSQLIterator;
			connectivity::OSQLParseNode*		m_pParseTree;
			MacabConnection*						m_pConnection;	// The owning Connection object
			MacabHeader*							m_pHeader;	// The header of the address book on which to run queries (provided by m_pConnection)
			::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XResultSet>    m_xResultSet;   // The last ResultSet created


		protected:
			class MacabCondition *analyseWhereClause(
				const OSQLParseNode *pParseNode) const;
			class MacabOrder *analyseOrderByClause(
				const OSQLParseNode *pParseNode) const;
			::rtl::OUString getTableName( ) const;
			void setMacabFields(class MacabResultSet *pResult) const;
			void selectRecords(MacabResultSet *pResult) const;
			void sortRecords(MacabResultSet *pResult) const;

			// OPropertyArrayUsageHelper
			virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

			// OPropertySetHelper
			virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();
			virtual sal_Bool SAL_CALL convertFastPropertyValue(
					::com::sun::star::uno::Any & rConvertedValue,
					::com::sun::star::uno::Any & rOldValue,
					sal_Int32 nHandle,
					const ::com::sun::star::uno::Any& rValue);
			virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
					sal_Int32 nHandle,
					const ::com::sun::star::uno::Any& rValue);
			virtual void SAL_CALL getFastPropertyValue(
					::com::sun::star::uno::Any& rValue,
					sal_Int32 nHandle) const;

			virtual void resetParameters() const;
			virtual void getNextParameter(::rtl::OUString &rParameter) const;
			virtual ~MacabCommonStatement();

		public:
			::cppu::OBroadcastHelper& rBHelper;

			MacabCommonStatement(MacabConnection *_pConnection);
			using MacabCommonStatement_BASE::operator ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >;

			// OComponentHelper
			virtual void SAL_CALL disposing();

			// XInterface
			virtual void SAL_CALL release() throw();
			virtual void SAL_CALL acquire() throw();
			virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
					const ::com::sun::star::uno::Type & rType
					);

			// XTypeProvider
			virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(
					);

			// XPropertySet
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(
					);

			// XStatement
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery(
					const ::rtl::OUString& sql );
			virtual sal_Int32 SAL_CALL executeUpdate(
					 const ::rtl::OUString& sql );
			virtual sal_Bool SAL_CALL execute(
					const ::rtl::OUString& sql );
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection(
					);

			// XWarningsSupplier
			virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(
					);
			virtual void SAL_CALL clearWarnings(
					);

			// XCancellable
			virtual void SAL_CALL cancel(
					);

			// XCloseable
			virtual void SAL_CALL close(
					);

			// other methods
			inline MacabConnection* getOwnConnection() const { return m_pConnection; }
		};

		//**************************************************************
		// Class MacabStatement
		//**************************************************************
		typedef ::cppu::ImplInheritanceHelper1<
				MacabCommonStatement, ::com::sun::star::lang::XServiceInfo > MacabStatement_BASE;

		class MacabStatement : public MacabStatement_BASE
		{
		protected:
			virtual ~MacabStatement() { }

		public:
			MacabStatement(MacabConnection* _pConnection);
			DECLARE_SERVICE_INFO();
		};
	}
}

#endif // _CONNECTIVITY_MACAB_STATEMENT_HXX_
