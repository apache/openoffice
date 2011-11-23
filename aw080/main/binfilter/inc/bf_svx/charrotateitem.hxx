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


#ifndef _SVX_CHARROTATEITEM_HXX
#define _SVX_CHARROTATEITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

// class SvxCharRotateItem ----------------------------------------------

/* [Description]

	This item defines a character rotation value (0,1 degree). Currently
	character can only be rotated 90,0 and 270,0 degrees.
	The flag FitToLine defines only a UI-Information -
	if true it must also create a SvxCharScaleItem.

*/

class SvxCharRotateItem : public SfxUInt16Item
{
	sal_Bool bFitToLine;
public:
	TYPEINFO();

	SvxCharRotateItem( sal_uInt16 nValue = 0,
					   sal_Bool bFitIntoLine = sal_False,
					   const sal_uInt16 nId = ITEMID_CHARROTATE );

/*NBFF*/ 	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
/*NBFF*/ 	virtual SfxPoolItem*	Create(SvStream &, USHORT) const;
/*NBFF*/ 	virtual SvStream& 		Store(SvStream & rStrm, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;

/*NBFF*/ 	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
/*NBFF*/ 									SfxMapUnit eCoreMetric,
/*NBFF*/ 									SfxMapUnit ePresMetric,
/*NBFF*/ 									String &rText,
/*NBFF*/                                     const ::IntlWrapper * = 0 ) const;

/*NBFF*/ 	virtual sal_Bool PutValue( const ::com::sun::star::uno::Any& rVal,
/*NBFF*/ 									BYTE nMemberId );
/*NBFF*/ 	virtual sal_Bool QueryValue( ::com::sun::star::uno::Any& rVal,
/*NBFF*/ 								BYTE nMemberId ) const;

/*NBFF*/ 	inline SvxCharRotateItem& operator=( const SvxCharRotateItem& rItem )
/*NBFF*/ 	{
/*NBFF*/ 		SetValue( rItem.GetValue() );
/*NBFF*/ 		SetFitToLine( rItem.IsFitToLine() );
/*NBFF*/ 		return *this;
/*NBFF*/ 	}

/*NBFF*/ 	virtual int 			 operator==( const SfxPoolItem& ) const;

	// our currently only degree values
/*NBFF*/ 	void SetTopToBotton() 					{ SetValue( 2700 ); }
/*NBFF*/ 	void SetBottomToTop() 					{ SetValue(  900 ); }
/*NBFF*/ 	sal_Bool IsTopToBotton() const			{ return 2700 == GetValue(); }
/*NBFF*/ 	sal_Bool IsBottomToTop() const			{ return  900 == GetValue(); }
/*NBFF*/ 
/*NBFF*/ 	sal_Bool IsFitToLine() const 			{ return bFitToLine; }
/*NBFF*/ 	void SetFitToLine( sal_Bool b )			{ bFitToLine = b; }
};

}//end of namespace binfilter
#endif

