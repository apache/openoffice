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


#ifndef _SPLARGS_HXX
#define _SPLARGS_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
#include <limits.h>       // USHRT_MAX
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

class SwTxtNode;
class SwIndex;
class SpellCheck;

#ifndef _COM_SUN_STAR_LINGUISTIC2_XSPELLCHECKER1_HPP_
#include <com/sun/star/linguistic2/XSpellChecker1.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_XHYPHENATEDWORD_HPP_
#include <com/sun/star/linguistic2/XHyphenatedWord.hpp>
#endif
namespace binfilter {

/*************************************************************************
 *                      struct SwConversionArgs
 *************************************************************************/


struct SwArgsBase     // used for text conversion (Hangul/Hanja, ...)
{
    SwTxtNode  *pStartNode;
    SwIndex    &rStartIdx;
    SwTxtNode  *pEndNode;
    SwIndex    &rEndIdx;

    SwArgsBase(
            SwTxtNode* pStart, SwIndex& rStart,
            SwTxtNode* pEnd, SwIndex& rEnd )
        : pStartNode( pStart ), rStartIdx( rStart ),
          pEndNode( pEnd ), rEndIdx( rEnd )
        {}
};

/*************************************************************************
 *                      struct SwConversionArgs
 * used for text conversion (Hangul/Hanja, ...)
 *************************************************************************/

struct SwConversionArgs : SwArgsBase
{
    ::rtl::OUString   aConvText;
    sal_Bool        bConvTextFound;     // convertible text portion found

    SwConversionArgs(
            SwTxtNode* pStart, SwIndex& rStart,
            SwTxtNode* pEnd, SwIndex& rEnd )
        : SwArgsBase( pStart, rStart, pEnd, rEnd ),
          bConvTextFound( sal_False )
        {}
};

/*************************************************************************
 *                      struct SwSpellArgs
 *************************************************************************/

struct SwSpellArgs : SwArgsBase
{
	::com::sun::star::uno::Reference<
		::com::sun::star::linguistic2::XSpellChecker1 >  	xSpeller;

	::com::sun::star::uno::Reference<
		::com::sun::star::linguistic2::XSpellAlternatives >	xSpellAlt;

	SwSpellArgs(::com::sun::star::uno::Reference<
			::com::sun::star::linguistic2::XSpellChecker1 > &rxSplChk,
			SwTxtNode* pStart, SwIndex& rStart,
            SwTxtNode* pEnd, SwIndex& rEnd )
        :   SwArgsBase( pStart, rStart, pEnd, rEnd ),
            xSpeller( rxSplChk )
		{}
};

/*************************************************************************
 *						class SwInterHyphInfo
 *************************************************************************/

// Parameter-Klasse fuer Hyphenate
// docedt.cxx:	SwDoc::Hyphenate()
// txtedt.cxx:	SwTxtNode::Hyphenate()
// txthyph.cxx: SwTxtFrm::Hyphenate()

class SwInterHyphInfo
{
	::com::sun::star::uno::Reference<
		::com::sun::star::linguistic2::XHyphenatedWord > 	xHyphWord;
	const	Point aCrsrPos;
	sal_Bool	bAuto	: 1;
	sal_Bool	bNoLang	: 1;
	sal_Bool	bCheck 	: 1;
public:
	xub_StrLen nStart;
	xub_StrLen nLen;
	xub_StrLen nWordStart;
	xub_StrLen nWordLen;
	xub_StrLen nHyphPos;
	sal_uInt16 nMinTrail;

	inline SwInterHyphInfo( const Point &rCrsrPos,
							const sal_uInt16 nStart = 0,
							const sal_uInt16 nLen = USHRT_MAX )
		 : aCrsrPos( rCrsrPos ),
		   bAuto(sal_False), bNoLang(sal_False), bCheck(sal_False),
		   nStart(nStart), nLen(nLen),
		   nWordStart(0), nWordLen(0),
		   nMinTrail(0), nHyphPos(0)
		 { }
	inline xub_StrLen GetStart() const { return nStart; }
	inline xub_StrLen GetLen() const { return nLen; }
	inline xub_StrLen GetEnd() const
	{ return STRING_LEN == nLen ? nLen : nStart + nLen; }
	inline const Point *GetCrsrPos() const
	{ return aCrsrPos.X() || aCrsrPos.Y() ? &aCrsrPos : 0; }
	inline sal_Bool IsAuto() const { return bAuto; }
	inline void SetAuto( const sal_Bool bNew ) { bAuto = bNew; }
	inline sal_Bool IsCheck() const { return bCheck; }
	inline void SetCheck( const sal_Bool bNew ) { bCheck = bNew; }
	inline sal_Bool IsNoLang() const { return bNoLang; }
	inline void SetNoLang( const sal_Bool bNew ) { bNoLang = bNew; }

	inline void
			SetHyphWord(const ::com::sun::star::uno::Reference<
				::com::sun::star::linguistic2::XHyphenatedWord >  &rxHW)
			{ xHyphWord = rxHW; }
	inline ::com::sun::star::uno::Reference<
		::com::sun::star::linguistic2::XHyphenatedWord >
			GetHyphWord() { return xHyphWord; }
};


} //namespace binfilter
#endif
