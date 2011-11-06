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


#include <memory.h>

#include "indexmap.hxx"
namespace binfilter {


/*N*/ ScIndexMap::ScIndexMap( USHORT nEntries )
/*N*/ {
/*N*/ 	nCount = nEntries;
/*N*/ 	ULONG nC = nEntries ? ((ULONG) nEntries * 2) : 2;
/*N*/ 	pMap = new USHORT [ nC ];
/*N*/ 	memset( pMap, 0, nC * sizeof(USHORT) );
/*N*/ }


/*N*/ ScIndexMap::~ScIndexMap()
/*N*/ {
/*N*/ 	delete [] pMap;
/*N*/ }


/*N*/ void ScIndexMap::SetPair( USHORT nEntry, USHORT nIndex1, USHORT nIndex2 )
/*N*/ {
/*N*/ 	if ( nEntry < nCount )
/*N*/ 	{
/*N*/ 		ULONG nOff = (ULONG) nEntry * 2;
/*N*/ 		pMap[nOff] = nIndex1;
/*N*/ 		pMap[nOff+1] = nIndex2;
/*N*/ 	}
/*N*/ }

}
