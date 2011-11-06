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



// header for class Application
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
// header for class OGuard
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#include "ChXChartDataChangeEventListener.hxx"
#include "ChXChartDocument.hxx"
namespace binfilter {

#ifndef SCH_ASCII_TO_OU
#define SCH_ASCII_TO_OU( s )  ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( s ) )
#endif

// using namespace osl;
using namespace rtl;
using namespace vos;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star;

ChXChartDataChangeEventListener::ChXChartDataChangeEventListener()
{
	mpXDoc = NULL;
}

void ChXChartDataChangeEventListener::SetOwner( ChXChartDocument* pXDoc ) throw()
{
	OGuard aGuard( Application::GetSolarMutex() );

	mpXDoc = pXDoc;
}

void ChXChartDataChangeEventListener::Reset() throw()
{
	OGuard aGuard( Application::GetSolarMutex() );
	mpXDoc = NULL;
}

// XChartDataChangeEventListener
void SAL_CALL ChXChartDataChangeEventListener::chartDataChanged( const chart::ChartDataChangeEvent& aEvent )
	throw( uno::RuntimeException )
{
	OGuard aGuard( Application::GetSolarMutex() );
	if( mpXDoc )
		mpXDoc->RefreshData( aEvent );
}

// XEventListener
void SAL_CALL ChXChartDataChangeEventListener::disposing( const lang::EventObject& Source ) throw( uno::RuntimeException )
{
	Reset();
}

// XServiceInfo
::rtl::OUString SAL_CALL ChXChartDataChangeEventListener::getImplementationName() throw( uno::RuntimeException )
{
	return SCH_ASCII_TO_OU( "ChXChartDataChangeEventListener" );
}

sal_Bool SAL_CALL ChXChartDataChangeEventListener::supportsService( const OUString& ServiceName )
	throw( uno::RuntimeException )
{
	uno::Sequence< OUString > aSNL = getSupportedServiceNames();
	const OUString * pArray = aSNL.getConstArray();
	for( INT32 i = 0; i < aSNL.getLength(); i++ )
		if( pArray[i] == ServiceName )
			return TRUE;
	return FALSE;	
}

uno::Sequence< ::rtl::OUString > SAL_CALL ChXChartDataChangeEventListener::getSupportedServiceNames()
	throw( uno::RuntimeException )
{
	OGuard aGuard( Application::GetSolarMutex() );

	uno::Sequence< OUString > aServSeq( 0 );
	return aServSeq;
}

}
