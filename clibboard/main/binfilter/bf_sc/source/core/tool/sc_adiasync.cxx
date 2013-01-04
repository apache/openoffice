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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

//------------------------------------------------------------------------

#include <bf_sfx2/objsh.hxx>

#include "adiasync.hxx"
#include "document.hxx"
#include "bf_sc.hrc"		// FID_DATACHANGED

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
namespace binfilter {


//------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma code_seg("SCSTATICS")
#endif

ScAddInAsyncs theAddInAsyncTbl;
static ScAddInAsync aSeekObj;

#ifdef _MSC_VER
#pragma code_seg()
#endif


/*N*/ SV_IMPL_OP_PTRARR_SORT( ScAddInAsyncs, ScAddInAsyncPtr );

/*N*/ SV_IMPL_PTRARR_SORT( ScAddInDocs, ScAddInDocPtr );




/*N*/ ScAddInAsync::ScAddInAsync() :
/*N*/ 	SfxBroadcaster(),
/*N*/ 	nHandle( 0 )
/*N*/ {	// nur fuer aSeekObj !
/*N*/ }



/*N*/ ScAddInAsync::ScAddInAsync( ULONG nHandleP, USHORT nIndex, ScDocument* pDoc ) :
/*N*/ 	SfxBroadcaster(),
/*N*/ 	pStr( NULL ),
/*N*/ 	nHandle( nHandleP ),
/*N*/ 	bValid( FALSE )
/*N*/ {
/*N*/ 	pDocs = new ScAddInDocs( 1, 1 );
/*N*/ 	pDocs->Insert( pDoc );
/*N*/ 	pFuncData = (FuncData*)ScGlobal::GetFuncCollection()->At(nIndex);
/*N*/ 	eType = pFuncData->GetAsyncType();
/*N*/ 	theAddInAsyncTbl.Insert( this );
/*N*/ }



/*N*/ ScAddInAsync::~ScAddInAsync()
/*N*/ {
/*N*/ 	// aSeekObj hat das alles nicht, Handle 0 gibt es sonst nicht
/*N*/ 	if ( nHandle )
/*N*/ 	{
/*?*/ 		// im dTor wg. theAddInAsyncTbl.DeleteAndDestroy in ScGlobal::Clear
/*?*/ 		DBG_BF_ASSERT(0, "STRIP"); //STRIP001 pFuncData->Unadvice( (double)nHandle );
/*N*/ 	}
/*N*/ }



/*N*/ ScAddInAsync* ScAddInAsync::Get( ULONG nHandleP )
/*N*/ {
/*N*/ 	USHORT nPos;
/*N*/ 	ScAddInAsync* pRet = 0;
/*N*/ 	aSeekObj.nHandle = nHandleP;
/*N*/ 	if ( theAddInAsyncTbl.Seek_Entry( &aSeekObj, &nPos ) )
/*N*/ 		pRet = theAddInAsyncTbl[ nPos ];
/*N*/ 	aSeekObj.nHandle = 0;
/*N*/ 	return pRet;
/*N*/ }






/*N*/ void ScAddInAsync::RemoveDocument( ScDocument* pDocumentP )
/*N*/ {
/*N*/ 	USHORT nPos = theAddInAsyncTbl.Count();
/*N*/ 	if ( nPos )
/*N*/ 	{
/*?*/ 		const ScAddInAsync** ppAsync =
/*?*/ 			(const ScAddInAsync**) theAddInAsyncTbl.GetData() + nPos - 1;
/*?*/ 		for ( ; nPos-- >0; ppAsync-- )
/*?*/ 		{	// rueckwaerts wg. Pointer-Aufrueckerei im Array
/*?*/ 			ScAddInDocs* p = ((ScAddInAsync*)*ppAsync)->pDocs;
/*?*/ 			USHORT nFoundPos;
/*?*/ 			if ( p->Seek_Entry( pDocumentP, &nFoundPos ) )
/*?*/ 			{
/*?*/ 				p->Remove( nFoundPos );
/*?*/ 				if ( p->Count() == 0 )
/*?*/ 				{	// dieses AddIn wird nicht mehr benutzt
/*?*/ 					ScAddInAsync* pAsync = (ScAddInAsync*)*ppAsync;
/*?*/ 					theAddInAsyncTbl.Remove( nPos );
/*?*/ 					delete pAsync;
/*?*/ 					ppAsync = (const ScAddInAsync**) theAddInAsyncTbl.GetData()
/*?*/ 						+ nPos;
/*?*/ 				}
/*?*/ 			}
/*?*/ 		}
/*N*/ 	}
/*N*/ }



}
