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



#ifndef _HDFTCTRL_HXX
#define _HDFTCTRL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXMNUITEM_HXX //autogen
#include <bf_sfx2/mnuitem.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <bf_sfx2/app.hxx>
#endif
#define _SVSTDARR_STRINGSSORTDTOR
#define _SVSTDARR_BOOLS
#include <bf_svtools/svstdarr.hxx>
namespace binfilter {

class SwDocShell;

class SwHeadFootMenuControl : public SfxMenuControl
{

public:
	SFX_DECL_MENU_CONTROL();

	SwHeadFootMenuControl( USHORT nPos, Menu& rMenu,	//STRIP001 	SwHeadFootMenuControl( USHORT nPos, Menu& rMenu,
		SfxBindings& rBindings ){DBG_BF_ASSERT(0, "STRIP");};	//STRIP001 								SfxBindings& rBindings );
};

} //namespace binfilter
#endif

