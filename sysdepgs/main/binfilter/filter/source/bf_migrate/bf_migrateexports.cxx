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


#include <stdio.h>

#include <osl/thread.h>


#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>
#endif

#ifndef _BF_MIGRATEFILTER_HXX
#include <bf_migratefilter.hxx>
#endif

#include <bf_sfx2/objuno.hxx>

namespace binfilter {

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;

extern "C"
{

//==================================================================================================
void SAL_CALL component_getImplementationEnvironment(const sal_Char** ppEnvTypeName, uno_Environment** ppEnv)
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

//==================================================================================================
void* SAL_CALL component_getFactory(const sal_Char* pImplName, void* pServiceManager, void* pRegistryKey)
{
	void* pRet = 0;

    OUString implName = OUString::createFromAscii(pImplName);

	if(pServiceManager && implName.equals(bf_MigrateFilter_getImplementationName()))
	{
		Reference< XSingleServiceFactory > xFactory(createSingleFactory(
			reinterpret_cast< XMultiServiceFactory * >(pServiceManager),
			OUString::createFromAscii(pImplName),
			bf_MigrateFilter_createInstance, bf_MigrateFilter_getSupportedServiceNames()));

		if(xFactory.is())
		{
			xFactory->acquire();
			pRet = xFactory.get();
		}

		// init LegacyServiceFactory
		legacysmgr_component_getFactory(
			pImplName,
			reinterpret_cast< XMultiServiceFactory *>(pServiceManager),
			reinterpret_cast<XRegistryKey*> (pRegistryKey) );
	}
    else if(pServiceManager && implName.equals(SfxStandaloneDocumentInfoObject::impl_getStaticImplementationName()))
	{
		Reference< XSingleServiceFactory > xFactory(createSingleFactory(
			reinterpret_cast< XMultiServiceFactory * >(pServiceManager),
			OUString::createFromAscii(pImplName),
            bf_BinaryDocInfo_createInstance, binfilter::SfxStandaloneDocumentInfoObject::impl_getStaticSupportedServiceNames()));

		if(xFactory.is())
		{
			xFactory->acquire();
			pRet = xFactory.get();
		}

		// init LegacyServiceFactory
		legacysmgr_component_getFactory(
			pImplName,
			reinterpret_cast< XMultiServiceFactory *>(pServiceManager),
			reinterpret_cast<XRegistryKey*> (pRegistryKey) );
	}

	return pRet;
}
}

// eof
}
