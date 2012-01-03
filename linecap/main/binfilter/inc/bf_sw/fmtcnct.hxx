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


#ifndef _FMTCNCT_HXX
#define _FMTCNCT_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper; 
namespace binfilter {


class SwFlyFrmFmt;


//Verbindung (Textfluss) zwischen zwei FlyFrms

class SwFmtChain: public SfxPoolItem
{
	SwClient aPrev,	//Vorgaenger (SwFlyFrmFmt), wenn es diesen gibt.
			 aNext; //Nachfolger (SwFlyFrmFmt), wenn es diesen gibt.


public:
	SwFmtChain() : SfxPoolItem( RES_CHAIN ) {}
    SwFmtChain( const SwFmtChain &rCpy );

    inline SwFmtChain &operator=( const SwFmtChain& );

	// "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;

	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;

	SwFlyFrmFmt* GetPrev() const { return (SwFlyFrmFmt*)aPrev.GetRegisteredIn(); }
	SwFlyFrmFmt* GetNext() const { return (SwFlyFrmFmt*)aNext.GetRegisteredIn(); }


    void SetPrev( SwFlyFrmFmt *pFmt );
    void SetNext( SwFlyFrmFmt *pFmt );
};

SwFmtChain &SwFmtChain::operator=( const SwFmtChain &rCpy )
{
    SetPrev( rCpy.GetPrev() );
    SetNext( rCpy.GetNext() );
    return *this;
}


#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtChain &SwAttrSet::GetChain(BOOL bInP) const
	{ return (const SwFmtChain&)Get( RES_CHAIN,bInP); }

inline const SwFmtChain &SwFmt::GetChain(BOOL bInP) const
	{ return aSet.GetChain(bInP); }
#endif

} //namespace binfilter
#endif

