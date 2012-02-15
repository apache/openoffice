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


// include ---------------------------------------------------------------

#define _DATETIMEITEM_CXX
#include <bf_svtools/dateitem.hxx>

#include <unotools/intlwrapper.hxx>
#include <comphelper/processfactory.hxx>


#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <tools/datetime.hxx>

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif


#include <vcl/svapp.hxx>

#include <bf_svtools/svtdata.hxx>
#include <bf_svtools/svtools.hrc>

namespace binfilter
{

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxDateTimeItem)


// -----------------------------------------------------------------------

TYPEINIT1(SfxDateTimeItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxDateTimeItem::SfxDateTimeItem( USHORT which, const DateTime& rDT ) :
	SfxPoolItem( which ),
	aDateTime( rDT )

{
	DBG_CTOR(SfxDateTimeItem, 0);
}

// -----------------------------------------------------------------------

SfxDateTimeItem::SfxDateTimeItem( const SfxDateTimeItem& rItem ) :
	SfxPoolItem( rItem ),
	aDateTime( rItem.aDateTime )
{
	DBG_CTOR(SfxDateTimeItem, 0);
}

// -----------------------------------------------------------------------

int SfxDateTimeItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
	return ( ( (SfxDateTimeItem&)rItem ).aDateTime == aDateTime );
}

// -----------------------------------------------------------------------

int SfxDateTimeItem::Compare( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

	// da X.Compare( Y ) am String einem Compare( Y, X ) entspricht,
	// vergleichen wir hier Y mit X
	if ( ( (const SfxDateTimeItem&)rItem ).aDateTime < aDateTime )
		return -1;
	else if ( ( (const SfxDateTimeItem&)rItem ).aDateTime == aDateTime )
		return 0;
	else
		return 1;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxDateTimeItem::Create( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	sal_uInt32 nDate = 0;
	sal_Int32 nTime = 0;
	rStream >> nDate;
	rStream >> nTime;
	DateTime aDT(nDate, nTime);
	return new SfxDateTimeItem( Which(), aDT );
}

// -----------------------------------------------------------------------

SvStream& SfxDateTimeItem::Store( SvStream& rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	rStream << aDateTime.GetDate();
	rStream << aDateTime.GetTime();
	return rStream;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxDateTimeItem::Clone( SfxItemPool* ) const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	return new SfxDateTimeItem( *this );
}

// -----------------------------------------------------------------------

SfxItemPresentation SfxDateTimeItem::GetPresentation
(
	SfxItemPresentation 	/*ePresentation*/,
	SfxMapUnit				/*eCoreMetric*/,
	SfxMapUnit				/*ePresentationMetric*/,
	XubString& 				rText,
    const ::IntlWrapper *   pIntlWrapper
)	const
{
	DBG_CHKTHIS(SfxDateTimeItem, 0);
	if (aDateTime.IsValid())
        if (pIntlWrapper)
		{
            rText = pIntlWrapper->getLocaleData()->getDate(aDateTime);
			rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
            rText += pIntlWrapper->getLocaleData()->getTime(aDateTime);
		}
		else
		{
			DBG_WARNING("SfxDateTimeItem::GetPresentation():"
                         " Using default en_US IntlWrapper");
            const ::IntlWrapper aIntlWrapper(
                ::comphelper::getProcessServiceFactory(), LANGUAGE_ENGLISH_US );
            rText = aIntlWrapper.getLocaleData()->getDate(aDateTime);
			rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
            rText += aIntlWrapper.getLocaleData()->getTime(aDateTime);
		}
	else
		rText.Erase();
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxDateTimeItem::PutValue( const com::sun::star::uno::Any& rVal,
						   		BYTE nMemberId )
{
	nMemberId &= ~CONVERT_TWIPS;
	com::sun::star::util::DateTime aValue;
	if ( rVal >>= aValue )
	{
		aDateTime = DateTime( Date( aValue.Day,
			  						aValue.Month,
			  						aValue.Year ),
							  Time( aValue.Hours,
			  						aValue.Minutes,
			  						aValue.Seconds,
			  						aValue.HundredthSeconds ) );
		return TRUE;
	}

	DBG_ERROR( "SfxDateTimeItem::PutValue - Wrong type!" );
	return FALSE;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxDateTimeItem::QueryValue( com::sun::star::uno::Any& rVal,
							 	  BYTE nMemberId ) const
{
	nMemberId &= ~CONVERT_TWIPS;
	com::sun::star::util::DateTime aValue( aDateTime.Get100Sec(),
    									   aDateTime.GetSec(),
   										   aDateTime.GetMin(),
    									   aDateTime.GetHour(),
    									   aDateTime.GetDay(),
    									   aDateTime.GetMonth(),
    									   aDateTime.GetYear() );
	rVal <<= aValue;
	return TRUE;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
/*
TYPEINIT1(SfxColumnDateTimeItem, SfxDateTimeItem);

SfxPoolItem* SfxColumnDateTimeItem::Clone( SfxItemPool* ) const
{
	return new SfxColumnDateTimeItem( *this );
}

SfxItemPresentation SfxColumnDateTimeItem::GetPresentation( SfxItemPresentation, SfxMapUnit, SfxMapUnit, XubString&, const ::IntlWrapper * )	const
{
	return SFX_ITEM_PRESENTATION_NAMELESS;
}
*/
}
