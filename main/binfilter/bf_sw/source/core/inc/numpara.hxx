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


#ifndef _NUMPARA_HXX
#define _NUMPARA_HXX

#include <bf_svtools/bf_solar.h>

#include "numrule.hxx"
#ifndef _NDARR_HXX //autogen
#include "ndarr.hxx"
#endif
#include "ndindex.hxx"
namespace binfilter {

class SwTxtNode;
class SwDoc;
class SwNumSection;
class SwNodes;

class _NumPara
{
	const SwNumSection& rNumSection;

	void _Init();

public:
	SwNodeIndex aNdIdx;
	ULONG nCnt;
	SwNum aNum;
	char nDiff;				// +1, 0, -1 !! dafuer reicht ein char !!
	BYTE nOldLevel;
	BOOL bInitNum : 1;
	BOOL bWasUndo : 1;
	BOOL bOverTbl : 1;		// Flag fuer ForEach-Methoden, Tabelle ueberspr.

	_NumPara( char nOffset, const SwNodeIndex& rNdIdx,
				const SwNumSection& rSect, ULONG nCntNodes );
	_NumPara( const SwNodeIndex& rNdIdx, const SwNumSection& rSect );
	~_NumPara();

	void UpdateNum( SwTxtNode& rTxtNd );
};

// Funktion fuer ForEach am Nodes-Array
BOOL _NumUpDown( const SwNodePtr& rpNode, void* pPara );


} //namespace binfilter
#endif	// _NUMPARA_HXX
