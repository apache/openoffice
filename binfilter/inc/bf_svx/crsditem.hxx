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


#ifndef _SVX_CRSDITEM_HXX
#define _SVX_CRSDITEM_HXX

// include ---------------------------------------------------------------

#ifndef _VCL_VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
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
// class SvxCrossedOutItem -----------------------------------------------

/*
	[Beschreibung]
	Dieses Item beschreibt, ob und wie durchstrichen ist.
*/

class SvxCrossedOutItem : public SfxEnumItem
{
public:
	TYPEINFO();

	SvxCrossedOutItem( const FontStrikeout eSt = STRIKEOUT_NONE,
					   const USHORT nId = ITEMID_CROSSEDOUT );

	// "pure virtual Methoden" vom SfxPoolItem

	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create(SvStream &, USHORT) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion) const;
	virtual USHORT          GetValueCount() const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	// MS VC4.0 kommt durcheinander
	void			 		SetValue( USHORT nNewVal )
								{SfxEnumItem::SetValue(nNewVal); }


	inline SvxCrossedOutItem& operator=(const SvxCrossedOutItem& rCross)
		{
			SetValue( rCross.GetValue() );
			return *this;
		}

	// enum cast
	FontStrikeout 			GetStrikeout() const
								{ return (FontStrikeout)GetValue(); }
	void 					SetStrikeout( FontStrikeout eNew )
								{ SetValue( (USHORT)eNew ); }
};

}//end of namespace binfilter
#endif // #ifndef _SVX_CRSDITEM_HXX

