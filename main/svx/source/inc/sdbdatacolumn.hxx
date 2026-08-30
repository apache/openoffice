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



#ifndef SVX_FORM_SDBDATACOLUMN_HXX
#define SVX_FORM_SDBDATACOLUMN_HXX

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sdb/XColumn.hpp>
#include <com/sun/star/sdb/XColumnUpdate.hpp>
#include <osl/diagnose.h>

//..............................................................................
namespace svxform
{
//..............................................................................

	//==========================================================================
	//= DataColumn - a class wrapping an object implementing a sdb::DataColumn service
	//==========================================================================
	class DataColumn
	{
		// interfaces needed for sddb::Column
		::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>	m_xPropertySet;
		// interfaces needed for sdb::DataColumn
		::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumn>			m_xColumn;
		::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumnUpdate> 	m_xColumnUpdate;

	public:
		DataColumn() { };
		DataColumn(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxIFace);
		// if the object behind _rxIFace doesn't fully support the DataColumn service,
		// (which is checked via the supported interfaces) _all_ members will be set to
		// void !, even if the object has some of the needed interfaces.

		sal_Bool is() const { return m_xColumn.is(); }
		sal_Bool Is() const { return m_xColumn.is(); }
		sal_Bool supportsUpdate() const { return m_xColumnUpdate.is(); }

		DataColumn* operator ->() { return this; }
		operator ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> () const{ return m_xColumn.get(); }

		// 'conversions'
		inline const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& getPropertySet() const
		{
			return m_xPropertySet;
		}
		inline const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumn>& getColumn() const
		{
			return m_xColumn;
		}
		inline const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XColumnUpdate>& getColumnUpdate() const
		{
			OSL_ENSURE(m_xColumnUpdate.is() , "DataColumn::getColumnUpdate: NULL!");
			return m_xColumnUpdate;
		}

		// das normale queryInterface
		::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& type)
		{ return m_xColumn->queryInterface(type); }

		// ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>
		inline ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> getPropertySetInfo() const;
		inline void setPropertyValue(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue);
		inline ::com::sun::star::uno::Any getPropertyValue(const ::rtl::OUString& PropertyName) const;
		inline void addPropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener>& xListener);
		inline void removePropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener>& aListener);
		inline void addVetoableChangeListener(const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener>& aListener);
		inline void removeVetoableChangeListener(const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener>& aListener);

		// ::com::sun::star::sdb::XColumn
		inline sal_Bool wasNull();
		inline ::rtl::OUString getString();
		inline sal_Bool getBoolean();
		inline sal_Int8 getByte();
		inline sal_Int16 getShort();
		inline sal_Int32 getInt();
		inline sal_Int64 getLong();
		inline float getFloat();
		inline double getDouble();
		inline ::com::sun::star::uno::Sequence< sal_Int8 > getBytes();
		inline ::com::sun::star::util::Date getDate();
		inline ::com::sun::star::util::Time getTime();
		inline ::com::sun::star::util::DateTime  getTimestamp();
		inline ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream> getBinaryStream();
		inline ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream> getCharacterStream();
		inline ::com::sun::star::uno::Any getObject(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>& typeMap);
		inline ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef> getRef();
		inline ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob> getBlob();
		inline ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob> getClob();
		inline ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray> getArray();

		// XColumnUpdate
		inline void updateNull(void);
		inline void updateBoolean(sal_Bool x);
		inline void updateByte(sal_Int8 x);
		inline void updateShort(sal_Int16 x);
		inline void updateInt(sal_Int32 x);
		inline void updateLong(sal_Int64 x);
		inline void updateFloat(float x);
		inline void updateDouble(double x);
		inline void updateString(const ::rtl::OUString& x);
		inline void updateBytes(const ::com::sun::star::uno::Sequence< sal_Int8 >& x);
		inline void updateDate(const com::sun::star::util::Date& x);
		inline void updateTime(const ::com::sun::star::util::Time& x);
		inline void updateTimestamp(const ::com::sun::star::util::DateTime& x);
		inline void updateBinaryStream(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>& x, sal_Int32 length);
		inline void updateCharacterStream(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>& x, sal_Int32 length);
		inline void updateObject(const ::com::sun::star::uno::Any& x);
		inline void updateNumericObject(const ::com::sun::star::uno::Any& x, sal_Int32 scale);
	};

#endif // SVX_FORM_SDBDATACOLUMN_HXX

//..............................................................................
}	// namespace svxform
//..............................................................................
