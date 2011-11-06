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


#ifndef SW_INPUTWIN_HXX
#define SW_INPUTWIN_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif
#ifndef _TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
namespace binfilter {

class SwFldMgr;
class SwWrtShell;
class SwView;
class SfxDispatcher;

//========================================================================
class InputEdit : public Edit
{
public:
					InputEdit(Window* pParent, WinBits nStyle) :
						Edit(pParent , nStyle){}
protected:
};

//========================================================================





class SwInputChild : public SfxChildWindow
{
	BOOL 			bObjVis;
	SfxDispatcher* 	pDispatch;
public:
	SwInputChild( Window* ,
						USHORT nId,
						SfxBindings*,
						SfxChildWinInfo*  );
	~SwInputChild();
	SFX_DECL_CHILDWINDOW( SwInputChild );
};

//==================================================================

} //namespace binfilter
#endif

