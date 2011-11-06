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


#ifndef _SVX_HYZNITEM_HXX
#define _SVX_HYZNITEM_HXX

#include <bf_svtools/bf_solar.h>

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
// class SvxHyphenZoneItem -----------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt ein Silbentrennungsattribut (Automatisch?, Anzahl der
Zeichen am Zeilenende und -anfang).
*/

class SvxHyphenZoneItem : public SfxPoolItem
{
	BOOL bHyphen:  1;
	BOOL bPageEnd: 1;
	BYTE nMinLead;
	BYTE nMinTrail;
	BYTE nMaxHyphens;

	friend SvStream & operator<<( SvStream & aS, SvxHyphenZoneItem & );

public:
	TYPEINFO();

	SvxHyphenZoneItem( const BOOL bHyph = FALSE,
					   const USHORT nId = ITEMID_HYPHENZONE );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion ) const;

	inline void SetHyphen( const BOOL bNew ) { bHyphen = bNew; }
	inline BOOL IsHyphen() const { return bHyphen; }

	inline void SetPageEnd( const BOOL bNew ) { bPageEnd = bNew; }
	inline BOOL IsPageEnd() const { return bPageEnd; }

	inline BYTE &GetMinLead() { return nMinLead; }
	inline BYTE GetMinLead() const { return nMinLead; }

	inline BYTE &GetMinTrail() { return nMinTrail; }
	inline BYTE GetMinTrail() const { return nMinTrail; }

	inline BYTE &GetMaxHyphens() { return nMaxHyphens; }
	inline BYTE GetMaxHyphens() const { return nMaxHyphens; }

	inline SvxHyphenZoneItem &operator=( const SvxHyphenZoneItem &rNew )
	{
		bHyphen = rNew.IsHyphen();
		bPageEnd = rNew.IsPageEnd();
		nMinLead = rNew.GetMinLead();
		nMinTrail = rNew.GetMinTrail();
		nMaxHyphens = rNew.GetMaxHyphens();
		return *this;
	}
};

}//end of namespace binfilter
#endif

