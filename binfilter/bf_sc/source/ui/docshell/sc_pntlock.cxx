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

#include "pntlock.hxx"
namespace binfilter {

//------------------------------------------------------------------------

/*N*/ ScPaintLockData::ScPaintLockData(USHORT nNewMode) :
/*N*/ 	nMode( nNewMode ),
/*N*/ 	nParts( 0 ),
/*N*/ 	nLevel( 0 ),
/*N*/ 	nDocLevel( 0 ),
/*N*/ 	bModified( FALSE )
/*N*/ {
/*N*/ }

/*N*/ ScPaintLockData::~ScPaintLockData()
/*N*/ {
/*N*/ }

/*N*/ void ScPaintLockData::AddRange( const ScRange& rRange, USHORT nP )
/*N*/ {
/*N*/ 	if (!xRangeList.Is())
/*N*/ 		xRangeList = new ScRangeList;
/*N*/ 
/*N*/ 	xRangeList->Join( rRange );
/*N*/ 	nParts |= nP;
/*N*/ }




}
