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


#ifndef _SFX_OBJSHIMP_HXX
#define _SFX_OBJSHIMP_HXX

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif

#include <bf_svtools/securityoptions.hxx>
#include "objsh.hxx"
#include "bitset.hxx"

namespace binfilter {

class AsynchronLink;
//====================================================================

DBG_NAMEEX(SfxObjectShell)//STRIP008 ;

class SfxViewFrame;
struct MarkData_Impl
{
	String aMark;
	String aUserData;
	SfxViewFrame* pFrame;
};

class SfxFrame;
class SfxDialogLibraryContainer;
class SfxScriptLibraryContainer;
class SfxImageManager;
class SfxToolBoxConfig;
struct SfxObjectShell_Impl
{
	SfxDocumentInfo*    pDocInfo;
	SfxConfigManager*	pCfgMgr;
	SfxInPlaceObject*	pInPlaceObj;		// das dazugeh"orige SO2-Objekt, falls this ein SfxInPlaceObject ist
	BasicManager*		pBasicMgr;			// Doc-BASIC oder 0
	SfxScriptLibraryContainer* pBasicLibContainer;
	SfxDialogLibraryContainer* pDialogLibContainer;
	SfxProgress*		pProgress;
	String				aTitle;
    String              aTempName;
	DateTime			nTime;
    sal_uInt16          nVisualDocumentNumber;
    sal_Bool            bTemplateConfig:1,
						bInList:1,  		// ob per First/Next erreichbar
						bClosing:1,         // sal_True w"aehrend Close(), um Benachrichtigungs-Rekursionen zu verhindern
						bSetInPlaceObj:1,	// sal_True, falls bereits versucht wurde pInPlaceObject zu casten
						bIsSaving:1,
						bPasswd:1,
						bIsTmp:1,
						bIsNamedVisible:1,
                        bDidWarnFormat:1,   // sal_True, falls schon wg. speichern in Fremformat gewarnt wurde
                        bSetStandardName:1, // sal_True, falls im FileSave Dialog xxxx.sdw im Standardverzeichnis vorgeschlagen werden soll.
						bDidDangerousSave:1,         // sal_True, falls ein Save in ein Alienformat durchgefuehrt wurde
						bIsTemplate:1,
						bIsAbortingImport:1,  // Importvorgang soll abgebrochen werden.
                        bImportDone : 1, //Import schon fertig? Fuer AutoReload von Docs.
						bInPrepareClose : 1,
						bPreparedForClose : 1,
						bWaitingForPicklist : 1,// Muss noch in die Pickliste
						bModuleSearched : 1,
						bIsBasicDefault : 1,
						bIsHelpObjSh : 1,
						bForbidCaching : 1,
						bForbidReload : 1,
						bSupportsEventMacros: 1,
                        bLoadingWindows: 1,
                        bBasicInitialized :1,
                        bHidden :1; // indicates a hidden view shell


	String              aNewName;  // Der Name, unter dem das Doc gespeichert
								   // werden soll
	IndexBitSet         aBitSet;
	sal_uInt32               lErr;
	sal_uInt16				nEventId;			// falls vor Activate noch ein
											// Open/Create gesendet werden mu/s
	sal_Bool				bDoNotTouchDocInfo;

	AutoReloadTimer_Impl *pReloadTimer;
	MarkData_Impl*      pMarkData;
	sal_uInt16              nLoadedFlags;
	String              aMark;
	Size				aViewSize;			// wird leider vom Writer beim
	sal_Bool				bInFrame;           // HTML-Import gebraucht
	sal_Bool				bModalMode;
    sal_Bool                bRunningMacro;
	sal_Bool                bReloadAvailable;
	sal_uInt16				nAutoLoadLocks;
	SfxModule*              pModule;
	SfxFrame*               pFrame;
    SfxImageManager*        pImageManager;
    SfxToolBoxConfig*       pTbxConfig;
	SfxEventConfigItem_Impl* pEventConfig;
	SfxObjectShellFlags     eFlags;
    AsynchronLink*			pCloser;
	String				    aBaseURL;
	sal_Bool				bReadOnlyUI;
	SvRefBaseRef            xHeaderAttributes;
	sal_Bool				bHiddenLockedByAPI;
	sal_Bool				bInCloseEvent;
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > 			xModel;
	sal_uInt16				nStyleFilter;
    sal_Int16                nMacroMode;
	sal_Bool				bDisposing;

	SfxObjectShell_Impl() :
		nTime(),
		bIsTmp( sal_False),
		bClosing( sal_False),
		bSetInPlaceObj( sal_False),
		bPasswd( sal_False),
		pInPlaceObj( 0),
		pBasicMgr( 0),
		pBasicLibContainer( 0 ),
		pDialogLibContainer( 0 ),
		pProgress( 0),
		nVisualDocumentNumber( USHRT_MAX),
		bIsSaving( sal_False),
		bIsNamedVisible( sal_False),
		pCfgMgr( 0),
		bTemplateConfig( sal_False),
		bDidWarnFormat( sal_False),
		bDidDangerousSave(sal_False),
		bIsBasicDefault( sal_True ),
		bIsTemplate(sal_False),
		lErr(ERRCODE_NONE),
		nEventId ( 0),
		pDocInfo ( 0),
		bIsAbortingImport ( sal_False),
		bInList ( sal_False),
		bImportDone ( sal_False),
		pReloadTimer ( 0),
		nLoadedFlags ( SFX_LOADED_MAINDOCUMENT ),
		pMarkData( 0 ),
		bInFrame( sal_False ),
		bModalMode( sal_False ),
        bRunningMacro( sal_False ),
		bReloadAvailable( sal_False ),
		nAutoLoadLocks( 0 ),
		bInPrepareClose( sal_False ),
		bPreparedForClose( sal_False ),
		bWaitingForPicklist( sal_False ),
        pModule( 0 ),
		bModuleSearched( sal_False ),
		pFrame( 0 ),
        pImageManager( 0 ),
        pTbxConfig( 0 ),
		pEventConfig(NULL),
		bIsHelpObjSh( sal_False ),
		bForbidCaching( sal_False ),
		bDoNotTouchDocInfo( sal_False ),
		bForbidReload( sal_False ),
        bBasicInitialized( sal_False ),
		eFlags( SFXOBJECTSHELL_UNDEFINED ),
		pCloser( 0 ),
		bSupportsEventMacros( sal_True ),
		bReadOnlyUI( sal_False ),
		bHiddenLockedByAPI( sal_False ),
		bInCloseEvent( sal_False ),
        bLoadingWindows( sal_False ),
        bHidden( sal_False )
		, nStyleFilter( 0 )
        , nMacroMode( -1 )
		, bDisposing( sal_False )
	{}
	~SfxObjectShell_Impl();

};

}//end of namespace binfilter
#endif

