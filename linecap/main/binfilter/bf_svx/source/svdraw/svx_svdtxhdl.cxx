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



#include "svdtxhdl.hxx"
#include <limits.h>
#include "svdmodel.hxx"
#include "svdotext.hxx"
#include "svdoutl.hxx"


#ifndef _OUTLOBJ_HXX //autogen
#include <outlobj.hxx>
#endif






////////////////////////////////////////////////////////////////////////////////////////////////////
// #101499#

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif



#ifndef _COM_SUN_STAR_I18N_CHARACTERITERATORMODE_HDL_
#include <com/sun/star/i18n/CharacterIteratorMode.hdl>
#endif


#ifndef _XOUTX_HXX
#include "xoutx.hxx"
#endif

namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::i18n;

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ ImpTextPortionHandler::ImpTextPortionHandler(SdrOutliner& rOutln, const SdrTextObj& rTxtObj):
/*N*/ 	rOutliner(rOutln),
/*N*/ 	rTextObj(rTxtObj),
/*N*/ 	// #101498# aPoly(0)
/*N*/ 	mpRecordPortions(0L)
/*N*/ {
/*N*/ 	pModel=rTextObj.GetModel();
/*N*/ 	MapMode aMap=aVDev.GetMapMode();
/*N*/ 
/*N*/ 	aMap.SetMapUnit(pModel->GetScaleUnit());
/*N*/ 	aMap.SetScaleX(pModel->GetScaleFraction());
/*N*/ 	aMap.SetScaleY(pModel->GetScaleFraction());
/*N*/ 	aVDev.SetMapMode(aMap);
/*N*/ }


/*N*/ void ImpTextPortionHandler::DrawTextToPath(ExtOutputDevice& rXOut, FASTBOOL bDrawEffect)
/*N*/ {
/*N*/ 	aFormTextBoundRect=Rectangle();
/*N*/ 	const Rectangle& rBR = rTextObj.GetSnapRect();
/*N*/ 
/*N*/ 	bDraw = bDrawEffect;
/*N*/ 
/*N*/ 	OutlinerParaObject* pPara=rTextObj.GetOutlinerParaObject();
/*N*/ 	if (rTextObj.IsTextEditActive()) {
/*?*/ 		pPara=rTextObj.GetEditOutlinerParaObject();
/*N*/ 	}
/*N*/ 	if (pPara!=NULL) {
/*N*/ 		XPolyPolygon aXPP;
/*N*/ 		//rTextObj.TakeContour(aXPP);
/*N*/ 		rTextObj.TakeXorPoly(aXPP,FALSE);
/*N*/ 		pXOut=&rXOut;
/*N*/ 		Font aFont(rXOut.GetOutDev()->GetFont());
/*N*/ 		rOutliner.Clear();
/*N*/ 		rOutliner.SetPaperSize(Size(LONG_MAX,LONG_MAX));
/*N*/ 		rOutliner.SetText(*pPara);
/*N*/ 		USHORT nCnt = Min(aXPP.Count(), (USHORT) rOutliner.GetParagraphCount());
/*N*/ 
/*N*/ 		if ( nCnt == 1 )    bToLastPoint = TRUE;
/*N*/ 		else                bToLastPoint = FALSE;
/*N*/ 
/*N*/ 		// #101498# completely different methodology needed here for making this BIDI-able
/*N*/ 		// iterate over paragraphs and Polygons, thus each paragraph will be put to
/*N*/ 		// one Polygon
/*N*/ 	    const sal_uInt32 nSavedLayoutMode(rXOut.GetOutDev()->GetLayoutMode());
/*N*/ 		sal_uInt32 nLayoutMode(nSavedLayoutMode);
/*N*/ 
/*N*/ 		if(TRUE)
/*N*/ 		{
/*N*/ 			nLayoutMode &= ~(TEXT_LAYOUT_BIDI_RTL|TEXT_LAYOUT_COMPLEX_DISABLED|TEXT_LAYOUT_BIDI_STRONG);
/*N*/ 		}
/*N*/ 		else
/*N*/ 		{
/*N*/ 			nLayoutMode &= ~(TEXT_LAYOUT_BIDI_RTL);
/*N*/ 			nLayoutMode |= ~(TEXT_LAYOUT_COMPLEX_DISABLED|TEXT_LAYOUT_BIDI_STRONG);
/*N*/ 		}
/*N*/ 
/*N*/ 		rXOut.GetOutDev()->SetLayoutMode(nLayoutMode);
/*N*/ 
/*N*/ 		for(nParagraph = 0; nParagraph < nCnt; nParagraph++)
/*N*/ 		{
/*N*/ 			Polygon aPoly = XOutCreatePolygon(aXPP[sal_uInt16(nParagraph)], rXOut.GetOutDev());
/*N*/ 
/*N*/ 			rOutliner.SetDrawPortionHdl(LINK(this, ImpTextPortionHandler, FormTextRecordPortionHdl));
/*N*/ 			rOutliner.StripPortions();
/*N*/ 
/*N*/ 			DrawFormTextRecordPortions(aPoly);
/*N*/ 			ClearFormTextRecordPortions();
/*N*/ 
/*N*/ 			const Rectangle& rFTBR = rXOut.GetFormTextBoundRect();
/*N*/ 			aFormTextBoundRect.Union(rFTBR);
/*N*/ 		}
/*N*/ 
/*N*/ 		rXOut.GetOutDev()->SetLayoutMode(nSavedLayoutMode);
/*N*/ 
/*N*/ 		//for (nParagraph = 0; nParagraph < nCnt; nParagraph++)
/*N*/ 		//{
/*N*/ 		//	aPoly = XOutCreatePolygon(aXPP[USHORT(nParagraph)], rXOut.GetOutDev());
/*N*/ 		//	nTextWidth = 0;
/*N*/ 		//
/*N*/ 		//	rOutliner.SetDrawPortionHdl(LINK(this,ImpTextPortionHandler,FormTextWidthHdl));
/*N*/ 		//	rOutliner.StripPortions();
/*N*/ 		//	rOutliner.SetDrawPortionHdl(LINK(this,ImpTextPortionHandler,FormTextDrawHdl));
/*N*/ 		//	rOutliner.StripPortions();
/*N*/ 		//	rOutliner.SetDrawPortionHdl(Link());
/*N*/ 		//
/*N*/ 		//	const Rectangle& rFTBR=rXOut.GetFormTextBoundRect();
/*N*/ 		//	aFormTextBoundRect.Union(rFTBR);
/*N*/ 		//}
/*N*/ 		
/*N*/ 		rXOut.GetOutDev()->SetFont(aFont);
/*N*/ 		rOutliner.Clear();
/*N*/ 	}
/*N*/ 	if (rTextObj.IsTextEditActive()) {
/*?*/ 		delete pPara;
/*N*/ 	}
/*N*/ }

// #101498# Record and sort all portions
/*N*/ IMPL_LINK(ImpTextPortionHandler, FormTextRecordPortionHdl, DrawPortionInfo*, pInfo)
/*N*/ {
/*N*/ 	if(pInfo->nPara == nParagraph) 
/*N*/ 	{
/*N*/ 		SortedAddFormTextRecordPortion(pInfo);
/*N*/ 	}
/*N*/ 
/*N*/ 	return 0;
/*N*/ }

// #101498# Helper class to remember text portions in sorted manner
/*N*/ class ImpRecordPortion
/*N*/ {
/*N*/ public:
/*N*/ 	Point						maPosition;
/*N*/ 	String						maText;
/*N*/ 	xub_StrLen					mnTextStart;
/*N*/ 	xub_StrLen					mnTextLength;
/*N*/ 	sal_uInt16					mnPara;
/*N*/ 	xub_StrLen					mnIndex;
/*N*/ 
/*N*/ 	// #102819# Here SvxFont needs to be used instead of Font
/*N*/ 	SvxFont						maFont;
/*N*/ 
/*N*/ 	sal_Int32*					mpDXArray;
/*N*/ 	sal_uInt8					mnBiDiLevel;
/*N*/ 
/*N*/ 	ImpRecordPortion(DrawPortionInfo* pInfo);
/*N*/ 	~ImpRecordPortion();
/*N*/ 
/*N*/ 	sal_uInt8 GetBiDiLevel() const { return mnBiDiLevel; }
/*N*/ };

/*N*/ ImpRecordPortion::ImpRecordPortion(DrawPortionInfo* pInfo)
/*N*/ :	maPosition(pInfo->rStartPos),
/*N*/ 	maText(pInfo->rText),
/*N*/ 	mnTextStart((xub_StrLen)pInfo->nTextStart),
/*N*/ 	mnTextLength((xub_StrLen)pInfo->nTextLen),
/*N*/ 	mnPara(pInfo->nPara),
/*N*/ 	mnIndex(pInfo->nIndex),
/*N*/ 	maFont(pInfo->rFont),
/*N*/ 	mpDXArray(NULL),
/*N*/ 	mnBiDiLevel(pInfo->GetBiDiLevel())
/*N*/ {
/*N*/ 	if(pInfo->pDXArray)
/*N*/ 	{
/*N*/ 		mpDXArray = new sal_Int32[pInfo->nTextLen];
/*N*/ 
/*N*/ 		for(sal_uInt32 a(0L); a < pInfo->nTextLen; a++)
/*N*/ 		{
/*N*/ 			mpDXArray[a] = pInfo->pDXArray[a];
/*N*/ 		}
/*N*/ 	}
/*N*/ }
/*N*/ 
/*N*/ ImpRecordPortion::~ImpRecordPortion()
/*N*/ {
/*N*/ 	if(mpDXArray)
/*N*/ 	{
/*N*/ 		delete[] mpDXArray;
/*N*/ 	}
/*N*/ }

/*N*/ // #101498# List classes for recording portions
/*N*/ DECLARE_LIST(ImpRecordPortionList, ImpRecordPortion*)//STRIP008 ;
/*N*/ DECLARE_LIST(ImpRecordPortionListList, ImpRecordPortionList*)//STRIP008 ;

/*N*/ // #101498# Draw recorded formtext along Poly
/*N*/ void ImpTextPortionHandler::DrawFormTextRecordPortions(Polygon aPoly)
/*N*/ {
/*N*/ 	sal_Int32 nTextWidth = -((sal_Int32)GetFormTextPortionsLength(pXOut->GetOutDev()));
/*N*/ 
/*N*/ 	ImpRecordPortionListList* pListList = (ImpRecordPortionListList*)mpRecordPortions;
/*N*/ 	if(pListList)
/*N*/ 	{
/*N*/ 		for(sal_uInt32 a(0L); a < pListList->Count(); a++)
/*N*/ 		{
/*N*/ 			ImpRecordPortionList* pList = pListList->GetObject(a);
/*N*/ 
/*N*/ 			for(sal_uInt32 b(0L); b < pList->Count(); b++)
/*N*/ 			{
/*N*/ 				ImpRecordPortion* pPortion = pList->GetObject(b);
/*N*/ 
/*N*/ 				DrawPortionInfo aNewInfo(
/*N*/ 					pPortion->maPosition,
/*N*/ 					pPortion->maText,
/*N*/ 					pPortion->mnTextStart,
/*N*/ 					pPortion->mnTextLength,
/*N*/ 					pPortion->maFont,
/*N*/ 					pPortion->mnPara,
/*N*/ 					pPortion->mnIndex,
/*N*/ 					pPortion->mpDXArray,
/*N*/ 					pPortion->mnBiDiLevel);
/*N*/ 
/*N*/ 				nTextWidth = pXOut->DrawFormText(&aNewInfo, aPoly, nTextWidth, bToLastPoint, bDraw);
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }

/*N*/ // #101498# Insert new portion sorted
/*N*/ void ImpTextPortionHandler::SortedAddFormTextRecordPortion(DrawPortionInfo* pInfo)
/*N*/ {
/*N*/ 	// get ListList and create on demand
/*N*/ 	ImpRecordPortionListList* pListList = (ImpRecordPortionListList*)mpRecordPortions;
/*N*/ 	if(!pListList)
/*N*/ 	{
/*N*/ 		mpRecordPortions = (void*)(new ImpRecordPortionListList(2, 2));
/*N*/ 		pListList = (ImpRecordPortionListList*)mpRecordPortions;
/*N*/ 	}
/*N*/ 
/*N*/ 	// create new portion
/*N*/ 	ImpRecordPortion* pNewPortion = new ImpRecordPortion(pInfo);
/*N*/ 
/*N*/ 	// look for the list where to insert new portion
/*N*/ 	ImpRecordPortionList* pList = 0L;
/*N*/ 
/*N*/ 	for(sal_uInt32 nListListIndex(0L); !pList && nListListIndex < pListList->Count(); nListListIndex++)
/*N*/ 	{
/*?*/ 		ImpRecordPortionList* pTmpList = pListList->GetObject(nListListIndex);
/*?*/ 
/*?*/ 		if(pTmpList->GetObject(0)->maPosition.Y() == pNewPortion->maPosition.Y())
/*?*/ 			pList = pTmpList;
/*N*/ 	}
/*N*/ 
/*N*/ 	if(!pList)
/*N*/ 	{
/*N*/ 		// no list for that Y-Coordinate yet, create a new one.
/*N*/ 		pList = new ImpRecordPortionList(8, 8);
/*N*/ 		pList->Insert(pNewPortion, LIST_APPEND);
/*N*/ 		pListList->Insert(pList, LIST_APPEND);
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		// found a list for that for that Y-Coordinate, sort in
/*?*/ 		sal_uInt32 nInsertInd(0L);
/*?*/ 
/*?*/ 		while(nInsertInd < pList->Count()
/*?*/ 			&& pList->GetObject(nInsertInd)->maPosition.X() < pNewPortion->maPosition.X())
/*?*/ 		{
/*?*/ 			nInsertInd++;
/*?*/ 		}
/*?*/ 
/*?*/ 		if(nInsertInd == pList->Count())
/*?*/ 			nInsertInd = LIST_APPEND;
/*?*/ 
/*?*/ 		pList->Insert(pNewPortion, nInsertInd);
/*N*/ 	}
/*N*/ }

// #101498# Calculate complete length of FormTextPortions
/*N*/ sal_uInt32 ImpTextPortionHandler::GetFormTextPortionsLength(OutputDevice* pOut)
/*N*/ {
/*N*/ 	sal_uInt32 nRetval(0L);
/*N*/ 
/*N*/ 	ImpRecordPortionListList* pListList = (ImpRecordPortionListList*)mpRecordPortions;
/*N*/ 	if(pListList)
/*N*/ 	{
/*N*/ 		for(sal_uInt32 a(0L); a < pListList->Count(); a++)
/*N*/ 		{
/*N*/ 			ImpRecordPortionList* pList = pListList->GetObject(a);
/*N*/ 
/*N*/ 			for(sal_uInt32 b(0L); b < pList->Count(); b++)
/*N*/ 			{
/*N*/ 				ImpRecordPortion* pPortion = pList->GetObject(b);
/*N*/ 
/*N*/ 				if(pPortion->mpDXArray)
/*N*/ 				{
/*N*/ 					if(pPortion->maFont.IsVertical() && pOut)
/*?*/ 						nRetval += pOut->GetTextHeight() * pPortion->mnTextLength;
/*N*/ 					else
/*N*/ 						nRetval += pPortion->mpDXArray[pPortion->mnTextLength - 1];
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	return nRetval;
/*N*/ }

// #101498# Cleanup recorded portions
/*N*/ void ImpTextPortionHandler::ClearFormTextRecordPortions()
/*N*/ {
/*N*/ 	ImpRecordPortionListList* pListList = (ImpRecordPortionListList*)mpRecordPortions;
/*N*/ 	if(pListList)
/*N*/ 	{
/*N*/ 		for(sal_uInt32 a(0L); a < pListList->Count(); a++)
/*N*/ 		{
/*N*/ 			ImpRecordPortionList* pList = pListList->GetObject(a);
/*N*/ 
/*N*/ 			for(sal_uInt32 b(0L); b < pList->Count(); b++)
/*N*/ 			{
/*N*/ 				delete pList->GetObject(b);
/*N*/ 			}
/*N*/ 
/*N*/ 			delete pList;
/*N*/ 		}
/*N*/ 
/*N*/ 		delete pListList;
/*N*/ 		mpRecordPortions = (void*)0L;
/*N*/ 	}
/*N*/ }




//IMPL_LINK(ImpTextPortionHandler, FormTextWidthHdl, DrawPortionInfo*, pInfo)
//{
//	// #101498# change calculation of nTextWidth
//	if(pInfo->nPara == nParagraph && pInfo->nTextLen)
//	{
//		// negative value is used because of the interface of
//		// XOutputDevice::ImpDrawFormText(...), please look there
//		// for more info.
//		nTextWidth -= pInfo->pDXArray[pInfo->nTextLen - 1];
//	}
//
//	return 0;
//}

//IMPL_LINK(ImpTextPortionHandler, FormTextDrawHdl, DrawPortionInfo*, pInfo)
//{
//	// #101498# Implementation of DrawFormText needs to be updated, too.
//	if(pInfo->nPara == nParagraph) 
//	{
//		nTextWidth = pXOut->DrawFormText(pInfo, aPoly, nTextWidth, bToLastPoint, bDraw);
//			//pInfo->rText, aPoly, pInfo->rFont, nTextWidth,
//			//bToLastPoint, bDraw, pInfo->pDXArray);
//	}
//
//	return 0;
//}
}
