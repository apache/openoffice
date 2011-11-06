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



#include "svdvmark.hxx"
#include "svdobj.hxx"
#include "svdpntv.hxx"

//#ifndef NOOLDSV //autogen
//#include <vcl/system.hxx>
//#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrViewUserMarker::SdrViewUserMarker(SdrPaintView* pView_)
/*N*/ :	pView(pView_),
/*N*/ 	pForcedOutDev(NULL),
/*N*/ 	pPoint(NULL),
/*N*/ 	pRect(NULL),
/*N*/ 	pPoly(NULL),
/*N*/ 	pPolyPoly(NULL),
/*N*/ 	pXPoly(NULL),
/*N*/ 	pXPolyPoly(NULL),
/*N*/ 	eAlign(SDRMARKER_ALIGNCENTER),
/*N*/ 	nPixelDistance(0),
/*N*/ 	nLineWdt(0),
/*N*/ 	nCrossSize(0),
/*N*/ 	bLineWdtLog(FALSE),
/*N*/ 	bCrossSizeLog(FALSE),
/*N*/ 	bSolidArea(FALSE),
/*N*/ 	bDashed(FALSE),
/*N*/ 	bCrossHair(FALSE),
/*N*/ 	bStripes(FALSE),
/*N*/ 	bEllipse(FALSE),
/*N*/ 	bPolyLine(FALSE),
/*N*/ 	bAnimate(FALSE),
/*N*/ 	bVisible(FALSE),
/*N*/ 	nAnimateDelay(0),
/*N*/ 	nAnimateSpeed(0),
/*N*/ 	nAnimateAnz(0),
/*N*/ 	bAnimateBwd(FALSE),
/*N*/ 	bAnimateToggle(FALSE),
/*N*/ 	nAnimateDelayCountDown(0),
/*N*/ 	nAnimateSpeedCountDown(0),
/*N*/ 	nAnimateNum(0),
/*N*/ 	bHasPointer(FALSE),
/*N*/ 	bMouseMovable(FALSE)
/*N*/ {
/*N*/ 	if (pView!=NULL) pView->ImpInsertUserMarker(this);
/*N*/ 	bAnimateToggle=TRUE;
/*N*/ }
/*N*/ 
/*N*/ SdrViewUserMarker::~SdrViewUserMarker()
/*N*/ {
/*N*/ 	if (bVisible) {DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 Hide();
/*N*/ 	if (pView!=NULL) pView->ImpRemoveUserMarker(this);
/*N*/ 	ImpDelGeometrics();
/*N*/ }
/*N*/ 
/*N*/ void SdrViewUserMarker::ImpDelGeometrics()
/*N*/ {
/*N*/ 	if (pPoint    !=NULL) { delete pPoint    ; pPoint    =NULL; }
/*N*/ 	if (pRect     !=NULL) { delete pRect     ; pRect     =NULL; }
/*N*/ 	if (pPoly     !=NULL) { delete pPoly     ; pPoly     =NULL; }
/*N*/ 	if (pPolyPoly !=NULL) { delete pPolyPoly ; pPolyPoly =NULL; }
/*N*/ 	if (pXPoly    !=NULL) { delete pXPoly    ; pXPoly    =NULL; }
/*N*/ 	if (pXPolyPoly!=NULL) { delete pXPolyPoly; pXPolyPoly=NULL; }
/*N*/ }

/*N*/ void SdrViewUserMarker::SetLineWidth(USHORT nWdt)
/*N*/ {
/*N*/ 	FASTBOOL bVis=bVisible;
/*N*/ 	if (bVis) Hide();
/*N*/ 	nLineWdt=nWdt;
/*N*/ 	if (bVis) Show();
/*N*/ }

/*N*/ void SdrViewUserMarker::Show()
/*N*/ {DBG_BF_ASSERT(0, "STRIP");
/*N*/ }

/*N*/ void SdrViewUserMarker::Hide()
/*N*/ {DBG_BF_ASSERT(0, "STRIP");
/*N*/ }


}
