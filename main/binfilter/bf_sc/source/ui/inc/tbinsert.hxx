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



#ifndef SC_TBINSERT_HXX
#define SC_TBINSERT_HXX

#ifndef _SFXTBXCTRL_HXX //autogen
#include <bf_sfx2/tbxctrl.hxx>
#endif

#ifndef _SFXTBXMGR_HXX //autogen
#include <bf_sfx2/tbxmgr.hxx>
#endif
namespace binfilter {

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class ScTbxInsertCtrl : public SfxToolBoxControl
{
	USHORT 					nLastSlotId;


public:
	SFX_DECL_TOOLBOX_CONTROL();

	ScTbxInsertCtrl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
	~ScTbxInsertCtrl();

	virtual void				StateChanged( USHORT nSID,
											  SfxItemState eState,
											  const SfxPoolItem* pState );
};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------




} //namespace binfilter
#endif

