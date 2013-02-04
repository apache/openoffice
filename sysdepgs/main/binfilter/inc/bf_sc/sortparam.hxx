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



#ifndef SC_SORTPARAM_HXX
#define SC_SORTPARAM_HXX

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#include <bf_svtools/bf_solar.h>
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
namespace binfilter {


#define MAXSORT		3


struct ScSubTotalParam;
struct ScQueryParam;

struct ScSortParam
{
	USHORT		nCol1;
	USHORT		nRow1;
	USHORT		nCol2;
	USHORT		nRow2;
	BOOL		bHasHeader;
	BOOL		bByRow;
	BOOL		bCaseSens;
	BOOL		bUserDef;
	USHORT		nUserIndex;
	BOOL		bIncludePattern;
	BOOL		bInplace;
	USHORT		nDestTab;
	USHORT		nDestCol;
	USHORT		nDestRow;
	BOOL		bDoSort[MAXSORT];
	USHORT		nField[MAXSORT];
	BOOL		bAscending[MAXSORT];
	::com::sun::star::lang::Locale		aCollatorLocale;
	String		aCollatorAlgorithm;

	ScSortParam();
	ScSortParam( const ScSortParam& r );
	/// SubTotals sort
	/// TopTen sort

	void			Clear		();

};


} //namespace binfilter
#endif // SC_SORTPARAM_HXX
