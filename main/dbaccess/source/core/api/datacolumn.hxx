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



#ifndef _DBACORE_DATACOLUMN_HXX_
#define _DBACORE_DATACOLUMN_HXX_

#ifndef _COM_SUN_STAR_SDBC_XROWUPDATE_HPP_
#include <com/sun/star/sdbc/XRowUpdate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATA_HDL_
#include <com/sun/star/sdbc/XResultSetMetaData.hdl>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOLUMN_HPP_
#include <com/sun/star/sdb/XColumn.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOLUMNUPDATE_HPP_
#include <com/sun/star/sdb/XColumnUpdate.hpp>
#endif
#ifndef _DBA_COREAPI_RESULTCOLUMN_HXX_
#include <resultcolumn.hxx>
#endif
namespace dbaccess
{
	//************************************************************
	//  ODataColumn
	//************************************************************
	class ODataColumn : public OResultColumn,
						public ::com::sun::star::sdb::XColumn,
						public ::com::sun::star::sdb::XColumnUpdate
	{
	protected:
		::com::sun::star::uno::Reference < ::com::sun::star::sdbc::XRow >		m_xRow;
		::com::sun::star::uno::Reference < ::com::sun::star::sdbc::XRowUpdate >	m_xRowUpdate;

		virtual ~ODataColumn();
	public:
		ODataColumn	 (const ::com::sun::star::uno::Reference < ::com::sun::star::sdbc::XResultSetMetaData >& _xMetaData,
					  const ::com::sun::star::uno::Reference < ::com::sun::star::sdbc::XRow >& _xRow,
					  const ::com::sun::star::uno::Reference < ::com::sun::star::sdbc::XRowUpdate >& _xRowUpdate,
					  sal_Int32 _nPos,
                      const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >& _rxDBMeta);

	// com::sun::star::lang::XTypeProvider
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

	// com::sun::star::uno::XInterface
		virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
		virtual void SAL_CALL acquire() throw() { OResultColumn::acquire(); }
		virtual void SAL_CALL release() throw() { OResultColumn::release(); }

	// ::com::sun::star::lang::XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// cppu::OComponentHelper
		virtual void SAL_CALL disposing(void);

	// ::com::sun::star::sdb::XColumn
		virtual sal_Bool SAL_CALL wasNull(  );
		virtual ::rtl::OUString SAL_CALL getString(  );
		virtual sal_Bool SAL_CALL getBoolean(  );
		virtual sal_Int8 SAL_CALL getByte(  );
		virtual sal_Int16 SAL_CALL getShort(  );
		virtual sal_Int32 SAL_CALL getInt(  );
		virtual sal_Int64 SAL_CALL getLong(  );
		virtual float SAL_CALL getFloat(  );
		virtual double SAL_CALL getDouble(  );
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBytes(  );
		virtual ::com::sun::star::util::Date SAL_CALL getDate(  );
		virtual ::com::sun::star::util::Time SAL_CALL getTime(  );
		virtual ::com::sun::star::util::DateTime SAL_CALL getTimestamp(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getBinaryStream(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getCharacterStream(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getObject( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef > SAL_CALL getRef(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob > SAL_CALL getBlob(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob > SAL_CALL getClob(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray > SAL_CALL getArray(  );

	// ::com::sun::star::sdb::XColumnUpdate
		virtual void SAL_CALL updateNull(  );
		virtual void SAL_CALL updateBoolean( sal_Bool x );
		virtual void SAL_CALL updateByte( sal_Int8 x );
		virtual void SAL_CALL updateShort( sal_Int16 x );
		virtual void SAL_CALL updateInt( sal_Int32 x );
		virtual void SAL_CALL updateLong( sal_Int64 x );
		virtual void SAL_CALL updateFloat( float x );
		virtual void SAL_CALL updateDouble( double x );
		virtual void SAL_CALL updateString( const ::rtl::OUString& x );
		virtual void SAL_CALL updateBytes( const ::com::sun::star::uno::Sequence< sal_Int8 >& x );
		virtual void SAL_CALL updateDate( const ::com::sun::star::util::Date& x );
		virtual void SAL_CALL updateTime( const ::com::sun::star::util::Time& x );
		virtual void SAL_CALL updateTimestamp( const ::com::sun::star::util::DateTime& x );
		virtual void SAL_CALL updateBinaryStream( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length );
		virtual void SAL_CALL updateCharacterStream( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length );
		virtual void SAL_CALL updateObject( const ::com::sun::star::uno::Any& x );
		virtual void SAL_CALL updateNumericObject( const ::com::sun::star::uno::Any& x, sal_Int32 scale );
	};
}
#endif // _DBACORE_RESULTCOLUMN_HXX_
