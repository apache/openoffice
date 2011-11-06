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


#ifndef _SVX_FHGTITEM_HXX
#define _SVX_FHGTITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;
// class SvxFontHeightItem -----------------------------------------------

// Achtung: Twips-Werte

/*	[Beschreibung]

	Dieses Item beschreibt die Font-Hoehe.
*/

#define	FONTHEIGHT_16_VERSION	((USHORT)0x0001)
#define	FONTHEIGHT_UNIT_VERSION	((USHORT)0x0002)

class SvxFontHeightItem : public SfxPoolItem
{
	UINT32	nHeight;
	USHORT	nProp;				// default 100%
	SfxMapUnit ePropUnit;		// Percent, Twip, ...
public:
	TYPEINFO();

	SvxFontHeightItem( const ULONG nSz = 240, const USHORT nPropHeight = 100,
					   const USHORT nId = ITEMID_FONTHEIGHT );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*     Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;
	virtual USHORT			 GetVersion( USHORT nItemVersion) const;

	inline SvxFontHeightItem& operator=(const SvxFontHeightItem& rSize)
		{
			SetHeightValue( rSize.GetHeight() );
			SetProp( rSize.GetProp(), ePropUnit );
			return *this;
		}

	void SetHeight( UINT32 nNewHeight, const USHORT nNewProp = 100,
					 SfxMapUnit eUnit = SFX_MAPUNIT_RELATIVE );


	UINT32 GetHeight() const { return nHeight; }

	void SetHeightValue( UINT32 nNewHeight )
		{
			DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
			nHeight = nNewHeight;
		}

	void SetProp( const USHORT nNewProp,
					SfxMapUnit eUnit = SFX_MAPUNIT_RELATIVE )
		{
			DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
			nProp = nNewProp;
			ePropUnit = eUnit;
		}

	USHORT GetProp() const { return nProp; }

	SfxMapUnit GetPropUnit() const { return ePropUnit;  }	// Percent, Twip, ...
};

}//end of namespace binfilter
#endif

