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

#include <float.h>

#ifndef _CELLATR_HXX
#include <cellatr.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
namespace binfilter {



//TYPEINIT1( SwFmt, SwClient );	//rtti fuer SwFmt

/*************************************************************************
|*
*************************************************************************/


/*N*/ SwTblBoxNumFormat::SwTblBoxNumFormat( UINT32 nFormat, BOOL bFlag )
/*N*/ 	: SfxUInt32Item( RES_BOXATR_FORMAT, nFormat ), bAuto( bFlag )
/*N*/ {
/*N*/ }


/*N*/ int SwTblBoxNumFormat::operator==( const SfxPoolItem& rAttr ) const
/*N*/ {
/*N*/ 	ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
/*N*/ 	return GetValue() == ((SwTblBoxNumFormat&)rAttr).GetValue() &&
/*N*/ 			bAuto == ((SwTblBoxNumFormat&)rAttr).bAuto;
/*N*/ }


/*N*/ SfxPoolItem* SwTblBoxNumFormat::Clone( SfxItemPool* ) const
/*N*/ {
/*N*/ 	return new SwTblBoxNumFormat( GetValue(), bAuto );
/*N*/ }


/*************************************************************************
|*
*************************************************************************/



/*N*/ SwTblBoxFormula::SwTblBoxFormula( const String& rFormula )
/*N*/ 	: SfxPoolItem( RES_BOXATR_FORMULA ),
/*N*/ 	SwTableFormula( rFormula ),
/*N*/ 	pDefinedIn( 0 )
/*N*/ {
/*N*/ }


/*N*/ int SwTblBoxFormula::operator==( const SfxPoolItem& rAttr ) const
/*N*/ {
/*?*/   ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
 /*?*/  return GetFormula() == ((SwTblBoxFormula&)rAttr).GetFormula() &&
 /*?*/          pDefinedIn == ((SwTblBoxFormula&)rAttr).pDefinedIn;
/*N*/ }


/*N*/ SfxPoolItem* SwTblBoxFormula::Clone( SfxItemPool* ) const
/*N*/ {
/*?*/  // auf externe Darstellung umschalten!!
    SwTblBoxFormula* pNew = new SwTblBoxFormula( GetFormula() );
    pNew->SwTableFormula::operator=( *this );
    return pNew;
/*N*/ }



	// suche den Node, in dem die Formel steht:
	//	TextFeld	-> TextNode,
	//	BoxAttribut	-> BoxStartNode
	// !!! MUSS VON JEDER ABLEITUNG UEBERLADEN WERDEN !!!
/*N*/ const SwNode* SwTblBoxFormula::GetNodeOfFormula() const
/*N*/ {
/*?*/       const SwNode* pRet = 0;
            if( pDefinedIn )
            {
                SwClient* pBox = SwClientIter( *pDefinedIn ).First( TYPE( SwTableBox ));
                if( pBox )
                    pRet = ((SwTableBox*)pBox)->GetSttNd();
            }
            return pRet;
/*N*/ }


SwTableBox* SwTblBoxFormula::GetTableBox()
{
    SwTableBox* pBox = 0;
    if( pDefinedIn )
        pBox = (SwTableBox*)SwClientIter( *pDefinedIn ).
                            First( TYPE( SwTableBox ));
    return pBox;
}

/*************************************************************************
|*
*************************************************************************/


/*N*/ SwTblBoxValue::SwTblBoxValue()
/*N*/ 	: SfxPoolItem( RES_BOXATR_VALUE ), nValue( 0 )
/*N*/ {
/*N*/ }


/*N*/ SwTblBoxValue::SwTblBoxValue( const double nVal )
/*N*/ 	: SfxPoolItem( RES_BOXATR_VALUE ), nValue( nVal )
/*N*/ {
/*N*/ }


/*N*/ int SwTblBoxValue::operator==( const SfxPoolItem& rAttr ) const
/*N*/ {
/*N*/ 	ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
/*N*/ 	return nValue == ((SwTblBoxValue&)rAttr).nValue;
/*N*/ }


/*N*/ SfxPoolItem* SwTblBoxValue::Clone( SfxItemPool* ) const
/*N*/ {
/*N*/ 	return new SwTblBoxValue( nValue );
/*N*/ }




}
