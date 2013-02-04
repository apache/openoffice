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


#ifndef _SC_DWFUNCTR_HXX
#define _SC_DWFUNCTR_HXX

#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif

#ifndef _SFXDOCKWIN_HXX //autogen
#include <bf_sfx2/dockwin.hxx>
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif

#ifndef _STDCTRL_HXX //autogen
#include <bf_svtools/stdctrl.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"		// ScAddress
#endif

namespace binfilter {

#ifndef	LRU_MAX
#define LRU_MAX 10
#endif
/*************************************************************************
|*
|* Ableitung vom SfxChildWindow als "Behaelter" fuer Controller
|*
\************************************************************************/

class ScFunctionChildWindow : public SfxChildWindow
{
 public:
	 ScFunctionChildWindow( Window* pParent, USHORT nId, SfxBindings* pBindings,SfxChildWinInfo* pInfo):SfxChildWindow( pParent, nId ){DBG_ASSERT(0, "STRIP")}; //STRIP001 ScFunctionChildWindow( Window*, USHORT, SfxBindings*,

	SFX_DECL_CHILDWINDOW(ScFunctionChildWindow);
};

} //namespace binfilter
#endif


