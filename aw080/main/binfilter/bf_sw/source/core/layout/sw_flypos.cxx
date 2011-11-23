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

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif


#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#include "flypos.hxx"
#include "frmfmt.hxx"
#include "dcontact.hxx"
#include "flyfrm.hxx"
#include "dflyobj.hxx"
#include "ndindex.hxx"
namespace binfilter {



/*N*/ SV_IMPL_OP_PTRARR_SORT( SwPosFlyFrms, SwPosFlyFrmPtr )

/*N*/ SwPosFlyFrm::SwPosFlyFrm( const SwNodeIndex& rIdx, const SwFrmFmt* pFmt,
/*N*/ 							USHORT nArrPos )
/*N*/ 	: pNdIdx( (SwNodeIndex*) &rIdx ), pFrmFmt( pFmt )
/*N*/ {
/*N*/ 	BOOL bFnd = FALSE;
/*N*/ 	const SwFmtAnchor& rAnchor = pFmt->GetAnchor();
/*N*/ 	if( FLY_PAGE == rAnchor.GetAnchorId() )
/*?*/ 		pNdIdx = new SwNodeIndex( rIdx );
/*N*/ 	else if( pFmt->GetDoc()->GetRootFrm() )
/*N*/ 	{
/*N*/ 		SwClientIter aIter( (SwFmt&)*pFmt );
/*N*/ 		if( RES_FLYFRMFMT == pFmt->Which() )
/*N*/ 		{
/*N*/ 			// Schauen, ob es ein SdrObject dafuer gibt
/*N*/ 			if( aIter.First( TYPE( SwFlyFrm) ) )
/*N*/ 				nOrdNum = ((SwFlyFrm*)aIter())->GetVirtDrawObj()->GetOrdNum(),
/*N*/ 				bFnd = TRUE;
/*N*/ 		}
/*N*/ 		else if( RES_DRAWFRMFMT == pFmt->Which() )
/*N*/ 		{
/*N*/ 			// Schauen, ob es ein SdrObject dafuer gibt
/*N*/ 			if( aIter.First( TYPE(SwDrawContact) ) )
/*N*/ 				nOrdNum = ((SwDrawContact*)aIter())->GetMaster()->GetOrdNum(),
/*N*/ 				bFnd = TRUE;
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	if( !bFnd )
/*N*/ 	{
/*N*/ 		nOrdNum = pFmt->GetDoc()->GetSpzFrmFmts()->Count();
/*N*/ 		nOrdNum += nArrPos;
/*N*/ 	}
/*N*/ }

/*N*/ SwPosFlyFrm::~SwPosFlyFrm()
/*N*/ {
/*N*/ 	const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
/*N*/ 	if( FLY_PAGE == rAnchor.GetAnchorId() )
/*?*/ 		delete pNdIdx;
/*N*/ }

/*N*/ BOOL SwPosFlyFrm::operator==( const SwPosFlyFrm& rPosFly )
/*N*/ {
/*N*/ 	return FALSE;	// FlyFrames koennen auf der gleichen Position stehen
/*N*/ }

/*N*/ BOOL SwPosFlyFrm::operator<( const SwPosFlyFrm& rPosFly )
/*N*/ {
/*N*/ 	if( pNdIdx->GetIndex() == rPosFly.pNdIdx->GetIndex() )
/*N*/ 	{
/*N*/ 		// dann entscheidet die Ordnungsnummer!
/*N*/ 		return nOrdNum < rPosFly.nOrdNum;
/*N*/ 	}
/*N*/ 	return pNdIdx->GetIndex() < rPosFly.pNdIdx->GetIndex();
/*N*/ }



}
