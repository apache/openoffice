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


#ifndef _SFX_APPDATA_HXX
#define _SFX_APPDATA_HXX

#include <tools/link.hxx>
#include <tools/list.hxx>
#include <bf_svtools/lstner.hxx>
#include <vcl/timer.hxx>
#include <tools/string.hxx>
#include "rtl/ref.hxx"

#include "bitset.hxx"
class SvStrings;
class Config;
class SvVerbList;
class SvtSaveOptions;
class SvtUndoOptions;
class SvtHelpOptions;

namespace binfilter {

class SvUShorts;
class SfxItemPool;
class SfxCancelManager;
class SfxApplication;

class SfxBmkMenu;
class SfxProgress;
class SfxChildWinFactArr_Impl;
class SfxToolBoxConfig;
class SfxDdeDocTopics_Impl;
class SfxEventConfiguration;
class SfxMacroConfig;
class SfxInitLinkList;
class SfxFilterMatcher;
struct SfxFrameObjectFactoryPtr;
struct SfxPluginObjectFactoryPtr;
class ISfxTemplateCommon;
class SfxFilterMatcher;
class SfxStatusDispatcher;
class SfxMiscCfg;
class SfxDocumentTemplates;
class SfxFrameArr_Impl;
class SfxObjectFactory;
class SfxObjectShell;
namespace sfx2 { namespace appl { class ImeStatusWindow; } }

//=========================================================================
// SfxAppData_Impl
//=========================================================================

class SfxAppData_Impl 
{
public:
    IndexBitSet                         aIndexBitSet;           // for counting noname documents

    // single instance classes
	SfxFrameObjectFactoryPtr*           pSfxFrameObjectFactoryPtr;

    // special members
	SfxInitLinkList*                    pInitLinkList;

    // application members
	SfxFilterMatcher*                   pMatcher;
	SfxCancelManager*                   pCancelMgr;

    // global pointers
	SfxItemPool*                        pPool;
	SfxEventConfiguration*              pEventConfig;
    SfxMiscCfg*                         pMiscConfig;

    // "current" functionality
	SfxObjectShell*						pThisDocument;
	SfxProgress*                        pProgress;

	USHORT                              nBasicCallLevel;
	USHORT                              nRescheduleLocks;
	USHORT                              nInReschedule;

    ::rtl::Reference< sfx2::appl::ImeStatusWindow > m_xImeStatusWindow;

	BOOL                                bInQuit : 1;
	BOOL                                bInException : 1;
    BOOL                                bOLEResize : 1;

                                        SfxAppData_Impl( SfxApplication* );
                                        ~SfxAppData_Impl();
};

}//end of namespace binfilter
#endif // #ifndef _SFX_APPDATA_HXX


