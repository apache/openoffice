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


#ifndef _SVX_FRMDIRITEM_HXX
#define _SVX_FRMDIRITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
#endif
#ifndef _SVX_FRMDIR_HXX
#include <bf_svx/frmdir.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

// class SvxFrameDirectionItem ----------------------------------------------

/* [Description]

	This item defines a frame direction, which place the content inside
	a frame. It exist different kind of directions which are used to the
	layout text for Western, CJK and CTL languages.
*/

class SvxFrameDirectionItem : public SfxUInt16Item
{
public:
	TYPEINFO();

	SvxFrameDirectionItem( SvxFrameDirection nValue = FRMDIR_HORI_LEFT_TOP,
							USHORT nWhich = ITEMID_FRAMEDIR );
	virtual ~SvxFrameDirectionItem();

	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT) const;
	virtual SvStream& 		Store(SvStream & rStrm, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;
	virtual int 			operator==( const SfxPoolItem& ) const;


	virtual sal_Bool PutValue( const ::com::sun::star::uno::Any& rVal,
									BYTE nMemberId );
	virtual sal_Bool QueryValue( ::com::sun::star::uno::Any& rVal,
								BYTE nMemberId ) const;

	inline SvxFrameDirectionItem& operator=( const SvxFrameDirectionItem& rItem )
	{
		SetValue( rItem.GetValue() );
		return *this;
	}
};

}//end of namespace binfilter
#endif // #ifndef _SVX_FRMDIRITEM_HXX

