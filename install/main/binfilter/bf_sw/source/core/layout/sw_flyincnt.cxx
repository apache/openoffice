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




#ifdef _MSC_VER
#pragma hdrstop
#endif


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "doc.hxx"
#include "frmtool.hxx"
#include "frmfmt.hxx"
#include "hints.hxx"

#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#include "txtfrm.hxx"		//fuer IsLocked()
#include "flyfrms.hxx"
namespace binfilter {

//aus FlyCnt.cxx
/*N*/ void DeepCalc( const SwFrm *pFrm );

/*************************************************************************
|*
|*	SwFlyInCntFrm::SwFlyInCntFrm(), ~SwFlyInCntFrm()
|*
|*	Ersterstellung		MA 01. Dec. 92
|*	Letzte Aenderung	MA 09. Apr. 99
|*
|*************************************************************************/
/*N*/ SwFlyInCntFrm::SwFlyInCntFrm( SwFlyFrmFmt *pFmt, SwFrm *pAnch ) :
/*N*/ 	SwFlyFrm( pFmt, pAnch )
/*N*/ {
/*N*/ 	bInCnt = bInvalidLayout = bInvalidCntnt = TRUE;
/*N*/     SwTwips nRel = pFmt->GetVertOrient().GetPos();
/*N*/ #ifdef VERTICAL_LAYOUT
/*N*/     if( pAnch && pAnch->IsVertical() )
/*?*/         aRelPos.X() = pAnch->IsReverse() ? nRel : -nRel;
/*N*/     else
/*N*/ #endif
/*N*/     aRelPos.Y() = nRel;
/*N*/ }

/*N*/ SwFlyInCntFrm::~SwFlyInCntFrm()
/*N*/ {
/*N*/ 	//und Tschuess.
/*N*/ 	if ( !GetFmt()->GetDoc()->IsInDtor() && GetAnchor() )
/*N*/ 	{
/*N*/ 		SwRect aTmp( AddSpacesToFrm() );
/*N*/ 		SwFlyInCntFrm::NotifyBackground( FindPageFrm(), aTmp, PREP_FLY_LEAVE );
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::SetRefPoint(),
|*
|*	Ersterstellung		MA 01. Dec. 92
|*	Letzte Aenderung	MA 06. Aug. 95
|*
|*************************************************************************/
/*M*/ void SwFlyInCntFrm::SetRefPoint( const Point& rPoint, const Point& rRelAttr,
/*M*/ 	const Point& rRelPos )
/*M*/ {
/*M*/ 	ASSERT( rPoint != aRef || rRelAttr != aRelPos, "SetRefPoint: no change" );
/*M*/     SwFlyNotify *pNotify = NULL;
/*M*/     // No notify at a locked fly frame, if a fly frame is locked, there's
/*M*/     // already a SwFlyNotify object on the stack (MakeAll).
/*M*/     if( !IsLocked() )
/*M*/         pNotify = new SwFlyNotify( this );
/*M*/ 	aRef = rPoint;
/*M*/ 	aRelPos = rRelAttr;
/*M*/ #ifdef VERTICAL_LAYOUT
/*M*/     SWRECTFN( GetAnchor() )
/*M*/     (Frm().*fnRect->fnSetPos)( rPoint + rRelPos );
/*M*/ #else
/*M*/ 	Frm().Pos( rPoint + rRelPos );
/*M*/ #endif
/*
	//Kein InvalidatePos hier, denn das wuerde dem Cntnt ein Prepare
	//senden - dieser hat uns aber gerade gerufen.
	//Da der Frm aber durchaus sein Position wechseln kann, muss hier
 	//der von ihm abdeckte Window-Bereich invalidiert werden damit keine
 	//Reste stehenbleiben.
 	//Fix: Nicht fuer PreView-Shells, dort ist es nicht notwendig und
 	//fuehrt zu fiesen Problemen (Der Absatz wird nur formatiert weil
	//er gepaintet wird und der Cache uebergelaufen ist, beim Paint durch
	//das Invalidate wird der Absatz formatiert weil...)
 	if ( Frm().HasArea() && GetShell()->ISA(SwCrsrShell) )
		GetShell()->InvalidateWindows( Frm() );
*/
/*M*/     if( pNotify )
/*M*/     {
/*M*/         InvalidatePage();
/*M*/         bValidPos = FALSE;
/*M*/         bInvalid  = TRUE;
/*M*/         Calc();
/*M*/         delete pNotify;
/*M*/     }
/*M*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::Modify()
|*
|*	Ersterstellung		MA 16. Dec. 92
|*	Letzte Aenderung	MA 02. Sep. 93
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew )
/*N*/ {
/*N*/ 	BOOL bCallPrepare = FALSE;
/*N*/ 	USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
/*N*/ 	if( RES_ATTRSET_CHG == nWhich )
/*N*/ 	{
/*N*/ 		if( SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->
/*N*/ 			GetItemState( RES_SURROUND, FALSE ) ||
/*N*/ 			SFX_ITEM_SET == ((SwAttrSetChg*)pNew)->GetChgSet()->
/*N*/ 			GetItemState( RES_FRMMACRO, FALSE ) )
/*N*/ 		{
/*N*/ 			SwAttrSetChg aOld( *(SwAttrSetChg*)pOld );
/*N*/ 			SwAttrSetChg aNew( *(SwAttrSetChg*)pNew );
/*N*/ 
/*N*/ 			aOld.ClearItem( RES_SURROUND );
/*N*/ 			aNew.ClearItem( RES_SURROUND );
/*N*/ 			aOld.ClearItem( RES_FRMMACRO );
/*N*/ 			aNew.ClearItem( RES_FRMMACRO );
/*N*/ 			if( aNew.Count() )
/*N*/ 			{
/*?*/ 				SwFlyFrm::Modify( &aOld, &aNew );
/*?*/ 				bCallPrepare = TRUE;
/*N*/ 			}
/*N*/ 		}
/*N*/ 		else if( ((SwAttrSetChg*)pNew)->GetChgSet()->Count())
/*N*/ 		{
/*N*/ 			SwFlyFrm::Modify( pOld, pNew );
/*N*/ 			bCallPrepare = TRUE;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else if( nWhich != RES_SURROUND && RES_FRMMACRO != nWhich )
/*N*/ 	{
/*N*/ 		SwFlyFrm::Modify( pOld, pNew );
/*N*/ 		bCallPrepare = TRUE;
/*N*/ 	}
/*N*/ 
/*N*/ 	if ( bCallPrepare && GetAnchor() )
/*N*/ 		GetAnchor()->Prepare( PREP_FLY_ATTR_CHG, GetFmt() );
/*N*/ }
/*************************************************************************
|*
|*	SwFlyInCntFrm::Format()
|*
|*	Beschreibung:		Hier wird der Inhalt initial mit Formatiert.
|*	Ersterstellung		MA 16. Dec. 92
|*	Letzte Aenderung	MA 19. May. 93
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::Format( const SwBorderAttrs *pAttrs )
/*N*/ {
/*N*/ 	if ( !Frm().Height() )
/*N*/ 	{
/*?*/ 		Lock();	//nicht hintenherum den Anker formatieren.
/*?*/ 		SwCntntFrm *pCntnt = ContainsCntnt();
/*?*/ 		while ( pCntnt )
/*?*/ 		{	pCntnt->Calc();
/*?*/ 			pCntnt = pCntnt->GetNextCntntFrm();
/*?*/ 		}
/*?*/ 		Unlock();
/*N*/ 	}
/*N*/ 	SwFlyFrm::Format( pAttrs );
/*N*/ }
/*************************************************************************
|*
|*	SwFlyInCntFrm::MakeFlyPos()
|*
|*	Beschreibung		Im Unterschied zu anderen Frms wird hier nur die
|*		die RelPos berechnet. Die absolute Position wird ausschliesslich
|*		per SetAbsPos errechnet.
|*	Ersterstellung		MA 03. Dec. 92
|*	Letzte Aenderung	MA 12. Apr. 96
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::MakeFlyPos()
/*N*/ {
/*N*/ 	if ( !bValidPos )
/*N*/ 	{
/*N*/ 		if ( !GetAnchor()->IsTxtFrm() || !((SwTxtFrm*)GetAnchor())->IsLocked() )
/*N*/ 			::binfilter::DeepCalc( GetAnchor() );
/*N*/ 		if( GetAnchor()->IsTxtFrm() )
/*N*/ 			((SwTxtFrm*)GetAnchor())->GetFormatted();
/*N*/ 		bValidPos = TRUE;
/*N*/ 		SwFlyFrmFmt *pFmt = (SwFlyFrmFmt*)GetFmt();
/*N*/ 		const SwFmtVertOrient &rVert = pFmt->GetVertOrient();
/*N*/ 		//Und ggf. noch die aktuellen Werte im Format updaten, dabei darf
/*N*/ 		//zu diesem Zeitpunkt natuerlich kein Modify verschickt werden.
/*N*/ #ifdef VERTICAL_LAYOUT
/*N*/         SWRECTFN( GetAnchor() )
/*N*/         SwTwips nOld = rVert.GetPos();
/*N*/         SwTwips nAct = bVert ? -aRelPos.X() : aRelPos.Y();
/*N*/         if( bRev )
/*?*/             nAct = -nAct;
/*N*/         if( nAct != nOld )
/*N*/ 		{
/*N*/ 			SwFmtVertOrient aVert( rVert );
/*N*/             aVert.SetPos( nAct );
/*N*/ #else
/*N*/ 		if ( rVert.GetPos() != aRelPos.Y() )
/*N*/ 		{
/*N*/ 			SwFmtVertOrient aVert( rVert );
/*N*/ 			aVert.SetPos( aRelPos.Y() );
/*N*/ #endif
/*N*/ 			pFmt->LockModify();
/*N*/ 			pFmt->SetAttr( aVert );
/*N*/ 			pFmt->UnlockModify();
/*N*/ 		}
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::NotifyBackground()
|*
|*	Ersterstellung		MA 03. Dec. 92
|*	Letzte Aenderung	MA 26. Aug. 93
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::NotifyBackground( SwPageFrm *, const SwRect& rRect,
/*N*/ 									   PrepareHint eHint)
/*N*/ {
/*N*/ 	if ( eHint == PREP_FLY_ATTR_CHG )
/*?*/ 		GetAnchor()->Prepare( PREP_FLY_ATTR_CHG );
/*N*/ 	else
/*N*/ 		GetAnchor()->Prepare( eHint, (void*)&rRect );
/*N*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::GetRelPos()
|*
|*	Ersterstellung		MA 04. Dec. 92
|*	Letzte Aenderung	MA 04. Dec. 92
|*
|*************************************************************************/
/*N*/ const Point &SwFlyInCntFrm::GetRelPos() const
/*N*/ {
/*N*/ 	Calc();
/*N*/ 	return GetCurRelPos();
/*N*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::RegistFlys()
|*
|*	Ersterstellung		MA 26. Nov. 93
|*	Letzte Aenderung	MA 26. Nov. 93
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::RegistFlys()
/*N*/ {
/*N*/ 	// vgl. SwRowFrm::RegistFlys()
/*N*/ 	SwPageFrm *pPage = FindPageFrm();
/*N*/ 	ASSERT( pPage, "Flys ohne Seite anmelden?" );
/*N*/ 	::binfilter::RegistFlys( pPage, this );
/*N*/ }

/*************************************************************************
|*
|*	SwFlyInCntFrm::MakeAll()
|*
|*	Ersterstellung		MA 18. Feb. 94
|*	Letzte Aenderung	MA 13. Jun. 96
|*
|*************************************************************************/
/*N*/ void SwFlyInCntFrm::MakeAll()
/*N*/ {
/*N*/ 	if ( !GetAnchor() || IsLocked() || IsColLocked() || !FindPageFrm() )
/*N*/ 		return;
/*N*/ 
/*N*/ 	Lock();	//Der Vorhang faellt
/*N*/ 
/*N*/ 		//uebernimmt im DTor die Benachrichtigung
/*N*/ 	const SwFlyNotify aNotify( this );
/*N*/ 	SwBorderAttrAccess aAccess( SwFrm::GetCache(), this );
/*N*/ 	const SwBorderAttrs &rAttrs = *aAccess.Get();
/*N*/ 	const Size &rSz = rAttrs.GetSize();
/*N*/ 	const SwFmtFrmSize &rFrmSz = GetFmt()->GetFrmSize();
/*N*/ 
/*N*/ 	if ( IsClipped() )
/*N*/ 		bValidSize = bHeightClipped = bWidthClipped = FALSE;
/*N*/ 
/*N*/ 	while ( !bValidPos || !bValidSize || !bValidPrtArea )
/*N*/ 	{
/*N*/ 		//Nur einstellen wenn das Flag gesetzt ist!!
/*N*/ 		if ( !bValidSize )
/*N*/ 		{
/*N*/ 			bValidPrtArea = FALSE;
/*N*/ 			long nOldWidth = aFrm.Width();
/*N*/ 			aFrm.Width( CalcRel( rFrmSz ).Width() );
/*N*/ 
/*N*/ 			if ( aFrm.Width() > nOldWidth )
/*N*/ 				//Damit sich der Inhalt anpasst
/*N*/ 				aFrm.Height( CalcRel( rFrmSz ).Height() );
/*N*/ 		}
/*N*/ 
/*N*/ 		if ( !bValidPrtArea )
/*N*/ 			MakePrtArea( rAttrs );
/*N*/ 
/*N*/ 		if ( !bValidSize )
/*N*/ 			Format( &rAttrs );
/*N*/ 
/*N*/ 		if ( !bValidPos )
/*N*/ 			MakeFlyPos();
/*N*/ 
/*N*/ 		if ( bValidPos && bValidSize )
/*N*/ 		{
/*N*/ 			SwFrm *pFrm = GetAnchor();
/*N*/ 			if (
/*N*/ //MA 03. Apr. 96 fix(26652), Das trifft uns bestimmt nocheinmal
/*N*/ //			!pFrm->IsMoveable() &&
/*N*/ 				 Frm().Left() == (pFrm->Frm().Left()+pFrm->Prt().Left()) &&
/*N*/ 				 Frm().Width() > pFrm->Prt().Width() )
/*N*/ 			{
/*N*/ 				Frm().Width( pFrm->Prt().Width() );
/*N*/ 				bValidPrtArea = FALSE;
/*N*/ 				bWidthClipped = TRUE;
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 	Unlock();
/*N*/ }

}
