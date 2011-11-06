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



#ifndef	_REDLNITR_HXX
#define	_REDLNITR_HXX

#include "ndhints.hxx"
#include "redlenum.hxx"	// SwRedlineType
#include "swfont.hxx"
#ifndef _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>
#endif
namespace binfilter {
class SfxItemSet; 

class SwTxtNode;
class SwDoc;

class SwAttrHandler;

class SwExtend
{
	SwFont *pFnt;
	const SvUShorts &rArr;	// XAMA: Array of xub_StrLen
	xub_StrLen nStart;
	xub_StrLen nPos;
	xub_StrLen nEnd;
	sal_Bool _Leave( SwFont& rFnt, xub_StrLen nNew );
	sal_Bool Inside() const { return ( nPos >= nStart && nPos < nEnd ); }
    void ActualizeFont( SwFont &rFnt, xub_StrLen nAttr );
public:
	SwExtend( const SvUShorts &rA, xub_StrLen nSt ) : rArr( rA ), pFnt(0),
		nStart( nSt ), nPos( STRING_LEN ), nEnd( nStart + rA.Count() ) {}
	~SwExtend() { delete pFnt; }
	sal_Bool IsOn() const { return pFnt != 0; }
	void Reset() { if( pFnt ) { delete pFnt; pFnt = NULL; } nPos = STRING_LEN; }
	sal_Bool Leave( SwFont& rFnt, xub_StrLen nNew )
		{ if( pFnt ) return _Leave( rFnt, nNew ); return sal_False; }
	short Enter( SwFont& rFnt, xub_StrLen nNew );
	xub_StrLen Next( xub_StrLen nNext );
    SwFont* GetFont()  { return pFnt; }
    void UpdateFont( SwFont &rFnt ) { ActualizeFont( rFnt, rArr[ nPos - nStart ] ); }
};

class SwRedlineItr
{
	SwExtend *pExt;
	sal_Bool bOn;
public:
SwRedlineItr( const SwTxtNode& rTxtNd, SwFont& rFnt, SwAttrHandler& rAH,//STRIP001 SwRedlineItr( const SwTxtNode& rTxtNd, SwFont& rFnt, SwAttrHandler& rAH,
xub_StrLen nRedlPos, sal_Bool bShw, const SvUShorts *pArr = 0,//STRIP001         xub_StrLen nRedlPos, sal_Bool bShw, const SvUShorts *pArr = 0,
xub_StrLen nStart = STRING_LEN ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001        xub_StrLen nStart = STRING_LEN );
	inline sal_Bool IsOn() const { return bOn || ( pExt && pExt->IsOn() ); }
			sal_Bool CheckLine( xub_StrLen nChkStart, xub_StrLen nChkEnd ){DBG_BF_ASSERT(0, "STRIP"); return FALSE;} //STRIP001 	sal_Bool CheckLine( xub_StrLen nChkStart, xub_StrLen nChkEnd );
	inline sal_Bool ExtOn() { if( pExt ) return pExt->IsOn(); return sal_False; }
};


} //namespace binfilter
#endif

