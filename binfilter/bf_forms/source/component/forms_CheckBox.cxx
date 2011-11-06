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



#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#include <comphelper/proparrhlp.hxx>
#endif

#ifndef _FORMS_CHECKBOX_HXX_
#include "CheckBox.hxx"
#endif
#ifndef _FRM_PROPERTY_HRC_
#include "property.hrc"
#endif
#ifndef _FRM_SERVICES_HXX_
#include "services.hxx"
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif

namespace binfilter {

//.........................................................................
namespace frm
{
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
//using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;

//==================================================================
//= OCheckBoxControl
//==================================================================

//------------------------------------------------------------------
OCheckBoxControl::OCheckBoxControl(const Reference<XMultiServiceFactory>& _rxFactory)
	:OBoundControl(_rxFactory, VCL_CONTROL_CHECKBOX)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL OCheckBoxControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory) throw (RuntimeException)
{
	return *(new OCheckBoxControl(_rxFactory));
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL	OCheckBoxControl::getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException)
{
	StringSequence aSupported = OBoundControl::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 1);

	::rtl::OUString* pArray = aSupported.getArray();
	pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_CHECKBOX;
	return aSupported;
}

//==================================================================
//= OCheckBoxModel
//==================================================================

//==================================================================
InterfaceRef SAL_CALL OCheckBoxModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory) throw (RuntimeException)
{
	return *(new OCheckBoxModel(_rxFactory));
}

//------------------------------------------------------------------
DBG_NAME( OCheckBoxModel )
//------------------------------------------------------------------
OCheckBoxModel::OCheckBoxModel(const Reference<XMultiServiceFactory>& _rxFactory)
	:OBoundControlModel(_rxFactory, VCL_CONTROLMODEL_CHECKBOX, FRM_CONTROL_CHECKBOX, sal_False, sal_False)
					// use the old control name for compytibility reasons
	,OPropertyChangeListener(m_aMutex)
	,m_bInReset(sal_False)
	,m_pAggregatePropertyMultiplexer(NULL)
{
	DBG_CTOR( OCheckBoxModel, NULL );
	implConstruct();

	m_nClassId = FormComponentType::CHECKBOX;
	m_nDefaultChecked = CB_NOCHECK;
	m_sDataFieldConnectivityProperty = PROPERTY_STATE;
}

//------------------------------------------------------------------
OCheckBoxModel::OCheckBoxModel( const OCheckBoxModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
	:OBoundControlModel( _pOriginal, _rxFactory, sal_False, sal_False )
	,OPropertyChangeListener( m_aMutex )
	,m_bInReset( sal_False )
	,m_pAggregatePropertyMultiplexer( NULL )
{
	DBG_CTOR( OCheckBoxModel, NULL );
	implConstruct();

	m_nDefaultChecked = _pOriginal->m_nDefaultChecked;
	m_sReferenceValue = _pOriginal->m_sReferenceValue;
}

//------------------------------------------------------------------------------
void OCheckBoxModel::implConstruct()
{
	increment( m_refCount );
	if ( m_xAggregateSet.is() )
	{
		m_pAggregatePropertyMultiplexer = new OPropertyChangeMultiplexer( this, m_xAggregateSet, sal_False );
		m_pAggregatePropertyMultiplexer->acquire();
		m_pAggregatePropertyMultiplexer->addProperty( PROPERTY_STATE );
	}
	decrement( m_refCount );

	doSetDelegator();
}

//------------------------------------------------------------------------------
OCheckBoxModel::~OCheckBoxModel()
{
	doResetDelegator();

	if (m_pAggregatePropertyMultiplexer)
	{
		m_pAggregatePropertyMultiplexer->dispose();
		m_pAggregatePropertyMultiplexer->release();
		m_pAggregatePropertyMultiplexer = NULL;
	}
	DBG_DTOR( OCheckBoxModel, NULL );
}

//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OCheckBoxModel )

//------------------------------------------------------------------------------
void SAL_CALL OCheckBoxModel::disposing()
{
	if (m_pAggregatePropertyMultiplexer)
		m_pAggregatePropertyMultiplexer->dispose();

	OBoundControlModel::disposing();
}

//------------------------------------------------------------------------------
void OCheckBoxModel::_propertyChanged(const PropertyChangeEvent& _rEvent) throw(RuntimeException)
{
    OSL_ENSURE( false, "OCheckBoxModel::_propertyChanged: dead code!?" );
}


// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL	OCheckBoxModel::getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException)
{
	StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 2);

	::rtl::OUString* pArray = aSupported.getArray();
	pArray[aSupported.getLength()-2] = FRM_SUN_COMPONENT_DATABASE_CHECKBOX;
	pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_CHECKBOX;
	return aSupported;
}

//------------------------------------------------------------------------------
void OCheckBoxModel::getFastPropertyValue(Any& _rValue, sal_Int32 _nHandle) const
{
	switch (_nHandle)
	{
		case PROPERTY_ID_REFVALUE: _rValue <<= m_sReferenceValue; break;
		case PROPERTY_ID_DEFAULTCHECKED : _rValue <<= m_nDefaultChecked; break;
		default:
			OBoundControlModel::getFastPropertyValue(_rValue, _nHandle);
	}
}

//------------------------------------------------------------------------------
void OCheckBoxModel::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue) throw (com::sun::star::uno::Exception)
{
	switch (_nHandle)
	{
		case PROPERTY_ID_REFVALUE :
			DBG_ASSERT(_rValue.getValueType().getTypeClass() == TypeClass_STRING, "OCheckBoxModel::setFastPropertyValue_NoBroadcast : invalid type !" );
			_rValue >>= m_sReferenceValue;
			break;

		case PROPERTY_ID_DEFAULTCHECKED :
			DBG_ASSERT(_rValue.getValueType().getTypeClass() == TypeClass_SHORT, "OCheckBoxModel::setFastPropertyValue_NoBroadcast : invalid type !" );
			_rValue >>= m_nDefaultChecked;
			//_reset();
			break;

		default:
			OBoundControlModel::setFastPropertyValue_NoBroadcast(_nHandle, _rValue);
	}
}

//------------------------------------------------------------------------------
sal_Bool OCheckBoxModel::convertFastPropertyValue(
			Any& _rConvertedValue, Any& _rOldValue, sal_Int32 _nHandle, const Any& _rValue)
			throw (IllegalArgumentException)
{
	sal_Bool bModified(sal_False);
	switch (_nHandle)
	{
		case PROPERTY_ID_REFVALUE :
			bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_sReferenceValue);
			break;
		case PROPERTY_ID_DEFAULTCHECKED :
			bModified = tryPropertyValue(_rConvertedValue, _rOldValue, _rValue, m_nDefaultChecked);
			break;
		default:
			bModified = OBoundControlModel::convertFastPropertyValue(_rConvertedValue, _rOldValue, _nHandle, _rValue);
			break;
	}
	return bModified;
}

//------------------------------------------------------------------------------
Reference<XPropertySetInfo> SAL_CALL OCheckBoxModel::getPropertySetInfo() throw(RuntimeException)
{
	Reference<XPropertySetInfo> xInfo( createPropertySetInfo( getInfoHelper() ) );
	return xInfo;
}

//------------------------------------------------------------------------------
cppu::IPropertyArrayHelper& OCheckBoxModel::getInfoHelper()
{
	return *const_cast<OCheckBoxModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void OCheckBoxModel::fillProperties(
		Sequence< Property >& _rProps,
		Sequence< Property >& _rAggregateProps ) const
{
	FRM_BEGIN_PROP_HELPER(10)
		// the "State" property is transient, so change this
//		ModifyPropertyAttributes(_rAggregateProps, PROPERTY_STATE, PropertyAttribute::TRANSIENT, 0);

		DECL_PROP2(CLASSID,			sal_Int16,			READONLY, TRANSIENT);
		DECL_PROP1(REFVALUE,		::rtl::OUString,	BOUND);
		DECL_PROP1(DEFAULTCHECKED,	sal_Int16,			BOUND);
		DECL_PROP1(NAME,			rtl::OUString,		BOUND);
		DECL_PROP1(TAG,				rtl::OUString,		BOUND);
		DECL_PROP1(TABINDEX,		sal_Int16,			BOUND);
		DECL_PROP1(CONTROLSOURCE,	rtl::OUString,		BOUND);
		DECL_IFACE_PROP3(BOUNDFIELD,	XPropertySet,	BOUND,READONLY, TRANSIENT);
		DECL_IFACE_PROP2(CONTROLLABEL,	XPropertySet,	BOUND, MAYBEVOID);
		DECL_PROP2(CONTROLSOURCEPROPERTY,	rtl::OUString,	READONLY, TRANSIENT);
	FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OCheckBoxModel::getServiceName() throw(RuntimeException)
{
	return FRM_COMPONENT_CHECKBOX;	// old (non-sun) name for compatibility !
}

//------------------------------------------------------------------------------
void SAL_CALL OCheckBoxModel::write(const Reference<stario::XObjectOutputStream>& _rxOutStream)
	throw(stario::IOException, RuntimeException)
{
	OBoundControlModel::write(_rxOutStream);

	// Version
	_rxOutStream->writeShort(0x0003);
	// Properties
	_rxOutStream << m_sReferenceValue;
	_rxOutStream << (sal_Int16)m_nDefaultChecked;
	writeHelpTextCompatibly(_rxOutStream);
	// from version 0x0003 : common properties
	writeCommonProperties(_rxOutStream);
}

//------------------------------------------------------------------------------
void SAL_CALL OCheckBoxModel::read(const Reference<stario::XObjectInputStream>& _rxInStream) throw(stario::IOException, RuntimeException)
{
	OBoundControlModel::read(_rxInStream);
	osl::MutexGuard aGuard(m_aMutex);

	// Version
	sal_uInt16 nVersion = _rxInStream->readShort();

	switch (nVersion)
	{
		case 0x0001 : _rxInStream >> m_sReferenceValue; m_nDefaultChecked = _rxInStream->readShort(); break;
		case 0x0002 :
			_rxInStream >> m_sReferenceValue;
			_rxInStream >> m_nDefaultChecked;
			readHelpTextCompatibly(_rxInStream);
			break;
		case 0x0003 :
			_rxInStream >> m_sReferenceValue;
			_rxInStream >> m_nDefaultChecked;
			readHelpTextCompatibly(_rxInStream);
			readCommonProperties(_rxInStream);
			break;
		default :
			DBG_ERROR("OCheckBoxModel::read : unknown version !");
			m_sReferenceValue = ::rtl::OUString();
			m_nDefaultChecked = 0;
			defaultCommonProperties();
			break;
	}

//	// Nach dem Lesen die Defaultwerte anzeigen
//	if (m_aControlSource.getLength())
//		// (not if we don't have a control source - the "State" property acts like it is persistent, then
//		_reset();
}

//------------------------------------------------------------------------------
Any OCheckBoxModel::_getControlValue() const
{
	return m_xAggregateSet->getPropertyValue(PROPERTY_STATE);
}

//-----------------------------------------------------------------------------
void OCheckBoxModel::reset(void) throw (com::sun::star::uno::RuntimeException)
{
	m_bInReset = sal_True;
	OBoundControlModel::reset();
	m_bInReset = sal_False;
}

//.........................................................................
}
//.........................................................................

}
