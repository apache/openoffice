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





#ifndef _SCH_TBX_WW_HXX
#define _SCH_TBX_WW_HXX

#ifndef _SV_TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX //autogen
#include <bf_sfx2/tbxctrl.hxx>
#endif


#include "schresid.hxx"
namespace binfilter {

//------------------------------------------------------------------------

class SchPopupWindowTbx : public SfxPopupWindow
{
private:
	ToolBox 			aTbx;

public:
	SchPopupWindowTbx( USHORT nId, SfxToolBoxControl* pTbxCtl,
					  SchResId aRIdWin, SchResId aRIdTbx,
					  SfxBindings& rBindings );
	~SchPopupWindowTbx() {}

	void StartSelection() { aTbx.StartSelection(); }
	void SelectHdl( void* p );
};

} //namespace binfilter
#endif		// _SCH_TBX_WW_HXX

