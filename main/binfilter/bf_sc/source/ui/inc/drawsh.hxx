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



#ifndef SC_DRAWSH_HXX
#define SC_DRAWSH_HXX

#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#include "shellids.hxx"
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif

#ifndef _SVDMARK_HXX //autogen
#include <bf_svx/svdmark.hxx>
#endif

#ifndef _LINK_HXX 
#include <tools/link.hxx>
#endif
namespace binfilter {

class SvxNameDialog;
class ScViewData;


class ScDrawShell : public SfxShell
{



public:
	TYPEINFO();
	SFX_DECL_INTERFACE(SCID_DRAW_SHELL);

	ScDrawShell(ScViewData* pData){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ScDrawShell(ScViewData* pData); 

	void 	StateDisableItems( SfxItemSet &rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void 	StateDisableItems( SfxItemSet &rSet );

	void	ExecDrawAttr(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	ExecDrawAttr(SfxRequest& rReq);
	void	GetDrawAttrState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetDrawAttrState(SfxItemSet &rSet);
	void	GetAttrFuncState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetAttrFuncState(SfxItemSet &rSet);

	void	ExecDrawFunc(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	ExecDrawFunc(SfxRequest& rReq);
	void	GetDrawFuncState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetDrawFuncState(SfxItemSet &rSet);
	void	GetState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetState(SfxItemSet &rSet);

	void	ExecFormText(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	ExecFormText(SfxRequest& rReq);		// StarFontWork
	void	GetFormTextState(SfxItemSet& rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetFormTextState(SfxItemSet& rSet);

	void	ExecuteHLink(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	ExecuteHLink(SfxRequest& rReq);		// Hyperlink
	void	GetHLinkState(SfxItemSet& rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetHLinkState(SfxItemSet& rSet);


};



} //namespace binfilter
#endif


