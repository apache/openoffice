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


#ifndef _SWFRMSH_HXX
#define _SWFRMSH_HXX

#include "basesh.hxx"
namespace binfilter {

class SwFrameShell: public SwBaseShell
{

public:
	SFX_DECL_INTERFACE(SW_FRAMESHELL);

	SwFrameShell(SwView &rView):SwBaseShell( rView ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 			SwFrameShell(SwView &rView);

	void	ExecFrameStyle(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecFrameStyle(SfxRequest& rReq);
	void	GetLineStyleState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	GetLineStyleState(SfxItemSet &rSet);
	void	StateInsert(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	StateInsert(SfxItemSet &rSet);
};

} //namespace binfilter
#endif
