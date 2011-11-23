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


#ifndef _SVX_COLRITEM_HXX
#define _SVX_COLRITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
#ifndef _COLOR_HXX //autogen
#include <tools/color.hxx>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;

// class SvxColorItem ----------------------------------------------------

/*	[Beschreibung]

	Dieses Item beschreibt eine Farbe.
*/

#define VERSION_USEAUTOCOLOR	1

class SvxColorItem : public SfxPoolItem
{
private:
	Color					mColor;

public:
	TYPEINFO();

	SvxColorItem( const USHORT nId = ITEMID_COLOR );
	SvxColorItem( const Color& aColor, const USHORT nId = ITEMID_COLOR );
	SvxColorItem( SvStream& rStrm, const USHORT nId = ITEMID_COLOR );
	SvxColorItem( const SvxColorItem& rCopy );
	~SvxColorItem();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;

	inline SvxColorItem& operator=(const SvxColorItem& rColor)
	{
		SetValue( rColor.GetValue() );
		return *this;
	}

	const	Color&			GetValue() const
							{
								return mColor;
							}
			void			SetValue( const Color& rNewCol );

};

}//end of namespace binfilter
#endif

