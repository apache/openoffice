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


//#define TEST_DELAYED_RESIZE

#ifdef TEST_DELAYED_RESIZE
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif


#include "htmltbl.hxx"
namespace binfilter {

#define COLFUZZY 20
#define MAX_TABWIDTH (USHRT_MAX - 2001)

/*N*/ SwHTMLTableLayout::~SwHTMLTableLayout()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

// Die Breiten der Umrandung werden zunaechst wie in Netscape berechnet:
// Aussere Umrandung: BORDER + CELLSPACING + CELLPADDING
// Innere Umrandung: CELLSPACING + CELLPADDING
// Allerdings wird die Breite der Umrandung im SW trotzdem beachtet, wenn
// bSwBorders gesetzt ist, damit nicht faellschlich umgebrochen wird.
// MIB 27.6.97: Dabei muss auch der Abstand zum Inhalt berueckichtigt werden,
// und zwar auch dann, wenn wenn nur die gegenueberliegende Seite
// eine Umrandung hat.













// nAbsAvail ist der verfuegbare Platz in TWIPS.
// nRelAvail ist der auf USHRT_MAX bezogene verfuegbare Platz oder 0
// nAbsSpace ist der Anteil von nAbsAvail, der durch der umgebende Zelle
//           fur die Umrandung und den Abstand zum Inhalt reserviert ist.











}
