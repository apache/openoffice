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

#ifndef _FORMS_FIXEDTEXT_HXX_
#include "FixedText.hxx"
#endif
#ifndef _FRM_SERVICES_HXX_
#include "services.hxx"
#endif
#ifndef _FRM_PROPERTY_HRC_
#include "property.hrc"
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
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

//------------------------------------------------------------------------------
InterfaceRef SAL_CALL OFixedTextModel_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory) throw (RuntimeException)
{
	return *(new OFixedTextModel(_rxFactory));
}

//------------------------------------------------------------------
DBG_NAME( OFixedTextModel )
//------------------------------------------------------------------
OFixedTextModel::OFixedTextModel( const Reference<XMultiServiceFactory>& _rxFactory )
		:OControlModel(_rxFactory, VCL_CONTROLMODEL_FIXEDTEXT)

{
	DBG_CTOR( OFixedTextModel, NULL );
	m_nClassId = FormComponentType::FIXEDTEXT;
}

//------------------------------------------------------------------
OFixedTextModel::OFixedTextModel( const OFixedTextModel* _pOriginal, const Reference<XMultiServiceFactory>& _rxFactory )
	:OControlModel( _pOriginal, _rxFactory )

{
	DBG_CTOR( OFixedTextModel, NULL );
}

//------------------------------------------------------------------
OFixedTextModel::~OFixedTextModel( )
{
	DBG_DTOR( OFixedTextModel, NULL );
}

//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OFixedTextModel )

//------------------------------------------------------------------------------
StringSequence SAL_CALL OFixedTextModel::getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException)
{
	StringSequence aSupported = OControlModel::getSupportedServiceNames();
	aSupported.realloc(aSupported.getLength() + 1);

	::rtl::OUString* pArray = aSupported.getArray();
	pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_FIXEDTEXT;
	return aSupported;
}

//------------------------------------------------------------------------------
Reference<starbeans::XPropertySetInfo> SAL_CALL OFixedTextModel::getPropertySetInfo() throw(RuntimeException)
{
	Reference<starbeans::XPropertySetInfo> xInfo(createPropertySetInfo(getInfoHelper()));
	return xInfo;
}

//------------------------------------------------------------------------------
cppu::IPropertyArrayHelper& OFixedTextModel::getInfoHelper()
{
	return *const_cast<OFixedTextModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void OFixedTextModel::fillProperties(
		Sequence< starbeans::Property >& _rProps,
		Sequence< starbeans::Property >& _rAggregateProps ) const
{
	FRM_BEGIN_PROP_HELPER(3)
		RemoveProperty(_rAggregateProps, PROPERTY_TABSTOP);

		DECL_PROP1(NAME,	rtl::OUString,	BOUND);
		DECL_PROP2(CLASSID,	sal_Int16,	READONLY, TRANSIENT);
		DECL_PROP1(TAG,		rtl::OUString,	BOUND);
	FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OFixedTextModel::getServiceName() throw(RuntimeException)
{
	return FRM_COMPONENT_FIXEDTEXT;	// old (non-sun) name for compatibility !
}

//------------------------------------------------------------------------------
void SAL_CALL OFixedTextModel::write(const Reference<XObjectOutputStream>& _rxOutStream)
	throw(IOException, RuntimeException)
{
	OControlModel::write(_rxOutStream);

	// Version
	_rxOutStream->writeShort(0x0002);
	writeHelpTextCompatibly(_rxOutStream);
}

//------------------------------------------------------------------------------
void SAL_CALL OFixedTextModel::read(const Reference<XObjectInputStream>& _rxInStream) throw(IOException, RuntimeException)
{
	OControlModel::read(_rxInStream);

	// Version
	sal_Int16 nVersion = _rxInStream->readShort();
	if (nVersion > 1)
		readHelpTextCompatibly(_rxInStream);
}

//.........................................................................
}
//.........................................................................

}
