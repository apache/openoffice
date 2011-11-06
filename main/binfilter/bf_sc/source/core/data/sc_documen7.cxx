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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

// INCLUDE ---------------------------------------------------------------


#if defined( WNT ) && defined( erBEEP )
#include <svwin.h>
#define erBEEPER() Beep( 666, 66 )
#else
#define erBEEPER()
#endif

#include "document.hxx"
#include "bcaslot.hxx"
#include "cell.hxx"
#include "scmod.hxx"   		// SC_MOD
#include "inputopt.hxx" 	// GetExpandRefs
#include "conditio.hxx"
#include "bclist.hxx"



#include "globstr.hrc"
namespace binfilter {

extern const ScFormulaCell* pLastFormulaTreeTop;	// cellform.cxx Err527 WorkAround

// STATIC DATA -----------------------------------------------------------

#ifdef erDEBUG
ULONG erCountBCAInserts = 0;
ULONG erCountBCAFinds = 0;
#endif

// -----------------------------------------------------------------------

/*N*/ void ScDocument::StartListeningArea( const ScRange& rRange,
/*N*/ 		SfxListener* pListener
/*N*/ 	)
/*N*/ {
/*N*/ 	if ( pBASM )
/*N*/ 		pBASM->StartListeningArea( rRange, pListener );
/*N*/ }


/*N*/ void ScDocument::EndListeningArea( const ScRange& rRange,
/*N*/ 		SfxListener* pListener
/*N*/ 	)
/*N*/ {
/*N*/ 	if ( pBASM )
/*N*/ 		pBASM->EndListeningArea( rRange, pListener );
/*N*/ }


/*N*/ void ScDocument::Broadcast( ULONG nHint, const ScAddress& rAddr,
/*N*/ 		ScBaseCell* pCell
/*N*/ 	)
/*N*/ {
/*N*/ 	if ( !pBASM )
/*N*/ 		return ;	// Clipboard or Undo
/*N*/     ScHint aHint( nHint, rAddr, pCell );
/*N*/     Broadcast( aHint );
/*N*/ }


/*N*/ void ScDocument::Broadcast( const ScHint& rHint )
/*N*/ {
/*N*/ 	if ( !pBASM )
/*N*/ 		return ;	// Clipboard or Undo
/*N*/ 	if ( !nHardRecalcState )
/*N*/ 	{
/*N*/ 		BOOL bIsBroadcasted = FALSE;
/*N*/         ScBaseCell* pCell = rHint.GetCell();
/*N*/ 		if ( pCell )
/*N*/ 		{
/*N*/ 			ScBroadcasterList* pBC = pCell->GetBroadcaster();
/*N*/ 			if ( pBC )
/*N*/ 			{
/*N*/ 				pBC->Broadcast( rHint );
/*N*/ 				bIsBroadcasted = TRUE;
/*N*/ 			}
/*N*/ 		}
/*N*/ 		if ( pBASM->AreaBroadcast( rHint ) || bIsBroadcasted )
/*N*/ 			TrackFormulas( rHint.GetId() );
/*N*/ 	}
/*N*/ 
/*N*/ 	//	Repaint fuer bedingte Formate mit relativen Referenzen:
/*N*/ 	if ( pCondFormList && rHint.GetAddress() != BCA_BRDCST_ALWAYS )
/*?*/ 		pCondFormList->SourceChanged( rHint.GetAddress() );
/*N*/ }


/*N*/ void ScDocument::AreaBroadcast( const ScHint& rHint )
/*N*/ {
/*N*/ 	if ( !pBASM )
/*N*/ 		return ;	// Clipboard or Undo
/*N*/ 	if ( !nHardRecalcState )
/*N*/ 	{
/*N*/ 		if ( pBASM->AreaBroadcast( rHint ) )
/*N*/ 			TrackFormulas( rHint.GetId() );
/*N*/ 	}
/*N*/ 
/*N*/ 	//	Repaint fuer bedingte Formate mit relativen Referenzen:
/*N*/ 	if ( pCondFormList && rHint.GetAddress() != BCA_BRDCST_ALWAYS )
/*N*/ 		pCondFormList->SourceChanged( rHint.GetAddress() );
/*N*/ }


/*N*/ void ScDocument::AreaBroadcastInRange( const ScRange& rRange, const ScHint& rHint )
/*N*/ {
/*N*/     if ( !pBASM )
/*N*/         return ;    // Clipboard or Undo
/*N*/     if ( !nHardRecalcState )
/*N*/     {
/*N*/         if ( pBASM->AreaBroadcastInRange( rRange, rHint ) )
/*N*/             TrackFormulas( rHint.GetId() );
/*N*/     }
/*N*/ 
/*N*/     // Repaint for conditional formats containing relative references.
/*N*/     //! This is _THE_ bottle neck!
/*N*/     if ( pCondFormList )
/*N*/     {
/*N*/         USHORT nCol, nRow, nTab, nCol1, nRow1, nTab1, nCol2, nRow2, nTab2;
/*N*/         rRange.GetVars( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
/*N*/         ScAddress aAddress( rRange.aStart );
/*N*/         for ( nTab = nTab1; nTab <= nTab2; ++nTab )
/*N*/         {
/*N*/             aAddress.SetTab( nTab );
/*N*/             for ( nCol = nCol1; nCol <= nCol2; ++nCol )
/*N*/             {
/*N*/                 aAddress.SetCol( nCol );
/*N*/                 for ( nRow = nRow1; nRow <= nRow2; ++nRow )
/*N*/                 {
/*N*/                     aAddress.SetRow( nRow );
/*N*/                     pCondFormList->SourceChanged( aAddress );
/*N*/                 }
/*N*/             }
/*N*/         }
/*N*/     }
/*N*/ }


/*N*/ void ScDocument::DelBroadcastAreasInRange( const ScRange& rRange )
/*N*/ {
/*N*/ 	if ( pBASM )
/*N*/ 		pBASM->DelBroadcastAreasInRange( rRange );
/*N*/ }

/*N*/ void ScDocument::StartListeningCell( const ScAddress& rAddress,
/*N*/ 											SfxListener* pListener )
/*N*/ {
/*N*/ 	DBG_ASSERT(pListener, "StartListeningCell: pListener Null");
/*N*/ 	USHORT nTab = rAddress.Tab();
/*N*/ 	if (pTab[nTab])
/*N*/ 		pTab[nTab]->StartListening( rAddress, pListener );
/*N*/ }

/*N*/ void ScDocument::EndListeningCell( const ScAddress& rAddress,
/*N*/ 											SfxListener* pListener )
/*N*/ {
/*N*/ 	DBG_ASSERT(pListener, "EndListeningCell: pListener Null");
/*N*/ 	USHORT nTab = rAddress.Tab();
/*N*/ 	if (pTab[nTab])
/*N*/ 		pTab[nTab]->EndListening( rAddress, pListener );
/*N*/ }


/*N*/ void ScDocument::PutInFormulaTree( ScFormulaCell* pCell )
/*N*/ {
/*N*/ 	DBG_ASSERT( pCell, "PutInFormulaTree: pCell Null" );
/*N*/ 	RemoveFromFormulaTree( pCell );
/*N*/ 	// anhaengen
/*N*/ 	if ( pEOFormulaTree )
/*N*/ 		pEOFormulaTree->SetNext( pCell );
/*N*/ 	else
/*N*/ 		pFormulaTree = pCell;				// kein Ende, kein Anfang..
/*N*/ 	pCell->SetPrevious( pEOFormulaTree );
/*N*/ 	pCell->SetNext( 0 );
/*N*/ 	pEOFormulaTree = pCell;
/*N*/ 	nFormulaCodeInTree += pCell->GetCode()->GetCodeLen();
/*N*/ }


/*N*/ void ScDocument::RemoveFromFormulaTree( ScFormulaCell* pCell )
/*N*/ {
/*N*/ 	DBG_ASSERT( pCell, "RemoveFromFormulaTree: pCell Null" );
/*N*/ 	ScFormulaCell* pPrev = pCell->GetPrevious();
/*N*/ 	// wenn die Zelle die erste oder sonstwo ist
/*N*/ 	if ( pPrev || pFormulaTree == pCell )
/*N*/ 	{
/*N*/ 		ScFormulaCell* pNext = pCell->GetNext();
/*N*/ 		if ( pPrev )
/*N*/ 			pPrev->SetNext( pNext );		// gibt Vorlaeufer
/*N*/ 		else
/*N*/ 			pFormulaTree = pNext;			// ist erste Zelle
/*N*/ 		if ( pNext )
/*N*/ 			pNext->SetPrevious( pPrev );	// gibt Nachfolger
/*N*/ 		else
/*N*/ 			pEOFormulaTree = pPrev;			// ist letzte Zelle
/*N*/ 		pCell->SetPrevious( 0 );
/*N*/ 		pCell->SetNext( 0 );
/*N*/ 		USHORT nRPN = pCell->GetCode()->GetCodeLen();
/*N*/ 		if ( nFormulaCodeInTree >= nRPN )
/*N*/ 			nFormulaCodeInTree -= nRPN;
/*N*/ 		else
/*N*/ 		{
/*N*/ 			DBG_ERRORFILE( "RemoveFromFormulaTree: nFormulaCodeInTree < nRPN" );
/*N*/ 			nFormulaCodeInTree = 0;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else if ( !pFormulaTree && nFormulaCodeInTree )
/*N*/ 	{
/*?*/ 		DBG_ERRORFILE( "!pFormulaTree && nFormulaCodeInTree != 0" );
/*?*/ 		nFormulaCodeInTree = 0;
/*N*/ 	}
/*N*/ }


/*N*/ BOOL ScDocument::IsInFormulaTree( ScFormulaCell* pCell ) const
/*N*/ {
/*N*/ 	return pCell->GetPrevious() || pFormulaTree == pCell;
/*N*/ }




/*N*/ void ScDocument::ClearFormulaTree()
/*N*/ {
/*N*/ 	ScFormulaCell* pCell;
/*N*/ 	ScFormulaCell* pTree = pFormulaTree;
/*N*/ 	while ( pTree )
/*N*/ 	{
/*N*/ 		pCell = pTree;
/*N*/ 		pTree = pCell->GetNext();
/*N*/ 		if ( !pCell->GetCode()->IsRecalcModeAlways() )
/*N*/ 			RemoveFromFormulaTree( pCell );
/*N*/ 	}
/*N*/ }


/*N*/ void ScDocument::AppendToFormulaTrack( ScFormulaCell* pCell )
/*N*/ {
/*N*/ 	DBG_ASSERT( pCell, "AppendToFormulaTrack: pCell Null" );
/*N*/ 	// Zelle kann nicht in beiden Listen gleichzeitig sein
/*N*/ 	RemoveFromFormulaTrack( pCell );
/*N*/ 	RemoveFromFormulaTree( pCell );
/*N*/ 	if ( pEOFormulaTrack )
/*N*/ 		pEOFormulaTrack->SetNextTrack( pCell );
/*N*/ 	else
/*N*/ 		pFormulaTrack = pCell;				// kein Ende, kein Anfang..
/*N*/ 	pCell->SetPreviousTrack( pEOFormulaTrack );
/*N*/ 	pCell->SetNextTrack( 0 );
/*N*/ 	pEOFormulaTrack = pCell;
/*N*/ 	++nFormulaTrackCount;
/*N*/ }


/*N*/ void ScDocument::RemoveFromFormulaTrack( ScFormulaCell* pCell )
/*N*/ {
/*N*/ 	DBG_ASSERT( pCell, "RemoveFromFormulaTrack: pCell Null" );
/*N*/ 	ScFormulaCell* pPrev = pCell->GetPreviousTrack();
/*N*/ 	// wenn die Zelle die erste oder sonstwo ist
/*N*/ 	if ( pPrev || pFormulaTrack == pCell )
/*N*/ 	{
/*N*/ 		ScFormulaCell* pNext = pCell->GetNextTrack();
/*N*/ 		if ( pPrev )
/*?*/ 			pPrev->SetNextTrack( pNext );		// gibt Vorlaeufer
/*N*/ 		else
/*N*/ 			pFormulaTrack = pNext;				// ist erste Zelle
/*N*/ 		if ( pNext )
/*N*/ 			pNext->SetPreviousTrack( pPrev );	// gibt Nachfolger
/*N*/ 		else
/*N*/ 			pEOFormulaTrack = pPrev;  			// ist letzte Zelle
/*N*/ 		pCell->SetPreviousTrack( 0 );
/*N*/ 		pCell->SetNextTrack( 0 );
/*N*/ 		--nFormulaTrackCount;
/*N*/ 	}
/*N*/ }


/*N*/ BOOL ScDocument::IsInFormulaTrack( ScFormulaCell* pCell ) const
/*N*/ {
/*N*/ 	return pCell->GetPreviousTrack() || pFormulaTrack == pCell;
/*N*/ }


/*
	Der erste wird gebroadcastet,
	die dadurch entstehenden werden durch das Notify an den Track gehaengt.
	Der nachfolgende broadcastet wieder usw.
	View stoesst Interpret an.
 */
/*N*/ void ScDocument::TrackFormulas( ULONG nHintId )
/*N*/ {
/*N*/ 
/*N*/ 	if ( pFormulaTrack )
/*N*/ 	{
///*N*/ 		BOOL bWasWaiting = Application::IsWait();
///*N*/ 		if ( !bWasWaiting )
///*N*/ 			Application::EnterWait();
/*N*/ 		erBEEPER();
/*N*/ 		ScBroadcasterList* pBC;
/*N*/ 		ScFormulaCell* pTrack;
/*N*/ 		ScFormulaCell* pNext;
/*N*/ 		BOOL bIsChanged = TRUE;
/*N*/ 		pTrack = pFormulaTrack;
/*N*/ 		do
/*N*/ 		{
/*N*/ 			ScHint aHint( nHintId, pTrack->aPos, pTrack );
/*N*/ 			if ( pBC = pTrack->GetBroadcaster() )
/*N*/ 				pBC->Broadcast( aHint );
/*N*/ 			pBASM->AreaBroadcast( aHint );
/*N*/ 			//	Repaint fuer bedingte Formate mit relativen Referenzen:
/*N*/ 			if ( pCondFormList )
/*N*/ 				pCondFormList->SourceChanged( pTrack->aPos );
/*N*/ 			pTrack = pTrack->GetNextTrack();
/*N*/ 		} while ( pTrack );
/*N*/ 		pTrack = pFormulaTrack;
/*N*/ 		BOOL bHaveForced = FALSE;
/*N*/ 		do
/*N*/ 		{
/*N*/ 			pNext = pTrack->GetNextTrack();
/*N*/ 			RemoveFromFormulaTrack( pTrack );
/*N*/ 			PutInFormulaTree( pTrack );
/*N*/ 			if ( pTrack->GetCode()->IsRecalcModeForced() )
/*N*/ 				bHaveForced = TRUE;
/*N*/ 			pTrack = pNext;
/*N*/ 		} while ( pTrack );
/*N*/ 		if ( bHaveForced )
/*N*/ 		{
/*?*/ 			SetForcedFormulas( TRUE );
/*?*/ 			if ( bAutoCalc && !IsAutoCalcShellDisabled() && !IsInInterpreter()
/*?*/ 					&& !IsCalculatingFormulaTree() )
{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 /*?*/ 				CalcFormulaTree( TRUE );
/*?*/ 			else
/*?*/ 				SetForcedFormulaPending( TRUE );
/*N*/ 		}
///*N*/ 		if ( !bWasWaiting )
///*N*/ 			Application::LeaveWait();
/*N*/ 	}
/*N*/ 	DBG_ASSERT( nFormulaTrackCount==0, "TrackFormulas: nFormulaTrackCount!=0" );
/*N*/ }


/*N*/ void ScDocument::StartAllListeners()
/*N*/ {
/*N*/ 	for ( USHORT i = 0; i <= MAXTAB; ++i )
/*N*/ 		if ( pTab[i] )
/*N*/ 			pTab[i]->StartAllListeners();
/*N*/ }

/*N*/ void ScDocument::UpdateBroadcastAreas( UpdateRefMode eUpdateRefMode,
/*N*/ 		const ScRange& rRange, short nDx, short nDy, short nDz
/*N*/ 	)
/*N*/ {
/*N*/ 	BOOL bExpandRefsOld = IsExpandRefs();
/*N*/ 	if ( eUpdateRefMode == URM_INSDEL && (nDx > 0 || nDy > 0 || nDz > 0) )
/*N*/ 		SetExpandRefs( SC_MOD()->GetInputOptions().GetExpandRefs() );
/*N*/ 	SetExpandRefs( bExpandRefsOld );
/*N*/ }

/*N*/ void ScDocument::SetAutoCalc( BOOL bNewAutoCalc )
/*N*/ {
/*N*/ 	BOOL bOld = bAutoCalc;
/*N*/ 	bAutoCalc = bNewAutoCalc;
/*N*/ 	if ( !bOld && bNewAutoCalc && bHasForcedFormulas )
/*N*/ 	{
/*?*/ 		if ( IsAutoCalcShellDisabled() )
/*?*/ 			SetForcedFormulaPending( TRUE );
/*?*/ 		else if ( !IsInInterpreter() )
DBG_BF_ASSERT(0, "STRIP"); //STRIP001 /*?*/ 			CalcFormulaTree( TRUE );
/*N*/ 	}
/*N*/ }



}
