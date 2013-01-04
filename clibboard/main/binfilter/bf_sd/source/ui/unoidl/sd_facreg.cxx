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



#include <string.h>


#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include <cppuhelper/factory.hxx>
#include <uno/lbnames.h>
namespace binfilter {

using namespace rtl;
using namespace ::com::sun::star;

extern uno::Reference< uno::XInterface > SAL_CALL SdDrawingDocument_createInstance( const uno::Reference< lang::XMultiServiceFactory > & _rxFactory );
extern OUString SdDrawingDocument_getImplementationName() throw( uno::RuntimeException );
extern uno::Sequence< OUString > SAL_CALL SdDrawingDocument_getSupportedServiceNames() throw( uno::RuntimeException );

extern uno::Reference< uno::XInterface > SAL_CALL SdPresentationDocument_createInstance( const uno::Reference< lang::XMultiServiceFactory > & _rxFactory );
extern OUString SdPresentationDocument_getImplementationName() throw( uno::RuntimeException );
extern uno::Sequence< OUString > SAL_CALL SdPresentationDocument_getSupportedServiceNames() throw( uno::RuntimeException );


#ifdef __cplusplus
extern "C"
{
#endif

void SAL_CALL component_getImplementationEnvironment( const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

void * SAL_CALL component_getFactory( const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
	void * pRet = 0;

	if( pServiceManager )
	{
		uno::Reference< lang::XMultiServiceFactory > xMSF( reinterpret_cast< lang::XMultiServiceFactory * >( pServiceManager ) );

		uno::Reference< lang::XSingleServiceFactory > xFactory;

		const sal_Int32 nImplNameLen = strlen( pImplName );
		if(0)//STRIP001 if( SdHtmlOptionsDialog_getImplementationName().equalsAsciiL( pImplName, nImplNameLen ) )
		{
		}
		else if( SdDrawingDocument_getImplementationName().equalsAsciiL( pImplName, nImplNameLen ) )
		{
			xFactory = ::cppu::createSingleFactory( xMSF,
				SdDrawingDocument_getImplementationName(),
				SdDrawingDocument_createInstance,
				SdDrawingDocument_getSupportedServiceNames() );
		}
		else if( SdPresentationDocument_getImplementationName().equalsAsciiL( pImplName, nImplNameLen ) )
		{
			xFactory = ::cppu::createSingleFactory( xMSF,
				SdPresentationDocument_getImplementationName(),
				SdPresentationDocument_createInstance,
				SdPresentationDocument_getSupportedServiceNames() );
		}


		if( xFactory.is())
		{
			xFactory->acquire();
			pRet = xFactory.get();
		}
	}

	return pRet;
}

}
}
