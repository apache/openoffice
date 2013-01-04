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




#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _FESH_HXX
#include <fesh.hxx>
#endif
#ifndef _VIEWIMP_HXX
#include <viewimp.hxx>      // Imp->SetFirstVisPageInvalid()
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>      //      -- " --
#endif
#ifndef _PTQUEUE_HXX
#include <ptqueue.hxx>
#endif

#include <bf_sfx2/printer.hxx>

namespace binfilter {

#define JOBSET_ERR_DEFAULT  		0
#define JOBSET_ERR_ERROR 			1
#define JOBSET_ERR_ISSTARTET 		2

//--------------------------------------------------------------------
//Klasse zum Puffern von Paints
class SwQueuedPaint
{
public:
	SwQueuedPaint *pNext;
	ViewShell	   *pSh;
	SwRect			aRect;

	SwQueuedPaint( ViewShell *pNew, const SwRect &rRect ) :
		pNext( 0 ),
		pSh( pNew ),
		aRect( rRect )
	{}
};

SwQueuedPaint *SwPaintQueue::pQueue = 0;

/*N*/ void SwPaintQueue::Remove( ViewShell *pSh )
/*N*/ {
/*N*/ 	SwQueuedPaint *pPt;
/*N*/ 	if ( 0 != (pPt = pQueue) )
/*N*/ 	{
/*?*/ 		SwQueuedPaint *pPrev = 0;
/*?*/ 		while ( pPt && pPt->pSh != pSh )
/*?*/ 		{
/*?*/ 			pPrev = pPt;
/*?*/ 			pPt = pPt->pNext;
/*?*/ 		}
/*?*/ 		if ( pPt )
/*?*/ 		{
/*?*/ 			if ( pPrev )
/*?*/ 				pPrev->pNext = pPt->pNext;
/*?*/ 			else if ( pPt == pQueue )
/*?*/ 				pQueue = 0;
/*?*/ 			delete pPt;
/*?*/ 		}
/*N*/ 	}
/*N*/ }

/*M*/ void ViewShell::InitPrt( SfxPrinter *pPrt, OutputDevice *pPDFOut )
/*M*/ {
/*M*/ 	//Fuer den Printer merken wir uns einen negativen Offset, der
/*M*/ 	//genau dem Offset de OutputSize entspricht. Das ist notwendig,
/*M*/ 	//weil unser Ursprung der linken ober Ecke der physikalischen
/*M*/ 	//Seite ist, die Ausgaben (SV) aber den Outputoffset als Urstprung
/*M*/ 	//betrachten.
/*M*/     OutputDevice *pTmpDev = pPDFOut ? pPDFOut : (OutputDevice *) pPrt;
/*M*/     if ( pTmpDev )
/*M*/ 	{
/*M*/         aPrtOffst = pPrt ? pPrt->GetPageOffset() : Point();
/*M*/ 
/*M*/         aPrtOffst += pTmpDev->GetMapMode().GetOrigin();
/*M*/         MapMode aMapMode( pTmpDev->GetMapMode() );
/*M*/ 		aMapMode.SetMapUnit( MAP_TWIP );
/*M*/         pTmpDev->SetMapMode( aMapMode );
/*M*/         pTmpDev->SetLineColor();
/*M*/         pTmpDev->SetFillColor();
/*M*/ 	}
/*M*/ 	else
/*M*/ 		aPrtOffst.X() = aPrtOffst.Y() = 0;
/*M*/ 
/*M*/ 	if ( !pWin )
/*M*/         pOut = pTmpDev;    //Oder was sonst?
/*M*/ }
}
