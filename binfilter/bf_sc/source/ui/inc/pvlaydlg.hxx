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



#ifndef SC_PVLAYDLG_HXX
#define SC_PVLAYDLG_HXX

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SCRBAR_HXX //autogen
#include <vcl/scrbar.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <bf_svtools/stdctrl.hxx>
#endif
#ifndef _MOREBTN_HXX //autogen
#include <vcl/morebtn.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_ANYREFDG_HXX
#include "anyrefdg.hxx"
#endif

#ifndef SC_FIELDWND_HXX
#include "fieldwnd.hxx"
#endif
namespace binfilter {


/*==========================================================================*\

	Eine Instanz der Klasse ScPivotLayoutDlg ist ein (semi-)modaler
	Dialog, in dem mit der Maus Felder mit Spaltenueberschriften den
	drei Pivot-Kategorien "Spalte", "Zeile" und "Daten" zugeordnet
	werden koennen.

	Der Dialog erhaelt in der Struktur LabelData Informationen ueber
	diese Ueberschriften (Name, Art (Zahl/String) und Funktionsmaske).
	Weiterhin werden drei PivotFeld-Arrays uebergeben, mit denen die
	drei Kategorie-Fenster initialisiert werden. Ein Kategorie-Fenster
	wird durch eine Instanz der Klasse FieldWindow dargestellt. Ein
	solches Fenster ist fuer die Darstellung der Datenstrukturen am
	Schirm zustaendig. Es meldet Mausaktionen an den Dialog weiter und
	bietet entsprechende Methoden zur Veraenderung der Darstellung.
	Der Dialog sorgt fuer den Abgleich der interenen Datenstrukturen mit
	der Bildschirmdarstellung. Ein weiteres FieldWindow (Select) bietet
	alle Tabellenueberschriften zur Auswahl an, ist also "read-only".

\*==========================================================================*/

//============================================================================

class ScViewData;
class ScDocument;
class ScRangeData;
struct FuncData;
class ScDPObject;

//============================================================================

#define FUNC_COUNT 11




} //namespace binfilter
#endif // SC_PVLAYDLG_HXX

