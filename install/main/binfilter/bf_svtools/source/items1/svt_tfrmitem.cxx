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



// MARKER(update_precomp.py): autogen include statement, do not remove


#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <tools/string.hxx>

#include <bf_svtools/tfrmitem.hxx>

namespace binfilter
{

DBG_NAME( SfxTargetFrameItem )
TYPEINIT1( SfxTargetFrameItem, SfxPoolItem );

// -----------------------------------------------------------------------

SfxTargetFrameItem::SfxTargetFrameItem( USHORT which ) :
	SfxPoolItem( which )
{
	DBG_CTOR( SfxTargetFrameItem, 0 );
}

// -----------------------------------------------------------------------

SfxTargetFrameItem::SfxTargetFrameItem( const SfxTargetFrameItem& rItem ) :
	SfxPoolItem( rItem )
{
	DBG_CTOR( SfxTargetFrameItem, 0 );
	for( USHORT nCur = 0; nCur <= (USHORT)SfxOpenModeLast; nCur++ )
		_aFrames[nCur] = rItem._aFrames[nCur];
}

// -----------------------------------------------------------------------

SfxTargetFrameItem::~SfxTargetFrameItem()
{
	DBG_DTOR(SfxTargetFrameItem, 0);
}

// -----------------------------------------------------------------------

int SfxTargetFrameItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS( SfxTargetFrameItem, 0 );
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

	for( USHORT nCur = 0; nCur <= (USHORT)SfxOpenModeLast; nCur++ )
	{
		if(	_aFrames[nCur] != ((const SfxTargetFrameItem&)rItem)._aFrames[nCur] )
			return 0;
	}
	return 1;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxTargetFrameItem::Create( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxTargetFrameItem, 0);
	SfxTargetFrameItem* pItem = new SfxTargetFrameItem( Which() );
	USHORT nCount = 0;
	rStream >> nCount;
	for(USHORT nCur=0; nCur<= (USHORT)SfxOpenModeLast && nCount; nCur++,nCount--)
	{
		readByteString(rStream, pItem->_aFrames[ nCur ]);
	}
	// die uebriggebliebenen ueberspringen
	String aTemp;
	while( nCount )
	{
		readByteString(rStream, aTemp);
		nCount--;
	}
	return pItem;
}

// -----------------------------------------------------------------------

SvStream& SfxTargetFrameItem::Store( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS( SfxTargetFrameItem, 0 );
	USHORT nCount = (USHORT)(SfxOpenModeLast+1);
	rStream << nCount;
	for( USHORT nCur = 0; nCur <= (USHORT)SfxOpenModeLast; nCur++ )
	{
		writeByteString(rStream, _aFrames[ nCur ]);
	}
	return rStream;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxTargetFrameItem::Clone( SfxItemPool* ) const
{
	DBG_CHKTHIS( SfxTargetFrameItem, 0 );
	return new SfxTargetFrameItem( *this );
}

// -----------------------------------------------------------------------
// virtual
BOOL SfxTargetFrameItem::QueryValue( com::sun::star::uno::Any& rVal,BYTE ) const
{
	String aVal;
	for ( int i = 0; i <= SfxOpenModeLast; i++ )
	{
		aVal += _aFrames[ i ];
		aVal += ';' ;
	}

	rVal <<= rtl::OUString( aVal );
	return TRUE;
}

// -----------------------------------------------------------------------
// virtual
BOOL SfxTargetFrameItem::PutValue( const com::sun::star::uno::Any& rVal,BYTE )
{
	rtl::OUString aValue;
	if ( rVal >>= aValue )
	{
		const String aVal( aValue );

		for ( USHORT i = 0; i <= SfxOpenModeLast; i++ )
			_aFrames[ i ] = aVal.GetToken( i );

		return TRUE;
	}

	DBG_ERROR( "SfxTargetFrameItem::PutValue - Wrong type!" );
	return FALSE;
}

}
