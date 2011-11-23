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
#include "lineinfo.hxx"
#include "charfmt.hxx"
#include "poolfmt.hxx"
#include "rootfrm.hxx"
#include "viewsh.hxx"
namespace binfilter {

/*N*/ void SwDoc::SetLineNumberInfo( const SwLineNumberInfo &rNew )
/*N*/ {
/*N*/ 	if ( GetRootFrm() &&
/*N*/ 		 (rNew.IsCountBlankLines() != pLineNumberInfo->IsCountBlankLines() ||
/*N*/ 		  rNew.IsRestartEachPage() != pLineNumberInfo->IsRestartEachPage()) )
/*N*/ 	{
/*?*/ 		GetRootFrm()->StartAllAction();
/*?*/ 		GetRootFrm()->InvalidateAllCntnt( INV_LINENUM );
/*?*/ 		GetRootFrm()->EndAllAction();
/*N*/ 	}
/*N*/ 	*pLineNumberInfo = rNew;
/*N*/ 	SetModified();
/*N*/ }

/*N*/ SwLineNumberInfo::SwLineNumberInfo() :
/*N*/ 	nPosFromLeft( MM50 ),
/*N*/ 	nCountBy( 5 ),
/*N*/ 	nDividerCountBy( 3 ),
/*N*/ 	ePos( LINENUMBER_POS_LEFT ),
/*N*/ 	bPaintLineNumbers( FALSE ),
/*N*/ 	bCountBlankLines( TRUE ),
/*N*/ 	bCountInFlys( FALSE ),
/*N*/ 	bRestartEachPage( FALSE )
/*N*/ {
/*N*/ }

/*N*/ SwLineNumberInfo::SwLineNumberInfo(const SwLineNumberInfo &rCpy ) :
/*N*/ 	aType( rCpy.GetNumType() ),
/*N*/ 	aDivider( rCpy.GetDivider() ),
/*N*/ 	nPosFromLeft( rCpy.GetPosFromLeft() ),
/*N*/ 	nCountBy( rCpy.GetCountBy() ),
/*N*/ 	nDividerCountBy( rCpy.GetDividerCountBy() ),
/*N*/ 	ePos( rCpy.GetPos() ),
/*N*/ 	bPaintLineNumbers( rCpy.IsPaintLineNumbers() ),
/*N*/ 	bCountBlankLines( rCpy.IsCountBlankLines() ),
/*N*/ 	bCountInFlys( rCpy.IsCountInFlys() ),
/*N*/ 	bRestartEachPage( rCpy.IsRestartEachPage() )
/*N*/ {
/*N*/ 	if ( rCpy.GetRegisteredIn() )
/*?*/ 		((SwModify*)rCpy.GetRegisteredIn())->Add( this );
/*N*/ }

/*N*/ SwLineNumberInfo& SwLineNumberInfo::operator=(const SwLineNumberInfo &rCpy)
/*N*/ {
/*N*/ 	if ( rCpy.GetRegisteredIn() )
/*N*/ 		((SwModify*)rCpy.GetRegisteredIn())->Add( this );
/*N*/ 	else if ( GetRegisteredIn() )
/*?*/ 		pRegisteredIn->Remove( this );
/*N*/ 
/*N*/ 	aType = rCpy.GetNumType();
/*N*/ 	aDivider = rCpy.GetDivider();
/*N*/ 	nPosFromLeft = rCpy.GetPosFromLeft();
/*N*/ 	nCountBy = rCpy.GetCountBy();
/*N*/ 	nDividerCountBy = rCpy.GetDividerCountBy();
/*N*/ 	ePos = rCpy.GetPos();
/*N*/ 	bPaintLineNumbers = rCpy.IsPaintLineNumbers();
/*N*/ 	bCountBlankLines = rCpy.IsCountBlankLines();
/*N*/ 	bCountInFlys = rCpy.IsCountInFlys();
/*N*/ 	bRestartEachPage = rCpy.IsRestartEachPage();
/*N*/ 
/*N*/ 	return *this;
/*N*/ }



/*N*/ SwCharFmt* SwLineNumberInfo::GetCharFmt(SwDoc &rDoc) const
/*N*/ {
/*N*/ 	if ( !GetRegisteredIn() )
/*?*/ 	{
/*?*/ 		SwCharFmt* pFmt = rDoc.GetCharFmtFromPool( RES_POOLCHR_LINENUM );
/*?*/ 		pFmt->Add( (SwClient*)this );
/*?*/ 	}
/*N*/ 	return (SwCharFmt*)GetRegisteredIn();
/*N*/ }

/*N*/ void SwLineNumberInfo::SetCharFmt( SwCharFmt *pChFmt )
/*N*/ {
/*N*/ 	ASSERT( pChFmt, "SetCharFmt, 0 is not a valid pointer" );
/*N*/ 	pChFmt->Add( this );
/*N*/ }

/*N*/ void SwLineNumberInfo::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
/*N*/ {
/*N*/ 	SwClient::Modify( pOld, pNew );
/*N*/ 	SwDoc *pDoc = ((SwCharFmt*)GetRegisteredIn())->GetDoc();
/*N*/ 	SwRootFrm* pRoot = pDoc->GetRootFrm();
/*N*/ 	if( pRoot && pRoot->GetCurrShell() )
/*N*/ 	{
/*?*/ 		pRoot->StartAllAction();
/*?*/ 		pRoot->GetCurrShell()->AddPaintRect( pRoot->Frm() );
/*?*/ 		pRoot->EndAllAction();
/*N*/ 	}
/*N*/ }

}
