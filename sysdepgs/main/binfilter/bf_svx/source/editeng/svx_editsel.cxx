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




#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#include <impedit.hxx>
#include <editview.hxx>
namespace binfilter {

//	----------------------------------------------------------------------
//	class EditSelFunctionSet
//	----------------------------------------------------------------------
/*N*/ EditSelFunctionSet::EditSelFunctionSet()
/*N*/ {
/*N*/ 	pCurView = NULL;
/*N*/ }

/*N*/ void __EXPORT EditSelFunctionSet::CreateAnchor()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*N*/ void __EXPORT EditSelFunctionSet::DestroyAnchor()
/*N*/ {
/*N*/ 	// Nur bei Mehrfachselektion
/*N*/ }

/*N*/ BOOL __EXPORT EditSelFunctionSet::SetCursorAtPoint( const Point& rPointPixel, BOOL )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	return FALSE;
/*N*/ }

/*N*/ BOOL __EXPORT EditSelFunctionSet::IsSelectionAtPoint( const Point& rPointPixel )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/     return FALSE;
/*N*/ }

/*N*/ void __EXPORT EditSelFunctionSet::DeselectAtPoint( const Point& )
/*N*/ {
/*N*/ // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*N*/ // !  Implementieren, wenn Mehrfachselektion moeglich  !
/*N*/ // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*N*/ }

/*N*/ void __EXPORT EditSelFunctionSet::BeginDrag()
/*N*/ {
/*N*/ 	// Nur bei Mehrfachselektion
/*N*/ }


/*N*/ void __EXPORT EditSelFunctionSet::DeselectAll()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

//	----------------------------------------------------------------------
//	class EditSelectionEngine
//	----------------------------------------------------------------------
/*N*/ EditSelectionEngine::EditSelectionEngine() : SelectionEngine( (Window*)0 )
/*N*/ {
/*N*/ 	// Wegen Bug OV: (1994)
/*N*/ 	// 1995: RangeSelection lassen, SingleSelection nur fuer ListBoxen geeignet!
/*N*/ 	SetSelectionMode( RANGE_SELECTION );
/*N*/ 	EnableDrag( TRUE );
/*N*/ }

/*N*/ void EditSelectionEngine::SetCurView( EditView* pNewView )
/*N*/ {
/*N*/ 	if ( GetFunctionSet() )
/*N*/ 		((EditSelFunctionSet*)GetFunctionSet())->SetCurView( pNewView );
/*N*/ 
/*N*/ 	if ( pNewView )
/*?*/ 		SetWindow( pNewView->GetWindow() );
/*N*/ 	else
/*N*/ 		SetWindow( (Window*)0 );
/*N*/ }


}
