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


#ifndef _MULTIPRO_HXX
#define _MULTIPRO_HXX

#ifndef _COM_SUN_STAR_FORM_XFORMCOMPONENT_HPP_
#include <com/sun/star/form/XFormComponent.hpp>
#endif
#ifndef _COMPHELPER_COMPOSEDPROPS_HXX_
#include <comphelper/composedprops.hxx>
#endif
#ifndef _SVDMARK_HXX //autogen
#include <svdmark.hxx>
#endif
namespace binfilter {

//.........................................................................
namespace svxform
{
//.........................................................................

	//==================================================================
	// FmXMultiSet
	//==================================================================
	typedef ::comphelper::OComposedPropertySet	FmXMultiSet_Base;
	class FmXMultiSet
		:public FmXMultiSet_Base
		,public ::com::sun::star::form::XFormComponent
	{
	protected:
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >	m_xParent;

	public:
		FmXMultiSet(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> > & _rPropertySets);

		// XInterface
		virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL acquire(  ) throw();
		virtual void SAL_CALL release(  ) throw();

		// XChild
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  ) throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent ) throw(::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException);

		static FmXMultiSet* Create(const SdrMarkList& rMarkList);
	};

//.........................................................................
}	// namespace svxform
//.........................................................................

}//end of namespace binfilter
#endif

