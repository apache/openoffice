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


#ifndef _SFXMODULE_HXX
#define _SFXMODULE_HXX	// intern
#define _SFXMOD_HXX		// extern

#ifndef _SFX_SHELL_HXX
#include <bf_sfx2/shell.hxx>
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

class ResMgr;

namespace binfilter {

class SfxObjectFactory;
class SfxObjectFactory;
class SfxModuleArr_Impl;
class SfxModule_Impl;
class SfxSlotPool;

class ISfxModule
{
public:
	virtual					~ISfxModule(){};
	virtual ResMgr*			GetResMgr() = 0;
};

//====================================================================

class SfxModule : public SfxShell, public ISfxModule
{
private:
	ResMgr*                     pResMgr;
    sal_Bool                    bDummy : 1;

protected:
	virtual sal_Bool                QueryUnload();

public:
								TYPEINFO();

								SfxModule( ResMgr* pMgrP, sal_Bool bDummy,
									SfxObjectFactory* pFactoryP, ... );
								~SfxModule();

	virtual	SfxModule*			Load();
	virtual void            	Free();
	sal_Bool						IsLoaded() const { return !bDummy; }

	virtual ResMgr*             GetResMgr();

#if _SOLAR__PRIVATE
    static SfxModuleArr_Impl&   GetModules_Impl();
#endif
};

#define SFX_IMPL_MODULE_LIBRARY( LibName )									\
																			\
		extern "C" void _CDECLARE_ Init##LibName##Dll()                     \
		{																	\
			LibName##DLL::Init();											\
		}																	\
		extern "C" void _CDECLARE_ DeInit##LibName##Dll()                   \
		{																	\
			LibName##DLL::Exit();											\
		}

}//end of namespace binfilter
#endif

