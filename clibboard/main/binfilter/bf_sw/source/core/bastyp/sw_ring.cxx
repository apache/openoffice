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

#include "ring.hxx"
namespace binfilter {


/*************************************************************************
|*
|*    Ring::Ring()
|*
|*    Ersterstellung    VB	02.07.91
|*    Letzte Aenderung  JP 10.10.97
|*
*************************************************************************/

/*N*/ Ring::Ring( Ring *pObj )
/*N*/ {
/*N*/ 	if( !pObj )
/*N*/ 		pNext = this, pPrev = this;
/*N*/ 	else
/*N*/ 	{
/*?*/ 		pNext = pObj;
/*?*/ 		pPrev = pObj->pPrev;
/*?*/ 		pObj->pPrev = this;
/*?*/ 		pPrev->pNext = this;
/*N*/ 	}
}

/*************************************************************************
|*
|*    Ring::~Ring()
|*
|*    Ersterstellung    VB	02.07.91
|*    Letzte Aenderung  JP 10.10.97
|*
*************************************************************************/

/*N*/ Ring::~Ring()
/*N*/ {
/*N*/ 	pNext->pPrev = pPrev;
/*N*/ 	pPrev->pNext = pNext;
/*N*/ }

/*************************************************************************
|*
|*    Ring::MoveTo
|*
|*    Ersterstellung    VB 4.3.91
|*    Letzte Aenderung  JP 10.10.97
|*
*************************************************************************/

/*N*/ void Ring::MoveTo(Ring *pDestRing)
/*N*/ {
/*N*/ 	// loeschen aus dem alten
/*N*/ 	pNext->pPrev = pPrev;
/*N*/ 	pPrev->pNext = pNext;
/*N*/ 
/*N*/ 	// im neuen einfuegen
/*N*/ 	if( pDestRing )
/*N*/ 	{
/*N*/ 		pNext = pDestRing;
/*N*/ 		pPrev = pDestRing->pPrev;
/*N*/ 		pDestRing->pPrev = this;
/*N*/ 		pPrev->pNext = this;
/*N*/ 	}
/*N*/ 	else
/*N*/ 		pNext = pPrev = this;
/*N*/ 
/*N*/ }



}
