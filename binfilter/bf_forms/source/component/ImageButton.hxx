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



#ifndef _FRM_IMAGE_BUTTON_HXX_
#define _FRM_IMAGE_BUTTON_HXX_

#ifndef _FRM_IMAGE_HXX_
#include "Image.hxx"
#endif

#ifndef _COM_SUN_STAR_AWT_XMOUSELISTENER_HPP_
#include <com/sun/star/awt/XMouseListener.hpp>
#endif
namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................

//==================================================================
// OImageButtonModel
//==================================================================
class OImageButtonModel
		:public OImageModel
		,public ::comphelper::OAggregationArrayUsageHelper< OImageButtonModel >
{
public:
	DECLARE_DEFAULT_LEAF_XTOR( OImageButtonModel );

// ::com::sun::star::lang::XServiceInfo
	IMPLEMENTATION_NAME(OImageButtonModel);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

// ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw(::com::sun::star::uno::RuntimeException);
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

// ::com::sun::star::io::XPersistObject
	virtual ::rtl::OUString SAL_CALL getServiceName() throw ( ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

// OAggregationArrayUsageHelper
	virtual void fillProperties(
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps,
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
		) const;
	IMPLEMENT_INFO_SERVICE()

protected:
	DECLARE_XCLONEABLE();
};

//==================================================================
// OImageButtonControl
//==================================================================
typedef ::cppu::ImplHelper1< ::com::sun::star::awt::XMouseListener> OImageButtonControl_BASE;
class OImageButtonControl : public OImageControl,
							public OImageButtonControl_BASE
{
protected:
	// UNO Anbindung
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

public:
	OImageButtonControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);

// ::com::sun::star::lang::XServiceInfo
	IMPLEMENTATION_NAME(OImageButtonControl);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

	// UNO Anbindung
	DECLARE_UNO3_AGG_DEFAULTS(OImageButtonControl, OImageControl);
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType) throw(::com::sun::star::uno::RuntimeException);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& _rSource) throw(::com::sun::star::uno::RuntimeException)
		{ OControl::disposing(_rSource); }

// ::com::sun::star::awt::XMouseListener
	virtual void SAL_CALL mousePressed(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL mouseReleased(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException) { }
	virtual void SAL_CALL mouseEntered(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException) { }
	virtual void SAL_CALL mouseExited(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException) { }
};

//.........................................................................
}	// namespace frm
//.........................................................................

}//end of namespace binfilter
#endif // _FRM_IMAGE_BUTTON_HXX_

