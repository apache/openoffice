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



#ifndef _SCH_FUPOOR_HXX
#define _SCH_FUPOOR_HXX
//------------------------------------------------------------------
//
// dieses Define dient nur zum Testen und darf auf keinen Fall aktiv bleiben
// Bei Umstellungen alle Files nach SIG_CHARTMODELDEFSCHDOC durchsuchen
// #define ChartModel SchChartDocument
//------------------------------------------------------------------

#ifndef _SV_DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#include "objid.hxx"
namespace binfilter {

class SchView;
class SchViewShell;
class SchWindow;
class ChartModel;
class SfxRequest;

#define HITPIX	  2		// Hit-Toleranz in Pixel
#define DRGPIX	  2		// Drag MinMove in Pixel


/*************************************************************************
|*
|* Basisklasse fuer alle Funktionen
|*
\************************************************************************/

class SchFuPoor
{
protected:
	SchView*		pView;
	SchViewShell*	pViewShell;
	SchWindow*		pWindow;
	ChartModel* pChDoc;

	USHORT			nSlotId;
	USHORT			nSlotValue;

	Dialog* 		pDialog;

	Timer			aDragTimer; 			// fuer Drag&Drop
	DECL_LINK(DragHdl, Timer*);
	BOOL			bIsInDragMode;
	Point			aMDPos; 				// Position von MouseButtonDown

	SdrObject   *pMarkedObj;
	SchObjectId *pMarkedObjId;
	UINT16      nMarkedID;

	BOOL			bFirstMouseMove;


public:

	SchFuPoor(SchViewShell* pViewSh, SchWindow* pWin, SchView* pView,
			  ChartModel* pDoc, SfxRequest& rReq);
	virtual ~SchFuPoor();


	// Mouse- & Key-Events; Returnwert=TRUE: Event wurde bearbeitet

	virtual void Activate();		// Function aktivieren
	virtual void Deactivate();		// Function deaktivieren



	USHORT GetSlotID() const { return( nSlotId ); }

};

} //namespace binfilter
#endif	// _SCH_FUPOOR_HXX


