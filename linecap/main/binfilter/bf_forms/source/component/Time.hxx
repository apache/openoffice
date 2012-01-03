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



#ifndef _FORMS_TIME_HXX_
#define _FORMS_TIME_HXX_

#ifndef _FORMS_EDITBASE_HXX_
#include "EditBase.hxx"
#endif
#ifndef _FORMS_LIMITED_FORMATS_HXX_
#include "limitedformats.hxx"
#endif
namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................

//==================================================================
//= OTimeModel
//==================================================================
class OTimeModel
				:public OEditBaseModel
				,public OLimitedFormats
				,public ::comphelper::OAggregationArrayUsageHelper< OTimeModel >
{
	::com::sun::star::uno::Any		m_aSaveValue;
	sal_Bool			m_bDateTimeField;
	static sal_Int32	nTimeHandle;

protected:
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

public:
	DECLARE_DEFAULT_LEAF_XTOR( OTimeModel );

	// stario::XPersistObject
	virtual ::rtl::OUString SAL_CALL getServiceName() throw ( ::com::sun::star::uno::RuntimeException);

	// ::com::sun::star::beans::XPropertySet
	virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;
	virtual sal_Bool SAL_CALL convertFastPropertyValue(::com::sun::star::uno::Any& rConvertedValue, ::com::sun::star::uno::Any& rOldValue,
										  sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
										throw(::com::sun::star::lang::IllegalArgumentException);
	virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue) throw ( ::com::sun::star::uno::Exception);

	// ::com::sun::star::lang::XServiceInfo
	IMPLEMENTATION_NAME(OTimeModel);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

	// ::com::sun::star::beans::XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw(::com::sun::star::uno::RuntimeException);
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

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
//= OTimeControl
//==================================================================
class OTimeControl: public OBoundControl
{
protected:
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

public:
	OTimeControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);
	DECLARE_UNO3_AGG_DEFAULTS(OTimeControl, OBoundControl);

	// ::com::sun::star::lang::XServiceInfo
	IMPLEMENTATION_NAME(OTimeControl);
	virtual StringSequence SAL_CALL getSupportedServiceNames() throw();
};

//.........................................................................
}	// namespace frm
//.........................................................................

}//end of namespace binfilter
#endif // _FORMS_TIME_HXX_

