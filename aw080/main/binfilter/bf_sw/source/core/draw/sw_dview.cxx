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

#include "hintids.hxx"




#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "frmfmt.hxx"
#include "dcontact.hxx"
#include "viewsh.hxx"
#include "viewimp.hxx"
#include "dview.hxx"

#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif

// #i7672#

// OD 18.06.2003 #108784#
#include <vector>
namespace binfilter {

class SwSdrHdl : public SdrHdl
{
public:
    SwSdrHdl(const Point& rPnt, bool bTopRight ) :
        SdrHdl( rPnt, bTopRight ? HDL_ANCHOR_TR : HDL_ANCHOR ) {}
};



/*************************************************************************
|*
|*	SwDrawView::Ctor
|*
|*	Ersterstellung		OK 18.11.94
|*	Letzte Aenderung	MA 22. Jul. 96
|*
*************************************************************************/



/*N*/ SwDrawView::SwDrawView( SwViewImp &rI, SdrModel *pMd, OutputDevice *pOutDev) :
/*N*/ 	FmFormView( (FmFormModel*)pMd, pOutDev ),
/*N*/ 	rImp( rI )
/*N*/ {
/*N*/ 	SetPageVisible( FALSE );
/*N*/ 	SetBordVisible( FALSE );
/*N*/ 	SetGridVisible( FALSE );
/*N*/ 	SetHlplVisible( FALSE );
/*N*/ 	SetGlueVisible( FALSE );
/*N*/ 	SetFrameDragSingles( TRUE );
/*N*/ 	SetVirtualObjectBundling( TRUE );
/*N*/ 	SetSwapAsynchron( TRUE );
/*N*/ 
/*N*/ 	EnableExtendedKeyInputDispatcher( FALSE );
/*N*/ 	EnableExtendedMouseEventDispatcher( FALSE );
/*N*/ 	EnableExtendedCommandEventDispatcher( FALSE );
/*N*/ 
/*N*/ 	SetHitTolerancePixel( GetMarkHdlSizePixel()/2 );
/*N*/ 
/*N*/ 	SetPrintPreview( rI.GetShell()->IsPreView() );
/*N*/ }

/*************************************************************************
|*
|*	SwDrawView::AddCustomHdl()
|*
|*  Gets called every time the handles need to be build
|*
|*	Ersterstellung		AW 06. Sep. 99
|*	Letzte Aenderung	AW 06. Sep. 99
|*
*************************************************************************/

/*M*/ void SwDrawView::AddCustomHdl()
/*M*/ {
/*M*/ 	const SdrMarkList &rMrkList = GetMarkList();
/*M*/ 
/*M*/ 	if(rMrkList.GetMarkCount() != 1 || !GetUserCall(rMrkList.GetMark( 0 )->GetObj()))
/*M*/ 		return;
/*M*/ 
/*?*/ 	SdrObject *pObj = rMrkList.GetMark(0)->GetObj();
/*?*/ 	const SwFmtAnchor &rAnchor = ::binfilter::FindFrmFmt(pObj)->GetAnchor();
/*?*/ 
/*?*/ 	if(FLY_IN_CNTNT == rAnchor.GetAnchorId())
/*?*/ 		return;
/*?*/ DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*M*/ }

/*************************************************************************
|*
|*	SwDrawView::GetMaxToTopObj(), _GetMaxToTopObj()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 18. Mar. 97
|*
*************************************************************************/



/*************************************************************************
|*
|*	SwDrawView::GetMaxToBtmObj()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 05. Sep. 96
|*
*************************************************************************/



/*************************************************************************
|*
|*	SwDrawView::ObjOrderChanged()
|*
|*	Ersterstellung		MA 31. Jul. 95
|*	Letzte Aenderung	MA 18. Mar. 97
|*
*************************************************************************/






/*************************************************************************
|*
|*	SwDrawView::TakeDragLimit()
|*
|*	Ersterstellung		AMA 26. Apr. 96
|*	Letzte Aenderung	MA 03. May. 96
|*
*************************************************************************/



/*************************************************************************
|*
|*	SwDrawView::CalcAnchor()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 08. Nov. 96
|*
*************************************************************************/





/*************************************************************************
|*
|*	SwDrawView::ShowDragXor(), HideDragXor()
|*
|*	Ersterstellung		MA 17. Jan. 95
|*	Letzte Aenderung	MA 27. Jan. 95
|*
*************************************************************************/





/*************************************************************************
|*
|*	SwDrawView::MarkListHasChanged()
|*
|*	Ersterstellung		OM 02. Feb. 95
|*	Letzte Aenderung	OM 07. Jul. 95
|*
*************************************************************************/


/*N*/ void SwDrawView::MarkListHasChanged()
/*N*/ {
/*N*/ 	Imp().GetShell()->DrawSelChanged(this);
/*N*/ 	FmFormView::MarkListHasChanged();
/*N*/ }

// #i7672#
/*N*/ void SwDrawView::ModelHasChanged()
/*N*/ {
	// The ModelHasChanged() call in DrawingLayer also updates
	// a eventually active text edit view (OutlinerView). This also leads
	// to newly setting the background color for that edit view. Thus,
	// this method rescues the current background color if a OutlinerView
	// exists and re-establishes it then. To be more safe, the OutlinerView
	// will be fetched again (maybe textedit has ended).
/*N*/ 	OutlinerView* pView = GetTextEditOutlinerView();
/*N*/ 	Color aBackColor;
/*N*/ 	sal_Bool bColorWasSaved(sal_False);
/*N*/ 
/*N*/ 	if(pView)
/*N*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ 
/*N*/ 	// call parent
/*N*/ 	FmFormView::ModelHasChanged();
/*N*/ 
/*N*/ 	if(bColorWasSaved)
/*N*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ }

}
