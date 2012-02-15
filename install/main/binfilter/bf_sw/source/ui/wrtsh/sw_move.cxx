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

#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
namespace binfilter {

/*	Immer:
	-	Zuruecksetzen des Cursorstacks
	-	Timer nachtriggern
	-	gfs. GCAttr

	bei Selektion
	-	SttSelect()

	sonst
	-	EndSelect()
 */

const long nReadOnlyScrollOfst = 10;

/*------------------------------------------------------------------------
 Beschreibung:	Spaltenweises Springen
 Parameter: 	mit oder ohne SSelection
 Return:		Erfolg oder Misserfolg
------------------------------------------------------------------------*/


/*N*/ BOOL SwWrtShell::PopCrsr(BOOL bUpdate, BOOL bSelect)
/*N*/ {
/*N*/ 	if( 0 == pCrsrStack)
/*N*/ 		return FALSE;

/*?*/ 	const BOOL bValidPos = pCrsrStack->bValidCurPos;
/*?*/ 	if( bUpdate && bValidPos )
/*?*/ 	{
/*?*/ 			// falls ein Vorgaenger auf dem Stack steht, dessen Flag fuer eine
/*?*/ 			// gueltige Position verwenden.
/*?*/ 		SwRect aTmpArea(VisArea());
/*?*/ 		aTmpArea.Pos().Y() -= pCrsrStack->lOffset;
/*?*/ 		if( aTmpArea.IsInside( pCrsrStack->aDocPos ) )
/*?*/ 		{
/*?*/ 			DBG_BF_ASSERT(0, "STRIP"); //STRIP001 if( bSelect )
/*?*/ 		}
/*?*/ 			// Falls eine Verschiebung zwischen dem sichtbaren Bereich
/*?*/ 			// und der gemerkten Cursorpositionen auftritt, werden
/*?*/ 			// alle gemerkten Positionen weggeschmissen
/*?*/ 		else
/*?*/ 		{
/*?*/ 			DBG_BF_ASSERT(0, "STRIP"); //STRIP001 _ResetCursorStack();
/*?*/ 			return FALSE;
/*?*/ 		}
/*?*/ 	}
/*?*/ 	CrsrStack *pTmp = pCrsrStack;
/*?*/ 	pCrsrStack = pCrsrStack->pNext;
/*?*/ 	delete pTmp;
/*?*/ 	if( 0 == pCrsrStack )
/*?*/ 	{
/*?*/ 		ePageMove = MV_NO;
/*?*/ 		bDestOnStack = FALSE;
/*?*/ 	}
/*?*/ 	return bValidPos;
/*N*/ }

}
