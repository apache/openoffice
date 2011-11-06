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


#ifndef _SFX_MACROCONF_HXX
#define _SFX_MACROCONF_HXX

#ifndef _ERRCODE_HXX //autogen
#include <tools/errcode.hxx>
#endif
#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>		// SvUShorts
#include <bf_sfx2/evntconf.hxx>
class SvStream;
namespace binfilter {

class SvxMacro;
class BasicManager;
class SbMethod;
class SbxValue;
class SbxObject;
class SbxArray;
class SfxMacroInfo;
class SfxSlot;
class SfxMacroInfoItem;
class SfxObjectShell;
struct SfxMacroConfig_Impl;

typedef SfxMacroInfo* SfxMacroInfoPtr;
#if _SOLAR__PRIVATE
SV_DECL_PTRARR(SfxMacroInfoArr_Impl, SfxMacroInfoPtr, 5, 5)//STRIP008 ;
#else
class SfxMacroInfoArr_Impl;
#endif

class SfxMacroInfo
{
friend class SfxMacroConfig;
friend class SfxEventConfiguration;
friend SvStream& operator >> (SvStream& rStream, SfxMacroInfo& rInfo);
friend SvStream& operator << (SvStream& rStream, const SfxMacroInfo& rInfo);

	SfxObjectShell* 		pDocShell;			// nur noch wg. Kompatib. drin
	String*   				pHelpText;
	sal_uInt16                  nRefCnt;
	sal_Bool					bAppBasic;
	String					aLibName;
	String					aModuleName;
	String					aMethodName;
	sal_uInt16					nSlotId;
	SfxSlot*				pSlot;

public:
    SfxMacroInfo(SfxObjectShell *pDoc=NULL);
	~SfxMacroInfo();
	sal_Bool operator==(const SfxMacroInfo& rOther) const;
	int Store (SvStream&);
	String				GetQualifiedName() const;
	String				GetBasicName() const;
	sal_Bool				IsAppMacro() const
						{ return bAppBasic; }
	const String&		GetModuleName() const
						{ return aModuleName; }
	const String&		GetLibName() const
						{ return aLibName; }
	const String&		GetMethodName() const
						{ return aMethodName; }
	sal_uInt16				GetSlotId() const
						{ return nSlotId; }
	SfxSlot*			GetSlot() const
						{ return pSlot; }
};

//ASDBG obsolete >= 582
//ASDBG class ::com::sun::star::uno::Reference< ::com::sun::star::script::XEngine > ;
//ASDBG class ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ;

class SfxMacroConfig
{
friend class SfxEventConfiguration;

	static SfxMacroConfig*	pMacroConfig;

	SfxMacroConfig_Impl* 	pImp;
	SvUShorts               aIdArray;

public:
							~SfxMacroConfig();

	static ErrCode			Call( SbxObject*, const String&, BasicManager*,
								SbxArray *pArgs=NULL, SbxValue *pRet=NULL );
//ASDBG obsolete >= 582
//ASDBG 	static void	CallStarScript( const ::com::sun::star::uno::Reference< ::com::sun::star::script::XEngine > & rxEngine, const String & rCode,
//ASDBG     const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & rSource, void *pArgs, void *pRet );
	static SbMethod*		GetMethod_Impl( const String&, BasicManager* );

#if _SOLAR__PRIVATE
	static void				Release_Impl();
	DECL_LINK(				CallbackHdl_Impl, SfxMacroConfig*);
	DECL_LINK(				EventHdl_Impl, SfxMacroInfo*);
#endif
};

}//end of namespace binfilter
#endif
