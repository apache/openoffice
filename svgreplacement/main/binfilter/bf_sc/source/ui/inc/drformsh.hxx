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



#ifndef DRFORMSH_HXX
#define DRFORMSH_HXX

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

class ScDrawFormShell: public ScDrawShell
{
public:

	SFX_DECL_INTERFACE(SCID_FORM_SHELL);

	ScDrawFormShell(ScViewData* pData):ScDrawShell(pData){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ScDrawFormShell(ScViewData* pData);

//	void		Execute(SfxRequest &);
//	void		GetState(SfxItemSet &);
};

} //namespace binfilter
#endif
