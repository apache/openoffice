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


#ifndef _MDIEXP_HXX
#define _MDIEXP_HXX

#include <bf_svtools/bf_solar.h>

class UniString; 
class Size; 
class Dialog; 

namespace binfilter {


class SwRect;
class ViewShell;
class SwDoc;
class SwDocShell;
class SfxObjectShell;
class SfxFrame;

extern void ScrollMDI(ViewShell* pVwSh, const SwRect &, USHORT nRangeX, USHORT nRangeY);
extern BOOL IsScrollMDI(ViewShell* pVwSh, const SwRect &);
extern void SizeNotify(ViewShell* pVwSh, const Size &);

//Update der Statusleiste, waehrend einer Action.
extern void PageNumNotify( ViewShell* pVwSh,
							USHORT nPhyNum,
							USHORT nVirtNum,
						   const UniString& rPg );

enum FlyMode { FLY_DRAG_START, FLY_DRAG, FLY_DRAG_END };
extern void FrameNotify( ViewShell* pVwSh, FlyMode eMode = FLY_DRAG );

void StartProgress	   ( USHORT nMessId, long nStartVal, long nEndVal, SwDocShell *pDocSh = 0 );
void EndProgress  	   ( SwDocShell *pDocSh = 0 );
void SetProgressState  ( long nPosition, SwDocShell *pDocShell );
void RescheduleProgress( SwDocShell *pDocShell );

void EnableCmdInterface(BOOL bEnable = TRUE);

Dialog* GetSearchDialog();

void RepaintPagePreview( ViewShell* pVwSh, const SwRect& rRect );

// ndgrf.cxx
// alle QuickDraw-Bitmaps des speziellen Docs loeschen
void DelAllGrfCacheEntries( SwDoc* pDoc );

extern void JavaScriptScrollMDI( SfxFrame* pFrame, INT32 nX, INT32 nY );

// ChgMode fuer Tabellen aus der Konfiguration lesen
USHORT GetTblChgDefaultMode();

BOOL JumpToSwMark( ViewShell* pVwSh, const UniString& rMark );


} //namespace binfilter
#endif
