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

#ifndef _LEGACYBINFILTERSMGR_HXX
#define _LEGACYBINFILTERSMGR_HXX

#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include "cppuhelper/factory.hxx"
namespace legacy_binfilters
{

::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > const & SAL_CALL getLegacyProcessServiceFactory();

} //namespace legacy_binfilters

extern "C"
{
//==================================================================================================
void SAL_CALL legacy_component_getImplementationEnvironment(
    sal_Char const ** ppEnvTypeName, uno_Environment ** ppEnv );
//==================================================================================================
sal_Bool SAL_CALL legacysmgr_component_writeInfo(
	::com::sun::star::lang::XMultiServiceFactory * smgr, ::com::sun::star::registry::XRegistryKey * key );
void * SAL_CALL legacysmgr_component_getFactory(
	sal_Char const * implName, ::com::sun::star::lang::XMultiServiceFactory * smgr, ::com::sun::star::registry::XRegistryKey * key );
}
#endif
