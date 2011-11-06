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


#ifndef _SVX_TSPTITEM_HXX
#define _SVX_TSPTITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SVX_SVXENUM_HXX //autogen
#include <bf_svx/svxenum.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

// class SvxTabStop ------------------------------------------------------

#define SVX_TAB_DEFCOUNT    10
#define SVX_TAB_DEFDIST     1134            // 2cm in twips
#define SVX_TAB_NOTFOUND    USHRT_MAX
#define cDfltDecimalChar    (sal_Unicode(0x00)) // aus IntlWrapper besorgen
#define cDfltFillChar       (sal_Unicode(' '))

class SvxTabStop
{
private:
	long            nTabPos;

	SvxTabAdjust    eAdjustment;
	sal_Unicode		cDecimal;
	sal_Unicode		cFill;

	friend SvStream& operator<<( SvStream&, SvxTabStop& );

public:
	SvxTabStop();
#if (_MSC_VER < 1300)
	SvxTabStop( const long nPos,
				const SvxTabAdjust eAdjst = SVX_TAB_ADJUST_LEFT,
				const sal_Unicode cDec = cDfltDecimalChar,
				const sal_Unicode cFil = cDfltFillChar );
#else	
	SvxTabStop::SvxTabStop( const long nPos,
				const SvxTabAdjust eAdjst = SVX_TAB_ADJUST_LEFT,
				const sal_Unicode cDec = cDfltDecimalChar,
				const sal_Unicode cFil = cDfltFillChar );
#endif

	long&           GetTabPos() { return nTabPos; }
	long            GetTabPos() const { return nTabPos; }

	SvxTabAdjust&   GetAdjustment() { return eAdjustment; }
	SvxTabAdjust    GetAdjustment() const { return eAdjustment; }

	sal_Unicode&  GetDecimal() { return cDecimal; }
	sal_Unicode   GetDecimal() const { return cDecimal; }

	sal_Unicode&  GetFill() { return cFill; }
	sal_Unicode   GetFill() const { return cFill; }


	// das "alte" operator==()
	BOOL			IsEqual( const SvxTabStop& rTS ) const
						{
							return ( nTabPos     == rTS.nTabPos     &&
									 eAdjustment == rTS.eAdjustment &&
									 cDecimal    == rTS.cDecimal    &&
									 cFill       == rTS.cFill );
						}

	// Fuer das SortedArray:
	BOOL            operator==( const SvxTabStop& rTS ) const
						{ return nTabPos == rTS.nTabPos; }
	BOOL            operator <( const SvxTabStop& rTS ) const
						{ return nTabPos < rTS.nTabPos; }

	SvxTabStop&     operator=( const SvxTabStop& rTS )
						{
							nTabPos = rTS.nTabPos;
							eAdjustment = rTS.eAdjustment;
							cDecimal = rTS.cDecimal;
							cFill = rTS.cFill;
							return *this;
						}
};

// class SvxTabStopItem --------------------------------------------------

SV_DECL_VARARR_SORT( SvxTabStopArr, SvxTabStop, SVX_TAB_DEFCOUNT, 1 )

/*
[Beschreibung]
Dieses Item beschreibt eine Liste von TabStops.
*/

class SvxTabStopItem : public SfxPoolItem, private SvxTabStopArr
{
//friend class SvxTabStopObject_Impl;

public:
	TYPEINFO();

	SvxTabStopItem( USHORT nWhich = ITEMID_TABSTOP );
	SvxTabStopItem( const USHORT nTabs,
					const USHORT nDist,
					const SvxTabAdjust eAdjst = SVX_TAB_ADJUST_DEFAULT,
					USHORT nWhich = ITEMID_TABSTOP );
	SvxTabStopItem( const SvxTabStopItem& rTSI );

	// Liefert Index-Position des Tabs zurueck oder TAB_NOTFOUND
	USHORT          GetPos( const SvxTabStop& rTab ) const;

	// Liefert Index-Position des Tabs an nPos zurueck oder TAB_NOTFOUND

	// entprivatisiert:
	USHORT          Count() const { return SvxTabStopArr::Count(); }
	BOOL            Insert( const SvxTabStop& rTab );
	void            Remove( SvxTabStop& rTab )
						{ SvxTabStopArr::Remove( rTab ); }
	void            Remove( const USHORT nPos, const USHORT nLen = 1 )
						{ SvxTabStopArr::Remove( nPos, nLen ); }

	// Zuweisungsoperator, Gleichheitsoperator (vorsicht: teuer!)

	int             operator!=( const SvxTabStopItem& rTSI ) const
						{ return !( operator==( rTSI ) ); }

	// SortedArrays liefern nur Stackobjekte zurueck!
	const SvxTabStop& operator[]( const USHORT nPos ) const
						{
							DBG_ASSERT( GetStart() &&
										nPos < Count(), "op[]" );
							return *( GetStart() + nPos );
						}
	const SvxTabStop*  GetStart() const
						{   return SvxTabStopArr::GetData(); }

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create( SvStream&, USHORT ) const;
	virtual SvStream&		 Store( SvStream& , USHORT nItemVersion ) const;
};

}//end of namespace binfilter
#endif

