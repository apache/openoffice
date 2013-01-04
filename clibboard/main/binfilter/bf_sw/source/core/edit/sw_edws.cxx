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


#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
namespace binfilter {

/********************************************************
 * Ctor/Dtor
 ********************************************************/

// ctor/dtor


/*N*/ SwEditShell::~SwEditShell() // USED
/*N*/ {
/*N*/ }

/******************************************************************************
 *				   void SwEditShell::StartAllAction()
 ******************************************************************************/


/*N*/ void SwEditShell::StartAllAction()
/*N*/ {
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do {
/*N*/ 		if( pSh->IsA( TYPE( SwEditShell ) ) )
/*N*/ 			((SwEditShell*)pSh)->StartAction();
/*N*/ 		else
/*?*/ 			pSh->StartAction();
/*N*/ 		pSh = (ViewShell *)pSh->GetNext();
/*N*/ 	} while(pSh != this);
/*N*/ }
/******************************************************************************
 *					void SwEditShell::EndAllAction()
 ******************************************************************************/


/*N*/ void SwEditShell::EndAllAction()
/*N*/ {
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do {
/*N*/ 		if( pSh->IsA( TYPE( SwEditShell ) ) )
/*N*/ 			((SwEditShell*)pSh)->EndAction();
/*N*/ 		else
/*?*/ 			pSh->EndAction();
/*N*/ 		pSh = (ViewShell *)pSh->GetNext();
/*N*/ 	} while(pSh != this);
/*N*/ }

/******************************************************************************
 *					void SwEditShell::CalcLayout()
 ******************************************************************************/


/*N*/ void SwEditShell::CalcLayout()
/*N*/ {
/*N*/ 	StartAllAction();
/*N*/ 	ViewShell::CalcLayout();
/*N*/ 
/*N*/ 	ViewShell *pSh = this;
/*N*/ 	do
/*N*/ 	{
/*N*/ 		if ( pSh->GetWin() )
/*N*/ 			pSh->GetWin()->Invalidate();
/*N*/ 		pSh = (ViewShell*)pSh->GetNext();
/*N*/ 
/*N*/ 	} while ( pSh != this );
/*N*/ 
/*N*/ 	EndAllAction();
/*N*/ }

}
