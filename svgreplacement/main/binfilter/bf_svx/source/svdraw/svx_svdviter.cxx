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



#include "svdviter.hxx"
#include "svdpage.hxx"
#include "svdview.hxx"
#include "svdpagv.hxx"

namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrViewIter::ImpInitVars()
/*N*/ {
/*N*/ 	nListenerNum=0;
/*N*/ 	nPageViewNum=0;
/*N*/ 	nOutDevNum=0;
/*N*/ 	pAktView=NULL;
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrViewIter::SdrViewIter(const SdrObject* pObject_, FASTBOOL bNoMasterPage_)
/*N*/ {
/*N*/ 	pObject=pObject_;
/*N*/ 	pModel=pObject_!=NULL ? pObject_->GetModel() : NULL;
/*N*/ 	pPage=pObject_!=NULL ? pObject_->GetPage() : NULL;
/*N*/ 	bNoMasterPage=bNoMasterPage_;
/*N*/ 	if (pModel==NULL || pPage==NULL) {
/*N*/ 		pModel=NULL;
/*N*/ 		pPage=NULL;
/*N*/ 	}
/*N*/ 	ImpInitVars();
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ FASTBOOL SdrViewIter::ImpCheckPageView(SdrPageView* pPV) const
/*N*/ {
/*N*/ 	if (pPage!=NULL) {
/*N*/ 		FASTBOOL bMaster=pPage->IsMasterPage();
/*N*/ 		USHORT nPageNum=pPage->GetPageNum();
/*N*/ 		SdrPage* pPg=pPV->GetPage();
/*N*/ 		if (pPg==pPage) {
/*N*/ 			if (pObject!=NULL) {
/*N*/ 				// Objekt gewuenscht? Na dann erstmal sehen, ob
/*N*/ 				// das Obj in dieser PageView auch sichtbar ist.
/*N*/ 				SetOfByte aObjLay;
/*N*/ 				pObject->GetLayer(aObjLay);
/*N*/ 				aObjLay&=pPV->GetVisibleLayers();
/*N*/ 				return !aObjLay.IsEmpty();
/*N*/ 			} else {
/*N*/ 				return TRUE;
/*N*/ 			}
/*N*/ 		} else {
/*N*/ 			if (!bNoMasterPage && bMaster && (pObject==NULL || !pObject->IsNotVisibleAsMaster())) {
/*N*/ 				USHORT nMasterPageAnz=pPg->GetMasterPageCount();
/*N*/ 				USHORT nMasterPagePos=0;
/*N*/ 				while (nMasterPagePos<nMasterPageAnz) {
/*N*/ 					if (nPageNum==pPg->GetMasterPageNum(nMasterPagePos)) {
/*N*/ 						// Aha, die gewuenschte Page ist also MasterPage in dieser PageView
/*N*/ 						if (pObject!=NULL) {
/*N*/ 							// Objekt gewuenscht? Na dann erstmal sehen, ob
/*N*/ 							// das Obj in dieser PageView auch sichtbar ist.
/*N*/ 							SetOfByte aObjLay;
/*N*/ 							pObject->GetLayer(aObjLay);
/*N*/ 							aObjLay&=pPV->GetVisibleLayers();
/*N*/ 							aObjLay&=pPg->GetMasterPageVisibleLayers(nMasterPagePos);
/*N*/ 							if (!aObjLay.IsEmpty()) {
/*N*/ 								return TRUE;
/*N*/ 							} // ansonsten die naechste MasterPage der Page ansehen...
/*N*/ 						} else {
/*N*/ 							return TRUE;
/*N*/ 						}
/*N*/ 					}
/*N*/ 					nMasterPagePos++;
/*N*/ 				}
/*N*/ 			}
/*N*/ 			// MasterPage nicht erlaubt oder keine passende gefunden
/*N*/ 			return FALSE;
/*N*/ 		}
/*N*/ 	} else {
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrView* SdrViewIter::ImpFindView()
/*N*/ {
/*N*/ 	if (pModel!=NULL) {
/*N*/ 		USHORT nLsAnz=pModel->GetListenerCount();
/*N*/ 		while (nListenerNum<nLsAnz) {
/*N*/ 			SfxListener* pLs=pModel->GetListener(nListenerNum);
/*N*/ 			pAktView=PTR_CAST(SdrView,pLs);
/*N*/ 			if (pAktView!=NULL) {
/*N*/ 				if (pPage!=NULL) {
/*N*/ 					USHORT nPvAnz=pAktView->GetPageViewCount();
/*N*/ 					USHORT nPvNum=0;
/*N*/ 					while (nPvNum<nPvAnz) {
/*N*/ 						SdrPageView* pPV=pAktView->GetPageViewPvNum(nPvNum);
/*N*/ 						if (ImpCheckPageView(pPV)) {
/*N*/ 							return pAktView;
/*N*/ 						}
/*N*/ 						nPvNum++;
/*N*/ 					}
/*N*/ 				} else {
/*N*/ 					return pAktView;
/*N*/ 				}
/*N*/ 			}
/*N*/ 			nListenerNum++;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	pAktView=NULL;
/*N*/ 	return pAktView;
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrView* SdrViewIter::FirstView()
/*N*/ {
/*N*/ 	ImpInitVars();
/*N*/ 	return ImpFindView();
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrView* SdrViewIter::NextView()
/*N*/ {
/*N*/ 	nListenerNum++;
/*N*/ 	return ImpFindView();
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////

}
