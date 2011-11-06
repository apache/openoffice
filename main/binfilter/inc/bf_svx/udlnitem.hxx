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


#ifndef _SVX_UDLNITEM_HXX
#define _SVX_UDLNITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
#ifndef _VCL_VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;

// class SvxUnderlineItem ------------------------------------------------

/*	[Beschreibung]

	Dieses Item beschreibt, ob und wie unterstrichen ist.
*/

class SvxUnderlineItem : public SfxEnumItem
{
	Color mColor;
public:
	TYPEINFO();

	SvxUnderlineItem( const FontUnderline eSt = UNDERLINE_NONE,
					  const USHORT nId = ITEMID_UNDERLINE );

	// "pure virtual Methoden" vom SfxPoolItem

	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create(SvStream &, USHORT) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion) const;
	virtual USHORT          GetValueCount() const;

	virtual	sal_Bool        QueryValue( ::com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 ) const;
	virtual	sal_Bool		PutValue( const ::com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 );

	// MS VC4.0 kommt durcheinander
	void			 		SetValue( USHORT nNewVal )
								{SfxEnumItem::SetValue(nNewVal); }
	virtual int 			HasBoolValue() const;
	virtual BOOL			GetBoolValue() const;

	virtual int 			 operator==( const SfxPoolItem& ) const;

	inline SvxUnderlineItem& operator=(const SvxUnderlineItem& rUnderline)
		{
			SetValue( rUnderline.GetValue() );
			SetColor( rUnderline.GetColor() );
			return *this;
		}

	// enum cast
	FontUnderline 			GetUnderline() const
								{ return (FontUnderline)GetValue(); }
	void 					SetUnderline ( FontUnderline eNew )
								{ SetValue((USHORT) eNew); }

	const Color&			GetColor() const				{ return mColor; }
	void 					SetColor( const Color& rCol )	{ mColor = rCol; }
};

}//end of namespace binfilter
#endif // #ifndef _SVX_UDLNITEM_HXX

