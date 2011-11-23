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



// System - Includes -----------------------------------------------------

#include <tools/string.hxx>
#include <bf_sfx2/docfac.hxx>

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#include "smmod.hxx"

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star;

::rtl::OUString SAL_CALL SmDocument_getImplementationName() throw()
{
	return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.FormulaDocument" ) );
}

uno::Sequence< ::rtl::OUString > SAL_CALL SmDocument_getSupportedServiceNames() throw()
{
	uno::Sequence< ::rtl::OUString > aSeq( 1 );
	aSeq[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.formula.FormulaProperties" ));
	return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmDocument_createInstance(
				const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

	// to create the service the SW_MOD should be already initialized
	DBG_ASSERT( SM_MOD(), "No StarMath module!" );
	
	if ( SM_MOD() )
	{
		::rtl::OUString aFactoryURL( RTL_CONSTASCII_USTRINGPARAM ( "private:factory/smath" ) );
		const SfxObjectFactory* pFactory = SfxObjectFactory::GetFactory( aFactoryURL );
		if ( pFactory )
		{
			SfxObjectShell* pShell = pFactory->CreateObject();
			if( pShell )
				return uno::Reference< uno::XInterface >( pShell->GetModel() );
		}
	}

	return uno::Reference< uno::XInterface >();
}


}
