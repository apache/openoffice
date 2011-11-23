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



#ifndef _SVX_SVXIDS_HRC
#define ITEMID_FONTLIST 		SID_ATTR_CHAR_FONTLIST
#define ITEMID_NUMBERINFO		SID_ATTR_NUMBERFORMAT_INFO
#define ITEMID_COLOR_TABLE		SID_COLOR_TABLE
#define ITEMID_GRADIENT_LIST	SID_GRADIENT_LIST
#define ITEMID_HATCH_LIST		SID_HATCH_LIST
#define ITEMID_BITMAP_LIST		SID_BITMAP_LIST
#define ITEMID_DASH_LIST		SID_DASH_LIST
#define ITEMID_LINEEND_LIST 	SID_LINEEND_LIST
#include <bf_svx/svxids.hrc>
#endif

#include <bf_sfx2/app.hxx>

#include "app.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "docshell.hxx"

namespace binfilter {

/*************************************************************************
|*
|*  Zeichnen der DocShell (mittels der Hilfsklasse SchViewSpecial)
|*
\************************************************************************/




/*************************************************************************
|*
|*
|*
\************************************************************************/


/*************************************************************************
|*
|*	Bitmap der StyleFamily zurueckgeben (zunaechst Dummy)
|*
\************************************************************************/

/*N*/ SfxStyleSheetBasePool* SchChartDocShell::GetStyleSheetPool() throw()
/*N*/ {
/*N*/ 	return (SfxStyleSheetBasePool*)pChDoc->GetStyleSheetPool();
/*N*/ }

/*************************************************************************
|*
|*	Vorlage einfuegen (zunaechst nur Dummy)
|*
\************************************************************************/


/*************************************************************************
|*
|*	Vorlage entfernen (zunaechst nur Dummy)
|*
\************************************************************************/


/*************************************************************************
|*
|*	Chart updaten
|*
\************************************************************************/


/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ Rectangle SchChartDocShell::GetVisArea(USHORT nAspect) const throw()
/*N*/ {
/*N*/ 	return SfxInPlaceObject::GetVisArea((nAspect == ASPECT_THUMBNAIL) ?
/*N*/ 										 ASPECT_CONTENT : nAspect);
/*N*/ }

/*************************************************************************
|*
|* VisArea fuer InPlace setzen
|*
\************************************************************************/

/*N*/ void SchChartDocShell::SetVisArea(const Rectangle& rRect) throw()
/*N*/ {
/*N*/ 	Rectangle aRect(rRect);
/*N*/ 	aRect.SetPos(Point(0, 0));
/*N*/ 
/*N*/ 	if( GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
/*N*/ 	{
/*N*/ 		// sets the modified flag
/*N*/ 		SfxInPlaceObject::SetVisArea( rRect );
/*N*/ 
/*N*/         // #107434# propagate modified status to changed status at model
/*N*/         if( pChDoc && IsModified() )
/*N*/             pChDoc->SetChanged( TRUE );
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/         // does not set the modified flag
/*?*/ 		SvEmbeddedObject::SetVisArea( rRect );
/*N*/ 	}
/*N*/ 
/*N*/     if( GetCreateMode() != SFX_CREATE_MODE_INTERNAL )
/*N*/ 	{
/*N*/ 		Size aSize;
/*N*/ 
/*N*/ 		if (rRect.IsEmpty ())
/*N*/ 			aSize = Size ();
/*N*/ 		else
/*N*/ 			aSize = GetVisArea(ASPECT_CONTENT).GetSize();
/*N*/ 
/*N*/ 		SdrPage* pPage = pChDoc->GetPage(0);
/*N*/ 
/*N*/ 		if(aSize.Width() > 0 && aSize.Height() > 0)
/*N*/ 		{
/*N*/ 			if( !pPage || aSize != pPage->GetSize() )
/*N*/ 			{
/*N*/ 				pChDoc->ResizePage(aSize);
/*N*/ 
/*N*/                 // notify the controller
/*N*/                 // note: the controller is only available in in-place
/*N*/                 // Broadcast( SchVisAreaChangedHint( rRect ) );
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* OLE-Status
|*
\************************************************************************/

/*N*/ ULONG SchChartDocShell::GetMiscStatus() const throw()
/*N*/ {
/*N*/ 	// Chart soll stets selber resizen
/*N*/ 	ULONG nStatus = SfxInPlaceObject::GetMiscStatus() |
/*N*/ 					SVOBJ_MISCSTATUS_SERVERRESIZE     |
/*N*/ 					SVOBJ_MISCSTATUS_RESIZEONPRINTERCHANGE;
/*N*/ 
/*N*/ 	return nStatus;
/*N*/ }

/*N*/ void SchChartDocShell::SetModified( BOOL bModified ) throw()
/*N*/ {
/*N*/ 	if( IsEnableSetModified() )
/*N*/ 	{
/*N*/ 		SfxInPlaceObject::SetModified( bModified );
/*N*/ 		DBG_ASSERT( pChDoc, "Invalid ChartModel" );
/*N*/ 		if( ! pChDoc->IsLockedBuild())
/*N*/ 			Broadcast( SfxSimpleHint( SFX_HINT_DOCCHANGED ) );
/*N*/ 	}
/*N*/ }



}
