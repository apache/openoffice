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



#ifndef __FRAMEWORK_HELPER_PROPERTYSETCONTAINER_HXX_
#define __FRAMEWORK_HELPER_PROPERTYSETCONTAINER_HXX_

/** Attention: stl headers must(!) be included at first. Otherwise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <threadhelp/threadhelpbase.hxx>
#include <framework/fwedllapi.h>

namespace framework
{

class FWE_DLLPUBLIC PropertySetContainer : public com::sun::star::container::XIndexContainer	,
							 public ThreadHelpBase								,	// Struct for right initialization of mutex member! Must be first of baseclasses.
							 public ::cppu::OWeakObject
{
	public:
		PropertySetContainer( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& );
		virtual ~PropertySetContainer();

		// XInterface
		virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType )
			throw (::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL acquire() throw ();
		virtual void SAL_CALL release() throw ();

		// XIndexContainer
		virtual void SAL_CALL insertByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element )
			throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL removeByIndex( sal_Int32 Index )
			throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XIndexReplace
		virtual void SAL_CALL replaceByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element )
			throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XIndexAccess
		virtual sal_Int32 SAL_CALL getCount()
			throw (::com::sun::star::uno::RuntimeException);

		virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index )
			throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType()
			throw (::com::sun::star::uno::RuntimeException)
		{
			return ::getCppuType((com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >*)0);
		}

		virtual sal_Bool SAL_CALL hasElements()
			throw (::com::sun::star::uno::RuntimeException);

	protected:
		com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >	m_xServiceManager;

	private:
		typedef std::vector< com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet > > PropertySetVector;
		PropertySetVector																m_aPropertySetVector;

};

}

#endif // __FRAMEWORK_CLASSES_PROPERTYSETCONTAINER_HXX_
