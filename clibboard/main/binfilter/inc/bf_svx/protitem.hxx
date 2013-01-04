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


#ifndef _SVX_PROTITEM_HXX
#define _SVX_PROTITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;

// class SvxProtectItem --------------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt, ob Inhalt, Groesse oder Position geschuetzt werden
sollen.
*/

class SvxProtectItem : public SfxPoolItem
{
	BOOL bCntnt :1;     //Inhalt geschuetzt
	BOOL bSize  :1;     //Groesse geschuetzt
	BOOL bPos   :1;     //Position geschuetzt

public:
	TYPEINFO();

	inline SvxProtectItem( const USHORT nId = ITEMID_PROTECT );
	inline SvxProtectItem &operator=( const SvxProtectItem &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;


	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion ) const;

	BOOL IsCntntProtected() const { return bCntnt; }
	BOOL IsSizeProtected()  const { return bSize;  }
	BOOL IsPosProtected()   const { return bPos;   }
	void SetCntntProtect( BOOL bNew ) { bCntnt = bNew; }
	void SetSizeProtect ( BOOL bNew ) { bSize  = bNew; }
	void SetPosProtect  ( BOOL bNew ) { bPos   = bNew; }

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
};

inline SvxProtectItem::SvxProtectItem( const USHORT nId )
	: SfxPoolItem( nId )
{
	bCntnt = bSize = bPos = FALSE;
}

inline SvxProtectItem &SvxProtectItem::operator=( const SvxProtectItem &rCpy )
{
	bCntnt = rCpy.IsCntntProtected();
	bSize  = rCpy.IsSizeProtected();
	bPos   = rCpy.IsPosProtected();
	return *this;
}

}//end of namespace binfilter
#endif

