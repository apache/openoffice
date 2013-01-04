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



#ifndef _BF_WRAPPER_HXX
#include <bf_wrapper.hxx>
#endif

#include <bf_offmgr/app.hxx>
#include <bf_sw/swdll.hxx>
#include <bf_sc/scdll.hxx>
#include <bf_sd/sddll.hxx>
#include <bf_sch/schdll.hxx>
#include <bf_starmath/smdll.hxx>
#include <bf_svx/svdetc.hxx>
#include <bf_svx/itemdata.hxx> //STRIP002
#include <framework/imageproducer.hxx>

#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <bf_svtools/moduleoptions.hxx>
#endif

// #i30187#
#include <vos/mutex.hxx>

namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;

static SwDLL*				pSwDLL = 0L;
static SdDLL*				pSdDLL = 0L;
static ScDLL*				pScDLL = 0L;
static SchDLL*				pSchDLL = 0L;
static SmDLL*				pSmDLL = 0L;
static bf_OfficeWrapper*	pOfficeWrapper = 0L;

SFX_IMPL_XSERVICEINFO( bf_OfficeWrapper, "com.sun.star.office.OfficeWrapper", "com.sun.star.comp.desktop.OfficeWrapper" )

Reference< XInterface >  SAL_CALL bf_OfficeWrapper_CreateInstance( const Reference< XMultiServiceFactory >  & rSMgr )
{
	static osl::Mutex aMutex;

	if ( 0L == pOfficeWrapper)
	{
		osl::MutexGuard guard( aMutex );

		if ( 0L == pOfficeWrapper )
		{
			// #i30187#
			::vos::OGuard aGuard( Application::GetSolarMutex() );

			return (XComponent*) ( new bf_OfficeWrapper( rSMgr ) );
		}
	}

	return (XComponent*)0;
}

//added by jmeng for i31251 begin
extern "C"{
	void legcy_setBinfilterInitState(void);
}
//added by jmeng for i31251 end
bf_OfficeWrapper::bf_OfficeWrapper( const Reference < XMultiServiceFactory >& xFactory )
:	aListeners( aMutex ),
    pApp( new OfficeApplication )
{
	SvtModuleOptions aMOpt;

	//	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
    {
        pSwDLL  = new SwDLL;
		SwDLL::LibInit();
    }

	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) || aMOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
    {
        pSdDLL  = new SdDLL;
		SdDLL::LibInit();
    }

	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
    {
        pScDLL  = new ScDLL;
		ScDLL::LibInit();
    }

	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SCHART ) )
	{
        pSchDLL = new SchDLL;
		SchDLL::LibInit();
	}

	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SMATH ) )
	{
        pSmDLL = new SmDLL;
		SmDLL::LibInit();
	}
	//added by jmeng for i31251 begin
	legcy_setBinfilterInitState();
	//added by jmeng for i31251 end
}

void SAL_CALL bf_OfficeWrapper::initialize( const Sequence< Any >& aArguments ) throw( Exception )
{
}

bf_OfficeWrapper::~bf_OfficeWrapper()
{
	{
		// all ConfigItems must be destroyed before destroying the SfxApp
		SvtModuleOptions aMOpt;

        if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SCHART ) )
		{
			SchDLL::LibExit();
			DELETEZ( pSchDLL );
		}

		if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SMATH ) )
		{
			SmDLL::LibExit();
			DELETEZ( pSmDLL );
		}

		//	if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
		{
			SwDLL::LibExit();
			DELETEZ( pSwDLL );
		}

		if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) || aMOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
		{
			SdDLL::LibExit();
			DELETEZ( pSdDLL );
		}

		if ( aMOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
		{
			ScDLL::PreExit();           // der Teil vom Exit der noch Svx etc. braucht
			ScDLL::LibExit();
			DELETEZ( pScDLL );
		}
    }

    delete pApp;
    delete &GetSdrGlobalData(); // ??????????

	(*(SdrGlobalData**)GetAppData(BF_SHL_SVD))=0;
	(*(SvxGlobalItemData**)GetAppData(BF_SHL_ITEM))=0; 

	SotData_Impl * pSotData = SOTDATA();
	SotFactory * pFact = pSotData->pFactoryList->First();

	while( pFact )
    {
        pFact = pSotData->pFactoryList->Next();
    }
}

void SAL_CALL bf_OfficeWrapper::dispose() throw ( RuntimeException )
{
    EventObject aObject;
    aObject.Source = (XComponent*)this;
    aListeners.disposeAndClear( aObject );
}

void SAL_CALL bf_OfficeWrapper::addEventListener( const Reference< XEventListener > & aListener) throw ( RuntimeException )
{
    aListeners.addInterface( aListener );
}

void SAL_CALL bf_OfficeWrapper::removeEventListener( const Reference< XEventListener > & aListener ) throw ( RuntimeException )
{
    aListeners.removeInterface( aListener );
}

extern "C"
{

void SAL_CALL component_getImplementationEnvironment(	
	const sal_Char** ppEnvironmentTypeName,
	uno_Environment** ppEnvironment)
{
	*ppEnvironmentTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

void* SAL_CALL component_getFactory(	
	const sal_Char* pImplementationName,
	void* pServiceManager,
	void* pRegistryKey)
{
	// Set default return value for this operation - if it failed.
	void* pReturn = NULL;

	if ( pImplementationName && pServiceManager )
	{
		// Define variables which are used in following macros.
        Reference< XSingleServiceFactory > xFactory;
        Reference< XMultiServiceFactory > xServiceManager( reinterpret_cast< XMultiServiceFactory* >( pServiceManager ) );

        if ( bf_OfficeWrapper::impl_getStaticImplementationName().compareToAscii( pImplementationName ) == COMPARE_EQUAL )
        {
			xFactory = Reference< XSingleServiceFactory >( 
				cppu::createOneInstanceFactory( xServiceManager, bf_OfficeWrapper::impl_getStaticImplementationName(),
				bf_OfficeWrapper_CreateInstance, bf_OfficeWrapper::impl_getStaticSupportedServiceNames() ) );
        }

		// Factory is valid - service was found.
		if ( xFactory.is() )
		{
			xFactory->acquire();
			pReturn = xFactory.get();
		}
	}

	// Return with result of this operation.
	return pReturn;
}

} // extern "C"

// eof
}
