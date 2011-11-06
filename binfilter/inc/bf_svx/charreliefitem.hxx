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


#ifndef _SVX_CHARRELIEFITEM_HXX
#define _SVX_CHARRELIEFITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _VCL_FNTSTYLE_HXX
#include <vcl/fntstyle.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

// class SvxCharRotateItem ----------------------------------------------

/* [Description]

	This item defines a character relief and has currently the values
	emboss, relief.

*/

class SvxCharReliefItem : public SfxEnumItem
{
public:
	TYPEINFO();

	SvxCharReliefItem( FontRelief eValue = RELIEF_NONE,
					   const sal_uInt16 nId = ITEMID_CHARRELIEF );

	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT) const;
	virtual SvStream& 		Store(SvStream & rStrm, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;

	virtual String			GetValueTextByPos( USHORT nPos ) const;
 	virtual USHORT			GetValueCount() const;

	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									String &rText,
                                    const ::IntlWrapper * = 0 ) const;

	virtual sal_Bool PutValue( const ::com::sun::star::uno::Any& rVal,
									BYTE nMemberId );
	virtual sal_Bool QueryValue( ::com::sun::star::uno::Any& rVal,
								BYTE nMemberId ) const;

	inline SvxCharReliefItem& operator=( const SvxCharReliefItem& rItem )
	{
		SetValue( rItem.GetValue() );
		return *this;
	}
};

}//end of namespace binfilter
#endif

