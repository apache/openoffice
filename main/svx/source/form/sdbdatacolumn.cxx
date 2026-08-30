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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"
#include "sdbdatacolumn.hxx"

//..............................................................................
namespace svxform
{
//..............................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::lang;
	using namespace ::com::sun::star::beans;
	using namespace ::com::sun::star::sdbc;
	using namespace ::com::sun::star::util;
	using namespace ::com::sun::star::io;
	using namespace ::com::sun::star::container;

	//==========================================================================
	//= DataColumn - a class wrapping an object implementing a sdb::DataColumn service
	//==========================================================================
	DataColumn::DataColumn(const Reference< ::com::sun::star::beans::XPropertySet>& _rxIFace)
	{
		m_xPropertySet = _rxIFace;
		m_xColumn = Reference< ::com::sun::star::sdb::XColumn>(_rxIFace, UNO_QUERY);
		m_xColumnUpdate = Reference< ::com::sun::star::sdb::XColumnUpdate>(_rxIFace, UNO_QUERY);

		if (!m_xPropertySet.is() || !m_xColumn.is())
		{
			m_xPropertySet = NULL;
			m_xColumn = NULL;
			m_xColumnUpdate = NULL;
		}
	}

	// Reference< XPropertySet>
	Reference< XPropertySetInfo> DataColumn::getPropertySetInfo() const
	{
		return m_xPropertySet->getPropertySetInfo();
	}

	void DataColumn::setPropertyValue(const ::rtl::OUString& aPropertyName, const Any& aValue)
	{
		m_xPropertySet->setPropertyValue(aPropertyName, aValue);
	}

	Any DataColumn::getPropertyValue(const ::rtl::OUString& PropertyName) const
	{
		return m_xPropertySet->getPropertyValue(PropertyName);
	}

	void DataColumn::addPropertyChangeListener(const ::rtl::OUString& aPropertyName, const Reference< XPropertyChangeListener>& xListener)
	{
		m_xPropertySet->addPropertyChangeListener(aPropertyName, xListener);
	}

	void DataColumn::removePropertyChangeListener(const ::rtl::OUString& aPropertyName, const Reference< XPropertyChangeListener>& aListener)
	{
		m_xPropertySet->removePropertyChangeListener(aPropertyName, aListener);
	}

	void DataColumn::addVetoableChangeListener(const ::rtl::OUString& PropertyName, const Reference< XVetoableChangeListener>& aListener)
	{
		m_xPropertySet->addVetoableChangeListener(PropertyName, aListener);
	}

	void DataColumn::removeVetoableChangeListener(const ::rtl::OUString& PropertyName, const Reference< XVetoableChangeListener>& aListener)
	{
		m_xPropertySet->removeVetoableChangeListener(PropertyName, aListener);
	}

	// XColumn
	sal_Bool DataColumn::wasNull()
	{
		return m_xColumn->wasNull();
	}

	::rtl::OUString DataColumn::getString()
	{
		return m_xColumn->getString();
	}

	sal_Bool DataColumn::getBoolean()
	{
		return m_xColumn->getBoolean();
	}

	sal_Int8 DataColumn::getByte()
	{
		return m_xColumn->getByte();
	}

	sal_Int16 DataColumn::getShort()
	{
		return m_xColumn->getShort();
	}

	sal_Int32 DataColumn::getInt()
	{
		return m_xColumn->getInt();
	}

	sal_Int64 DataColumn::getLong()
	{
		return m_xColumn->getLong();
	}

	float DataColumn::getFloat()
	{
		return m_xColumn->getFloat();
	}

	double DataColumn::getDouble()
	{
		return m_xColumn->getDouble();
	}

	Sequence< sal_Int8 > DataColumn::getBytes()
	{
		return m_xColumn->getBytes();
	}

	com::sun::star::util::Date DataColumn::getDate()
	{
		return m_xColumn->getDate();
	}

	com::sun::star::util::Time DataColumn::getTime()
	{
		return m_xColumn->getTime();
	}

	com::sun::star::util::DateTime DataColumn::getTimestamp()
	{
		return m_xColumn->getTimestamp();
	}

	Reference< XInputStream> DataColumn::getBinaryStream()
	{
		return m_xColumn->getBinaryStream();
	}

	Reference< XInputStream> DataColumn::getCharacterStream()
	{
		return m_xColumn->getCharacterStream();
	}

	Any DataColumn::getObject(const Reference< XNameAccess>& typeMap)
	{
		return m_xColumn->getObject(typeMap);
	}

	Reference< XRef> DataColumn::getRef()
	{
		return m_xColumn->getRef();
	}

	Reference< XBlob> DataColumn::getBlob()
	{
		return m_xColumn->getBlob();
	}

	Reference< XClob> DataColumn::getClob()
	{
		return m_xColumn->getClob();
	}

	Reference< XArray> DataColumn::getArray()
	{
		return m_xColumn->getArray();
	}

	// XColumnUpdate
	void DataColumn::updateNull()
	{
		m_xColumnUpdate->updateNull();
	}

	void DataColumn::updateBoolean(sal_Bool x)
	{
		m_xColumnUpdate->updateBoolean(x);
	}

	void DataColumn::updateByte(sal_Int8 x)
	{
		m_xColumnUpdate->updateByte(x);
	}

	void DataColumn::updateShort(sal_Int16 x)
	{
		m_xColumnUpdate->updateShort(x);
	}

	void DataColumn::updateInt(sal_Int32 x)
	{
		m_xColumnUpdate->updateInt(x);
	}

	void DataColumn::updateLong(sal_Int64 x)
	{
		m_xColumnUpdate->updateLong(x);
	}

	void DataColumn::updateFloat(float x)
	{
		m_xColumnUpdate->updateFloat(x);
	}

	void DataColumn::updateDouble(double x)
	{
		m_xColumnUpdate->updateDouble(x);
	}

	void DataColumn::updateString(const ::rtl::OUString& x)
	{
		m_xColumnUpdate->updateString(x);
	}

	void DataColumn::updateBytes(const Sequence< sal_Int8 >& x)
	{
		m_xColumnUpdate->updateBytes(x);
	}

	void DataColumn::updateDate(const com::sun::star::util::Date& x)
	{
		m_xColumnUpdate->updateDate(x);
	}

	void DataColumn::updateTime(const com::sun::star::util::Time& x)
	{
		m_xColumnUpdate->updateTime(x);
	}

	void DataColumn::updateTimestamp(const com::sun::star::util::DateTime& x)
	{
		m_xColumnUpdate->updateTimestamp(x);
	}

	void DataColumn::updateBinaryStream(const Reference< XInputStream>& x, sal_Int32 length)
	{
		m_xColumnUpdate->updateBinaryStream(x, length);
	}

	void DataColumn::updateCharacterStream(const Reference< XInputStream>& x, sal_Int32 length)
	{
		m_xColumnUpdate->updateCharacterStream(x, length);
	}

	void DataColumn::updateObject(const Any& x)
	{
		m_xColumnUpdate->updateObject(x);
	}

	void DataColumn::updateNumericObject(const Any& x, sal_Int32 scale)
	{
		m_xColumnUpdate->updateNumericObject(x, scale);
	}

	//..............................................................................
}	// namespace svxform
//..............................................................................
