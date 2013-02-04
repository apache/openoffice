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


#ifndef _FMTFORDR_HXX
#define _FMTFORDR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
namespace binfilter {

//Die FillOrder ---------------------------------

enum SwFillOrder
{
	SW_FILL_ORDER_BEGIN,
	ATT_TOP_DOWN = SW_FILL_ORDER_BEGIN,
	ATT_BOTTOM_UP,
	ATT_LEFT_TO_RIGHT,
	ATT_RIGHT_TO_LEFT,
	SW_FILL_ORDER_END
};

class SwFmtFillOrder: public SfxEnumItem
{
public:
	SwFmtFillOrder( SwFillOrder = ATT_TOP_DOWN );
	inline SwFmtFillOrder &operator=( const SwFmtFillOrder &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetValueCount() const{DBG_BF_ASSERT(0, "STRIP"); return 0;} //STRIP001 virtual USHORT			GetValueCount() const;

	SwFillOrder GetFillOrder() const { return SwFillOrder(GetValue()); }
	void  SetFillOrder( const SwFillOrder eNew ) { SetValue( USHORT(eNew) ); }
};

inline SwFmtFillOrder &SwFmtFillOrder::operator=( const SwFmtFillOrder &rCpy )
{
	SetValue( rCpy.GetValue() );
	return *this;
}

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtFillOrder &SwAttrSet::GetFillOrder(BOOL bInP) const
	{ return (const SwFmtFillOrder&)Get( RES_FILL_ORDER,bInP); }

inline const SwFmtFillOrder &SwFmt::GetFillOrder(BOOL bInP) const
	{ return aSet.GetFillOrder(bInP); }
#endif

} //namespace binfilter
#endif

