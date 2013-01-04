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


#ifndef _SVX_GRFCROP_HXX
#define _SVX_GRFCROP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

#define GRFCROP_VERSION_SWDEFAULT		0
#define GRFCROP_VERSION_MOVETOSVX 		1

class SvxGrfCrop : public SfxPoolItem
{
	sal_Int32	nLeft, nRight, nTop, nBottom;
public:
//	TYPEINFO();

	SvxGrfCrop( USHORT = ITEMID_GRF_CROP );
	SvxGrfCrop( sal_Int32 nLeft,	sal_Int32 nRight,
				sal_Int32 nTop,		sal_Int32 nBottom,
				USHORT = ITEMID_GRF_CROP );
	virtual ~SvxGrfCrop();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int            		operator==( const SfxPoolItem& ) const;
// must be implement by the specific items
//	virtual SfxPoolItem*		Clone( SfxItemPool *pPool = 0 ) const;
//	virtual USHORT			 	GetVersion( USHORT nFileVersion ) const;
	virtual SfxPoolItem*		Create(SvStream &, USHORT nVer) const;
	virtual SvStream&			Store(SvStream &, USHORT nIVer) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 );

	void SetLeft( sal_Int32 nVal )		{ nLeft = nVal; }
	void SetRight( sal_Int32 nVal )		{ nRight = nVal; }
	void SetTop( sal_Int32 nVal )		{ nTop = nVal; }
	void SetBottom( sal_Int32 nVal )	{ nBottom = nVal; }

	sal_Int32 GetLeft() const			{ return nLeft; }
	sal_Int32 GetRight() const 			{ return nRight; }
	sal_Int32 GetTop() const			{ return nTop; }
	sal_Int32 GetBottom() const			{ return nBottom; }

	inline SvxGrfCrop& operator=( const SvxGrfCrop& rCrop )
		{
			nLeft = rCrop.GetLeft(); 		nTop = rCrop.GetTop();
			nRight = rCrop.GetRight();		nBottom = rCrop.GetBottom();
			return *this;
		}
};

}//end of namespace binfilter
#endif  // _GRFATR_HXX
