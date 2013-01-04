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



#ifndef _SCH_SCHVIEW_HXX
#define _SCH_SCHVIEW_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_svx/svdedxv.hxx>
#include <bf_svx/view3d.hxx>
#include <bf_svx/svdpage.hxx>

#include "chtmodel.hxx"
#include "docshell.hxx"

class Graphic;
namespace binfilter {

class SchViewShell;
class SchPage;
class SchWindow;
class ChartModel;
class SchDragServerRef;
class E3dScene;
} //namespace binfilter

namespace binfilter {//STRIP009
DECLARE_LIST(E3dLogicalObjList, E3dObject*)//STRIP008 DECLARE_LIST(E3dLogicalObjList, E3dObject*);

/*************************************************************************
|*
|* View
|*
\************************************************************************/

class SchView : public E3dView
{
 protected:
	ChartModel*   pDoc;
	SchChartDocShell*   pDocSh;
	SchViewShell*       pViewSh;
	BOOL                bDragActive;
	USHORT              nLogicalMarked;
	USHORT              nLogicalEntered;

	Timer aTimer;
	void             Construct();
//  	SchDragServerRef CreateDataObject(SchView* pView, const Point& rDragPos);
//  	SchDragServerRef CreateDataObject(ChartModel* pDocument);
	void                SetMarkHandles();

 public:

	//void NotifySelection();
	DECL_LINK(NotifySelection,void*);

	SchView(SchChartDocShell* pDocSh, OutputDevice* pOutDev, SchViewShell* pShell);
	virtual ~SchView();

	ChartModel& GetDoc() const { return *pDoc; }


	// clipboard / drag and drop methods using XTransferable

	void UpdateSelectionClipboard( BOOL bForceDeselect );


	BOOL IsActive() const { return bDragActive; }



	virtual void MarkListHasChanged();


	FASTBOOL	IsLogicalGroupMarked() { return (nLogicalMarked != 0); }
	FASTBOOL	IsLogicalGroupEntered() { return (nLogicalEntered != 0); }



	SchChartDocShell* GetDocShell() const { return pDocSh; }

	// drag and drop


    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};

} //namespace binfilter
#endif		// _SCH_SCHVIEW_HXX
