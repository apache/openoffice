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

#ifndef _SVX_SVXIDS_HRC //autogen
#include <bf_svx/svxids.hrc>
#endif

#ifndef DOCUMENT_HXX
#include <document.hxx>
#endif

#ifndef _STARMATH_HRC
#include <starmath.hrc>
#endif
namespace binfilter {

BOOL SmDLL::bInitialized = FALSE;

/*************************************************************************
|*
|* Initialisierung
|*
\************************************************************************/
/*N*/ void SmDLL::Init()
/*N*/ {
/*N*/     if ( bInitialized )
/*N*/         return;
/*N*/ 
/*N*/     bInitialized = TRUE;
/*N*/ 
/*N*/ 	// called directly after loading the DLL
/*N*/ 	// do whatever you want, you may use Sd-DLL too
/*N*/ 
/*N*/ 	// the SdModule must be created
/*N*/ 
/*N*/ 	SmModuleDummy** ppShlPtr = (SmModuleDummy**) GetAppData(BF_SHL_SM);
/*N*/ 
/*N*/     SvFactory* pFact = PTR_CAST(SvFactory,(*ppShlPtr)->pSmDocShellFactory);
/*N*/     delete (*ppShlPtr);
/*N*/     (*ppShlPtr) = new SmModule(pFact);
/*N*/     (*ppShlPtr)->pSmDocShellFactory = pFact;
/*N*/ 
/*N*/ 	String aResDll(C2S("sm"));
/*N*/ 
/*N*/ 	SfxModule *p = SM_MOD1();
/*N*/ 	SmModule *pp = (SmModule *) p;
/*N*/ 
}

/*************************************************************************
|*
|* Deinitialisierung
|*
\************************************************************************/
/*N*/ void SmDLL::Exit()
/*N*/ {
/*N*/ 	// the SdModule must be destroyed
/*N*/ 	SmModuleDummy** ppShlPtr = (SmModuleDummy**) GetAppData(BF_SHL_SM);
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	(*ppShlPtr) = NULL;
/*N*/ 
/*N*/ 	*GetAppData(BF_SHL_SM) = 0;
/*N*/ }


}
