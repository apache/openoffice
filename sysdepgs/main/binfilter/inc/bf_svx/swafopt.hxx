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


#ifndef _SVXSWAFOPT_HXX
#define _SVXSWAFOPT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _VCL_FONT_HXX //autogen
#include <vcl/font.hxx>
#endif
class SvStringsISortDtor;
namespace binfilter {


// Klasse fuer Optionen vom Autoformat
struct SvxSwAutoFmtFlags
{
	Font aBulletFont;
	Font aByInputBulletFont;
	const SvStringsISortDtor* pAutoCmpltList;  // only valid inside the Dialog!!!

	sal_Unicode cBullet;
	sal_Unicode cByInputBullet;

	USHORT nAutoCmpltWordLen, nAutoCmpltListLen;
	USHORT nAutoCmpltExpandKey;

	BYTE nRightMargin;

	BOOL bReplaceQuote : 1;
	BOOL bAutoCorrect : 1;
	BOOL bCptlSttSntnc : 1;
	BOOL bCptlSttWrd : 1;
	BOOL bChkFontAttr : 1;

	BOOL bChgUserColl : 1;
	BOOL bChgEnumNum : 1;

	BOOL bAFmtByInput : 1;
	BOOL bDelEmptyNode : 1;
	BOOL bSetNumRule : 1;

	BOOL bChgFracionSymbol : 1;
	BOOL bChgOrdinalNumber : 1;
	BOOL bChgToEnEmDash : 1;
	BOOL bChgWeightUnderl : 1;
	BOOL bSetINetAttr : 1;

	BOOL bSetBorder : 1;
	BOOL bCreateTable : 1;
	BOOL bReplaceStyles : 1;
	BOOL bDummy : 1;

	BOOL bWithRedlining : 1;

	BOOL bRightMargin : 1;

	BOOL bAutoCompleteWords : 1;
	BOOL bAutoCmpltCollectWords : 1;
	BOOL bAutoCmpltEndless : 1;
// -- under NT hier starts a new long
	BOOL bAutoCmpltAppendBlanc : 1;
	BOOL bAutoCmpltShowAsTip : 1;

	BOOL bAFmtDelSpacesAtSttEnd : 1;
	BOOL bAFmtDelSpacesBetweenLines : 1;
	BOOL bAFmtByInpDelSpacesAtSttEnd : 1;
	BOOL bAFmtByInpDelSpacesBetweenLines : 1;

    BOOL bAutoCmpltKeepList : 1;

	// some dummies for any new options
    BOOL bDummy5 : 1,
		 bDummy6 : 1,
		 bDummy7 : 1,
		 bDummy8 : 1
		 ;

	SvxSwAutoFmtFlags( const SvxSwAutoFmtFlags& rAFFlags ) { *this = rAFFlags; }
	SvxSwAutoFmtFlags& operator=( const SvxSwAutoFmtFlags& );
};

}//end of namespace binfilter
#endif

