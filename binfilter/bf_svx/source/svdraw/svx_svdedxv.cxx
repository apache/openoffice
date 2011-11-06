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




#include "svdedxv.hxx"
#include <bf_svtools/solar.hrc>

#ifndef _STRING_H
#include <tools/string.h>
#endif










#include "svditext.hxx"
#include "svdoutl.hxx"
#include "svdstr.hrc"   // Namen aus der Resource



// #98988#
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrObjEditView::ImpClearVars()
/*N*/ {
/*N*/     bQuickTextEditMode=TRUE;
/*N*/     bMacroMode=TRUE;
/*N*/     pTextEditObj=NULL;
/*N*/     pTextEditOutliner=NULL;
/*N*/     pTextEditOutlinerView=NULL;
/*N*/     pTextEditPV=NULL;
/*N*/     pTextEditWin=NULL;
/*N*/     pTextEditCursorMerker=NULL;
/*N*/     pEditPara=NULL;
/*N*/     bTextEditNewObj=FALSE;
/*N*/     bMacroDown=FALSE;
/*N*/     pMacroObj=NULL;
/*N*/     pMacroPV=NULL;
/*N*/     pMacroWin=NULL;
/*N*/     nMacroTol=0;
/*N*/     bTextEditDontDelete=FALSE;
/*N*/     bTextEditOnlyOneView=FALSE;
/*N*/ }

/*N*/ SdrObjEditView::SdrObjEditView(SdrModel* pModel1, OutputDevice* pOut):
/*N*/     SdrGlueEditView(pModel1,pOut)
/*N*/ {
/*N*/     ImpClearVars();
/*N*/ }

/*N*/ SdrObjEditView::~SdrObjEditView()
/*N*/ {
/*N*/ 	pTextEditWin = NULL;            // Damit es in EndTextEdit kein ShowCursor gibt
/*N*/ 	if (IsTextEdit()){DBG_BF_ASSERT(0, "STRIP"); }//STRIP001  EndTextEdit();
/*N*/     if (pTextEditOutliner!=NULL) {
/*?*/         delete pTextEditOutliner;
/*N*/     }
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ BOOL SdrObjEditView::IsAction() const
/*N*/ {
/*N*/     return IsMacroObj() || SdrGlueEditView::IsAction();
/*N*/ }




/*N*/ void SdrObjEditView::BrkAction()
/*N*/ {
/*N*/     BrkMacroObj();
/*N*/     SdrGlueEditView::BrkAction();
/*N*/ }


/*N*/ void __EXPORT SdrObjEditView::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
/*N*/ {
/*N*/     SdrGlueEditView::SFX_NOTIFY(rBC,rBCType,rHint,rHintType);
/*N*/     // Printerwechsel waerend des Editierens
/*N*/     SdrHint* pSdrHint=PTR_CAST(SdrHint,&rHint);
/*N*/     if (pSdrHint!=NULL && pTextEditOutliner!=NULL) {
/*?*/         SdrHintKind eKind=pSdrHint->GetKind();
/*?*/         if (eKind==HINT_REFDEVICECHG) {
/*?*/             pTextEditOutliner->SetRefDevice(pMod->GetRefDevice());
/*?*/         }
/*?*/         if (eKind==HINT_DEFAULTTABCHG) {
/*?*/             pTextEditOutliner->SetDefTab(pMod->GetDefaultTabulator());
/*?*/         }
/*?*/         if (eKind==HINT_DEFFONTHGTCHG) {
/*?*/             // ...
/*?*/         }
/*?*/         if (eKind==HINT_MODELSAVED) { // #43095#
/*?*/             DBG_BF_ASSERT(0, "STRIP");
/*?*/         }
/*N*/     }
/*N*/ }

/*N*/ void SdrObjEditView::ModelHasChanged()
/*N*/ {
/*N*/     SdrGlueEditView::ModelHasChanged();
/*N*/     if (IsTextEdit() && !pTextEditObj->IsInserted()) {DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 EndTextEdit(); // Objekt geloescht
/*N*/     // TextEditObj geaendert?
/*N*/     if (IsTextEdit()) {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/     }
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@@ @@@@@ @@   @@ @@@@@@  @@@@@ @@@@@  @@ @@@@@@
//    @@   @@    @@@ @@@   @@    @@    @@  @@ @@   @@
//    @@   @@     @@@@@    @@    @@    @@  @@ @@   @@
//    @@   @@@@    @@@     @@    @@@@  @@  @@ @@   @@
//    @@   @@     @@@@@    @@    @@    @@  @@ @@   @@
//    @@   @@    @@@ @@@   @@    @@    @@  @@ @@   @@
//    @@   @@@@@ @@   @@   @@    @@@@@ @@@@@  @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////







/*N*/ BOOL SdrObjEditView::BegTextEdit(SdrObject* pObj, SdrPageView* pPV, Window* pWin,
/*N*/ 	SdrOutliner* pGivenOutliner, OutlinerView* pGivenOutlinerView,
/*N*/ 	BOOL bDontDeleteOutliner, BOOL bOnlyOneView)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001 
/*N*/ }

/*N*/ SdrEndTextEditKind SdrObjEditView::EndTextEdit(BOOL bDontDeleteReally)
/*N*/ {DBG_BF_ASSERT(0, "STRIP");SdrEndTextEditKind eRet=SDRENDTEXTEDIT_UNCHANGED; return eRet; //STRIP001 
/*N*/ }

/*N*/ void SdrObjEditView::DelWin(OutputDevice* pWin1)
/*N*/ {
/*N*/     SdrGlueEditView::DelWin(pWin1);
/*N*/     if (pTextEditObj!=NULL && !bTextEditOnlyOneView && pWin1->GetOutDevType()==OUTDEV_WINDOW) {
/*?*/         DBG_BF_ASSERT(0, "STRIP"); //STRIP001 for (ULONG i=pTextEditOutliner->GetViewCount(); i>0;) {
/*N*/     }
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@   @@  @@@@   @@@@  @@@@@   @@@@   @@   @@  @@@@  @@@@@  @@@@@
//  @@@ @@@ @@  @@ @@  @@ @@  @@ @@  @@  @@@ @@@ @@  @@ @@  @@ @@
//  @@@@@@@ @@  @@ @@     @@  @@ @@  @@  @@@@@@@ @@  @@ @@  @@ @@
//  @@@@@@@ @@@@@@ @@     @@@@@  @@  @@  @@@@@@@ @@  @@ @@  @@ @@@@
//  @@ @ @@ @@  @@ @@     @@  @@ @@  @@  @@ @ @@ @@  @@ @@  @@ @@
//  @@   @@ @@  @@ @@  @@ @@  @@ @@  @@  @@   @@ @@  @@ @@  @@ @@
//  @@   @@ @@  @@  @@@@  @@  @@  @@@@   @@   @@  @@@@  @@@@@  @@@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////





/*N*/ void SdrObjEditView::BrkMacroObj()
/*N*/ {
/*N*/     if (pMacroObj!=NULL) {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/     }
/*N*/ }


}
