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
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#if OSL_DEBUG_LEVEL > 1
//nur zum debugen
#endif
namespace binfilter {

using namespace ::com::sun::star::lang;




/*--------------------------------------------------------------------
	Beschreibung: Ein Sortierelement fuers Sort konstruieren
 --------------------------------------------------------------------*/









/*--------------------------------------------------------------------
	Beschreibung: Operatoren zum Vergleichen
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: Kleiner-Operator fuers sortieren
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: SortierElemente fuer Text
 --------------------------------------------------------------------*/






/*--------------------------------------------------------------------
	Beschreibung: Key ermitteln
 --------------------------------------------------------------------*/




/*--------------------------------------------------------------------
	Beschreibung: Sortier-Elemente fuer Tabellen
 --------------------------------------------------------------------*/




/*--------------------------------------------------------------------
	Beschreibung: Schluessel zu einer Zelle ermitteln
 --------------------------------------------------------------------*/




/*--------------------------------------------------------------------
	Beschreibung: Text sortieren im Document
 --------------------------------------------------------------------*/


/*N*/ BOOL SwDoc::SortText(const SwPaM& rPaM, const SwSortOptions& rOpt)
/*N*/ {
/*N*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 // pruefen ob Rahmen im Text
/*N*/  return TRUE;
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Tabelle sortieren im Document
 --------------------------------------------------------------------*/

/*N*/ BOOL SwDoc::SortTbl(const SwSelBoxes& rBoxes, const SwSortOptions& rOpt)
/*N*/ {
/*N*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 // uebers SwDoc fuer Undo !!
/*N*/ 	return TRUE;
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Zeilenweise verschieben
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: Spaltenweise verschieben
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: Eine einzelne Zelle verschieben
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: Zweidimensionales Array aus FndBoxes generieren
 --------------------------------------------------------------------*/





/*--------------------------------------------------------------------
	Beschreibung:	Alle Lines einer Box muessen gleichviel Boxen haben
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung:	Box auf Symmetrie pruefen
					Alle Boxen einer Line muessen gleichviele Lines haben
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: max Anzahl der Spalten (Boxes)
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: max Anzahl der Zeilen (Lines)
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: lineares Array aus atomaren FndBoxes erzeugen
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung: Zugriff auf eine bestimmte Zelle
 --------------------------------------------------------------------*/





}
