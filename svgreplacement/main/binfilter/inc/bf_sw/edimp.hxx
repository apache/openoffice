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



#ifndef _EDIMP_HXX
#define _EDIMP_HXX

#include <bf_svtools/bf_solar.h>

#include "crsrsh.hxx"
namespace binfilter {

/*
 * MACROS um ueber alle Bereiche zu iterieren
 */
#define PCURCRSR (_pStartCrsr)

#define FOREACHPAM_START(pCURSH) \
	{\
		SwPaM *_pStartCrsr = (pCURSH)->GetCrsr(), *__pStartCrsr = _pStartCrsr; \
		do {

#define FOREACHPAM_END() \
		} while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) != __pStartCrsr ); \
	}


#define FOREACHCURSOR_START(pCURSH) \
	{\
		SwShellCrsr *_pStartCrsr = *(pCURSH)->GetSwCrsr(), *__pStartCrsr = _pStartCrsr; \
		do {

#define FOREACHCURSOR_END() \
		} while( (_pStartCrsr=*(SwCursor*)_pStartCrsr->GetNext()) != __pStartCrsr ); \
	}


struct SwPamRange
{
	ULONG nStart, nEnd;

	SwPamRange() : nStart( 0 ), nEnd( 0 )	{}
	SwPamRange( ULONG nS, ULONG nE ) : nStart( nS ), nEnd( nE )	{}

	BOOL operator==( const SwPamRange& rRg )
		{ return nStart == rRg.nStart ? TRUE : FALSE; }
	BOOL operator<( const SwPamRange& rRg )
		{ return nStart < rRg.nStart ? TRUE : FALSE; }
};

SV_DECL_VARARR_SORT( _SwPamRanges, SwPamRange, 0, 1 )

class SwPamRanges : private _SwPamRanges
{
public:
	SwPamRanges( const SwPaM& rRing ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SwPamRanges( const SwPaM& rRing );

	SwPaM& SetPam( USHORT nArrPos, SwPaM& rPam ){DBG_BF_ASSERT(0, "STRIP"); return rPam;} //STRIP001 SwPaM& SetPam( USHORT nArrPos, SwPaM& rPam );

	USHORT Count() const
				{	return _SwPamRanges::Count(); }
};


} //namespace binfilter
#endif
