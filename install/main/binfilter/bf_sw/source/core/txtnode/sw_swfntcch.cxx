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

#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#include "swfntcch.hxx"
#include "fmtcol.hxx"
namespace binfilter {

// aus atrstck.cxx
extern const BYTE StackPos[];

// globale Variablen, werden in SwFntCch.Hxx bekanntgegeben
// Der FontCache wird in TxtInit.Cxx _TXTINIT erzeugt und in _TXTEXIT geloescht
SwFontCache *pSwFontCache = NULL;

/*************************************************************************
|*
|*	SwFontObj::SwFontObj(), ~SwFontObj()
|*
|*	Ersterstellung		AMA 25. Jun. 95
|*	Letzte Aenderung	AMA 25. Jun. 95
|*
|*************************************************************************/

/*N*/ SwFontObj::SwFontObj( const void *pOwner, ViewShell *pSh ) :
/*N*/ 	SwCacheObj( (void*)pOwner ),
/*N*/ 	aSwFont( &((SwTxtFmtColl *)pOwner)->GetAttrSet(), pSh ? pSh->GetDoc() : 0 )
/*N*/ {
/*N*/ 	aSwFont.GoMagic( pSh, aSwFont.GetActual() );
/*N*/     const SwAttrSet& rAttrSet = ((SwTxtFmtColl *)pOwner)->GetAttrSet();
/*N*/     for (USHORT i = RES_CHRATR_BEGIN; i < RES_CHRATR_END; i++)
/*N*/         pDefaultArray[ StackPos[ i ] ] = &rAttrSet.Get( i, TRUE );
/*N*/ }

/*N*/ SwFontObj::~SwFontObj()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|*	SwFontAccess::SwFontAccess()
|*
|*	Ersterstellung		AMA 25. Jun. 95
|*	Letzte Aenderung	AMA 25. Jun. 95
|*
|*************************************************************************/

/*N*/ SwFontAccess::SwFontAccess( const void *pOwner, ViewShell *pSh ) :
/*N*/ 	SwCacheAccess( *pSwFontCache, pOwner,
/*N*/ 			(BOOL) ((SwTxtFmtColl*)pOwner)->IsInSwFntCache() ),
/*N*/ 	pShell( pSh )
/*N*/ {
/*N*/ }

/*N*/ SwFontObj *SwFontAccess::Get( )
/*N*/ {
/*N*/ 	return (SwFontObj *) SwCacheAccess::Get( );
/*N*/ }

/*N*/ SwCacheObj *SwFontAccess::NewObj( )
/*N*/ {
/*N*/ 	((SwTxtFmtColl*)pOwner)->SetInSwFntCache( TRUE );
/*N*/ 	return new SwFontObj( pOwner, pShell );
/*N*/ }


}
