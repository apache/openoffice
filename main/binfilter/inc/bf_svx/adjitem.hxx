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


#ifndef _SVX_ADJITEM_HXX
#define _SVX_ADJITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SVX_SVXENUM_HXX
#include <bf_svx/svxenum.hxx>
#endif

#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif

#ifndef _EEITEM_HXX //autogen
#include <bf_svx/eeitem.hxx>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;
// class SvxAdjustItem ---------------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt die Zeilenausrichtung.
*/
#define	ADJUST_LASTBLOCK_VERSION		((USHORT)0x0001)

class SvxAdjustItem : public SfxEnumItemInterface
{
	BOOL    bLeft      : 1;
	BOOL    bRight     : 1;
	BOOL    bCenter    : 1;
	BOOL    bBlock     : 1;

	// nur aktiv, wenn bBlock
	BOOL    bOneBlock : 1;
	BOOL    bLastCenter : 1;
	BOOL    bLastBlock : 1;

	friend SvStream& operator<<( SvStream&, SvxAdjustItem& ); //$ ostream
public:
	TYPEINFO();

	SvxAdjustItem( const SvxAdjust eAdjst = SVX_ADJUST_LEFT,
				   const USHORT nId = ITEMID_ADJUST );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual USHORT			 GetValueCount() const;
	virtual USHORT			 GetEnumValue() const;
	virtual void			 SetEnumValue( USHORT nNewVal );
	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion ) const;
	virtual USHORT			 GetVersion( USHORT nFileVersion ) const;

	inline void SetOneWord( const SvxAdjust eType )
	{
		bOneBlock  = eType == SVX_ADJUST_BLOCK;
	}

	inline void SetLastBlock( const SvxAdjust eType )
	{
		bLastBlock = eType == SVX_ADJUST_BLOCK;
		bLastCenter = eType == SVX_ADJUST_CENTER;
	}

	inline void SetAdjust( const SvxAdjust eType )
	{
		bLeft = eType == SVX_ADJUST_LEFT;
		bRight = eType == SVX_ADJUST_RIGHT;
		bCenter = eType == SVX_ADJUST_CENTER;
		bBlock = eType == SVX_ADJUST_BLOCK;
	}

	inline SvxAdjust GetLastBlock() const
	{
		SvxAdjust eRet = SVX_ADJUST_LEFT;

		if ( bLastBlock )
			eRet = SVX_ADJUST_BLOCK;
		else if( bLastCenter )
			eRet = SVX_ADJUST_CENTER;
		return eRet;
	}

	inline SvxAdjust GetOneWord() const
	{
		SvxAdjust eRet = SVX_ADJUST_LEFT;

		if ( bBlock && bOneBlock )
			eRet = SVX_ADJUST_BLOCK;
		return eRet;
	}

	inline SvxAdjust GetAdjust() const
	{
		SvxAdjust eRet = SVX_ADJUST_LEFT;

		if ( bRight )
			eRet = SVX_ADJUST_RIGHT;
		else if ( bCenter )
			eRet = SVX_ADJUST_CENTER;
		else if ( bBlock )
			eRet = SVX_ADJUST_BLOCK;
		return eRet;
	}
};

}//end of namespace binfilter
#endif

