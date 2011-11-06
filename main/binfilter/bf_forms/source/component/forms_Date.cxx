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

#ifndef _FORMS_DATE_HXX_
#include "Date.hxx"
#endif

#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif

#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif


#ifndef _FRM_SERVICES_HXX_
#include "services.hxx"
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
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;

//------------------------------------------------------------------
ODateControl::ODateControl(const Reference<XMultiServiceFactory>& _rxFactory)
			   :OBoundControl(_rxFactory, VCL_CONTROL_DATEFIELD)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL ODateControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new ODateControl(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> ODateControl::_getTypes()
{
	return OBoundControl::_getTypes();
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL ODateControl::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControl::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 1);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_DATEFIELD;
	return aSupported;
}

/*************************************************************************/
sal_Int32	ODateModel::nDateHandle = -1;
//------------------------------------------------------------------
InterfaceRef SAL_CALL ODateModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new ODateModel(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> ODateModel::_getTypes()
{
	return OEditBaseModel::_getTypes();
}

//------------------------------------------------------------------
DBG_NAME( ODateModel )
//------------------------------------------------------------------
ODateModel::ODateModel(const Reference<XMultiServiceFactory>& _rxFactory)
			:OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_DATEFIELD, FRM_CONTROL_DATEFIELD )
						// use the old control name for compytibility reasons
			,OLimitedFormats(_rxFactory, FormComponentType::DATEFIELD)
{
	DBG_CTOR( ODateModel, NULL );

	m_nClassId = FormComponentType::DATEFIELD;
	m_sDataFieldConnectivityProperty = PROPERTY_DATE;
	if (ODateModel::nDateHandle == -1)
		ODateModel::nDateHandle = getOriginalHandle(PROPERTY_ID_DATE);

	setAggregateSet(m_xAggregateFastSet, getOriginalHandle(PROPERTY_ID_DATEFORMAT));
}

//------------------------------------------------------------------------------
ODateModel::ODateModel( const ODateModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
	:OEditBaseModel( _pOriginal, _rxFactory )
	,OLimitedFormats( _rxFactory, FormComponentType::DATEFIELD )
{
	DBG_CTOR( ODateModel, NULL );

	setAggregateSet( m_xAggregateFastSet, getOriginalHandle( PROPERTY_ID_DATEFORMAT ) );
}

//------------------------------------------------------------------------------
ODateModel::~ODateModel( )
{
	setAggregateSet(Reference< XFastPropertySet >(), -1);
	DBG_DTOR( ODateModel, NULL );
}

// XCloneable
//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( ODateModel )

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL ODateModel::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 2);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-2] = FRM_SUN_COMPONENT_DATABASE_DATEFIELD;
	pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_DATEFIELD;
	return aSupported;
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL ODateModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
	return FRM_COMPONENT_DATEFIELD;	// old (non-sun) name for compatibility !
}

// XPropertySet
//------------------------------------------------------------------------------
Reference<XPropertySetInfo> SAL_CALL ODateModel::getPropertySetInfo() throw( RuntimeException )
{
	Reference<XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
	return xInfo;
}

//------------------------------------------------------------------------------
void ODateModel::fillProperties(
		Sequence< Property >& _rProps,
		Sequence< Property >& _rAggregateProps ) const
{
	FRM_BEGIN_PROP_HELPER(11)
		DECL_PROP1(NAME,					::rtl::OUString,		BOUND);
		DECL_PROP2(CLASSID,					sal_Int16,				READONLY, TRANSIENT);
		DECL_PROP3(DEFAULT_DATE,			sal_Int32,				BOUND, MAYBEDEFAULT, MAYBEVOID);
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
::cppu::IPropertyArrayHelper& ODateModel::getInfoHelper()
{
	return *const_cast<ODateModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void SAL_CALL ODateModel::getFastPropertyValue(Any& _rValue, sal_Int32 _nHandle ) const
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
sal_Bool SAL_CALL ODateModel::convertFastPropertyValue(Any& _rConvertedValue, Any& _rOldValue,
		sal_Int32 _nHandle, const Any& _rValue ) throw(IllegalArgumentException)
{
	if (PROPERTY_ID_FORMATKEY == _nHandle)
		return convertFormatKeyPropertyValue(_rConvertedValue, _rOldValue, _rValue);
	else
		return OEditBaseModel::convertFastPropertyValue(_rConvertedValue, _rOldValue, _nHandle, _rValue );
}

//------------------------------------------------------------------------------
void SAL_CALL ODateModel::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue) throw ( ::com::sun::star::uno::Exception)
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
