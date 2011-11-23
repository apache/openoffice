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


#ifndef _BASMGR_HXX //autogen
#include "bf_basic/basmgr.hxx"
#endif

#ifdef WIN
#define _TL_LANG_SPECIAL
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#include "appdata.hxx"
#include "plugobj.hxx"
#include "arrdecl.hxx"
#include "macrconf.hxx"

#include <bf_svtools/imgdef.hxx>

#include "frameobj.hxx"
#include "docfile.hxx"
#include "sfxtypes.hxx"
#include "appimp.hxx"
#include "fcontnr.hxx"
#include "nochaos.hxx"
#include "doctempl.hxx"
#include "dlgcont.hxx"
#include "scriptcont.hxx"
#include "misccfg.hxx"
#include "docfac.hxx"

namespace binfilter {

DECLARE_LIST( SfxFrameWindowFactoryArray_Impl, SfxFrameWindowFactory* )
SV_DECL_PTRARR(SfxInitLinkList, Link*, 2, 2)//STRIP008 ;

//-------------------------------------------------------------------------

/*?*/ void SfxApplication::Deinitialize()
/*?*/ {
/*?*/     if ( bDowning )
/*?*/         return;
/*?*/ 
/*?*/ 	StarBASIC::Stop();
/*?*/ 
/*?*/ 	bDowning = TRUE; // wegen Timer aus DecAliveCount und QueryExit
/*?*/ 
/*?*/ 	bDowning = FALSE;
/*?*/ 	DBG_ASSERT( !SfxObjectShell::GetFirst(),
/*?*/ 				"existing SfxObjectShell after Execute" );
/*?*/ 	bDowning = TRUE;
/*?*/ 
/*?*/ 	// call derived application-exit
/*?*/ 	bInExit = TRUE;
/*?*/ 	Exit();
/*?*/ 
/*?*/     // Controller u."a. freigeben
/*?*/     // dabei sollten auch restliche Komponenten ( Beamer! ) verschwinden
/*?*/ 	SfxObjectFactory::ClearAll_Impl();
//      pBasMgr = NULL;
/*?*/ 	if( pImp->pBasicLibContainer )
/*?*/ 		pImp->pBasicLibContainer->release();
/*?*/ 	if( pImp->pDialogLibContainer )
/*?*/ 		pImp->pDialogLibContainer->release();
/*?*/ 
/*?*/ 	bInExit = FALSE;
/*?*/ 
/*?*/ 	// ab hier d"urfen keine SvObjects mehr existieren
/*?*/ 	DELETEX(pAppData_Impl->pMatcher);
/*?*/ 	DELETEX(pAppData_Impl->pSfxFrameObjectFactoryPtr);
/*?*/ 
/*?*/ 	DELETEX(pAppData_Impl->pEventConfig);
/*?*/   DELETEX(pAppData_Impl->pMiscConfig);
/*?*/ 	SfxMacroConfig::Release_Impl();
/*?*/ 	DELETEX(pAppData_Impl->pInitLinkList);
/*?*/ 
/*?*/     DELETEX(pImp->pEventHdl);
/*?*/     DELETEX(pImp->pObjShells);
/*?*/ 
/*?*/ 	NoChaos::ReleaseItemPool();
/*?*/ 	pAppData_Impl->pPool = NULL;
/*?*/ }
}
