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



#ifndef SC_NAVIPI_HXX
#define SC_NAVIPI_HXX

#ifndef _TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <bf_svtools/stdctrl.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif

#ifndef SC_CONTENT_HXX
#include "content.hxx"
#endif

#ifndef _SVEDIT_HXX //autogen
#include <bf_svtools/svmedit.hxx>
#endif
namespace binfilter {


class ScTabViewShell;
class ScViewData;
class ScArea;
class ScScenarioWindow;
class ScNavigatorControllerItem;
class ScNavigatorDialogWrapper;
class ScNavigatorDlg;
class ScNavigatorSettings;
class ScRange;

//========================================================================

#define SC_DROPMODE_URL			0
#define SC_DROPMODE_LINK		1
#define SC_DROPMODE_COPY		2

enum NavListMode { NAV_LMODE_NONE		= 0x4000,
				   NAV_LMODE_AREAS		= 0x2000,
				   NAV_LMODE_DBAREAS	= 0x1000,
				   NAV_LMODE_DOCS		= 0x800,
				   NAV_LMODE_SCENARIOS	= 0x400 };

//========================================================================
// class ScScenarioListBox -----------------------------------------------
//========================================================================

//========================================================================
// class ScScenarioWindow ------------------------------------------------
//========================================================================

//==================================================================
//	class ColumnEdit
//==================================================================


//==================================================================
//	class RowEdit
//==================================================================


//==================================================================
//	class ScDocListBox
//==================================================================


//==================================================================
//	class CommandToolBox
//==================================================================

//==================================================================
//	class ScNavigatorDlg
//==================================================================


//==================================================================

class ScNavigatorDialogWrapper: public SfxChildWindowContext
{
public:
			ScNavigatorDialogWrapper( Window*			pParent,
									  USHORT			nId,
									  SfxBindings*		pBindings,
									  SfxChildWinInfo*	pInfo ):SfxChildWindowContext( nId ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SfxChildWinInfo*	pInfo );

	SFX_DECL_CHILDWINDOW_CONTEXT(ScNavigatorDialogWrapper)


};



} //namespace binfilter
#endif // SC_NAVIPI_HXX

