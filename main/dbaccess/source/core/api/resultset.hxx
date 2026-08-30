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


#ifndef _DBA_COREAPI_RESULTSET_HXX_
#define _DBA_COREAPI_RESULTSET_HXX_

#ifndef _DBA_COREAPI_COLUMN_HXX_
#include "column.hxx"
#endif
#ifndef DBTOOLS_WARNINGSCONTAINER_HXX
#include <connectivity/warningscontainer.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XSTATEMENT_HPP_
#include <com/sun/star/sdbc/XStatement.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCLOSEABLE_HPP_
#include <com/sun/star/sdbc/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATASUPPLIER_HPP_
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XWARNINGSSUPPLIER_HPP_
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCOLUMNLOCATE_HPP_
#include <com/sun/star/sdbc/XColumnLocate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XROWLOCATE_HPP_
#include <com/sun/star/sdbcx/XRowLocate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROWUPDATE_HPP_
#include <com/sun/star/sdbc/XRowUpdate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETUPDATE_HPP_
#include <com/sun/star/sdbc/XResultSetUpdate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_RESULTSETCONCURRENCY_HPP_
#include <com/sun/star/sdbc/ResultSetConcurrency.hpp>
#endif

#ifndef _CPPUHELPER_PROPSHLP_HXX
#include <cppuhelper/propshlp.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#include <comphelper/proparrhlp.hxx>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _CPPUHELPER_COMPBASE11_HXX_
#include <cppuhelper/compbase11.hxx>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif

namespace dbaccess
{
	typedef ::cppu::WeakComponentImplHelper11< ::com::sun::star::sdbc::XWarningsSupplier,
											  ::com::sun::star::sdbc::XResultSet,
											  ::com::sun::star::sdbc::XResultSetMetaDataSupplier,
											  ::com::sun::star::sdbc::XRow,
											  ::com::sun::star::sdbc::XCloseable,
											  ::com::sun::star::sdbc::XColumnLocate,
											  ::com::sun::star::sdbcx::XRowLocate,
											  ::com::sun::star::sdbcx::XColumnsSupplier,
											  ::com::sun::star::sdbc::XResultSetUpdate,
											  ::com::sun::star::sdbc::XRowUpdate,
											  ::com::sun::star::lang::XServiceInfo > OResultSetBase;

	typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> ONoWeakStatement;

	//************************************************************
	//  OResultSet
	//************************************************************
	class OResultSet :	public comphelper::OBaseMutex,
						public OResultSetBase,
						public ::cppu::OPropertySetHelper,
						public ::comphelper::OPropertyArrayUsageHelper < OResultSet >
	{
	protected:
		ONoWeakStatement				m_aStatement;

		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet >	        m_xDelegatorResultSet;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetUpdate >	m_xDelegatorResultSetUpdate;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRow >		        m_xDelegatorRow;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowUpdate >	        m_xDelegatorRowUpdate;

        ::dbtools::WarningsContainer    m_aWarnings;
		OColumns*					    m_pColumns;
		sal_Int32					    m_nResultSetType;
		sal_Int32					    m_nResultSetConcurrency;
		sal_Bool					    m_bIsBookmarkable : 1;

	public:
		OResultSet(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet >& _xResultSet,
				   const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _xStatement,
				   sal_Bool _bCaseSensitive);
		virtual ~OResultSet();

	// ::com::sun::star::lang::XTypeProvider
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

	// ::com::sun::star::uno::XInterface
		virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
		virtual void SAL_CALL acquire() throw();
		virtual void SAL_CALL release() throw();

	// ::com::sun::star::lang::XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// ::cppu::OComponentHelper
		virtual void SAL_CALL disposing(void);

	// ::com::sun::star::sdbc::XCloseable
		virtual void SAL_CALL close(  );

	// com::sun::star::beans::XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );

	// comphelper::OPropertyArrayUsageHelper
		virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;

	// cppu::OPropertySetHelper
		virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

		virtual sal_Bool SAL_CALL convertFastPropertyValue(
								::com::sun::star::uno::Any & rConvertedValue,
								::com::sun::star::uno::Any & rOldValue,
								sal_Int32 nHandle,
								const ::com::sun::star::uno::Any& rValue );
		virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
									sal_Int32 nHandle,
									const ::com::sun::star::uno::Any& rValue
													 );
		virtual void SAL_CALL getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;

	// ::com::sun::star::sdbc::XWarningsSupplier
		virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  );
		virtual void SAL_CALL clearWarnings(  );

	// ::com::sun::star::sdbc::XResultSetMetaDataSupplier
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData(  );

	// ::com::sun::star::sdbc::XColumnLocate
		virtual sal_Int32 SAL_CALL findColumn( const ::rtl::OUString& columnName );

	// ::com::sun::star::sdbcx::XColumnsSupplier
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getColumns(  );

	// ::com::sun::star::sdbc::XRow
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
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getStatement(  );

	// ::com::sun::star::sdbcx::XRowLocate
		virtual ::com::sun::star::uno::Any SAL_CALL getBookmark(  );
		virtual sal_Bool SAL_CALL moveToBookmark( const ::com::sun::star::uno::Any& bookmark );
		virtual sal_Bool SAL_CALL moveRelativeToBookmark( const ::com::sun::star::uno::Any& bookmark, sal_Int32 rows );
		virtual sal_Int32 SAL_CALL compareBookmarks( const ::com::sun::star::uno::Any& first, const ::com::sun::star::uno::Any& second );
		virtual sal_Bool SAL_CALL hasOrderedBookmarks(  );
		virtual sal_Int32 SAL_CALL hashBookmark( const ::com::sun::star::uno::Any& bookmark );

	// ::com::sun::star::sdbc::XResultSetUpdate
		virtual void SAL_CALL insertRow(  );
		virtual void SAL_CALL updateRow(  );
		virtual void SAL_CALL deleteRow(  );
		virtual void SAL_CALL cancelRowUpdates(  );
		virtual void SAL_CALL moveToInsertRow(  );
		virtual void SAL_CALL moveToCurrentRow(  );

	// ::com::sun::star::sdbc::XRowUpdate
		virtual void SAL_CALL updateNull( sal_Int32 columnIndex );
		virtual void SAL_CALL updateBoolean( sal_Int32 columnIndex, sal_Bool x );
		virtual void SAL_CALL updateByte( sal_Int32 columnIndex, sal_Int8 x );
		virtual void SAL_CALL updateShort( sal_Int32 columnIndex, sal_Int16 x );
		virtual void SAL_CALL updateInt( sal_Int32 columnIndex, sal_Int32 x );
		virtual void SAL_CALL updateLong( sal_Int32 columnIndex, sal_Int64 x );
		virtual void SAL_CALL updateFloat( sal_Int32 columnIndex, float x );
		virtual void SAL_CALL updateDouble( sal_Int32 columnIndex, double x );
		virtual void SAL_CALL updateString( sal_Int32 columnIndex, const ::rtl::OUString& x );
		virtual void SAL_CALL updateBytes( sal_Int32 columnIndex, const ::com::sun::star::uno::Sequence< sal_Int8 >& x );
		virtual void SAL_CALL updateDate( sal_Int32 columnIndex, const ::com::sun::star::util::Date& x );
		virtual void SAL_CALL updateTime( sal_Int32 columnIndex, const ::com::sun::star::util::Time& x );
		virtual void SAL_CALL updateTimestamp( sal_Int32 columnIndex, const ::com::sun::star::util::DateTime& x );
		virtual void SAL_CALL updateBinaryStream( sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length );
		virtual void SAL_CALL updateCharacterStream( sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length );
		virtual void SAL_CALL updateObject( sal_Int32 columnIndex, const ::com::sun::star::uno::Any& x );
		virtual void SAL_CALL updateNumericObject( sal_Int32 columnIndex, const ::com::sun::star::uno::Any& x, sal_Int32 scale );

	protected:
		void checkReadOnly() const;
		void checkBookmarkable() const;

    private:
        using ::cppu::OPropertySetHelper::getFastPropertyValue;
	};
}
#endif // _DBA_COREAPI_RESULTSET_HXX_
