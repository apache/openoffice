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



#ifndef SMDLL0_HXX
#define SMDLL0_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_so3/pseudo.hxx>

#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif
#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
class SvFactory; 
class SotFactory; 

namespace binfilter {

/*************************************************************************
|*
|* define dummy-version only if not in Sd-DLL
|*
\************************************************************************/

#define SM_MOD() ( *(SmModuleDummy**) GetAppData(BF_SHL_SM) )

/*************************************************************************
|*
|* This tricky class keeps pointers to the SvFactories while
|* the DLL isn`t loaded. A pointer to the one instance is available
|* through SD_MOD() (shared-lib-app-data).
|*
\************************************************************************/

class SmModuleDummy : public SfxModule
{
public:
	TYPEINFO();

				// SvFactory name convention:
				// 'p' + SfxObjectShell-subclass + 'Factory'
	SotFactory *pSmDocShellFactory;

	SmModuleDummy(ResMgr	*pResMgr,
				  BOOL		bDummy,
				  SotFactory *pObjFact) :
	   SfxModule(pResMgr, bDummy, (SfxObjectFactory*) pObjFact, NULL),
	   pSmDocShellFactory(pObjFact)
	{
	}

	virtual SfxModule *Load ();

	static const SvGlobalName GetID(USHORT nFileFormat);
	static USHORT HasID (const SvGlobalName& rID);
};

} //namespace binfilter
#endif
