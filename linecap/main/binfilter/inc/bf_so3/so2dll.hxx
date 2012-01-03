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



#ifndef _SO2DLL_HXX
#define _SO2DLL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _PSTM_HXX //autogen
#include <tools/pstm.hxx>
#endif
#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _GLOBNAME_HXX //autogen
#include <tools/globname.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

/*************************************************************************
*************************************************************************/
class SotFactory;
class SotFactoryList;
class SvAdviseSinkIndex;
class WorkWindow;
class MDIWindow;
class ImpSO2Clipboard;
class ResMgr;
class SvSoIPCServer;
class SvIPCServerInfo;
class Impl_DeathTimer;
class Timer;

namespace binfilter
{
class SvInPlaceClient;
class SvInPlaceClientList;
class SvInPlaceObjectList;
class ImplSvEditObjectProtocol;
class SvContainerEnvironmentList;
class SvInPlaceEnvironment;
class SvVerbList;
struct SvBindingData;
class SvObjectList;
class ImpFactory;

//=========================================================================
struct SO3_DLLPRIVATE ConvertTo_Impl
/*  [Beschreibung]

	In dieser Struktur werden Informationen "uber den Vorg"anger und den
	Nachfolger von Datenformaten und dazugeh"origen CLSId's gepflegt.
	Die Struktur kann nur in einem Array sinnvoll eingesetzt werden.
*/
{
	SvGlobalName    aName;          // CLSID    des Servers
	SvGlobalName    aSvName;        // Starview-Klasse die diese CLSID supported
	long            aFormat;        // Format

	ConvertTo_Impl() {};
	ConvertTo_Impl( const SvGlobalName & a1, long a2 )
		: aName( a1 )
		, aSvName( a1 )
		, aFormat( a2 )
	{}
	ConvertTo_Impl( const SvGlobalName & a1,
					const SvGlobalName & rSvClass,
					long a2 )
		: aName( a1 )
		, aSvName( rSvClass )
		, aFormat( a2 )
	{}
};
#define SO3_OFFICE_VERSIONS	5

class SO3_DLLPUBLIC SoDll
{
friend class ImpFactory;
friend class SvFactory;
friend class Impl_DeathTimer;
	BOOL            bInit;
	BOOL            bSelfInit;
    ResMgr*         pResMgr;

public:
	// Factories
	SotFactory *     pSvObjectFactory;
	SotFactory *     pSvStorageStreamFactory;
	SotFactory *     pSvStorageFactory;
	SotFactory *     pSvEmbeddedObjectFactory;
	SotFactory *     pSvEmbeddedClientFactory;
	SotFactory *     pSvInPlaceObjectFactory;
	SotFactory *     pSvPlugInObjectFactory;
	SotFactory *     pSvAppletObjectFactory;
	SotFactory *     pSvInPlaceClientFactory;
	SotFactory *     pSvPersistFactory;
	SotFactory *     pSvPseudoObjectFactory;
	SotFactory *     pSvSimplePersistFactory;
	SotFactory *     pSvObjectContainerFactory;
	SotFactory *     pSvFactory_ImplFactory;
	SotFactory *	 pDfltPlugInFactory;
	SotFactory *	 pDfltAppletFactory;
	SotFactory *	 pSoBindingFactory;

	// Listen
	SvContainerEnvironmentList *pContEnvList;

	// InPlace-Bearbeitung
	SvInPlaceClientList *		pIPActiveClientList;
	SvInPlaceObjectList *		pIPActiveObjectList;
	SvInPlaceEnvironment * 		pUIShowIPEnv;
	ImplSvEditObjectProtocol *	pIEOPDflt;

	// Sonstiges
	SvClassManager  			aInfoClassMgr;
	SvGlobalName    			aSvInterface;

	// PlugIn
	SvVerbList *				pPlugInVerbList;
	ULONG						nPlugInDocFormat;

	// PlugIn
	SvVerbList *				pAppletVerbList;
	ULONG						nAppletDocFormat;

	Timer *						pDeathTimer;
	void * 						pDead;
	SvBindingData*              pBindingData;
	ConvertTo_Impl 				(*pConvTable)[SO3_OFFICE_VERSIONS];
	USHORT						nConvTableEntries;

							SoDll();
							~SoDll();
	ResMgr *    			GetResMgr();
	SvIPCServerInfo * 		GetIPCServerInfo();

	static SoDll*			GetOrCreate();
};

}
#endif //_SO2DLL_HXX

