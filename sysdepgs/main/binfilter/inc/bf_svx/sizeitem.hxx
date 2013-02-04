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


#ifndef _SVX_SIZEITEM_HXX
#define _SVX_SIZEITEM_HXX

// include ---------------------------------------------------------------

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

// class SvxSizeItem -----------------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt eine zweidimensionale Groesse (Size).
*/

class SvxSizeItem : public SfxPoolItem
{

	Size aSize;

public:
	TYPEINFO();

	SvxSizeItem( const USHORT nId = ITEMID_SIZE);
	SvxSizeItem( const USHORT nId, const Size& rSize);

	inline SvxSizeItem& operator=( const SvxSizeItem &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion ) const;

	const Size& GetSize() const { return aSize; }
	void		SetSize(const Size& rSize) { aSize = rSize; }
};

inline SvxSizeItem& SvxSizeItem::operator=( const SvxSizeItem &rCpy )
{
	aSize = rCpy.aSize;
	return *this;
}

}//end of namespace binfilter
#endif

