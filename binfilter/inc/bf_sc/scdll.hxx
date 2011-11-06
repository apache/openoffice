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



#ifndef SC_SCDLL_HXX
#define SC_SCDLL_HXX

#include <bf_svtools/bf_solar.h>

class ResMgr;
class SvFactory; 
class SotFactory;
class StatusBar;
class SfxMedium;
class SfxFilter;

#ifndef _SFXDEFS_HXX //autogen
#include <bf_sfx2/sfxdefs.hxx>
#endif
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif
#ifndef _SFX_OBJFAC_HXX
#include <bf_sfx2/docfac.hxx>
#endif
namespace binfilter {

//-------------------------------------------------------------------------

class ScDLL

/*	[Description]

	This class is a wrapper for a Load-On-Demand-DLL. One instance
	per SfxApplication will be created for the runtime of
	SfxApplication-subclass::Main().

	Remember: Do export this class! It is used by the application.
*/

{
public:
					// Ctor/Dtor must be linked to the application
					ScDLL();
					~ScDLL();

					// static-init/exit-code must be linked to the application
	static void 	LibInit();	// called from SfxApplication-subclass::Init()
	static void 	LibExit();	// called from SfxApplication-subclass::Exit()
	static void 	PreExit();	// muss vor LibExit gerufen werden

					// DLL-init/exit-code must be linked to the DLL only
	static void 	Init(); 	// called directly after loading the DLL
	static void 	Exit(); 	// called directly befor unloading the DLL


	static ULONG	DetectFilter( SfxMedium& rMedium, const SfxFilter** ppFilter,
									SfxFilterFlags nMust, SfxFilterFlags nDont );
};

//-------------------------------------------------------------------------

class ScModuleDummy: public SfxModule

/*	[Description]

	This tricky class keeps pointers to the SvFactories while
	the DLL isn`t loaded. A pointer to the one instance is available
	through SXX_MOD() (shared-lib-app-data).
*/

{
public:
					// SvFactory name convention:
					// 'p' + SfxObjectShell-subclass + 'Factory'
	SotFactory* 	pScDocShellFactory;

					ScModuleDummy( ResMgr *pResMgr, BOOL bDummy, SfxObjectFactory* pFact )
					:	SfxModule(pResMgr, bDummy, pFact, NULL),
						pScDocShellFactory(pFact)
					{}

	virtual SfxModule* Load();
};

//-------------------------------------------------------------------------

#define SC_DLL() ( *(ScModuleDummy**) GetAppData(BF_SHL_CALC) )

} //namespace binfilter
#endif

