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


#ifndef _SWBASESH_HXX
#define _SWBASESH_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SHELLID_HXX
#include <shellid.hxx>
#endif

#define _SVSTDARR_USHORTSSORT
#define _SVSTDARR_USHORTS

#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif
#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#include <bf_svtools/svstdarr.hxx>
class Graphic; 

namespace binfilter {

class GraphicFilter; 
class SfxItemSet; 

class SwWrtShell;
class SwCrsrShell;
class SwView;

class SwFlyFrmAttrMgr;

extern void DisableAllItems(SfxItemSet &);
struct DBTextStruct_Impl;
class SwBaseShell: public SfxShell
{
	SwView      &rView;

	// DragModus
	static USHORT nFrameMode;

	// Bug 75078 - if in GetState the asynch call of GetGraphic returns
	//				synch, the set the state directly into the itemset
	SfxItemSet* 		pGetStateSet;

	//Update-Timer fuer Graphic
	SvUShortsSort aGrfUpdateSlots;

	DECL_LINK( GraphicArrivedHdl, SwCrsrShell* );

protected:

	SwWrtShell& GetShell   ();

	SwView&     GetView() { return rView; }

	SwFlyFrmAttrMgr *pFrmMgr;

    DECL_STATIC_LINK( SwBaseShell, InsertDBTextHdl, DBTextStruct_Impl* );

	void SetGetStateSet( SfxItemSet* p )			{ pGetStateSet = p; }
	BOOL AddGrfUpdateSlot( USHORT nSlot )
								{ return aGrfUpdateSlots.Insert( nSlot ); }

public:
	SwBaseShell(SwView &rShell);
	virtual     ~SwBaseShell();
	SFX_DECL_INTERFACE(SW_BASESHELL);
	TYPEINFO();

	void        ExecDelete(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 void        ExecDelete(SfxRequest &);

	void        ExecClpbrd(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void        ExecClpbrd(SfxRequest &);
	void        StateClpbrd(SfxItemSet &);

	void        ExecUndo(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void        ExecUndo(SfxRequest &);
	void        StateUndo(SfxItemSet &);

	void        Execute(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void        Execute(SfxRequest &);
	void        GetState(SfxItemSet &);
	void        StateStyle(SfxItemSet &);

	void		ExecuteGallery(SfxRequest&){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		ExecuteGallery(SfxRequest&);
	void		GetGalleryState(SfxItemSet&){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		GetGalleryState(SfxItemSet&);

	void        ExecDlg(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void        ExecDlg(SfxRequest &);

	void        StateStatusLine(SfxItemSet &rSet);
	void        ExecTxtCtrl(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void        ExecTxtCtrl(SfxRequest& rReq);
	void        GetTxtFontCtrlState(SfxItemSet& rSet);
	void        GetTxtCtrlState(SfxItemSet& rSet);
	void 		GetBorderState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void 		GetBorderState(SfxItemSet &rSet);
	void        GetBckColState(SfxItemSet &rSet);

 	void        ExecBckCol(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void        ExecBckCol(SfxRequest& rReq);
	void		SetWrapMode( USHORT nSlot );

	void		StateDisableItems(SfxItemSet &){DBG_BF_ASSERT(0, "STRIP");};//STRIP001 void		StateDisableItems(SfxItemSet &);

	void		EditRegionDialog(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP"); };//STRIP001 void		EditRegionDialog(SfxRequest& rReq);
	void		InsertRegionDialog(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP"); };//STRIP001 void		InsertRegionDialog(SfxRequest& rReq);

	void		ExecField(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		ExecField(SfxRequest& rReq);

	static void   _SetFrmMode( USHORT nMode )   { nFrameMode = nMode; }
	static USHORT  GetFrmMode()                 { return nFrameMode;  }

	//public fuer D&D

};


} //namespace binfilter
#endif
