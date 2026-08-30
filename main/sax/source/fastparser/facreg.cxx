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

#include <cppuhelper/factory.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implementationentry.hxx>

#include "../tools/fastserializer.hxx"
#include "fastparser.hxx"

using namespace sax_fastparser;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::registry;
using ::rtl::OUString;
using namespace ::com::sun::star::lang;

namespace sax_fastparser
{

//--------------------------------------
// the extern interface
//---------------------------------------
Reference< XInterface > SAL_CALL FastSaxParser_CreateInstance( const Reference< XComponentContext >  & )
{
	FastSaxParser *p = new FastSaxParser;
	return Reference< XInterface > ( (OWeakObject * ) p );
}

Reference< XInterface > SAL_CALL FastSaxSerializer_CreateInstance( const Reference< XComponentContext >  & )
{
	FastSaxSerializer *p = new FastSaxSerializer;
	return Reference< XInterface > ( (OWeakObject * ) p );
}

struct ::cppu::ImplementationEntry g_component_entries[] =
{
	{
		FastSaxParser_CreateInstance,
		FastSaxParser::getImplementationName_Static,
		FastSaxParser::getSupportedServiceNames_Static,
		::cppu::createSingleComponentFactory,
		0,
		0
	},
	{
		FastSaxSerializer_CreateInstance,
		FastSaxSerializer::getImplementationName_Static,
		FastSaxSerializer::getSupportedServiceNames_Static,
		::cppu::createSingleComponentFactory,
		0,
		0
	},
	{ 0, 0, 0, 0, 0, 0 }
};

}

extern "C"
{

SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment(
	const sal_Char ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory( const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
	return ::cppu::component_getFactoryHelper( pImplName, pServiceManager, pRegistryKey, g_component_entries );
}


}
