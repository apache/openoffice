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
#include "precompiled_linguistic.hxx"


#include <cppuhelper/factory.hxx>	// helper for factories
#include <cppuhelper/implementationentry.hxx>
#include <rtl/string.hxx>

#include "lngsvcmgr.hxx"
#include "lngopt.hxx"
#include "dlistimp.hxx"
#include "convdiclist.hxx"
#include "gciterator.hxx"

#include <com/sun/star/registry/XRegistryKey.hpp>

using namespace com::sun::star::lang;

using namespace com::sun::star::registry;

using namespace com::sun::star;

extern uno::Reference< uno::XInterface > SAL_CALL LngSvcMgr_CreateInstance(
        const uno::Reference< uno::XComponentContext  > & /*rCtx*/ )
    throw(uno::Exception);

extern uno::Reference< uno::XInterface > SAL_CALL LinguProps_CreateInstance(
        const uno::Reference< uno::XComponentContext > & /*rCtx*/ )
    throw(uno::Exception);

extern uno::Reference< uno::XInterface > SAL_CALL DicList_CreateInstance(
        const uno::Reference< uno::XComponentContext > & /*rCtx*/ )
    throw(uno::Exception);

extern uno::Reference< uno::XInterface > SAL_CALL ConvDicList_CreateInstance(
        const uno::Reference< uno::XComponentContext > & /*rCtx*/ )
    throw(uno::Exception);

extern uno::Reference< uno::XInterface > SAL_CALL GrammarCheckingIterator_createInstance(
        const uno::Reference< uno::XComponentContext > & rxCtx )
    throw(uno::Exception);

////////////////////////////////////////
// definition of the two functions that are used to provide the services
//

struct ::cppu::ImplementationEntry g_component_entries [] =
{
    {
        LngSvcMgr_CreateInstance,
        LngSvcMgr::getImplementationName_Static,
        LngSvcMgr::getSupportedServiceNames_Static,
        ::cppu::createSingleComponentFactory,
        0,
        0
    },
    {
        LinguProps_CreateInstance,
        LinguProps::getImplementationName_Static,
        LinguProps::getSupportedServiceNames_Static,
        ::cppu::createSingleComponentFactory,
        0,
        0
    },
    {
        DicList_CreateInstance,
        DicList::getImplementationName_Static,
        DicList::getSupportedServiceNames_Static,
        ::cppu::createSingleComponentFactory,
        0,
        0
    },
    {
        ConvDicList_CreateInstance,
        ConvDicList::getImplementationName_Static,
        ConvDicList::getSupportedServiceNames_Static,
        ::cppu::createSingleComponentFactory,
        0,
        0
    },
    {
        GrammarCheckingIterator_createInstance,
        GrammarCheckingIterator::getImplementationName_Static,
        GrammarCheckingIterator::getSupportedServiceNames_Static,
        ::cppu::createSingleComponentFactory,
        0,
        0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{

SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
	const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    return ::cppu::component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey, g_component_entries );
}
}

///////////////////////////////////////////////////////////////////////////
