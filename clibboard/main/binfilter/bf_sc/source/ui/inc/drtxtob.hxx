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



#ifndef SC_DRTXTOB_HXX
#define SC_DRTXTOB_HXX

#ifndef _SFX_HXX
#endif

#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif
#ifndef _LINK_HXX 
#include <tools/link.hxx>
#endif

#include "shellids.hxx"
class TransferableDataHelper;
class TransferableClipboardListener;
namespace binfilter {

USHORT ScGetFontWorkId();		// statt SvxFontWorkChildWindow::GetChildWindowId()

class ScViewData;

class ScDrawTextObjectBar : public SfxShell
{

public:
	SFX_DECL_INTERFACE(SCID_DRAW_TEXT_SHELL);

	ScDrawTextObjectBar(ScViewData* pData){DBG_BF_ASSERT(0, "STRIP"); }

	void StateDisableItems( SfxItemSet &rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void StateDisableItems( SfxItemSet &rSet );

	void Execute( SfxRequest &rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void Execute( SfxRequest &rReq );
	void ExecuteTrans( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void ExecuteTrans( SfxRequest& rReq );
	void GetState( SfxItemSet& rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void GetState( SfxItemSet& rSet );
	void GetClipState( SfxItemSet& rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void GetClipState( SfxItemSet& rSet );

	void ExecuteAttr( SfxRequest &rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 //STRIP001 	void ExecuteAttr( SfxRequest &rReq );
	void GetAttrState( SfxItemSet& rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void GetAttrState( SfxItemSet& rSet );
	void ExecuteToggle( SfxRequest &rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void ExecuteToggle( SfxRequest &rReq );
	void ExecuteExtra( SfxRequest &rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void ExecuteExtra( SfxRequest &rReq );
	void ExecFormText(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void ExecFormText(SfxRequest& rReq);		// StarFontWork
	void GetFormTextState(SfxItemSet& rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void GetFormTextState(SfxItemSet& rSet);
};



} //namespace binfilter
#endif

