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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _SFXDISPATCH_HXX //autogen
#include <bf_sfx2/dispatch.hxx>
#endif

#ifndef _SFXDOCKWIN_HXX
#include <bf_sfx2/dockwin.hxx>
#endif

#include <bf_svx/f3dchild.hxx>

#include "float3d.hrc"
namespace binfilter {

/*N*/ SFX_IMPL_DOCKINGWINDOW( Svx3DChildWindow, SID_3D_WIN )


/*************************************************************************
|*
|* Ableitung vom SfxChildWindow als "Behaelter" fuer Effekte
|*
\************************************************************************/
/*N*/ __EXPORT Svx3DChildWindow::Svx3DChildWindow( Window* pParent,
/*N*/ 														 USHORT nId,
/*N*/ 														 SfxBindings* pBindings,
/*N*/ 														 SfxChildWinInfo* pInfo ) :
/*N*/ 	SfxChildWindow( pParent, nId )
/*N*/ {
DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	Svx3DWin* pWin = new Svx3DWin( pBindings, this, pParent );
/*N*/ }


}
