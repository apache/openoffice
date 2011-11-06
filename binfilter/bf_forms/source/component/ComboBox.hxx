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



#ifndef _FORMS_COMBOBOX_HXX_
#define _FORMS_COMBOBOX_HXX_

#ifndef _FORMS_FORMCOMPONENT_HXX_
#include "FormComponent.hxx"
#endif

#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATTER_HPP_
#include <com/sun/star/util/XNumberFormatter.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_LISTSOURCETYPE_HPP_
#include <com/sun/star/form/ListSourceType.hpp>
#endif
#ifndef FORMS_ERRORBROADCASTER_HXX
#include "errorbroadcaster.hxx"
#endif
namespace binfilter {

//.........................................................................
namespace frm
{

//==================================================================
// OComboBoxModel
//==================================================================
class OComboBoxModel
			:public OBoundControlModel
			,public OErrorBroadcaster
			,public ::comphelper::OAggregationArrayUsageHelper< OComboBoxModel >
{
	::com::sun::star::uno::Any				m_aBoundColumn; 		// obsolet
	::rtl::OUString 						m_aListSource;			//
	::rtl::OUString 						m_aDefaultText; 		// DefaultText
	::rtl::OUString 						m_aSaveValue;

	StringSequence							m_aDesignModeStringItems;
		// upon loading, in some cases we reset fill our string item list ourself. We don't want
		// to lose the user's items then, so we remember them here.

	
	::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter> m_xFormatter;

	::com::sun::star::form::ListSourceType	m_eListSourceType;		// type der list source
	::com::sun::star::util::Date			m_aNullDate;
	sal_Int32								m_nFormatKey;
	sal_Int16								m_nFieldType;
	sal_Int16								m_nKeyType;
	sal_Bool								m_bEmptyIsNull; 		// LeerString wird als NULL interpretiert

	

	static sal_Int32		nTextHandle;

protected:
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>	_getTypes();

public:
	DECLARE_DEFAULT_LEAF_XTOR( OComboBoxModel );

	virtual void SAL_CALL disposing();

	// OPropertySetHelper
	virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle) const;
	virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
				throw (::com::sun::star::uno::Exception);
	virtual sal_Bool SAL_CALL convertFastPropertyValue(
				::com::sun::star::uno::Any& _rConvertedValue, ::com::sun::star::uno::Any& _rOldValue, sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue )
				throw (::com::sun::star::lang::IllegalArgumentException);

	virtual void SAL_CALL reloaded( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);

	// XServiceInfo
	IMPLEMENTATION_NAME(OComboBoxModel);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);

	// UNO
	DECLARE_UNO3_AGG_DEFAULTS(OComboBoxModel, OBoundControlModel);
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw (::com::sun::star::uno::RuntimeException);

	// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw(::com::sun::star::uno::RuntimeException);
	virtual cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

	// XPersistObject
	virtual ::rtl::OUString SAL_CALL	getServiceName() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL
		write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL
		read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

	// OAggregationArrayUsageHelper
	virtual void fillProperties(
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps,
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
		) const;
	IMPLEMENT_INFO_SERVICE()

protected:
	void loadData();

	DECLARE_XCLONEABLE();
};

//==================================================================
// OComboBoxControl
//==================================================================

class OComboBoxControl : public OBoundControl
{
public:
	OComboBoxControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);

	// XServiceInfo
	IMPLEMENTATION_NAME(OComboBoxControl);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
};

//.........................................................................
}
//.........................................................................

}//end of namespace binfilter
#endif // _FORMS_COMBOBOX_HXX_

