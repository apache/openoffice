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


#ifndef _SORTOPT_HXX
#define _SORTOPT_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
namespace binfilter {

enum SwSortOrder  		{ SRT_ASCENDING, SRT_DESCENDING	};
enum SwSortDirection	{ SRT_COLUMNS, SRT_ROWS			};

/*--------------------------------------------------------------------
	Beschreibung: SortierSchluessel
 --------------------------------------------------------------------*/
struct SwSortKey
{
	SwSortKey();
	SwSortKey( const SwSortKey& rOld );

	String			sSortType;
	SwSortOrder		eSortOrder;
	USHORT 			nColumnId;
	BOOL			bIsNumeric;
};

SV_DECL_PTRARR(SwSortKeys, SwSortKey*, 3, 1)

 struct SwSortOptions
 {
 	SwSortOptions();
 	~SwSortOptions();
 	SwSortOptions(const SwSortOptions& rOpt);
 
  	SwSortKeys		aKeys;
 	SwSortDirection	eDirection;
 	sal_Unicode		cDeli;
 	USHORT			nLanguage;
 	BOOL			bTable;
 	BOOL 			bIgnoreCase;
 };

} //namespace binfilter
#endif	// _SORTOPT_HXX
