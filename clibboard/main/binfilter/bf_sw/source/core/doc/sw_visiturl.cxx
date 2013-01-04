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

#ifndef _SFXDOCFILE_HXX //autogen
#include <bf_sfx2/docfile.hxx>
#endif
#ifndef _INETHIST_HXX //autogen
#include <bf_svtools/inethist.hxx>
#endif

#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _TXTINET_HXX //autogen
#include <txtinet.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _VISITURL_HXX
#include <visiturl.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
namespace binfilter {


/*N*/ SwURLStateChanged::SwURLStateChanged( const SwDoc* pD )
/*N*/ 	: pDoc( pD )
/*N*/ {
/*N*/ 	StartListening( *INetURLHistory::GetOrCreate() );
/*N*/ }

/*N*/ SwURLStateChanged::~SwURLStateChanged()
/*N*/ {
/*N*/ 	EndListening( *INetURLHistory::GetOrCreate() );
/*N*/ }

/*N*/ void SwURLStateChanged::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
/*N*/ {
/*N*/ 	if( rHint.ISA( INetURLHistoryHint ) && pDoc->GetRootFrm() )
/*N*/ 	{
/*N*/ 		// diese URL wurde veraendert:
/*N*/ 		const INetURLObject* pIURL = ((INetURLHistoryHint&)rHint).GetObject();
/*N*/ 		String sURL( pIURL->GetMainURL( INetURLObject::NO_DECODE ) ), sBkmk;
/*N*/ 
/*N*/ 		SwEditShell* pESh = pDoc->GetEditShell();
/*N*/ 
/*N*/ 		if( pDoc->GetDocShell() && pDoc->GetDocShell()->GetMedium() &&
/*N*/ 			// falls das unser Doc ist, kann es auch lokale Spruenge geben!
/*N*/ 			sURL == pDoc->GetDocShell()->GetMedium()->GetName() )
/*?*/ 			(sBkmk = pIURL->GetMark()).Insert( INET_MARK_TOKEN, 0 );
/*N*/ 
/*N*/ 		BOOL bAction = FALSE, bUnLockView = FALSE;
/*N*/ 		const SwFmtINetFmt* pItem;
/*N*/ 		const SwTxtINetFmt* pTxtAttr;
/*N*/ 		const SwTxtNode* pTxtNd;
/*N*/ 		USHORT n, nMaxItems = pDoc->GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
/*N*/ 		for( n = 0; n < nMaxItems; ++n )
/*N*/ 			if( 0 != (pItem = (SwFmtINetFmt*)pDoc->GetAttrPool().GetItem(
/*N*/ 				RES_TXTATR_INETFMT, n ) ) &&
/*N*/ 				( pItem->GetValue() == sURL ||
/*N*/ 					( sBkmk.Len() && pItem->GetValue() == sBkmk )) &&
/*N*/ 				0 != ( pTxtAttr = pItem->GetTxtINetFmt()) &&
/*N*/ 				0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) )
/*N*/ 			{
/*?*/ 				if( !bAction && pESh )
/*?*/ 				{
/*?*/ 					pESh->StartAllAction();
/*?*/ 					bAction = TRUE;
/*?*/ 					bUnLockView = !pESh->IsViewLocked();
/*?*/ 					pESh->LockView( TRUE );
/*?*/ 				}
/*?*/ 				((SwTxtINetFmt*)pTxtAttr)->SetValidVis( FALSE );
/*?*/ 				const SwTxtAttr* pAttr = pTxtAttr;
/*?*/ 				SwUpdateAttr aUpdateAttr( *pAttr->GetStart(),
/*?*/ 										  *pAttr->GetEnd(),
/*?*/ 										  RES_FMT_CHG );
/*?*/ 				((SwTxtNode*)pTxtNd)->SwCntntNode::Modify( &aUpdateAttr,
/*?*/ 															&aUpdateAttr );
/*?*/ 			}
/*N*/ 
/*N*/ 		if( bAction )
/*?*/ 			pESh->EndAllAction();
/*N*/  		if( bUnLockView )
/*?*/      		pESh->LockView( FALSE );
/*N*/ 	}
/*N*/ }

	// erfrage ob die URL besucht war. Uebers Doc, falls nur ein Bookmark
	// angegeben ist. Dann muss der Doc. Name davor gesetzt werden!
/*N*/ BOOL SwDoc::IsVisitedURL( const String& rURL ) const
/*N*/ {
/*N*/ #if OSL_DEBUG_LEVEL > 1
/*N*/ 	static long nTmp = 0;
/*N*/ 	++nTmp;
/*N*/ #endif
/*N*/ 
/*N*/ 	BOOL bRet = FALSE;
/*N*/ 	if( rURL.Len() )
/*N*/ 	{
/*N*/ 		INetURLHistory *pHist = INetURLHistory::GetOrCreate();
/*N*/ 		if( '#' == rURL.GetChar( 0 ) && pDocShell && pDocShell->GetMedium() )
/*N*/ 		{
/*N*/ 			INetURLObject aIObj( pDocShell->GetMedium()->GetURLObject() );
/*N*/ 			aIObj.SetMark( rURL.Copy( 1 ) );
/*N*/ 			bRet = pHist->QueryUrl( aIObj );
/*N*/ 		}
/*N*/ 		else
/*N*/ 			bRet = pHist->QueryUrl( rURL );
/*N*/ 
/*N*/ 		// dann  wollen wird auch ueber Statusaenderungen in der History
/*N*/ 		// informiert werden!
/*N*/ 		if( !pURLStateChgd )
/*N*/ 		{
/*N*/ 			SwDoc* pD = (SwDoc*)this;
/*N*/ 			pD->pURLStateChgd = new SwURLStateChanged( this );
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return bRet;
/*N*/ }



}
