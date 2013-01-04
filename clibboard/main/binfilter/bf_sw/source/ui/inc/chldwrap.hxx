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


#ifndef _CHLDWRAP_HXX
#define _CHLDWRAP_HXX

#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif

#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
namespace binfilter {

class SwDocShell;

class SwChildWinWrapper : public SfxChildWindow
{


	// Implementation in fldtdlg.cxx
protected:
	SwChildWinWrapper(Window *pParentWindow, USHORT nId):SfxChildWindow(pParentWindow, nId){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SwChildWinWrapper(Window *pParentWindow, USHORT nId);

};


} //namespace binfilter
#endif

