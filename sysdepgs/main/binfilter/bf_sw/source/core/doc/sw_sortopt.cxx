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

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif

#ifndef _SORTOPT_HXX
#include <sortopt.hxx>
#endif
namespace binfilter {


/*N*/ SV_IMPL_PTRARR(SwSortKeys, SwSortKey*)

/*--------------------------------------------------------------------
	Beschreibung:	Sortier-Schluessel
 --------------------------------------------------------------------*/

/*N*/ SwSortKey::SwSortKey() :
/*N*/ 	eSortOrder( SRT_ASCENDING ),
/*N*/ 	nColumnId( 0 ),
/*N*/ 	bIsNumeric( TRUE )
/*N*/ {
/*N*/ }

/*N*/ SwSortKey::SwSortKey(const SwSortKey& rOld) :
/*N*/ 	eSortOrder( rOld.eSortOrder ),
/*N*/ 	sSortType( rOld.sSortType ),
/*N*/ 	nColumnId( rOld.nColumnId ),
/*N*/ 	bIsNumeric( rOld.bIsNumeric )
/*N*/ {
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Sortieroptionen fuers Sortieren
 --------------------------------------------------------------------*/


/*N*/ SwSortOptions::SwSortOptions()
/*?*/ 	: //STRIP001 eDirection( SRT_ROWS ),
/*N*/ 	nLanguage( LANGUAGE_SYSTEM ),
/*N*/ 	cDeli( 9 ),
/*N*/ 	bTable( FALSE ),
/*N*/ 	bIgnoreCase( FALSE )
/*N*/ {
/*N*/ }


/*N*/ SwSortOptions::SwSortOptions(const SwSortOptions& rOpt) :
/*N*/ 	eDirection( rOpt.eDirection ),
/*N*/ 	cDeli( rOpt.cDeli ),
/*N*/ 	nLanguage( rOpt.nLanguage ),
/*N*/ 	bTable( rOpt.bTable ),
/*N*/ 	bIgnoreCase( rOpt.bIgnoreCase )
/*N*/ {
/*N*/ 	for( USHORT i=0; i < rOpt.aKeys.Count(); ++i )
/*N*/ 	{
/*N*/ 		SwSortKey* pNew = new SwSortKey(*rOpt.aKeys[i]);
/*N*/ 		aKeys.C40_INSERT( SwSortKey, pNew, aKeys.Count());
/*N*/ 	}
/*N*/ }


/*N*/ SwSortOptions::~SwSortOptions()
/*N*/ {
/*N*/ 	aKeys.DeleteAndDestroy(0, aKeys.Count());
/*N*/ }



}
