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



#include "svdmrkv.hxx"
namespace binfilter {

/*N*/ BOOL SdrMarkView::HasMarkedPoints() const
/*N*/ {
/*N*/ 	ForceUndirtyMrkPnt();
/*N*/ 	BOOL bRet=FALSE;
/*N*/ 	if (!ImpIsFrameHandles()) {
/*N*/ 		ULONG nMarkAnz=aMark.GetMarkCount();
/*N*/ 		if (nMarkAnz<=nFrameHandlesLimit) {
/*N*/ 			for (ULONG nMarkNum=0; nMarkNum<nMarkAnz && !bRet; nMarkNum++) {
/*?*/ 				const SdrMark* pM=aMark.GetMark(nMarkNum);
/*?*/ 				const SdrUShortCont* pPts=pM->GetMarkedPoints();
/*?*/ 				bRet=pPts!=NULL && pPts->GetCount()!=0;
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return bRet;
/*N*/ }

/*N*/ void SdrMarkView::SetPlusHandlesAlwaysVisible(BOOL bOn)
/*N*/ { // HandlePaint optimieren !!!!!!!
/*N*/ 	ForceUndirtyMrkPnt();
/*N*/ 	if (bOn!=bPlusHdlAlways) {
/*?*/ 		BOOL bVis=IsMarkHdlShown();
/*?*/ 		if (bVis) HideMarkHdl(NULL);
/*?*/ 		bPlusHdlAlways=bOn;
/*?*/ 		SetMarkHandles();
/*?*/ 		if (bVis) ShowMarkHdl(NULL);
/*?*/ 		MarkListHasChanged();
/*N*/ 	}
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// ImpSetPointsRects() ist fuer PolyPoints und GluePoints!
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// UndirtyMrkPnt() ist fuer PolyPoints und GluePoints!
////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrMarkView::UndirtyMrkPnt() const
/*N*/ {
/*N*/ 	BOOL bChg=FALSE;
/*N*/ 	ULONG nMarkAnz=aMark.GetMarkCount();
/*N*/ 	for (ULONG nMarkNum=0; nMarkNum<nMarkAnz; nMarkNum++) {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ 	if (bChg) ((SdrMarkView*)this)->bMarkedPointsRectsDirty=TRUE;
/*N*/ 	((SdrMarkView*)this)->bMrkPntDirty=FALSE;
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



/*N*/ BOOL SdrMarkView::HasMarkedGluePoints() const
/*N*/ {
/*N*/ 	ForceUndirtyMrkPnt();
/*N*/ 	BOOL bRet=FALSE;
/*N*/ 	ULONG nMarkAnz=aMark.GetMarkCount();
/*N*/ 	for (ULONG nMarkNum=0; nMarkNum<nMarkAnz && !bRet; nMarkNum++) {
/*?*/ 		const SdrMark* pM=aMark.GetMark(nMarkNum);
/*?*/ 		const SdrUShortCont* pPts=pM->GetMarkedGluePoints();
/*?*/ 		bRet=pPts!=NULL && pPts->GetCount()!=0;
/*N*/ 	}
/*N*/ 	return bRet;
/*N*/ }


/*N*/ BOOL SdrMarkView::MarkGluePoints(const Rectangle* pRect, BOOL bUnmark)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001 
/*N*/ }

}
