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


#ifndef _SVX_SVXIDS_HRC //autogen
#include <bf_svx/svxids.hrc>
#endif

#define ITEMID_COLOR_TABLE		SID_COLOR_TABLE
#define ITEMID_GRADIENT_LIST	SID_GRADIENT_LIST
#define ITEMID_HATCH_LIST		SID_HATCH_LIST
#define ITEMID_BITMAP_LIST		SID_BITMAP_LIST
#define ITEMID_DASH_LIST		SID_DASH_LIST
#define ITEMID_LINEEND_LIST 	SID_LINEEND_LIST


#ifndef _OFF_APP_HXX //autogen
#include <bf_offmgr/app.hxx>
#endif
#ifndef _SVX_DRAWITEM_HXX //autogen
#include <bf_svx/drawitem.hxx>
#endif
#ifndef _SVDMODEL_HXX //autogen
#include <bf_svx/svdmodel.hxx>
#endif
#ifndef _SVDOUTL_HXX
#include <bf_svx/svdoutl.hxx>
#endif

#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
namespace binfilter {

/*--------------------------------------------------------------------
	Beschreibung: Document laden
 --------------------------------------------------------------------*/


/*N*/ void  SwDocShell::InitDraw()
/*N*/ {
/*N*/ 	SdrModel *pDrDoc = pDoc->GetDrawModel();
/*N*/ 	if( pDrDoc )
/*N*/ 	{
/*N*/ 		// Listen, bzw. Tables im ItemSet der DocShell anlegen
/*N*/ 		PutItem( SvxGradientListItem( pDrDoc->GetGradientList() ) );
/*N*/ 		PutItem( SvxHatchListItem( pDrDoc->GetHatchList() ) );
/*N*/ 		PutItem( SvxBitmapListItem( pDrDoc->GetBitmapList() ) );
/*N*/ 		PutItem( SvxDashListItem( pDrDoc->GetDashList() ) );
/*N*/ 		PutItem( SvxLineEndListItem( pDrDoc->GetLineEndList() ) );
/*N*/ 
/*N*/ 		Outliner& rOutliner = pDrDoc->GetDrawOutliner();
/*N*/ 	}
/*N*/ 	else
/*N*/ 		PutItem( SvxColorTableItem( OFF_APP()->GetStdColorTable() ));
/*N*/ }

}
