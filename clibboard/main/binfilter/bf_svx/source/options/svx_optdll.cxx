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
#include <tools/svwin.h>
#endif
#ifndef _SYSDEP_HXX
#include <vcl/sysdep.hxx>
#endif
#endif
namespace binfilter {

#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifdef WIN

/***************************************************************************
|*
|*    LibMain()
|*
|*    Beschreibung       Initialisierungsfunktion der DLL
|*    Ersterstellung     TH 05.05.93
|*    Letzte Aenderung   PB 04.09.96
|*
***************************************************************************/

extern "C" int CALLBACK LibMain( HINSTANCE hDLL, WORD, WORD nHeap, LPSTR )
{
	if ( nHeap )
		UnlockData( 0 );
	return TRUE;
}

/***************************************************************************
|*
|*    WEP()
|*
|*    Beschreibung      DLL-Deinitialisierung
|*    Ersterstellung     TH 05.05.93
|*    Letzte Aenderung   TH 05.05.93
|*
***************************************************************************/

extern "C" int CALLBACK WEP( int )
{
	return 1;
}

//==========================================================================

#endif

}
