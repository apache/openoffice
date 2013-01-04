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

#ifndef _DOC_HXX
#include <doc.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _SW3IMP_HXX
#include <sw3imp.hxx>
#endif

#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
namespace binfilter {

#define STREAM_STGREAD  ( STREAM_READ | STREAM_SHARE_DENYWRITE | STREAM_NOCREATE )
#define STREAM_STGWRITE ( STREAM_READ | STREAM_WRITE | STREAM_SHARE_DENYWRITE )

sal_Char __FAR_DATA N_BLOCKINFO[] = "AutotextInfo";


/*************************************************************************
*
* 	Enumeration aller Textbausteine
*
*************************************************************************/

// Das Directory enthaelt die Kurz- und Langnamen der Bausteine. Die
// Bausteine selbst sind unter ihrem Kurznamen in eigenen Streams
// abgelegt. Die Namen selbst sind nach folgender Konvention aufgebaut:
// 1. Zeichen: #
// ! == 0x01
// / == 0x0F
// \ == 0x0C
// : == 0x0A
// . == 0x0E


// Der Fehlercode wird zurueckgeliefert. Das Ende der Liste
// wird durch einen leeren Short-Namen angezeigt.


/*************************************************************************
*
* 	I/O einzelner Textbausteine
*
*************************************************************************/


////////////////////////////////////////////////////////////////////////////


// Textbaustein-Konversionsmode ein/ausschalten
// Das Flag verhindert das Commit auf die Root nach dem Schreiben
// eines Textbausteins; daher wird hier committed, wenn das
// Flag geloescht wird.



// ggf. eine SvPersist-Instanz einrichten

/*N*/ BOOL Sw3IoImp::CheckPersist()
/*N*/ {
/*N*/ 	// Haben wir schon einen Persist?
/*N*/ 	if( pDoc->GetPersist() )
/*N*/ 		return TRUE;
		DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001 /*?*/ 	pPersist = new Sw3Persist;
/*N*/ }

// ggf. eine SvPersist-Instanz freigeben


//////////////////////////////////////////////////////////////////////////



}
