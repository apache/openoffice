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

#define _SVSTDARR_ULONGS
#define _SVSTDARR_STRINGSDTOR

#include <bf_svtools/svstdarr.hxx>
#include <clipfmtitem.hxx>
namespace binfilter {



struct SvxClipboardFmtItem_Impl
{
	SvStringsDtor aFmtNms;
	SvULongs aFmtIds;
	static String sEmptyStr;

	SvxClipboardFmtItem_Impl() : aFmtNms( 8, 8 ), aFmtIds( 8, 8 ) {}
	SvxClipboardFmtItem_Impl( const SvxClipboardFmtItem_Impl& );
};

String SvxClipboardFmtItem_Impl::sEmptyStr;

/*N*/ TYPEINIT1( SvxClipboardFmtItem, SfxPoolItem );

/*N*/ SvxClipboardFmtItem_Impl::SvxClipboardFmtItem_Impl(
/*N*/ 							const SvxClipboardFmtItem_Impl& rCpy )
/*N*/ {
/*N*/ 	aFmtIds.Insert( &rCpy.aFmtIds, 0 );
/*N*/ 	for( USHORT n = 0, nEnd = rCpy.aFmtNms.Count(); n < nEnd; ++n )
/*N*/ 	{
/*N*/ 		String* pStr = rCpy.aFmtNms[ n ];
/*N*/ 		if( pStr )
/*N*/ 			pStr = new String( *pStr );
/*N*/ 		aFmtNms.Insert( pStr, n );
/*N*/ 	}
/*N*/ }

/*N*/ SvxClipboardFmtItem::SvxClipboardFmtItem( const SvxClipboardFmtItem& rCpy )
/*N*/ 	: SfxPoolItem( rCpy.Which() ),
/*N*/ 	pImpl( new SvxClipboardFmtItem_Impl( *rCpy.pImpl ) )
/*N*/ {
/*N*/ }

/*N*/ SvxClipboardFmtItem::~SvxClipboardFmtItem()
/*N*/ {
/*N*/ 	delete pImpl;
/*N*/ }

/*N*/ int SvxClipboardFmtItem::operator==( const SfxPoolItem& rComp ) const
/*N*/ {
/*N*/ 	int nRet = 0;
/*N*/ 	const SvxClipboardFmtItem& rCmp = (SvxClipboardFmtItem&)rComp;
/*N*/ 	if( rCmp.pImpl->aFmtNms.Count() == pImpl->aFmtNms.Count() )
/*N*/ 	{
/*N*/ 		nRet = 1;
/*N*/ 		const String* pStr1, *pStr2;
/*N*/ 		for( USHORT n = 0, nEnd = rCmp.pImpl->aFmtNms.Count(); n < nEnd; ++n )
/*N*/ 		{
/*N*/ 			if( pImpl->aFmtIds[ n ] != rCmp.pImpl->aFmtIds[ n ] ||
/*N*/ 				( (0 == ( pStr1 = pImpl->aFmtNms[ n ] )) ^
/*N*/ 				  (0 == ( pStr2 = rCmp.pImpl->aFmtNms[ n ] ) )) ||
/*N*/ 				( pStr1 && *pStr1 != *pStr2 ))
/*N*/ 			{
/*N*/ 				nRet = 0;
/*N*/ 				break;
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return nRet;
/*N*/ }

/*N*/ SfxPoolItem* SvxClipboardFmtItem::Clone( SfxItemPool *pPool ) const
/*N*/ {
/*N*/ 	return new SvxClipboardFmtItem( *this );
/*N*/ }
}
