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

#define ITEMID_GRF_CROP	 		0

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _SVX_GRFCROP_HXX
#include <grfcrop.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_GRAPHICCROP_HPP_
#include <com/sun/star/text/GraphicCrop.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;

#define TWIP_TO_MM100(TWIP) 	((TWIP) >= 0 ? (((TWIP)*127L+36L)/72L) : (((TWIP)*127L-36L)/72L))
#define MM100_TO_TWIP(MM100)	((MM100) >= 0 ? (((MM100)*72L+63L)/127L) : (((MM100)*72L-63L)/127L))
//TYPEINIT1_AUTOFACTORY( SvxGrfCrop, SfxPoolItem )

/******************************************************************************
 *	Implementierung		class SwCropGrf
 ******************************************************************************/

/*N*/ SvxGrfCrop::SvxGrfCrop( USHORT nItemId )
/*N*/ 	: SfxPoolItem( nItemId ),
/*N*/ 	nLeft( 0 ), nRight( 0 ), nTop( 0 ), nBottom( 0 )
/*N*/ {}

/*N*/ SvxGrfCrop::SvxGrfCrop( sal_Int32 nL, sal_Int32 nR,
/*N*/ 						sal_Int32 nT, sal_Int32 nB, USHORT nItemId )
/*N*/ 	: SfxPoolItem( nItemId ),
/*N*/ 	nLeft( nL ), nRight( nR ), nTop( nT ), nBottom( nB )
/*N*/ {}

/*N*/ SvxGrfCrop::~SvxGrfCrop()
/*N*/ {
/*N*/ }

/*N*/ int SvxGrfCrop::operator==( const SfxPoolItem& rAttr ) const
/*N*/ {
/*N*/ 	DBG_ASSERT( SfxPoolItem::operator==( rAttr ), "not equal attributes" );
/*N*/ 	return nLeft 	== ((const SvxGrfCrop&)rAttr).GetLeft() &&
/*N*/ 		   nRight 	== ((const SvxGrfCrop&)rAttr).GetRight() &&
/*N*/ 		   nTop 	== ((const SvxGrfCrop&)rAttr).GetTop() &&
/*N*/ 		   nBottom	== ((const SvxGrfCrop&)rAttr).GetBottom();
/*N*/ }

/*
SfxPoolItem* SvxGrfCrop::Clone( SfxItemPool* ) const
{
	return new SvxGrfCrop( *this );
}
*/

/*
USHORT SvxGrfCrop::GetVersion( USHORT nFFVer ) const
{
	DBG_ASSERT( SOFFICE_FILEFORMAT_31==nFFVer ||
				SOFFICE_FILEFORMAT_40==nFFVer ||
				SOFFICE_FILEFORMAT_NOW==nFFVer,
				"SvxGrfCrop: exist a new fileformat?" );
	return GRFCROP_VERSION_SWDEFAULT;
}
*/

/*N*/ SfxPoolItem* SvxGrfCrop::Create( SvStream& rStrm, USHORT nVersion ) const
/*N*/ {
/*N*/ 	INT32 top, left, right, bottom;
/*N*/ 	rStrm >> top >> left >> right >> bottom;
/*N*/ 
/*N*/ 	if( GRFCROP_VERSION_SWDEFAULT == nVersion )
/*N*/ 		top = -top, bottom = -bottom, left = -left, right = -right;
/*N*/ 
/*N*/ 	SvxGrfCrop* pNew = (SvxGrfCrop*)Clone();
/*N*/ 	pNew->SetLeft( left );
/*N*/ 	pNew->SetRight( right );
/*N*/ 	pNew->SetTop( top );
/*N*/ 	pNew->SetBottom( bottom );
/*N*/ 	return pNew;
/*N*/ }


/*N*/ SvStream& SvxGrfCrop::Store( SvStream& rStrm, USHORT nVersion ) const
/*N*/ {
/*N*/ 	INT32 left = GetLeft(), right = GetRight(),
/*N*/ 			top = GetTop(), bottom = GetBottom();
/*N*/ 	if( GRFCROP_VERSION_SWDEFAULT == nVersion )
/*N*/ 		top = -top, bottom = -bottom, left = -left, right = -right;
/*N*/ 
/*N*/ 	rStrm << top << left << right << bottom;
/*N*/ 
/*N*/ 	return rStrm;
/*N*/ }



/*N*/ BOOL SvxGrfCrop::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
/*N*/ {
/*N*/     sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
/*N*/     nMemberId &= ~CONVERT_TWIPS;
/*N*/ 	text::GraphicCrop aRet;
/*N*/ 	aRet.Left 	= nLeft;
/*N*/ 	aRet.Right	= nRight;
/*N*/ 	aRet.Top 	= nTop;
/*N*/ 	aRet.Bottom = nBottom;
/*N*/ 
/*N*/     if( bConvert )
/*N*/ 	{
/*N*/ 	   aRet.Right 	= TWIP_TO_MM100(aRet.Right );
/*N*/ 	   aRet.Top    	= TWIP_TO_MM100(aRet.Top );
/*N*/ 	   aRet.Left  	= TWIP_TO_MM100(aRet.Left 	);
/*N*/ 	   aRet.Bottom	= TWIP_TO_MM100(aRet.Bottom);
/*N*/ 	}
/*N*/ 
/*N*/ 
/*N*/ 	rVal <<= aRet;
/*N*/ 	return	 sal_True;
/*N*/ }

/*N*/ BOOL SvxGrfCrop::PutValue( const uno::Any& rVal, BYTE nMemberId )
/*N*/ {
/*N*/     sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
/*N*/     nMemberId &= ~CONVERT_TWIPS;
/*N*/ 	text::GraphicCrop aVal;
/*N*/ 
/*N*/ 	if(!(rVal >>= aVal))
/*N*/ 		return sal_False;
/*N*/     if( bConvert )
/*N*/ 	{
/*N*/ 	   aVal.Right 	= MM100_TO_TWIP(aVal.Right );
/*N*/ 	   aVal.Top    	= MM100_TO_TWIP(aVal.Top );
/*N*/ 	   aVal.Left  	= MM100_TO_TWIP(aVal.Left 	);
/*N*/ 	   aVal.Bottom	= MM100_TO_TWIP(aVal.Bottom);
/*N*/ 	}
/*N*/ 
/*N*/ 	nLeft	= aVal.Left	 ;
/*N*/ 	nRight  = aVal.Right ;
/*N*/ 	nTop	= aVal.Top	 ;
/*N*/ 	nBottom = aVal.Bottom;
/*N*/ 	return	sal_True;
/*N*/ }





}
