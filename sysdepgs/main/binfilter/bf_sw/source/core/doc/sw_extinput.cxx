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



#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _EXTINPUT_HXX
#include <extinput.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
namespace binfilter {








// die Doc Schnittstellen:



/*N*/ SwExtTextInput* SwDoc::GetExtTextInput( const SwNode& rNd,
/*N*/ 										xub_StrLen nCntntPos ) const
/*N*/ {
/*N*/ 	SwExtTextInput* pRet = 0;
/*N*/ 	if( pExtInputRing )
/*N*/ 	{
/*?*/ 		ULONG nNdIdx = rNd.GetIndex();
/*?*/ 		SwExtTextInput* pTmp = (SwExtTextInput*)pExtInputRing;
/*?*/ 		do {
/*?*/ 			ULONG nPt = pTmp->GetPoint()->nNode.GetIndex(),
/*?*/ 				  nMk = pTmp->GetMark()->nNode.GetIndex();
/*?*/ 			xub_StrLen nPtCnt = pTmp->GetPoint()->nContent.GetIndex(),
/*?*/ 				  	   nMkCnt = pTmp->GetMark()->nContent.GetIndex();
/*?*/ 
/*?*/ 			if( nPt < nMk || ( nPt == nMk && nPtCnt < nMkCnt ))
/*?*/ 			{
/*?*/ 				ULONG nTmp = nMk; nMk = nPt; nPt = nTmp;
/*?*/ 				nTmp = nMkCnt; nMkCnt = nPtCnt; nPtCnt = (xub_StrLen)nTmp;
/*?*/ 			}
/*?*/ 
/*?*/ 			if( nMk <= nNdIdx && nNdIdx <= nPt &&
/*?*/ 				( STRING_NOTFOUND == nCntntPos ||
/*?*/ 					( nMkCnt <= nCntntPos && nCntntPos <= nPtCnt )))
/*?*/ 			{
/*?*/ 				pRet = pTmp;
/*?*/ 				break;
/*?*/ 			}
/*?*/ 		} while( pExtInputRing != (pTmp = (SwExtTextInput*)pExtInputRing ) );
/*N*/ 	}
/*N*/ 	return pRet;
/*N*/ }



}
