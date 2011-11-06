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



#ifndef _SVDOUTL_HXX //autogen
#include "svdoutl.hxx"
#endif
#ifndef _SVDOTEXT_HXX //autogen
#include "svdotext.hxx"
#endif
#ifndef _EDITSTAT_HXX //autogen wg. EE_CNTRL_STRETCHING
#include <editstat.hxx>
#endif
namespace binfilter {


//TYPEINIT1( SdrOutliner, Outliner );

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
/*N*/ SdrOutliner::SdrOutliner( SfxItemPool* pItemPool, USHORT nMode ):
/*N*/ Outliner( pItemPool, nMode ),
/*N*/ pTextObj( NULL )
/*N*/ {
/*N*/ }


/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/
/*N*/ SdrOutliner::~SdrOutliner()
/*N*/ {
/*N*/ }


/*************************************************************************
|*
|*
|*
\************************************************************************/
/*N*/ void SdrOutliner::SetTextObj( const SdrTextObj* pObj )
/*N*/ {
/*N*/ 	if( pObj && pObj != pTextObj )
/*N*/ 	{
/*N*/ 		SetUpdateMode(FALSE);
/*N*/ 		USHORT nOutlinerMode = OUTLINERMODE_OUTLINEOBJECT;
/*N*/ 		if ( !pObj->IsOutlText() )
/*N*/ 			nOutlinerMode = OUTLINERMODE_TEXTOBJECT;
/*N*/ 		Init( nOutlinerMode );
/*N*/ 
/*N*/ 		SetGlobalCharStretching(100,100);
/*N*/ 
/*N*/ 		ULONG nStat = GetControlWord();
/*N*/ 		nStat &= ~( EE_CNTRL_STRETCHING | EE_CNTRL_AUTOPAGESIZE );
/*N*/ 		SetControlWord(nStat);
/*N*/ 
/*N*/ 		Size aNullSize;
/*N*/ 		Size aMaxSize( 100000,100000 );
/*N*/ 		SetMinAutoPaperSize( aNullSize );
/*N*/ 		SetMaxAutoPaperSize( aMaxSize );
/*N*/ 		SetPaperSize( aMaxSize );
/*N*/ 		ClearPolygon();
/*N*/ 	}
/*N*/ 
/*N*/ 	pTextObj = pObj;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/
/*N*/ void SdrOutliner::SetTextObjNoInit( const SdrTextObj* pObj )
/*N*/ {
/*N*/ 	pTextObj = pObj;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/
/*N*/ XubString SdrOutliner::CalcFieldValue(const SvxFieldItem& rField, USHORT nPara, USHORT nPos,
/*N*/                                      Color*& rpTxtColor, Color*& rpFldColor)
/*N*/ {
/*N*/ 	FASTBOOL bOk = FALSE;
/*N*/ 	XubString aRet;
/*N*/ 
/*N*/ 	if (pTextObj)
/*N*/ 		bOk = pTextObj->CalcFieldValue(rField, nPara, nPos, FALSE, rpTxtColor, rpFldColor, aRet);
/*N*/ 
/*N*/ 	if (!bOk)
/*N*/ 		aRet = Outliner::CalcFieldValue(rField, nPara, nPos, rpTxtColor, rpFldColor);
/*N*/ 
/*N*/ 	return aRet;
/*N*/ }





}
