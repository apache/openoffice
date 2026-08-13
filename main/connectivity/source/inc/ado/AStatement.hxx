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



#ifndef _CONNECTIVITY_ADO_ASTATEMENT_HXX_
#define _CONNECTIVITY_ADO_ASTATEMENT_HXX_

#include <com/sun/star/sdbc/XStatement.hpp>
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#include <com/sun/star/sdbc/XMultipleResults.hpp>
#include <com/sun/star/sdbc/XBatchExecution.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/util/XCancellable.hpp>
#include <cppuhelper/compbase5.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/broadcasthelper.hxx>
#include "ado/AConnection.hxx"
#ifndef _LIST_
#include <list>
#endif
#include "ado/Awrapado.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace connectivity
{
	namespace ado
	{
        typedef ::cppu::WeakComponentImplHelper5<   ::com::sun::star::sdbc::XStatement,
                                                    ::com::sun::star::sdbc::XWarningsSupplier,
                                                    ::com::sun::star::util::XCancellable,
                                                    ::com::sun::star::sdbc::XCloseable,
                                                    ::com::sun::star::sdbc::XMultipleResults> OStatement_BASE;

		//**************************************************************
		//************ Class: java.sql.Statement
		//**************************************************************
		class OStatement_Base		:	public comphelper::OBaseMutex,
										public	OStatement_BASE,
										public	::cppu::OPropertySetHelper,
										public	::comphelper::OPropertyArrayUsageHelper<OStatement_Base>,
										public  connectivity::OSubComponent<OStatement_Base, OStatement_BASE>

		{
			friend class connectivity::OSubComponent<OStatement_Base, OStatement_BASE>;
			friend class OResultSet;

			::com::sun::star::sdbc::SQLWarning			m_aLastWarning;

		protected:
			::std::list< ::rtl::OUString>				m_aBatchList;

            ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XResultSet>    m_xResultSet;   // The last ResultSet created
																		//  for this Statement

			OConnection*								m_pConnection;// The owning Connection object
			WpADOCommand								m_Command;
			WpADORecordset								m_RecordSet;
			OLEVariant									m_RecordsAffected;
			OLEVariant									m_Parameters;
			::std::vector<connectivity::OTypeInfo>		m_aTypeInfo;	// Hashtable containing an entry
																		//  for each row returned by
																		//  DatabaseMetaData.getTypeInfo.
			sal_Int32									m_nMaxRows;
			sal_Int32									m_nFetchSize;
			LockTypeEnum								m_eLockType;
			CursorTypeEnum								m_eCursorType;

			using OStatement_BASE::rBHelper;
		private:

			sal_Int32 getQueryTimeOut()         const;
            sal_Int32 getMaxFieldSize()         const;
            sal_Int32 getMaxRows()              const;
            sal_Int32 getResultSetConcurrency() const;
            sal_Int32 getResultSetType()        const;
            sal_Int32 getFetchDirection()       const;
            sal_Int32 getFetchSize()            const;
            ::rtl::OUString getCursorName()     const;

            void setQueryTimeOut(sal_Int32 _par0);
            void setMaxFieldSize(sal_Int32 _par0);
            void setMaxRows(sal_Int32 _par0);
            void setResultSetConcurrency(sal_Int32 _par0);
            void setResultSetType(sal_Int32 _par0);
            void setFetchDirection(sal_Int32 _par0);
            void setFetchSize(sal_Int32 _par0);
            void setCursorName(const ::rtl::OUString &_par0);

		protected:

			void assignRecordSet( ADORecordset* _pRS );

            void reset ();
            void clearMyResultSet ();
            void setWarning (const  ::com::sun::star::sdbc::SQLWarning &ex);
            sal_Int32 getColumnCount ();
            sal_Int32 getRowCount ();
			sal_Int32 getPrecision ( sal_Int32 sqlType);

			void disposeResultSet();

			// OPropertyArrayUsageHelper
			virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;
			// OPropertySetHelper
			virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();
			virtual sal_Bool SAL_CALL convertFastPropertyValue(
                                ::com::sun::star::uno::Any & rConvertedValue,
                                ::com::sun::star::uno::Any & rOldValue,
								sal_Int32 nHandle,
                                const ::com::sun::star::uno::Any& rValue );
			virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
									sal_Int32 nHandle,
                                    const ::com::sun::star::uno::Any& rValue
									);
			virtual void SAL_CALL getFastPropertyValue(
                                    ::com::sun::star::uno::Any& rValue,
									sal_Int32 nHandle
									) const;
		public:
			OStatement_Base(OConnection* _pConnection );

			using OStatement_BASE::operator ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >;
			// OComponentHelper
			virtual void SAL_CALL disposing(void);
			// XInterface
            virtual void SAL_CALL acquire() throw();
			virtual void SAL_CALL release() throw();
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
			//XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );

			// XPropertySet
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
			// XStatement
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL executeQuery( const ::rtl::OUString& sql ) ;
            virtual sal_Int32 SAL_CALL executeUpdate( const ::rtl::OUString& sql ) ;
            virtual sal_Bool SAL_CALL execute( const ::rtl::OUString& sql ) ;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection(  ) ;
			// XWarningsSupplier
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  );
            virtual void SAL_CALL clearWarnings(  );
			// XCancellable
            virtual void SAL_CALL cancel(  );
			// XCloseable
            virtual void SAL_CALL close(  );
			// XMultipleResults
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getResultSet(  );
            virtual sal_Int32 SAL_CALL getUpdateCount(  );
            virtual sal_Bool SAL_CALL getMoreResults(  );
		};

		class OStatement :	public OStatement_Base,
                            public ::com::sun::star::sdbc::XBatchExecution,
                            public ::com::sun::star::lang::XServiceInfo
		{
		public:
			// ein Konstruktor, der fuer das Returnen des Objektes benoetigt wird:
			OStatement( OConnection* _pConnection) : OStatement_Base( _pConnection){};
			~OStatement();

			DECLARE_SERVICE_INFO();

			virtual void SAL_CALL acquire() throw();
            virtual void SAL_CALL release() throw();
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
			// XBatchExecution
            virtual void SAL_CALL addBatch( const ::rtl::OUString& sql );
            virtual void SAL_CALL clearBatch(  );
            virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL executeBatch(  );
		};
	}
}
#endif // _CONNECTIVITY_ADO_ASTATEMENT_HXX_
