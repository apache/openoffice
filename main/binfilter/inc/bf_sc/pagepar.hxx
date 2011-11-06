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



#ifndef SC_PAGEPAR_HXX
#define SC_PAGEPAR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

//-----------------------------------------------------------------------

struct ScPageTableParam
{
	BOOL	bNotes;
	BOOL	bGrid;
	BOOL	bHeaders;
	BOOL	bCharts;
	BOOL	bObjects;
	BOOL	bDrawings;
	BOOL	bFormulas;
	BOOL	bNullVals;
	BOOL	bTopDown;
	BOOL	bLeftRight;
	BOOL	bSkipEmpty;
	BOOL	bScaleNone;
	BOOL	bScaleAll;
	BOOL	bScalePageNum;
	USHORT	nScaleAll;
	USHORT	nScalePageNum;
	USHORT	nFirstPageNo;

	ScPageTableParam();
	ScPageTableParam( const ScPageTableParam& r );
	~ScPageTableParam();

	void				Reset		();
};

struct ScPageAreaParam
{
	BOOL	bPrintArea;
	BOOL	bRepeatRow;
	BOOL	bRepeatCol;
	ScRange	aPrintArea;
	ScRange	aRepeatRow;
	ScRange	aRepeatCol;

	ScPageAreaParam();
	ScPageAreaParam( const ScPageAreaParam& r );
	~ScPageAreaParam();

	void				Reset		();
};


} //namespace binfilter
#endif


