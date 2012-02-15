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

#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER
#include "hintids.hxx"

#ifdef WIN
#define NEEDED_BY_FESHVIEW
#define _FESHVIEW_ONLY_INLINE_NEEDED
#endif

#ifndef _SVDOPATH_HXX
#include <bf_svx/svdopath.hxx>
#endif
#ifndef _SVDVMARK_HXX //autogen
#include <bf_svx/svdvmark.hxx>
#endif
#ifndef _SVX_PROTITEM_HXX //autogen
#include <bf_svx/protitem.hxx>
#endif
#ifndef _IPOBJ_HXX //autogen
#include <bf_so3/ipobj.hxx>
#endif

#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>      // fuer InitFldTypes
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "fesh.hxx"
#include "doc.hxx"
#include "dview.hxx"
#include "dflyobj.hxx"
#include "dcontact.hxx"
#include "viewimp.hxx"
#include "flyfrm.hxx"
#include "ndole.hxx"
#include "txtfrm.hxx"
namespace binfilter {

#define SCROLLVAL 75

//Tattergrenze fuer Drawing-SS
#define MINMOVE ((USHORT)GetOut()->PixelToLogic(Size(Imp()->GetDrawView()->GetMarkHdlSizePixel()/2,0)).Width())

/*N*/ SwFlyFrm *GetFlyFromMarked( const SdrMarkList *pLst, ViewShell *pSh )
/*N*/ {
/*N*/ 	if ( !pLst )
/*?*/ 		pLst = pSh->HasDrawView() ? &pSh->Imp()->GetDrawView()->GetMarkList():0;
/*N*/ 
/*N*/ 	if ( pLst && pLst->GetMarkCount() == 1 )
/*N*/ 	{
/*?*/ 		SdrObject *pO = pLst->GetMark( 0 )->GetObj();
/*?*/ 		if ( pO->IsWriterFlyFrame() )
/*?*/ 			return ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
/*N*/ 	}
/*N*/ 	return 0;
/*N*/ }

/*N*/ BOOL SwFEShell::IsFrmSelected() const
/*N*/ {
/*N*/ 	if ( !Imp()->HasDrawView() )
/*N*/ 		return FALSE;
/*N*/ 	else
/*N*/ 		return 0 != ::binfilter::GetFlyFromMarked( &Imp()->GetDrawView()->GetMarkList(),
/*N*/ 										(ViewShell*)this );
/*N*/ }



// zeige das akt. selektierte "Object" an
/*N*/ void SwFEShell::MakeSelVisible()
/*N*/ {
/*N*/ 	if( Imp()->HasDrawView() &&
/*N*/ 		Imp()->GetDrawView()->GetMarkList().GetMarkCount() )
/*N*/ 	{
/*?*/ 		DBG_BF_ASSERT(0, "STRIP"); //STRIP001 MakeVisible( Imp()->GetDrawView()->GetAllMarkedRect() );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		SwCrsrShell::MakeSelVisible();
/*N*/ }


/*N*/ void SwFEShell::HideChainMarker()
/*N*/ {
/*N*/ 	if ( pChainFrom )
/*?*/ 		{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 pChainFrom->Hide();
/*N*/ 	if ( pChainTo )
/*?*/ 		{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 pChainTo->Hide();
/*N*/ }

/*N*/ void SwFEShell::SetChainMarker()
/*N*/ {
/*N*/ 	FASTBOOL bDelFrom = TRUE,
/*N*/ 			 bDelTo	  = TRUE;
/*N*/ 	if ( IsFrmSelected() )
/*N*/ 	{
/*?*/ 		SwFlyFrm *pFly = FindFlyFrm();
/*?*/ 
/*?*/ 		XPolygon aPoly(3);
/*?*/ 		if ( pFly->GetPrevLink() )
/*?*/ 		{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 		}
/*?*/ 		if ( pFly->GetNextLink() )
/*?*/ 		{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 		}
/*N*/ 	}
/*N*/ 	if ( bDelFrom )
/*N*/ 		delete pChainFrom, pChainFrom = 0;
/*N*/ 	if ( bDelTo )
/*N*/ 		delete pChainTo,   pChainTo = 0;
/*N*/ }

}
