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



#ifndef SC_PFUNCACHE_HXX
#define SC_PFUNCACHE_HXX

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"
#endif
namespace binfilter {

class ScDocShell;
class ScMarkData;


/** Possible types of selection for print functions */

enum ScPrintSelectionMode
{
	SC_PRINTSEL_INVALID,
	SC_PRINTSEL_DOCUMENT,
	SC_PRINTSEL_CURSOR,
	SC_PRINTSEL_RANGE
};


/** Stores the selection in the ScPrintFuncCache so it is only used
	for the same selection again. */

class ScPrintSelectionStatus
{
	ScPrintSelectionMode	eMode;
	ScRangeList				aRanges;

public:
			ScPrintSelectionStatus() : eMode(SC_PRINTSEL_INVALID) {}
			~ScPrintSelectionStatus() {}

	void	SetMode(ScPrintSelectionMode eNew)	{ eMode = eNew; }
	void	SetRanges(const ScRangeList& rNew)	{ aRanges = rNew; }

	BOOL	operator==(const ScPrintSelectionStatus& rOther) const
			{ return eMode == rOther.eMode && aRanges == rOther.aRanges; }
};


/** Stores the data for printing that is needed from several sheets,
	so it doesn't have to be calculated for rendering each page. */

class ScPrintFuncCache
{
	ScPrintSelectionStatus	aSelection;
	ScDocShell*				pDocSh;
	long					nTotalPages;
	long					nPages[MAXTAB+1];
	long					nFirstAttr[MAXTAB+1];

public:
			ScPrintFuncCache( ScDocShell* pD, const ScMarkData& rMark,
								const ScPrintSelectionStatus& rStatus );
			~ScPrintFuncCache();

	BOOL	IsSameSelection( const ScPrintSelectionStatus& rStatus ) const;

	long	GetPageCount() const				{ return nTotalPages; }
	long	GetFirstAttr( USHORT nTab ) const	{ return nFirstAttr[nTab]; }
	USHORT	GetTabForPage( long nPage ) const;
	long	GetTabStart( USHORT nTab ) const;
	long	GetDisplayStart( USHORT nTab ) const;
};

} //namespace binfilter
#endif

