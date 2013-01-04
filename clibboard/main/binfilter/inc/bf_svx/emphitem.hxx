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


#ifndef _SVX_EMPHITEM_HXX
#define _SVX_EMPTITEM_HXX

// include ---------------------------------------------------------------

#ifndef _VCL_VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
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
// class SvxEmphasisMarkItem ----------------------------------------------

/* [Beschreibung]

	Dieses Item beschreibt die Font-Betonung.
*/

class SvxEmphasisMarkItem : public SfxUInt16Item
{
public:
	TYPEINFO();

	SvxEmphasisMarkItem(  const FontEmphasisMark eVal = EMPHASISMARK_NONE,
						  const USHORT nId = ITEMID_EMPHASISMARK );

	// "pure virtual Methoden" vom SfxPoolItem + SfxEnumItem

	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion) const;
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;

	virtual	sal_Bool        QueryValue( ::com::sun::star::uno::Any& rVal,
											BYTE nMemberId = 0 ) const;
	virtual	sal_Bool		PutValue( const ::com::sun::star::uno::Any& rVal,
											BYTE nMemberId = 0 );

	inline SvxEmphasisMarkItem& operator=(const SvxEmphasisMarkItem& rItem )
	{
		SetValue( rItem.GetValue() );
		return *this;
	}

	// enum cast
	FontEmphasisMark		GetEmphasisMark() const
								{ return (FontEmphasisMark)GetValue(); }
	void					SetEmphasisMark( FontEmphasisMark eNew )
								{ SetValue( (USHORT)eNew ); }
};

}//end of namespace binfilter
#endif // #ifndef _SVX_EMPHITEM_HXX

