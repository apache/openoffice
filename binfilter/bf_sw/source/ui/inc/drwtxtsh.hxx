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


#ifndef _SWDRWTXTSH_HXX
#define _SWDRWTXTSH_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#include "shellid.hxx"
namespace binfilter {

class OutlinerView;
class SdrOutliner;
class SdrView;
class SwView;
class SfxModule;
class SwWrtShell;

class SwDrawTextShell: public SfxShell
{
	SwView		&rView;

	SdrView		*pSdrView;
	OutlinerView*pOLV;
	SdrOutliner	*pOutliner;

	BOOL 		bRotate : 1;
	BOOL 		bSelMove: 1;

	void 		SetAttrToMarked(const SfxItemSet& rAttr);
	void		InsertSymbol();
	BOOL		IsTextEdit();
public:
	SFX_DECL_INTERFACE(SW_DRWTXTSHELL);
	TYPEINFO();

	SwView	   &GetView() { return rView; }
	SwWrtShell &GetShell();

 				SwDrawTextShell(SwView &rView);

	void		StateDisableItems(SfxItemSet &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		StateDisableItems(SfxItemSet &);

	void		Execute(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		Execute(SfxRequest &);
	void		ExecDraw(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		ExecDraw(SfxRequest &);
	void		GetState(SfxItemSet &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		GetState(SfxItemSet &);
	void		GetDrawTxtCtrlState(SfxItemSet&){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		GetDrawTxtCtrlState(SfxItemSet&);

	void		ExecFontWork(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		ExecFontWork(SfxRequest& rReq);
	void		StateFontWork(SfxItemSet& rSet){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		StateFontWork(SfxItemSet& rSet);
	void		ExecFormText(SfxRequest& rReq){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		ExecFormText(SfxRequest& rReq);
	void		GetFormTextState(SfxItemSet& rSet){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		GetFormTextState(SfxItemSet& rSet);
	void		ExecDrawLingu(SfxRequest &rReq){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		ExecDrawLingu(SfxRequest &rReq);
	void		ExecUndo(SfxRequest &rReq){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		ExecUndo(SfxRequest &rReq);
	void		StateUndo(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		StateUndo(SfxItemSet &rSet);
	void		ExecClpbrd(SfxRequest &rReq){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		ExecClpbrd(SfxRequest &rReq);
	void		StateClpbrd(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		StateClpbrd(SfxItemSet &rSet);
	void		StateInsert(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void		StateInsert(SfxItemSet &rSet);
	void 		ExecTransliteration(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void 		ExecTransliteration(SfxRequest &);

};

} //namespace binfilter
#endif
