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


#define _OUTLINER_CXX
#include <outleeng.hxx>










#ifndef _MyEDITVIEW_HXX
#include "editview.hxx"
#endif

namespace binfilter {


// Breite der Randzonen innerhalb derer beim D&D gescrollt wird
#define OL_SCROLL_LRBORDERWIDTHPIX	10
#define OL_SCROLL_TBBORDERWIDTHPIX	10

// Wert, um den Fensterinhalt beim D&D gescrollt wird
#define OL_SCROLL_HOROFFSET			20  /* in % von VisibleSize.Width */
#define OL_SCROLL_VEROFFSET			20  /* in % von VisibleSize.Height */

/*N*/ DBG_NAME(OutlinerView)

/*N*/ OutlinerView::~OutlinerView()
/*N*/ {
/*N*/ 	DBG_DTOR(OutlinerView,0);
/*N*/ 	delete pEditView;
/*N*/ }


/*N*/ Rectangle OutlinerView::GetVisArea() const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerView,0);
/*N*/ 	return pEditView->GetVisArea();
/*N*/ }



// MT 05/00: Wofuer dies ImpXXXScroll, sollte das nicht die EditEngine machen???


























/*N*/ void OutlinerView::Cut()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*N*/ void OutlinerView::Paste()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }




// =====================================================================
// ======================   Einfache Durchreicher =======================
// ======================================================================





/*N*/ void OutlinerView::SetSelection( const ESelection& rSel )
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerView,0);
/*N*/ 	pEditView->SetSelection( rSel );
/*N*/ }


/*N*/ Window* OutlinerView::GetWindow() const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerView,0);
/*N*/ 	return pEditView->GetWindow();
/*N*/ }




/*N*/ Rectangle OutlinerView::GetOutputArea() const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerView,0);
/*N*/ 	return pEditView->GetOutputArea();
/*N*/ }

/*N*/ ESelection OutlinerView::GetSelection()
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerView,0);
/*N*/ 	return pEditView->GetSelection();
/*N*/ }


















/*N*/ void OutlinerView::Copy()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }





















#ifndef SVX_LIGHT
#endif





}
