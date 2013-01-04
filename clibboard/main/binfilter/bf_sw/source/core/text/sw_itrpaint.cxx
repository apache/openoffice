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




#ifdef _MSC_VER
#pragma hdrstop
#endif


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "txtatr.hxx"  // SwINetFmt


#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx> // SwPageDesc
#endif

#include "itrpaint.hxx"
#include "pormulti.hxx"
namespace binfilter {

/*************************************************************************
 *                  IsUnderlineBreak
 *
 * Returns, if we have an underline breaking situation
 * Adding some more conditions here means you also have to change them
 * in SwTxtPainter::CheckSpecialUnderline
 *************************************************************************/
/*N*/ sal_Bool IsUnderlineBreak( const SwLinePortion& rPor, const SwFont& rFnt )
/*N*/ {
/*N*/     return UNDERLINE_NONE == rFnt.GetUnderline() ||
/*N*/            rPor.IsFlyPortion() || rPor.IsFlyCntPortion() ||
/*N*/            rPor.IsBreakPortion() || rPor.IsMarginPortion() ||
/*N*/            rPor.IsHolePortion() ||
/*N*/           ( rPor.IsMultiPortion() && ! ((SwMultiPortion&)rPor).IsBidi() ) ||
/*N*/            rFnt.GetEscapement() < 0 || rFnt.IsWordLineMode() ||
/*N*/            SVX_CASEMAP_KAPITAELCHEN == rFnt.GetCaseMap();
/*N*/ }

/*************************************************************************
 *					SwTxtPainter::CtorInit()
 *************************************************************************/
/*N*/ void SwTxtPainter::CtorInit( SwTxtFrm *pFrm, SwTxtPaintInfo *pNewInf )
/*N*/ {
/*N*/ 	SwTxtCursor::CtorInit( pFrm, pNewInf );
/*N*/ 	pInf = pNewInf;
/*N*/ 	SwFont *pFnt = GetFnt();
/*N*/ 	GetInfo().SetFont( pFnt );
/*N*/ #ifdef DBG_UTIL
/*N*/ 	if( ALIGN_BASELINE != pFnt->GetAlign() )
/*N*/ 	{
/*?*/ 		ASSERT( ALIGN_BASELINE == pFnt->GetAlign(),
/*?*/ 				"+SwTxtPainter::CTOR: font alignment revolution" );
/*?*/ 		pFnt->SetAlign( ALIGN_BASELINE );
/*N*/ 	}
/*N*/ #endif
/*N*/ 	bPaintDrop = sal_False;
/*N*/ }


/*************************************************************************
 *					  SwTxtPainter::CalcPaintOfst()
 *************************************************************************/

/*************************************************************************
 *                    SwTxtPainter::DrawTextLine()
 *
 * Es gibt zwei Moeglichkeiten bei transparenten Font auszugeben:
 * 1) DrawRect auf die ganze Zeile und die DrawText hinterher
 *	  (objektiv schnell, subjektiv langsam).
 * 2) Fuer jede Portion ein DrawRect mit anschliessendem DrawText
 *	  ausgefuehrt (objektiv langsam, subjektiv schnell).
 * Da der User in der Regel subjektiv urteilt, wird die 2. Methode
 * als Default eingestellt.
 *************************************************************************/

}
