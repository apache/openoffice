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

#include "scitems.hxx"
#include <bf_svx/dialogs.hrc>

#include "bf_sc.hrc"
#include "docpool.hxx"

#include "msgpool.hxx"
namespace binfilter {

//------------------------------------------------------------------------

/*N*/ static SfxItemInfo __READONLY_DATA aMsgItemInfos[] =
/*N*/ {
/*N*/ 	{ 0,						 SFX_ITEM_POOLABLE },	// SCITEM_STRING
/*N*/ 	{ 0,						 SFX_ITEM_POOLABLE },	// SCITEM_SEARCHDATA - nicht mehr benutzt !!!
/*N*/ 	{ SID_SORT,					 SFX_ITEM_POOLABLE },	// SCITEM_SORTDATA
/*N*/ 	{ SID_QUERY,				 SFX_ITEM_POOLABLE },	// SCITEM_QUERYDATA
/*N*/ 	{ SID_SUBTOTALS,			 SFX_ITEM_POOLABLE },	// SCITEM_SUBTDATA
/*N*/ 	{ SID_CONSOLIDATE,			 SFX_ITEM_POOLABLE },	// SCITEM_CONSOLIDATEDATA
/*N*/ 	{ SID_PIVOT_TABLE,			 SFX_ITEM_POOLABLE },	// SCITEM_PIVOTDATA
/*N*/ 	{ SID_SOLVE,				 SFX_ITEM_POOLABLE },	// SCITEM_SOLVEDATA
/*N*/ 	{ SID_SCUSERLISTS,			 SFX_ITEM_POOLABLE },	// SCITEM_USERLIST
/*N*/ 	{ SID_PRINTER_NOTFOUND_WARN, SFX_ITEM_POOLABLE }	// SCITEM_PRINTWARN
/*N*/ };

//------------------------------------------------------------------------

/*N*/ ScMessagePool::ScMessagePool()
/*N*/ 	:	SfxItemPool         ( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("ScMessagePool")),
/*N*/ 							  MSGPOOL_START, MSGPOOL_END,
/*N*/ 							  aMsgItemInfos, NULL ),
/*N*/ 	//
/*N*/ 	aGlobalStringItem   	( SfxStringItem			( SCITEM_STRING, String() ) ),
/*N*/ 	aGlobalSearchItem   	( SvxSearchItem			( SCITEM_SEARCHDATA ) ),
/*N*/ 	aGlobalSortItem     	( ScSortItem			( SCITEM_SORTDATA, NULL ) ),
/*N*/ 	aGlobalQueryItem		( ScQueryItem			( SCITEM_QUERYDATA, NULL, NULL ) ),
/*N*/ 	aGlobalSubTotalItem		( ScSubTotalItem		( SCITEM_SUBTDATA, NULL, NULL ) ),
/*N*/ 	aGlobalConsolidateItem	( ScConsolidateItem		( SCITEM_CONSOLIDATEDATA, NULL ) ),
/*N*/ 	aGlobalPivotItem		( ScPivotItem			( SCITEM_PIVOTDATA, NULL, NULL, FALSE ) ),
/*N*/ 	aGlobalSolveItem		( ScSolveItem			( SCITEM_SOLVEDATA, NULL ) ),
/*N*/ 	aGlobalUserListItem		( ScUserListItem		( SCITEM_USERLIST ) ),
/*N*/ 	//
/*N*/ 	aPrintWarnItem      	( SfxBoolItem			( SCITEM_PRINTWARN, FALSE ) )
/*N*/ {
/*N*/ 	ppPoolDefaults = new SfxPoolItem*[MSGPOOL_END - MSGPOOL_START + 1];
/*N*/ 
/*N*/ 	ppPoolDefaults[SCITEM_STRING			- MSGPOOL_START] = &aGlobalStringItem;
/*N*/ 	ppPoolDefaults[SCITEM_SEARCHDATA		- MSGPOOL_START] = &aGlobalSearchItem;
/*N*/ 	ppPoolDefaults[SCITEM_SORTDATA			- MSGPOOL_START] = &aGlobalSortItem;
/*N*/ 	ppPoolDefaults[SCITEM_QUERYDATA			- MSGPOOL_START] = &aGlobalQueryItem;
/*N*/ 	ppPoolDefaults[SCITEM_SUBTDATA			- MSGPOOL_START] = &aGlobalSubTotalItem;
/*N*/ 	ppPoolDefaults[SCITEM_CONSOLIDATEDATA	- MSGPOOL_START] = &aGlobalConsolidateItem;
/*N*/ 	ppPoolDefaults[SCITEM_PIVOTDATA			- MSGPOOL_START] = &aGlobalPivotItem;
/*N*/ 	ppPoolDefaults[SCITEM_SOLVEDATA			- MSGPOOL_START] = &aGlobalSolveItem;
/*N*/ 	ppPoolDefaults[SCITEM_USERLIST			- MSGPOOL_START] = &aGlobalUserListItem;
/*N*/ 	ppPoolDefaults[SCITEM_PRINTWARN			- MSGPOOL_START] = &aPrintWarnItem;
/*N*/ 
/*N*/ 	SetDefaults( ppPoolDefaults );
/*N*/ 
/*N*/ 	pDocPool = new ScDocumentPool;
/*N*/ 
/*N*/ 	SetSecondaryPool( pDocPool );
/*N*/ }


/*N*/ __EXPORT ScMessagePool::~ScMessagePool()
/*N*/ {
/*N*/ 	Delete();
/*N*/ 	SetSecondaryPool( NULL );		// before deleting defaults (accesses defaults)
/*N*/ 
/*N*/ 	for ( USHORT i=0; i <= MSGPOOL_END-MSGPOOL_START; i++ )
/*N*/ 		SetRefCount( *ppPoolDefaults[i], 0 );
/*N*/ 
/*N*/ 	delete[] ppPoolDefaults;
/*N*/ 
/*N*/ 	delete pDocPool;
/*N*/ }


/*N*/ SfxMapUnit __EXPORT ScMessagePool::GetMetric( USHORT nWhich ) const
/*N*/ {
/*N*/ 	//	eigene Attribute: Twips, alles andere 1/100 mm
/*N*/ 
/*N*/ 	if ( nWhich >= ATTR_STARTINDEX && nWhich <= ATTR_ENDINDEX )
/*N*/ 		return SFX_MAPUNIT_TWIP;
/*N*/ 	else
/*N*/ 		return SFX_MAPUNIT_100TH_MM;
/*N*/ }





}
