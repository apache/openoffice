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


#ifndef _WORKCTRL_HXX
#define _WORKCTRL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXTBXCTRL_HXX //autogen
#include <bf_sfx2/tbxctrl.hxx>
#endif

#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif
#ifndef _STDCTRL_HXX
#include <bf_svtools/stdctrl.hxx>
#endif
#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/button.hxx>
#endif
class PopupMenu; 
namespace binfilter {


class SwView;

// doppelter Eintrag! hrc und hxx
// diese Ids bestimmen, was die Buttons unter dem Scrollbar tun
#define NID_START   20000
#define NID_NEXT 	20000
#define NID_PREV	20001
#define NID_TBL     20002
#define NID_FRM     20003
#define NID_PGE     20004
#define NID_DRW     20005
#define NID_CTRL    20006
#define NID_REG     20007
#define NID_BKM     20008
#define NID_GRF    	20009
#define NID_OLE    	20010
#define NID_OUTL   	20011
#define NID_SEL    	20012
#define NID_FTN   	20013
#define NID_MARK	20014
#define NID_POSTIT  20015
#define NID_SRCH_REP 20016
#define NID_INDEX_ENTRY  20017
#define NID_TABLE_FORMULA	20018
#define NID_TABLE_FORMULA_ERROR		20019
#define NID_COUNT  20



//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwTbxInsertCtrl : public SfxToolBoxControl
{
	USHORT 					nLastSlotId;


public:
	SFX_DECL_TOOLBOX_CONTROL();

	SwTbxInsertCtrl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
	~SwTbxInsertCtrl();

	virtual void				StateChanged( USHORT nSID,
											  SfxItemState eState,
											  const SfxPoolItem* pState );

};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwTbxAutoTextCtrl : public SfxToolBoxControl
{
	PopupMenu*				pPopup;
	SwView* 				pView;


	void					DelPopup();
public:
	SFX_DECL_TOOLBOX_CONTROL();

	SwTbxAutoTextCtrl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
	~SwTbxAutoTextCtrl();

	virtual void				StateChanged( USHORT nSID,
											  SfxItemState eState,
											  const SfxPoolItem* pState );

};


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwScrollNaviToolBox : public ToolBox
{
	virtual void	MouseButtonUp( const MouseEvent& rMEvt );
	virtual void	RequestHelp( const HelpEvent& rHEvt );

	public:
        SwScrollNaviToolBox(Window* pParent, WinBits nWinStyle ) :
             ToolBox(pParent, nWinStyle ) {}
};

class SwScrollNaviPopup : public SfxPopupWindow
{
    SwScrollNaviToolBox    aToolBox;
    FixedLine       aSeparator;
    FixedInfo       aInfoField;
	ImageList		aIList;
    ImageList       aIListH;

	String			sQuickHelp[2 * NID_COUNT];

	USHORT			nFwdId;
	USHORT			nBackId;

    void            ApplyImageList();
    protected:

	public:
        SwScrollNaviPopup( USHORT nId, SfxBindings & );
		~SwScrollNaviPopup();

};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//	Die ImageButtons muessen sich bei Bedarf den HelpText selbst setzen
//----------------------------------------------------------------------------

class SwHlpImageButton : public ImageButton
{
	BOOL		bUp;
public:
 		SwHlpImageButton(Window* pParent, const ResId& rResId, BOOL bUpBtn) :
			ImageButton(pParent, rResId), bUp(bUpBtn){}


};
/* -----------------26.11.2002 09:25-----------------
 * 
 * --------------------------------------------------*/
class SwPreviewZoomControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();

    SwPreviewZoomControl( USHORT nId, ToolBox& rTbx, SfxBindings& rBind );
    ~SwPreviewZoomControl();


};
} //namespace binfilter
#endif



