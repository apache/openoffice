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



#ifndef _SC_FUPOOR_HXX
#define _SC_FUPOOR_HXX

#ifndef _EVENT_HXX //autogen
#include <vcl/event.hxx>
#endif
#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <bf_sfx2/request.hxx>
#endif
class Window;
class Dialog;
namespace binfilter {

class SdrView;
class ScTabViewShell;
class SdrModel;

// #98185# Create default drawing objects via keyboard
class SdrObject;

//	Return-Werte fuer Command
#define SC_CMD_NONE		0
#define SC_CMD_USED		1
#define SC_CMD_IGNORE	2

/*************************************************************************
|*
|* Basisklasse fuer alle Funktionen
|*
\************************************************************************/

class FuPoor
{
 protected:
	SdrView* 		pView;
	ScTabViewShell*	pViewShell;
	Window*			pWindow;
	SdrModel* 		pDrDoc;

	SfxRequest		aSfxRequest;
	Dialog* 		pDialog;

	Timer			aScrollTimer;			// fuer Autoscrolling
	DECL_LINK( ScrollHdl, Timer * );

	Timer			aDragTimer; 			// fuer Drag&Drop
	DECL_LINK( DragTimerHdl, Timer * );
	BOOL			bIsInDragMode;
	Point			aMDPos; 				// Position von MouseButtonDown

	// #95491# member to hold state of the mouse buttons for creation
	// of own MouseEvents (like in ScrollHdl)
private:
	sal_uInt16		mnCode;

 public:
	FuPoor(ScTabViewShell* pViewSh, Window* pWin, SdrView* pView,
		   SdrModel* pDoc, SfxRequest& rReq);
	virtual ~FuPoor();

	USHORT GetSlotID() const { DBG_BF_ASSERT(0, "STRIP"); return 0;} //STRIP001 USHORT GetSlotID() const { return( aSfxRequest.GetSlot() );}
	void	StopDragTimer();
};



} //namespace binfilter
#endif		// _SD_FUPOOR_HXX

