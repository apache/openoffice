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


#ifndef _SPLCSTK_HXX
#define _SPLCSTK_HXX

#include <bf_svtools/bf_solar.h>
namespace binfilter {

class SwIntrnlCrsr;
class SwCrsrShell;
struct SwPosition;

class SwSpellCrsrStk
{
	SwPosition* pErrorPos;
	SwIntrnlCrsr * pCurCrsr;		// der aktuelle Cursor
	SwIntrnlCrsr * pCrsrStk;		// der Cursor-Stack
	SwCrsrShell & rCrsrShell;		// die akt. CrsrShell

public:
	SwSpellCrsrStk( SwCrsrShell& rShell );
	~SwSpellCrsrStk();

	void SetCurCrsr();          // akt. Cursor in der CrsrShell mit akt. setzen
	BOOL SetNextCrsr();			// gehe zum naechsten Cursor im Ring (falls am
								// Ende returne FALSE;
	void RestoreCrsrs();		// alle Cursor aus dem CursorStack wieder
								// in der Shell setzen
	void StoreErrPos();			// speicher die akt. Start-Position als Error,
								// an dieser wird spaeter wieder aufgesetzt
	void SetErrPos();			// akt. Cursor auf die Error Position setzen

#if 0
							// ??? wird das ueberhaupt benoetigt ???
								// fuer die EditShell, damit sie die Cursor
								// aus den Loeschbereichen verschieben kann
	SwPaM* GetCurCrsr() const;
	SwPosition* GetErrorPos() const { return pErrorPos; }
#endif
};


} //namespace binfilter
#endif	// _SPLCSTK_HXX
