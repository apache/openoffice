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

#ifndef _FORMS_CURRENCY_HXX_
#include "Currency.hxx"
#endif

#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX 
#include <bf_svtools/syslocale.hxx>
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

namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................
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
// OCurrencyControl
//==================================================================
//------------------------------------------------------------------
OCurrencyControl::OCurrencyControl(const Reference<XMultiServiceFactory>& _rxFactory)
	:OBoundControl(_rxFactory, VCL_CONTROL_CURRENCYFIELD)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL OCurrencyControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new OCurrencyControl(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OCurrencyControl::_getTypes()
{
	return OBoundControl::_getTypes();
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL OCurrencyControl::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControl::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 1);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_CURRENCYFIELD;
	return aSupported;
}

//==================================================================
// OCurrencyModel
//==================================================================
sal_Int32	OCurrencyModel::nValueHandle = -1;

//------------------------------------------------------------------
InterfaceRef SAL_CALL OCurrencyModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
	return *(new OCurrencyModel(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OCurrencyModel::_getTypes()
{
	return OEditBaseModel::_getTypes();
}

//------------------------------------------------------------------
void OCurrencyModel::implConstruct()
{
	if (m_xAggregateSet.is())
	{
		try
		{
			// get the system international informations
			const LocaleDataWrapper& aLocaleInfo = SvtSysLocale().GetLocaleData();

			::rtl::OUString sCurrencySymbol;
			sal_Bool bPrependCurrencySymbol;
			switch ( aLocaleInfo.getCurrPositiveFormat() )
			{
				case 0:	// $1
					sCurrencySymbol = String(aLocaleInfo.getCurrSymbol());
					bPrependCurrencySymbol = sal_True;
					break;
				case 1:	// 1$
					sCurrencySymbol = String(aLocaleInfo.getCurrSymbol());
					bPrependCurrencySymbol = sal_False;
					break;
				case 2:	// $ 1
					sCurrencySymbol = ::rtl::OUString(String(aLocaleInfo.getCurrSymbol())) + ::rtl::OUString::createFromAscii(" ");
					bPrependCurrencySymbol = sal_True;
					break;
				case 3: // 1 $
					sCurrencySymbol = ::rtl::OUString::createFromAscii(" ") + ::rtl::OUString(String(aLocaleInfo.getCurrSymbol()));
					bPrependCurrencySymbol = sal_False;
					break;
			}
			if (sCurrencySymbol.getLength())
			{
				m_xAggregateSet->setPropertyValue(PROPERTY_CURRENCYSYMBOL, makeAny(sCurrencySymbol));
				m_xAggregateSet->setPropertyValue(PROPERTY_CURRSYM_POSITION, makeAny(bPrependCurrencySymbol));
			}
		}
		catch(Exception&)
		{
			DBG_ERROR( "OCurrencyModel::implConstruct: caught an exception while initializing the aggregate!" );
		}
	}
}

//------------------------------------------------------------------
DBG_NAME( OCurrencyModel )
//------------------------------------------------------------------
OCurrencyModel::OCurrencyModel(const Reference<XMultiServiceFactory>& _rxFactory)
	:OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_CURRENCYFIELD, FRM_CONTROL_CURRENCYFIELD)
									// use the old control name for compytibility reasons
{
	DBG_CTOR( OCurrencyModel, NULL );

	m_nClassId = FormComponentType::CURRENCYFIELD;
	m_sDataFieldConnectivityProperty = PROPERTY_VALUE;
	if (OCurrencyModel::nValueHandle == -1)
		OCurrencyModel::nValueHandle = getOriginalHandle(PROPERTY_ID_VALUE);

	implConstruct();
}

//------------------------------------------------------------------
OCurrencyModel::OCurrencyModel( const OCurrencyModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
	:OEditBaseModel( _pOriginal, _rxFactory )
{
	DBG_CTOR( OCurrencyModel, NULL );
	implConstruct();
}

//------------------------------------------------------------------
OCurrencyModel::~OCurrencyModel()
{
	DBG_DTOR( OCurrencyModel, NULL );
}

// XCloneable
//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OCurrencyModel )

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL OCurrencyModel::getSupportedServiceNames() throw()
{
	StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 2);

	::rtl::OUString*pArray = aSupported.getArray();
	pArray[aSupported.getLength()-2] = FRM_SUN_COMPONENT_DATABASE_CURRENCYFIELD;
	pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_CURRENCYFIELD;
	return aSupported;
}

//------------------------------------------------------------------------------
Reference<XPropertySetInfo> SAL_CALL OCurrencyModel::getPropertySetInfo() throw( RuntimeException )
{
	Reference<XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
	return xInfo;
}

//------------------------------------------------------------------------------
void OCurrencyModel::fillProperties(
		Sequence< Property >& _rProps,
		Sequence< Property >& _rAggregateProps ) const
{
	FRM_BEGIN_PROP_HELPER(9)
		// Value auf transient setzen
//		ModifyPropertyAttributes(_rAggregateProps, PROPERTY_VALUE, PropertyAttribute::TRANSIENT, 0);

		DECL_PROP2(CLASSID,		sal_Int16,					READONLY, TRANSIENT);
		DECL_PROP3(DEFAULT_VALUE,		double,				BOUND, MAYBEDEFAULT, MAYBEVOID);
		DECL_PROP1(NAME,		::rtl::OUString,			BOUND);
		DECL_PROP1(TAG,		::rtl::OUString,				BOUND);
		DECL_PROP1(TABINDEX,		sal_Int16,				BOUND);
		DECL_PROP1(CONTROLSOURCE,		::rtl::OUString,	BOUND);
		DECL_IFACE_PROP3(BOUNDFIELD,		XPropertySet,	BOUND,READONLY, TRANSIENT);
		DECL_IFACE_PROP2(CONTROLLABEL,		XPropertySet,	BOUND, MAYBEVOID);
		DECL_PROP2(CONTROLSOURCEPROPERTY,	rtl::OUString,	READONLY, TRANSIENT);
	FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& OCurrencyModel::getInfoHelper()
{
	return *const_cast<OCurrencyModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OCurrencyModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
	return FRM_COMPONENT_CURRENCYFIELD;	// old (non-sun) name for compatibility !
}

//.........................................................................
}	// namespace frm
//.........................................................................

}
