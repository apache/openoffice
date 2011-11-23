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



// INCLUDE ---------------------------------------------------------------

// System - Includes -----------------------------------------------------

#ifdef PCH
#endif


#include <string.h>

#include "pagepar.hxx"
namespace binfilter {


//========================================================================
// struct ScPageTableParam:

/*N*/ ScPageTableParam::ScPageTableParam()
/*N*/ {
/*N*/ 	Reset();
/*N*/ }

//------------------------------------------------------------------------

/*N*/ ScPageTableParam::ScPageTableParam( const ScPageTableParam& r )
/*N*/ {
/*N*/ 	*this = r;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ __EXPORT ScPageTableParam::~ScPageTableParam()
/*N*/ {
/*N*/ }

//------------------------------------------------------------------------

/*N*/ void __EXPORT ScPageTableParam::Reset()
/*N*/ {
/*N*/ 	bNotes=bGrid=bHeaders=bDrawings=
/*N*/ 	bLeftRight=bScaleAll=bScalePageNum=
/*N*/ 	bFormulas=bNullVals=bSkipEmpty			= FALSE;
/*N*/ 	bTopDown=bScaleNone=bCharts=bObjects	= TRUE;
/*N*/ 	nScaleAll		= 100;
/*N*/ 	nScalePageNum	= 0;
/*N*/ 	nFirstPageNo	= 1;
/*N*/ }

//------------------------------------------------------------------------


//------------------------------------------------------------------------


//========================================================================
// struct ScPageAreaParam:

/*N*/ ScPageAreaParam::ScPageAreaParam()
/*N*/ {
/*N*/ 	Reset();
/*N*/ }

//------------------------------------------------------------------------

/*N*/ ScPageAreaParam::ScPageAreaParam( const ScPageAreaParam& r )
/*N*/ {
/*N*/ 	*this = r;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ __EXPORT ScPageAreaParam::~ScPageAreaParam()
/*N*/ {
/*N*/ }

//------------------------------------------------------------------------

/*N*/ void __EXPORT ScPageAreaParam::Reset()
/*N*/ {
/*N*/ 	bPrintArea = bRepeatRow = bRepeatCol = FALSE;
/*N*/ 
/*N*/ 	memset( &aPrintArea, 0, sizeof(ScRange) );
/*N*/ 	memset( &aRepeatRow, 0, sizeof(ScRange) );
/*N*/ 	memset( &aRepeatCol, 0, sizeof(ScRange) );
/*N*/ }

//------------------------------------------------------------------------


//------------------------------------------------------------------------

}
