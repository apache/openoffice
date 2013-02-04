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


#ifndef _FMTCNTNT_HXX
#define _FMTCNTNT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
namespace binfilter {

class SwNodeIndex;
class SwStartNode;

//Cntnt, Inhalt des Frame (Header, Footer, Fly) ----

class SwFmtCntnt: public SfxPoolItem
{
	SwNodeIndex *pStartNode;

	SwFmtCntnt &operator=( const SwFmtCntnt & ); //Kopieren verboten

public:
//	SwFmtCntnt( const SwNodeIndex *pStartNodeIdx = 0 );
	SwFmtCntnt( const SwStartNode* pStartNode = 0 );
	SwFmtCntnt( const SwFmtCntnt &rCpy );
	~SwFmtCntnt();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;

	const SwNodeIndex *GetCntntIdx() const { return pStartNode; }
	void SetNewCntntIdx( const SwNodeIndex *pIdx );
};

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.hxx; revisit with gcc3
inline const SwFmtCntnt &SwAttrSet::GetCntnt(BOOL bInP) const
	{ return (const SwFmtCntnt&)Get( RES_CNTNT,bInP); }

inline const SwFmtCntnt &SwFmt::GetCntnt(BOOL bInP) const
	{ return aSet.GetCntnt(bInP); }
#endif

} //namespace binfilter
#endif

