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



#ifndef GRAPHSH_HXX
#define GRAPHSH_HXX

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

class ScViewData;

#include "drawsh.hxx"
namespace binfilter {

class ScGraphicShell: public ScDrawShell
{
public:

	SFX_DECL_INTERFACE(SCID_GRAPHIC_SHELL);

	ScGraphicShell(ScViewData* pData):ScDrawShell(pData){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ScGraphicShell(ScViewData* pData);
	void	Execute(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	Execute(SfxRequest& rReq);
	void	GetAttrState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetAttrState(SfxItemSet &rSet);
 
	void	ExecuteFilter(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	ExecuteFilter(SfxRequest& rReq);
	void	GetFilterState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 //STRIP001 	void	GetFilterState(SfxItemSet &rSet);
};

} //namespace binfilter
#endif
