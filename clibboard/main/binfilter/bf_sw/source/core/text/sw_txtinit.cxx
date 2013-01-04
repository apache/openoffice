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

#include "fntcache.hxx"     // pFntCache  ( SwFont/ScrFont-PrtFont Cache )
#include "swfntcch.hxx"     // pSwFontCache  ( SwAttrSet/SwFont Cache )
#include "txtfrm.hxx"
#include "txtcache.hxx"
#include "porrst.hxx"

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "pordrop.hxx"
#include "txtfly.hxx"	// SwContourCache
#include "dbg_lay.hxx"  // Layout Debug Fileausgabe
namespace binfilter { 

/*N*/ SwCache *SwTxtFrm::pTxtCache = 0;
/*N*/ long SwTxtFrm::nMinPrtLine = 0;
/*N*/ SwContourCache *pContourCache = 0;

#ifndef PROFILE
// Code zum Initialisieren von Statics im eigenen Code-Segment
#ifdef _MSC_VER
#pragma code_seg( "SWSTATICS" )
#endif
#endif

/*N*/ IMPL_FIXEDMEMPOOL_NEWDEL( SwTxtLine, 	  50,  50 )
/*N*/ IMPL_FIXEDMEMPOOL_NEWDEL( SwParaPortion,  50,  50 )	//Absaetze
/*N*/ IMPL_FIXEDMEMPOOL_NEWDEL( SwLineLayout,  150, 150 )	//Zeilen
/*N*/ IMPL_FIXEDMEMPOOL_NEWDEL( SwHolePortion, 150, 150 )	//z.B. Blanks am Zeilenende
/*N*/ IMPL_FIXEDMEMPOOL_NEWDEL( SwTxtPortion,  200, 100 )	//Attributwechsel

#ifndef PROFILE
#ifdef _MSC_VER
#pragma code_seg()
#endif
#endif

/*************************************************************************
 *                  _TextInit(), _TextFinit()
 *************************************************************************/

// Werden _nur_ in init.cxx verwendet, dort stehen extern void _TextFinit()
// und extern void _TextInit(...)

/*N*/ void _TextInit()
/*N*/ {
/*N*/ 	pFntCache = new SwFntCache;
/*N*/ 	pSwFontCache = new SwFontCache;
/*N*/ 	pWaveCol = new Color( COL_GRAY );
/*N*/ 
/*N*/ 	//Pauschale groesse 250, plus 100 pro Shell
/*N*/ 	SwCache *pTxtCache = new SwCache( 250, 100
/*N*/ #ifdef DBG_UTIL
/*N*/ 	, "static SwTxtFrm::pTxtCache"
/*N*/ #endif
/*N*/ 	);
/*N*/ 	SwTxtFrm::SetTxtCache( pTxtCache );
/*N*/ 	PROTOCOL_INIT
/*N*/ }

/*N*/ void _TextFinit()
/*N*/ {
/*N*/ 	PROTOCOL_STOP
/*N*/ 	delete SwTxtFrm::GetTxtCache();
/*N*/ 	delete pSwFontCache;
/*N*/ 	delete pFntCache;
/*N*/ 	delete pWaveCol;
/*N*/ 	delete pContourCache;
/*N*/ }



}
