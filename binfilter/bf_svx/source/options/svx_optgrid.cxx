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



// include ---------------------------------------------------------------

#define _SVX_OPTGRID_CXX

#include "svxids.hrc"
#include "optgrid.hxx"
#include "dialogs.hrc"
namespace binfilter {

/*--------------------------------------------------------------------
	Beschreibung: Rastereinstellungen Ctor
 --------------------------------------------------------------------*/

/*N*/ SvxOptionsGrid::SvxOptionsGrid() :
/*N*/ 	nFldDrawX		( 100 ),
/*N*/ 	nFldDivisionX	( 0 ),
/*N*/ 	nFldDrawY		( 100 ),
/*N*/ 	nFldDivisionY	( 0 ),
/*N*/ 	nFldSnapX		( 100 ),
/*N*/ 	nFldSnapY		( 100 ),
/*N*/ 	bUseGridsnap	( 0 ),
/*N*/ 	bSynchronize	( 1 ),
/*N*/ 	bGridVisible	( 0 ),
/*N*/ 	bEqualGrid		( 1 )
/*N*/ {
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Rastereinstellungen Dtor
 --------------------------------------------------------------------*/

/*N*/ SvxOptionsGrid::~SvxOptionsGrid()
/*N*/ {
/*N*/ }

}
