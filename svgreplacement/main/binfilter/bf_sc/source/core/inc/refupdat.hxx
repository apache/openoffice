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



#ifndef SC_REFUPDAT_HXX
#define SC_REFUPDAT_HXX

#include "global.hxx"
namespace binfilter {

class ScDocument;
class ScBigRange;
struct ComplRefData;

enum ScRefUpdateRes {
	UR_NOTHING	= 0,		// keine Anpassungen
	UR_UPDATED	= 1,		// Anpassungen erfolgt
	UR_INVALID	= 2			// Referenz wurde ungueltig
};

class ScRefUpdate
{
public:
	static ScRefUpdateRes Update
		( ScDocument* pDoc, UpdateRefMode eUpdateRefMode,
							USHORT nCol1, USHORT nRow1, USHORT nTab1,
							USHORT nCol2, USHORT nRow2, USHORT nTab2,
							short nDx, short nDy, short nDz,
							USHORT& theCol1, USHORT& theRow1, USHORT& theTab1,
							USHORT& theCol2, USHORT& theRow2, USHORT& theTab2 );

 	static ScRefUpdateRes Update( UpdateRefMode eUpdateRefMode,
 								const ScBigRange& rWhere,
 								INT32 nDx, INT32 nDy, INT32 nDz,
 								ScBigRange& rWhat );

 	static ScRefUpdateRes Update( ScDocument* pDoc,
 								  UpdateRefMode eUpdateRefMode,
 								  const ScAddress& rPos, const ScRange& rRange,
 								  short nDx, short nDy, short nDz,
 								  ComplRefData& rRef );
 	static ScRefUpdateRes Move( ScDocument* pDoc, const ScAddress& rPos,
 								short nDx, short nDy, short nDz,
 								ComplRefData& rRef, BOOL bWrap, BOOL bAbsolute );
	static void MoveRelWrap( ScDocument* pDoc, const ScAddress& rPos,
								ComplRefData& rRef );
};


} //namespace binfilter
#endif

