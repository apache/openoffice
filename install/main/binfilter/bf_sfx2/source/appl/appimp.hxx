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


#ifndef _SFXAPPIMP_HXX
#define _SFXAPPIMP_HXX

#include <tools/time.hxx>
#include <tools/string.hxx>
class MenuBar;
class UniqueIndex;
class BasicManager;
class Timer;
namespace binfilter {

class SfxTbxCtrlFactArr_Impl;
class SfxStbCtrlFactArr_Impl;
class SfxMenuCtrlFactArr_Impl;
class SfxViewFrameArr_Impl;
class SfxViewShellArr_Impl;
class SfxObjectShellArr_Impl;
class IntroWindow_Impl;
class SfxTemplateDialog;
class SfxDialogLibraryContainer;
class SfxScriptLibraryContainer;
class SfxBasicTestWin;

struct SfxApplication_Impl
{
    Time                        aAutoSaveTime;
    String                      aMemExceptionString;
    String                      aResWarningString;
    String                      aResExceptionString;
    String                      aSysResExceptionString;
    String                      aDoubleExceptionString;
    String                      aBasicSourceName;
    SfxTbxCtrlFactArr_Impl*     pTbxCtrlFac;
    SfxStbCtrlFactArr_Impl*     pStbCtrlFac;
    SfxMenuCtrlFactArr_Impl*    pMenuCtrlFac;
    SfxViewFrameArr_Impl*       pViewFrames;
    SfxViewShellArr_Impl*       pViewShells;
    SfxObjectShellArr_Impl*     pObjShells;
    MenuBar*                    pEmptyMenu;     	// dummy for no-menu
    IntroWindow_Impl*			pIntro;
    UniqueIndex*                pEventHdl;  		// Hook-Liste fuer UserEvents
    SfxTemplateDialog*          pTemplateDlg;
	SfxScriptLibraryContainer*	pBasicLibContainer;
	SfxDialogLibraryContainer*	pDialogLibContainer;
    SfxBasicTestWin*            pBasicTestWin;
    Timer*                		pAutoSaveTimer;
    USHORT                      nDocNo;     		// Laufende Doc-Nummer (AutoName)
    USHORT                      nWarnLevel;
    BOOL                        bConfigLoaded:1;
    BOOL                        bAutoSaveNow:1; 	// ist TRUE, wenn der Timer abgelaufen ist, wenn die App nicht aktiv war
};

}//end of namespace binfilter
#endif

