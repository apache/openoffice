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

#include <bf_sfx2/docfac.hxx>


#include "schmod.hxx"

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star;

::rtl::OUString SAL_CALL SchDocument_getImplementationName() throw()
{
	return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Chart.ChartDocument" ) );
}

uno::Sequence< ::rtl::OUString > SAL_CALL SchDocument_getSupportedServiceNames() throw()
{
	uno::Sequence< ::rtl::OUString > aSeq( 3 );
	aSeq[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart.ChartDocument" ) );
	aSeq[1] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart.ChartTableAddressSupplier" ) );
	aSeq[2] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.xml.UserDefinedAttributeSupplier" ) );

	return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SchDocument_createInstance(
				const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

	// to create the service the SW_MOD should be already initialized
	DBG_ASSERT( SCH_MOD1(), "No StarChart module!" );
	
	if ( SCH_MOD1() )
	{
		::rtl::OUString aFactoryURL( RTL_CONSTASCII_USTRINGPARAM ( "private:factory/schart" ) );
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
