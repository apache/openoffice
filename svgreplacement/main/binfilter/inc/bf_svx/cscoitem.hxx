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


#ifndef _SVX_CSCOITEM_HXX
#define _SVX_CSCOITEM_HXX

// include ---------------------------------------------------------------

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX
#include <bf_svx/colritem.hxx>
#endif
namespace binfilter {

// class SvxCharSetColorItem ---------------------------------------------

/*	[Beschreibung]

	PB: wird nur intern im Reader des Writers benoetigt
*/

class SvxCharSetColorItem : public SvxColorItem
{
	rtl_TextEncoding eFrom;
public:
	TYPEINFO();

	SvxCharSetColorItem( const USHORT nId = ITEMID_CHARSETCOLOR );
	SvxCharSetColorItem( const Color& aColor, const rtl_TextEncoding eFrom,
					 const USHORT nId = ITEMID_CHARSETCOLOR );

	// "pure virtual Methoden" vom SfxPoolItem

	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*     Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;

	inline rtl_TextEncoding&	GetCharSet() { return eFrom; }
	inline rtl_TextEncoding 	GetCharSet() const { return eFrom; }

	inline SvxCharSetColorItem& operator=(const SvxCharSetColorItem& rColor)
	{
		SetValue( rColor.GetValue() );
		eFrom = rColor.GetCharSet();
		return *this;
	}
};

}//end of namespace binfilter
#endif

