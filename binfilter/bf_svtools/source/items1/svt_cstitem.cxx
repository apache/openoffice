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

#include <unotools/intlwrapper.hxx>

#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <tools/datetime.hxx>

#include <bf_svtools/svtdata.hxx>
#include <bf_svtools/cstitem.hxx>

#include <bf_svtools/svtools.hrc>

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

namespace binfilter
{

DBG_NAME( SfxCrawlStatusItem )
TYPEINIT1( SfxCrawlStatusItem, SfxPoolItem );

// -----------------------------------------------------------------------

SfxCrawlStatusItem::SfxCrawlStatusItem( USHORT which, CrawlStatus eStat ) :
	SfxPoolItem( which ),
	eStatus( eStat )
{
	DBG_CTOR( SfxCrawlStatusItem, 0 );
}

// -----------------------------------------------------------------------

SfxCrawlStatusItem::SfxCrawlStatusItem( const SfxCrawlStatusItem& rItem ) :
	SfxPoolItem( rItem ),
	eStatus( rItem.eStatus )
{
	DBG_CTOR( SfxCrawlStatusItem, 0 );
}

// -----------------------------------------------------------------------

int SfxCrawlStatusItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS( SfxCrawlStatusItem, 0 );
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

	return ((SfxCrawlStatusItem&)rItem).eStatus == eStatus;
}

// -----------------------------------------------------------------------

int SfxCrawlStatusItem::Compare( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS( SfxCrawlStatusItem, 0 );
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

	if( ((const SfxCrawlStatusItem&)rItem).eStatus < eStatus )
		return -1;
	else if( ((const SfxCrawlStatusItem&)rItem).eStatus == eStatus )
		return 0;
	else
		return 1;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxCrawlStatusItem::Create( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxCrawlStatusItem, 0);

	USHORT _eStatus;
	rStream >> _eStatus;

	return new SfxCrawlStatusItem( Which(), (CrawlStatus)_eStatus );
}

// -----------------------------------------------------------------------

SvStream& SfxCrawlStatusItem::Store( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS( SfxCrawlStatusItem, 0 );

	USHORT nStatus = (USHORT)eStatus;
	rStream << (USHORT) nStatus;

	return rStream;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxCrawlStatusItem::Clone( SfxItemPool* ) const
{
	DBG_CHKTHIS( SfxCrawlStatusItem, 0 );
	return new SfxCrawlStatusItem( *this );
}
// -----------------------------------------------------------------------

SfxItemPresentation SfxCrawlStatusItem::GetPresentation( SfxItemPresentation, SfxMapUnit, SfxMapUnit, XubString&, const ::IntlWrapper * )	const
{
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxCrawlStatusItem::PutValue( const com::sun::star::uno::Any& rVal,BYTE )
{
	sal_Int16 aValue = sal_Int16();
	if ( rVal >>= aValue )
	{
		SetStatus( static_cast< CrawlStatus >( aValue ) );
		return TRUE;
	}

	DBG_ERROR( "SfxCrawlStatusItem::PutValue - Wrong type!" );
	return FALSE;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxCrawlStatusItem::QueryValue( com::sun::star::uno::Any& rVal,BYTE ) const
{
	sal_Int16 aValue = sal::static_int_cast< sal_Int16 >(GetStatus());
	rVal <<= aValue;
	return TRUE;
}

}
