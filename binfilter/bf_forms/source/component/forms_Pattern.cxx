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

#ifndef _FORMS_PATTERN_HXX_
#include "Pattern.hxx"
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
// OPatternControl
//==================================================================
//------------------------------------------------------------------
OPatternControl::OPatternControl(const Reference<XMultiServiceFactory>& _rxFactory)
    :OBoundControl(_rxFactory, VCL_CONTROL_PATTERNFIELD)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL OPatternControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
    return *(new OPatternControl(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OPatternControl::_getTypes()
{
    return OBoundControl::_getTypes();
}

//------------------------------------------------------------------------------
StringSequence OPatternControl::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OBoundControl::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_PATTERNFIELD;
    return aSupported;
}

//==================================================================
// OPatternModel
//==================================================================
sal_Int32   OPatternModel::nTextHandle = -1;

//------------------------------------------------------------------
InterfaceRef SAL_CALL OPatternModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
    return *(new OPatternModel(_rxFactory));
}

//------------------------------------------------------------------------------
Sequence<Type> OPatternModel::_getTypes()
{
    return OEditBaseModel::_getTypes();
}

//------------------------------------------------------------------
DBG_NAME( OPatternModel )
//------------------------------------------------------------------
OPatternModel::OPatternModel(const Reference<XMultiServiceFactory>& _rxFactory)
    :OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_PATTERNFIELD, FRM_CONTROL_PATTERNFIELD)
                                    // use the old control name for compytibility reasons
{
	DBG_CTOR( OPatternModel, NULL );

    m_nClassId = FormComponentType::PATTERNFIELD;
    m_sDataFieldConnectivityProperty = PROPERTY_TEXT;
    if (OPatternModel::nTextHandle == -1)
        OPatternModel::nTextHandle = getOriginalHandle(PROPERTY_ID_TEXT);
}

//------------------------------------------------------------------
OPatternModel::OPatternModel( const OPatternModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
    :OEditBaseModel( _pOriginal, _rxFactory )
{
	DBG_CTOR( OPatternModel, NULL );
}

//------------------------------------------------------------------
OPatternModel::~OPatternModel()
{
	DBG_DTOR( OPatternModel, NULL );
}

// XCloneable
//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OPatternModel )

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL OPatternModel::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 2);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-2] = FRM_SUN_COMPONENT_DATABASE_PATTERNFIELD;
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_PATTERNFIELD;
    return aSupported;
}


//------------------------------------------------------------------------------
Reference<XPropertySetInfo> SAL_CALL OPatternModel::getPropertySetInfo() throw( RuntimeException )
{
    Reference<XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

//------------------------------------------------------------------------------
void OPatternModel::fillProperties(
        Sequence< Property >& _rProps,
        Sequence< Property >& _rAggregateProps ) const
{
    FRM_BEGIN_PROP_HELPER(11)
        // Text auf transient setzen
//      ModifyPropertyAttributes(_rAggregateProps, PROPERTY_TEXT, PropertyAttribute::TRANSIENT, 0);
        DECL_PROP1(NAME,            ::rtl::OUString,    BOUND);
        DECL_PROP2(CLASSID,         sal_Int16,          READONLY, TRANSIENT);
        DECL_PROP2(DEFAULT_TEXT,    ::rtl::OUString,    BOUND, MAYBEDEFAULT);
        DECL_BOOL_PROP1(EMPTY_IS_NULL,                  BOUND);
        DECL_PROP1(TAG,             ::rtl::OUString,    BOUND);
        DECL_PROP1(TABINDEX,        sal_Int16,          BOUND);
        DECL_PROP1(CONTROLSOURCE,   ::rtl::OUString,    BOUND);
        DECL_IFACE_PROP3(BOUNDFIELD,    XPropertySet,   BOUND,READONLY, TRANSIENT);
        DECL_PROP2(FILTERPROPOSAL,  sal_Bool,           BOUND, MAYBEDEFAULT);
        DECL_IFACE_PROP2(CONTROLLABEL,  XPropertySet,   BOUND, MAYBEVOID);
        DECL_PROP2(CONTROLSOURCEPROPERTY,   rtl::OUString,  READONLY, TRANSIENT);
    FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& OPatternModel::getInfoHelper()
{
    return *const_cast<OPatternModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OPatternModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
    return FRM_COMPONENT_PATTERNFIELD;  // old (non-sun) name for compatibility !
}

//.........................................................................
}   // namespace frm
//.........................................................................

}
