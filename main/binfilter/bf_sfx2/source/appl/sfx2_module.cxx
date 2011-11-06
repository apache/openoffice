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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <stdio.h>
#include <tools/rcid.h>

#include <cstdarg>
#include "app.hxx"
#include "arrdecl.hxx"
#include "docfac.hxx"
#include "module.hxx"

namespace binfilter {

static SfxModuleArr_Impl* pModules=0;

/*N*/ TYPEINIT1(SfxModule, SfxShell);

/*?*/ BOOL SfxModule::QueryUnload()
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001
/*?*/ }

/*?*/ SfxModule* SfxModule::Load()
/*?*/ {
/*?*/ 	return 0;
/*?*/ }

/*?*/ void SfxModule::Free()
/*?*/ {
/*?*/ }


/*N*/ ResMgr* SfxModule::GetResMgr()
/*N*/ {
/*N*/ 	return pResMgr;
/*N*/ }

//====================================================================
/*N*/ SfxModule::SfxModule( ResMgr* pMgrP, BOOL bDummyP,
/*N*/ 					  SfxObjectFactory* pFactoryP, ... )
/*N*/ 	: pResMgr( pMgrP ), bDummy( bDummyP )
/*N*/ {
/*N*/ 	if( !bDummy )
/*N*/ 	{
/*N*/ 		SfxApplication *pApp = SFX_APP();
/*N*/       SfxModuleArr_Impl& rArr = GetModules_Impl();
/*N*/ 		SfxModule* pPtr = (SfxModule*)this;
/*N*/ 		rArr.C40_INSERT( SfxModule, pPtr, rArr.Count() );
/*N*/ 		SetPool( &pApp->GetPool() );
/*N*/ 	}
/*N*/ 	std::va_list pVarArgs;
/*N*/ 	va_start( pVarArgs, pFactoryP );
/*N*/ 	for ( SfxObjectFactory *pArg = pFactoryP; pArg;
/*N*/ 		 pArg = va_arg( pVarArgs, SfxObjectFactory* ) )
/*N*/ 		pArg->SetModule_Impl( this );
/*N*/ 	va_end(pVarArgs);
/*N*/ }

/*N*/ SfxModule::~SfxModule()
/*N*/ {
/*N*/ 	if( !bDummy )
/*N*/ 	{
/*N*/ 		if ( SFX_APP()->Get_Impl() )
/*N*/ 		{
/*N*/ 			// Das Modul wird noch vor dem DeInitialize zerst"ort, also auis dem Array entfernen
/*N*/           SfxModuleArr_Impl& rArr = GetModules_Impl();
/*N*/ 			for( USHORT nPos = rArr.Count(); nPos--; )
/*N*/ 			{
/*N*/ 				if( rArr[ nPos ] == this )
/*N*/ 				{
/*N*/ 					rArr.Remove( nPos );
/*N*/ 					break;
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/
/*N*/ 		delete pResMgr;
/*N*/ 	}
/*N*/ }

/*N*/ SfxModuleArr_Impl& SfxModule::GetModules_Impl()
/*N*/ {
/*N*/     if( !pModules )
/*N*/         pModules = new SfxModuleArr_Impl;
/*N*/     return *pModules;
/*N*/ };
}
