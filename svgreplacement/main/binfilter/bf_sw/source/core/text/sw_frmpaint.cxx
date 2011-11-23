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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif



#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx> // SwPageDesc
#endif

#ifndef _ITRPAINT_HXX
#include <itrpaint.hxx>     // SwTxtPainter
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

namespace binfilter {


// steht im number.cxx
extern const sal_Char __FAR_DATA sBulletFntName[];

extern FASTBOOL bOneBeepOnly;

sal_Bool bInitFont = sal_True;

#define REDLINE_DISTANCE 567/4
#define REDLINE_MINDIST  567/10


/*************************************************************************
 * SwExtraPainter::PaintExtra()
 **************************************************************************/




/*************************************************************************
 *                      SwTxtFrm::Paint()
 *************************************************************************/

/*N*/ SwRect SwTxtFrm::Paint()
/*N*/ {
/*N*/ #if OSL_DEBUG_LEVEL > 1
/*N*/ 	const SwTwips nDbgY = Frm().Top();
/*N*/ #endif
/*N*/ 
/*N*/ 	// finger layout
/*N*/ 	ASSERT( GetValidPosFlag(), "+SwTxtFrm::Paint: no Calc()" );
/*N*/ 
/*N*/ 	SwRect aRet( Prt() );
/*N*/ 	if ( IsEmpty() || !HasPara() )
/*N*/ 		aRet += Frm().Pos();
/*N*/ 	else
/*N*/ 	{
/*N*/ 		// AMA: Wir liefern jetzt mal das richtige Repaintrechteck zurueck,
/*N*/ 		// 		d.h. als linken Rand den berechneten PaintOfst!
/*N*/ 		SwRepaint *pRepaint = GetPara()->GetRepaint();
/*N*/ 		long l;
/*N*/ 		if( pRepaint->GetOfst() )
/*N*/ 			pRepaint->Left( pRepaint->GetOfst() );
/*N*/ 
/*N*/ 		l = pRepaint->GetRightOfst();
/*N*/ 		if( l && ( pRepaint->GetOfst() || l > pRepaint->Right() ) )
/*N*/ 			 pRepaint->Right( l );
/*N*/ 		pRepaint->SetOfst( 0 );
/*N*/ 		aRet = *pRepaint;
/*N*/ 
/*N*/ #ifdef BIDI
/*N*/         if ( IsRightToLeft() )
				{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 /*?*/             SwitchLTRtoRTL( aRet );
/*N*/ #endif
/*N*/         if ( IsVertical() )
/*N*/             SwitchHorizontalToVertical( aRet );
/*N*/ 	}
/*N*/ 	ResetRepaint();
/*N*/ 
/*N*/     return aRet;
/*N*/ }

/*************************************************************************
 *                      SwTxtFrm::Paint()
 *************************************************************************/


/*************************************************************************
 *                      SwTxtFrm::Paint()
 *************************************************************************/


}
