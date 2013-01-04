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



// MARKER(update_precomp.py): autogen include statement, do not remove


#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

namespace css = com::sun::star;
using css::uno::Reference;
using css::uno::Sequence;
using rtl::OUString;

// -------------------------------------------------------------------------------------

namespace binfilter
{

#define DECLARE_CREATEINSTANCE( ImplName ) \
	Reference< css::uno::XInterface > SAL_CALL ImplName##_CreateInstance( const Reference< css::lang::XMultiServiceFactory >& );

DECLARE_CREATEINSTANCE( SvNumberFormatterServiceObj )
DECLARE_CREATEINSTANCE( SvNumberFormatsSupplierServiceObject )

}

// -------------------------------------------------------------------------------------

extern "C"
{

SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment (
	const sal_Char ** ppEnvTypeName, uno_Environment ** /* ppEnv */)
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT void* SAL_CALL component_getFactory (
	const sal_Char * pImplementationName, void * _pServiceManager, void * /* _pRegistryKey*/)
{
	void * pResult = 0;
	if ( _pServiceManager )
	{
		Reference< css::lang::XSingleServiceFactory > xFactory;
		if (rtl_str_compare(
				pImplementationName,
				"com.sun.star.uno.util.numbers.SvNumberFormatsSupplierServiceObject") == 0)
		{
			Sequence< OUString > aServiceNames(1);
			aServiceNames.getArray()[0] =
				OUString::createFromAscii( "com.sun.star.util.NumberFormatsSupplier" );

            xFactory = ::cppu::createSingleFactory(
				reinterpret_cast< css::lang::XMultiServiceFactory* >(_pServiceManager),
				OUString::createFromAscii( pImplementationName ),
				::binfilter::SvNumberFormatsSupplierServiceObject_CreateInstance,
				aServiceNames);
		}
		else if (rtl_str_compare(
					 pImplementationName,
					 "com.sun.star.uno.util.numbers.SvNumberFormatterServiceObject") == 0)
		{
			Sequence< OUString > aServiceNames(1);
			aServiceNames.getArray()[0] =
				OUString::createFromAscii( "com.sun.star.util.NumberFormatter" );

            xFactory = ::cppu::createSingleFactory(
				reinterpret_cast< css::lang::XMultiServiceFactory* >(_pServiceManager),
				OUString::createFromAscii( pImplementationName ),
				::binfilter::SvNumberFormatterServiceObj_CreateInstance,
				aServiceNames);
		}
		if ( xFactory.is() )
		{
			xFactory->acquire();
			pResult = xFactory.get();
		}
	}
	return pResult;
}

}	// "C"

