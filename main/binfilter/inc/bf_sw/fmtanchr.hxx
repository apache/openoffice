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


#ifndef _FMTANCHR_HXX
#define _FMTANCHR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SWTYPES_HXX //autogen
#include <swtypes.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper; 
namespace binfilter {

struct SwPosition;

#define IVER_FMTANCHOR_LONGIDX ((USHORT)1)

//FlyAnchor, Anker des Freifliegenden Rahmen ----

class SwFmtAnchor: public SfxPoolItem
{
	SwPosition *pCntntAnchor;	//0 Fuer Seitengebundene Rahmen.
								//Index fuer Absatzgebundene Rahmen.
								//Position fuer Zeichengebundene Rahmen
	RndStdIds  nAnchorId;
	USHORT	   nPageNum;		//Seitennummer bei Seitengeb. Rahmen.

public:
	SwFmtAnchor( RndStdIds eRnd = FLY_PAGE, USHORT nPageNum = 0 );
	SwFmtAnchor( const SwFmtAnchor &rCpy );
	~SwFmtAnchor();

	//	Zuweisungsoperator
	SwFmtAnchor &operator=( const SwFmtAnchor& );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;

	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	RndStdIds GetAnchorId() const { return nAnchorId; }
	USHORT GetPageNum() const { return nPageNum; }
	const SwPosition *GetCntntAnchor() const { return pCntntAnchor; }

	void SetPageNum( USHORT nNew ) { nPageNum = nNew; }

	void SetType( RndStdIds nRndId ) { nAnchorId = nRndId; }
	void SetAnchor( const SwPosition *pPos );
};

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtAnchor &SwAttrSet::GetAnchor(BOOL bInP) const
 	{ return (const SwFmtAnchor&)Get( RES_ANCHOR,bInP); }
 
 inline const SwFmtAnchor &SwFmt::GetAnchor(BOOL bInP) const
 	{ return aSet.GetAnchor(bInP); }
#endif

} //namespace binfilter
#endif

