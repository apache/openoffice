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

#ifndef _FORMS_TIME_HXX_
#include "Time.hxx"
#endif

#ifndef _TOOLS_TIME_HXX
#include <tools/time.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif

#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _FRM_SERVICES_HXX_
#include "services.hxx"
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif

namespace binfilter {

using namespace dbtools;

//.........................................................................
namespace frm
{
//.........................................................................
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;

//==================================================================
//= 
//==================================================================

//==================================================================
//= OTimeControl
//==================================================================
//------------------------------------------------------------------
OTimeControl::OTimeControl(const Reference<XMultiServiceFactory>& _rxFactory)
			   :OBoundControl(_rxFactory, VCL_CONTROL_TIMEFIELD)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL OTimeControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new OTimeControl(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OTimeControl::_getTypes()
{
	return OBoundControl::_getTypes();
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL OTimeControl::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControl::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 1);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_TIMEFIELD;
	return aSupported;
}

//==================================================================
//= OTimeModel
//==================================================================
sal_Int32	OTimeModel::nTimeHandle = -1;

//------------------------------------------------------------------
InterfaceRef SAL_CALL OTimeModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new OTimeModel(_rxFactory));
}

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL OTimeModel::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 2);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-2] = FRM_SUN_COMPONENT_DATABASE_TIMEFIELD;
	pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_TIMEFIELD;
	return aSupported;
}

//------------------------------------------------------------------------------
Sequence<Type> OTimeModel::_getTypes()
{
	return OBoundControlModel::_getTypes();
}

//------------------------------------------------------------------
DBG_NAME( OTimeModel )
//------------------------------------------------------------------
OTimeModel::OTimeModel(const Reference<XMultiServiceFactory>& _rxFactory)
			:OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_TIMEFIELD, FRM_CONTROL_TIMEFIELD )
									// use the old control name for compytibility reasons
			,OLimitedFormats(_rxFactory, FormComponentType::TIMEFIELD)
{
	DBG_CTOR( OTimeModel, NULL );

	m_nClassId = FormComponentType::TIMEFIELD;
	m_sDataFieldConnectivityProperty = PROPERTY_TIME;
	if (OTimeModel::nTimeHandle == -1)
		OTimeModel::nTimeHandle = getOriginalHandle(PROPERTY_ID_TIME);

	setAggregateSet(m_xAggregateFastSet, getOriginalHandle(PROPERTY_ID_TIMEFORMAT));
}

//------------------------------------------------------------------------------
OTimeModel::OTimeModel( const OTimeModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
	:OEditBaseModel( _pOriginal, _rxFactory )
	,OLimitedFormats( _rxFactory, FormComponentType::TIMEFIELD )
{
	DBG_CTOR( OTimeModel, NULL );

	setAggregateSet( m_xAggregateFastSet, getOriginalHandle( PROPERTY_ID_TIMEFORMAT ) );
}

//------------------------------------------------------------------------------
OTimeModel::~OTimeModel( )
{
	setAggregateSet(Reference< XFastPropertySet >(), -1);
	DBG_DTOR( OTimeModel, NULL );
}

// XCloneable
//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OTimeModel )

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OTimeModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
	return FRM_COMPONENT_TIMEFIELD;	// old (non-sun) name for compatibility !
}

// XPropertySet
//------------------------------------------------------------------------------
Reference<XPropertySetInfo> SAL_CALL OTimeModel::getPropertySetInfo() throw( RuntimeException )
{
	Reference<XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
	return xInfo;
}

//------------------------------------------------------------------------------
void OTimeModel::fillProperties(
		Sequence< Property >& _rProps,
		Sequence< Property >& _rAggregateProps ) const
{
	FRM_BEGIN_PROP_HELPER(11)
		DECL_PROP1(NAME,					::rtl::OUString,		BOUND);
		DECL_PROP2(CLASSID,					sal_Int16,				READONLY, TRANSIENT);
		DECL_PROP3(DEFAULT_TIME,			sal_Int32,				BOUND, MAYBEDEFAULT, MAYBEVOID);
		DECL_PROP1(TAG,						::rtl::OUString,		BOUND);
		DECL_PROP1(TABINDEX,				sal_Int16,				BOUND);
		DECL_PROP1(CONTROLSOURCE,			::rtl::OUString,		BOUND);
		DECL_IFACE_PROP3(BOUNDFIELD,		XPropertySet,			BOUND,READONLY, TRANSIENT);
		DECL_IFACE_PROP2(CONTROLLABEL,		XPropertySet,			BOUND, MAYBEVOID);
		DECL_PROP2(CONTROLSOURCEPROPERTY,	::rtl::OUString,		READONLY, TRANSIENT);
		DECL_PROP1(FORMATKEY,				sal_Int32,				TRANSIENT);
		DECL_IFACE_PROP2(FORMATSSUPPLIER,	XNumberFormatsSupplier,	READONLY, TRANSIENT);
	FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& OTimeModel::getInfoHelper()
{
	return *const_cast<OTimeModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void SAL_CALL OTimeModel::getFastPropertyValue(Any& _rValue, sal_Int32 _nHandle ) const
{
	switch (_nHandle)
	{
		case PROPERTY_ID_FORMATKEY:
			getFormatKeyPropertyValue(_rValue);
			break;
		case PROPERTY_ID_FORMATSSUPPLIER:
			_rValue <<= getFormatsSupplier();
			break;
		default:
			OEditBaseModel::getFastPropertyValue(_rValue, _nHandle);
			break;
	}
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL OTimeModel::convertFastPropertyValue(Any& _rConvertedValue, Any& _rOldValue,
		sal_Int32 _nHandle, const Any& _rValue ) throw(IllegalArgumentException)
{
	if (PROPERTY_ID_FORMATKEY == _nHandle)
		return convertFormatKeyPropertyValue(_rConvertedValue, _rOldValue, _rValue);
	else
		return OEditBaseModel::convertFastPropertyValue(_rConvertedValue, _rOldValue, _nHandle, _rValue );
}

//------------------------------------------------------------------------------
void SAL_CALL OTimeModel::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue) throw ( ::com::sun::star::uno::Exception)
{
	if (PROPERTY_ID_FORMATKEY == _nHandle)
		setFormatKeyPropertyValue(_rValue);
	else
		OEditBaseModel::setFastPropertyValue_NoBroadcast(_nHandle, _rValue);
}

//.........................................................................
}	// namespace frm
//.........................................................................

}
