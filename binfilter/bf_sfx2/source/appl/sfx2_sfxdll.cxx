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



#ifdef WIN
#ifndef _SVWIN_H
#include <svwin.h>
#endif
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifdef WIN
namespace binfilter {

// Statische DLL-Verwaltungs-Variablen
static HINSTANCE hDLLInst = 0;

//==========================================================================

/*N*/ extern "C" int CALLBACK LibMain( HINSTANCE hDLL, WORD, WORD nHeap, LPSTR )
/*N*/ {
/*N*/ #ifndef WNT
/*N*/ 	if ( nHeap )
/*N*/ 		UnlockData( 0 );
/*N*/ #endif
/*N*/ 
/*N*/ 	hDLLInst = hDLL;
/*N*/ 
/*N*/ 	return TRUE;
/*N*/ }


//--------------------------------------------------------------------------

/*N*/ extern "C" int CALLBACK WEP( int )
/*N*/ {
/*N*/ 	return 1;
/*N*/ }



//==========================================================================
}
#endif



