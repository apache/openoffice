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



#ifndef _SW_XPRINTPREVIEWSETTINGS_HXX_
#define _SW_XPRINTPREVIEWSETTINGS_HXX_

#ifdef PRECOMPILED
#endif
#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _COMPHELPER_CHAINABLEPROPERTYSET_HXX_
#include <comphelper/ChainablePropertySet.hxx>
#endif
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
namespace binfilter {

class SwXTextDocument;
class SwDocShell;
class SwDoc;
class SwPagePreViewPrtData;

class SwXPrintPreviewSettings :public comphelper::ChainablePropertySet,
						public cppu::OWeakObject,
						public ::com::sun::star::lang::XServiceInfo
{
	friend class SwXDocumentSettings;
protected:
	sal_Bool 		mbPreviewDataChanged;
	SwDoc*			mpDoc;
	SwPagePreViewPrtData *mpPreViewData;
	const SwPagePreViewPrtData *mpConstPreViewData;

	virtual void _preSetValues ()
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
	virtual void _setSingleValue( const comphelper::PropertyInfo & rInfo, const ::com::sun::star::uno::Any &rValue ) 
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
	virtual void _postSetValues ()
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );

	virtual void _preGetValues ()
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
	virtual void _getSingleValue( const comphelper::PropertyInfo & rInfo, ::com::sun::star::uno::Any & rValue ) 
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException );
	virtual void _postGetValues ()
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
	virtual ~SwXPrintPreviewSettings()
		throw();
public:
	SwXPrintPreviewSettings(SwDoc *pDoc);
	

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) 
		throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL acquire(  ) 
		throw ();
	virtual void SAL_CALL release(  ) 
		throw ();

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) 
		throw( ::com::sun::star::uno::RuntimeException );
	virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName) 
		throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) 
		throw( ::com::sun::star::uno::RuntimeException );
};
} //namespace binfilter
#endif
