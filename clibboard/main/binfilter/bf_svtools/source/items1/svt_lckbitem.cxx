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


#define _LCKBITEM_CXX
#include <bf_svtools/lckbitem.hxx>

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _CACHESTR_HXX //autogen
#include <tools/cachestr.hxx>
#endif

// STATIC DATA -----------------------------------------------------------

namespace binfilter
{


// -----------------------------------------------------------------------

TYPEINIT1_AUTOFACTORY(SfxLockBytesItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem()
{
}

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem( USHORT nW, SvStream &rStream )
:	SfxPoolItem( nW )
{
	rStream.Seek( 0L );
	_xVal = new SvLockBytes( new SvCacheStream(), TRUE );

	SvStream aLockBytesStream( _xVal );
	rStream >> aLockBytesStream;
}

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem( const SfxLockBytesItem& rItem )
:	SfxPoolItem( rItem ),
	_xVal( rItem._xVal )
{
}

// -----------------------------------------------------------------------

SfxLockBytesItem::~SfxLockBytesItem()
{
}

// -----------------------------------------------------------------------

int SfxLockBytesItem::operator==( const SfxPoolItem& rItem ) const
{
	return ((SfxLockBytesItem&)rItem)._xVal == _xVal;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxLockBytesItem::Clone(SfxItemPool *) const
{
	return new SfxLockBytesItem( *this );
}

// -----------------------------------------------------------------------

#define MAX_BUF	32000

SfxPoolItem* SfxLockBytesItem::Create( SvStream &rStream, USHORT ) const
{
	sal_uInt32 nSize = 0;
	ULONG nActRead = 0;
	sal_Char cTmpBuf[MAX_BUF];
	SvMemoryStream aNewStream;
	rStream >> nSize;

	do {
		ULONG nToRead;
		if( (nSize - nActRead) > MAX_BUF )
			nToRead = MAX_BUF;
		else
			nToRead = nSize - nActRead;
		nActRead += rStream.Read( cTmpBuf, nToRead );
		aNewStream.Write( cTmpBuf, nToRead );
	} while( nSize > nActRead );

	return new SfxLockBytesItem( Which(), aNewStream );
}

// -----------------------------------------------------------------------

SvStream& SfxLockBytesItem::Store(SvStream &rStream, USHORT ) const
{
	SvStream aLockBytesStream( _xVal );
	sal_uInt32 nSize = aLockBytesStream.Seek( STREAM_SEEK_TO_END );
	aLockBytesStream.Seek( 0L );

	rStream << nSize;
	rStream << aLockBytesStream;

	return rStream;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxLockBytesItem::PutValue( const com::sun::star::uno::Any& rVal, BYTE )
{
	com::sun::star::uno::Sequence< sal_Int8 > aSeq;
	if ( rVal >>= aSeq )
	{
		if ( aSeq.getLength() )
		{
			SvCacheStream* pStream = new SvCacheStream;
			pStream->Write( (void*)aSeq.getConstArray(), aSeq.getLength() );
			pStream->Seek(0);

			_xVal = new SvLockBytes( pStream, TRUE );
		}
		else
			_xVal = NULL;

		return TRUE;
	}

	DBG_ERROR( "SfxLockBytesItem::PutValue - Wrong type!" );
	return FALSE;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxLockBytesItem::QueryValue( com::sun::star::uno::Any& rVal,BYTE ) const
{
	if ( _xVal.Is() )
	{
		sal_uInt32 nLen;
		SvLockBytesStat aStat;

		if ( _xVal->Stat( &aStat, SVSTATFLAG_DEFAULT ) == ERRCODE_NONE )
			nLen = aStat.nSize;
		else
			return FALSE;

		ULONG nRead = 0;
		com::sun::star::uno::Sequence< sal_Int8 > aSeq( nLen );

		_xVal->ReadAt( 0, aSeq.getArray(), nLen, &nRead );
		rVal <<= aSeq;
	}
	else
	{
		com::sun::star::uno::Sequence< sal_Int8 > aSeq( 0 );
		rVal <<= aSeq;
	}

	return TRUE;
}

}
