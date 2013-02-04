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

#ifndef _TXTRFMRK_HXX //autogen
#include <txtrfmrk.hxx>
#endif
#ifndef _FMTRFMRK_HXX //autogen
#include <fmtrfmrk.hxx>
#endif
namespace binfilter {


/****************************************************************************
 *
 *  class SwFmtRefMark
 *
 ****************************************************************************/

/*N*/ SwFmtRefMark::~SwFmtRefMark( )
/*N*/ {
/*N*/ }

/*N*/ SwFmtRefMark::SwFmtRefMark( const XubString& rName )
/*N*/ 	: SfxPoolItem( RES_TXTATR_REFMARK ),
/*N*/ 	aRefName( rName ),
/*N*/ 	pTxtAttr( 0 )
/*N*/ {
/*N*/ }

/*N*/ SwFmtRefMark::SwFmtRefMark( const SwFmtRefMark& rAttr )
/*N*/ 	: SfxPoolItem( RES_TXTATR_REFMARK ),
/*N*/ 	aRefName( rAttr.aRefName ),
/*N*/ 	pTxtAttr( 0 )
/*N*/ {
/*N*/ }

int SwFmtRefMark::operator==( const SfxPoolItem& rAttr ) const
{
	{DBG_BF_ASSERT(0, "STRIP");} return 0;//STRIP001 	ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
}

/*N*/ SfxPoolItem* SwFmtRefMark::Clone( SfxItemPool* ) const
/*N*/ {
/*N*/ 	return new SwFmtRefMark( *this );
/*N*/ }

/*************************************************************************
 *						class SwTxtRefMark
 *************************************************************************/

// Attribut fuer Inhalts-/Positions-Referenzen im Text

/*N*/ SwTxtRefMark::SwTxtRefMark( const SwFmtRefMark& rAttr,
/*N*/ 					xub_StrLen nStart, xub_StrLen* pEnde )
/*N*/ 	: SwTxtAttrEnd( rAttr, nStart, nStart ),
/*N*/ 	pEnd( 0 ),
/*N*/ 	pMyTxtNd( 0 )
/*N*/ {
/*N*/ 	((SwFmtRefMark&)rAttr).pTxtAttr = this;
/*N*/ 	if( pEnde )
/*N*/ 	{
/*N*/ 		nEnd = *pEnde;
/*N*/ 		pEnd = &nEnd;
/*N*/ 	}
/*N*/ 	SetDontMergeAttr( TRUE );
/*N*/ 	SetDontMoveAttr( TRUE );
/*N*/ 	SetOverlapAllowedAttr( TRUE );
/*N*/ }

/*N*/ xub_StrLen* SwTxtRefMark::GetEnd()
/*N*/ {
/*N*/ 	return pEnd;
/*N*/ }

}
