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



#include "svdpoev.hxx"
#include <math.h>
#include "svdopath.hxx"
#include "svdstr.hrc"   // Namen aus der Resource
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrPolyEditView::ImpResetPolyPossibilityFlags()
/*N*/ {
/*N*/ 	eMarkedPointsSmooth=SDRPATHSMOOTH_DONTCARE;
/*N*/ 	eMarkedSegmentsKind=SDRPATHSEGMENT_DONTCARE;
/*N*/ 	bSetMarkedPointsSmoothPossible=FALSE;
/*N*/ 	bSetMarkedSegmentsKindPossible=FALSE;
/*N*/ }

/*N*/ void SdrPolyEditView::ImpClearVars()
/*N*/ {
/*N*/ 	ImpResetPolyPossibilityFlags();
/*N*/ }

/*N*/ SdrPolyEditView::SdrPolyEditView(SdrModel* pModel1, OutputDevice* pOut):
/*N*/ 	SdrEditView(pModel1,pOut)
/*N*/ {
/*N*/ 	ImpClearVars();
/*N*/ }

/*N*/ SdrPolyEditView::~SdrPolyEditView()
/*N*/ {
/*N*/ }

/*N*/ void SdrPolyEditView::ImpCheckPolyPossibilities()
/*N*/ {
/*N*/ 	ImpResetPolyPossibilityFlags();
/*N*/ 	ULONG nMarkAnz=aMark.GetMarkCount();
/*N*/ 	if (nMarkAnz>0 && !ImpIsFrameHandles()) {
/*?*/ 		BOOL bReadOnly=FALSE;
/*?*/ 		BOOL b1stSmooth=TRUE;
/*?*/ 		BOOL b1stSegm=TRUE;
/*?*/ 		BOOL bCurve=FALSE;
/*?*/ 		BOOL bSmoothFuz=FALSE;
/*?*/ 		BOOL bSegmFuz=FALSE;
/*?*/ 		XPolyFlags eSmooth=XPOLY_NORMAL;
/*?*/ 
/*?*/ 		for (ULONG nMarkNum=0; nMarkNum<nMarkAnz; nMarkNum++) {
/*?*/ 			SdrMark* pM=aMark.GetMark(nMarkNum);
/*?*/ 			SdrObject* pObj=pM->GetObj();
/*?*/ 			SdrUShortCont* pPts=pM->GetMarkedPoints();
/*?*/ 			SdrPathObj* pPath=PTR_CAST(SdrPathObj,pObj);
/*?*/ 			if (pPath!=NULL && pPts!=NULL) {
/*?*/ 				ULONG nMarkedPntAnz=pPts->GetCount();
/*?*/ 				if (nMarkedPntAnz!=0) {
/*?*/ 					BOOL bClosed=pPath->IsClosed();
/*?*/ 					bSetMarkedPointsSmoothPossible=TRUE;
/*?*/ 					if (bClosed) bSetMarkedSegmentsKindPossible=TRUE;
/*?*/ 					const XPolyPolygon& rXPP=pPath->GetPathPoly();
/*?*/ 					for (USHORT nMarkedPntNum=0; nMarkedPntNum<(USHORT)nMarkedPntAnz; nMarkedPntNum++) {
/*?*/ 						USHORT nNum=pPts->GetObject(nMarkedPntNum);
/*?*/ 						USHORT nPolyNum=0,nPntNum=0;
/*?*/ 						if (pPath->FindPolyPnt(nNum,nPolyNum,nPntNum,FALSE)) {
/*?*/ 							const XPolygon& rXP=rXPP[nPolyNum];
/*?*/ 							BOOL bCanSegment=bClosed || nPntNum<rXP.GetPointCount()-1;
/*?*/ 
/*?*/ 							if (!bSetMarkedSegmentsKindPossible && bCanSegment) {
/*?*/ 								bSetMarkedSegmentsKindPossible=TRUE;
/*?*/ 							}
/*?*/ 							if (!bSmoothFuz) {
/*?*/ 								if (b1stSmooth) {
/*?*/ 									b1stSmooth=FALSE;
/*?*/ 									eSmooth=rXP.GetFlags(nPntNum);
/*?*/ 								} else {
/*?*/ 									bSmoothFuz=eSmooth!=rXP.GetFlags(nPntNum);
/*?*/ 								}
/*?*/ 							}
/*?*/ 							if (!bSegmFuz) {
/*?*/ 								if (bCanSegment) {
/*?*/ 									BOOL bCrv=rXP.IsControl(nPntNum+1);
/*?*/ 									if (b1stSegm) {
/*?*/ 										b1stSegm=FALSE;
/*?*/ 										bCurve=bCrv;
/*?*/ 									} else {
/*?*/ 										bSegmFuz=bCrv!=bCurve;
/*?*/ 									}
/*?*/ 								}
/*?*/ 							}
/*?*/ 						}
/*?*/ 					}
/*?*/ 					if (!b1stSmooth && !bSmoothFuz) {
/*?*/ 						if (eSmooth==XPOLY_NORMAL) eMarkedPointsSmooth=SDRPATHSMOOTH_ANGULAR;
/*?*/ 						if (eSmooth==XPOLY_SMOOTH) eMarkedPointsSmooth=SDRPATHSMOOTH_ASYMMETRIC;
/*?*/ 						if (eSmooth==XPOLY_SYMMTR) eMarkedPointsSmooth=SDRPATHSMOOTH_SYMMETRIC;
/*?*/ 					}
/*?*/ 					if (!b1stSegm && !bSegmFuz) {
/*?*/ 						eMarkedSegmentsKind= bCurve ? SDRPATHSEGMENT_CURVE : SDRPATHSEGMENT_LINE;
/*?*/ 					}
/*?*/ 				}
/*?*/ 			}
/*?*/ 		}
/*?*/ 		if (bReadOnly) {
/*?*/ 			bSetMarkedPointsSmoothPossible=FALSE;
/*?*/ 			bSetMarkedSegmentsKindPossible=FALSE;
/*?*/ 		}
/*N*/ 	}
/*N*/ }










////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////



}
