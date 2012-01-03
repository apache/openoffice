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


#ifndef _TXTFTN_HXX
#define _TXTFTN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
namespace binfilter {

class SwNodeIndex;
class SwTxtNode;
class SwNodes;
class SwDoc;

// ATT_FTN **********************************************************

class SwTxtFtn : public SwTxtAttr
{
	SwNodeIndex *pStartNode;
	SwTxtNode* pMyTxtNd;
	USHORT nSeqNo;

	// Zuweisung und Copy-Ctor sind nicht erlaubt.
	SwTxtFtn& operator=( const SwTxtFtn &rTxtFtn );
	SwTxtFtn( const SwTxtFtn &rTxtFtn );

public:
	SwTxtFtn( const SwFmtFtn& rAttr, xub_StrLen nStart );
	~SwTxtFtn();

	inline SwNodeIndex *GetStartNode() const { return pStartNode; }
	void SetStartNode( const SwNodeIndex *pNode, BOOL bDelNodes = TRUE );
	void SetNumber( const USHORT nNumber, const String* = 0 );

	// erfrage und setze den TxtNode Pointer
	inline const SwTxtNode& GetTxtNode() const;
	void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }

		// lege eine neue leere TextSection fuer diese Fussnote an
	void MakeNewTextSection( SwNodes& rNodes );

		// loesche die FtnFrame aus der Seite
    void DelFrms();
		// bedingten Absatzvorlagen checken

		// fuer die Querverweise auf Fussnoten
	USHORT SetSeqRefNo();
	void SetSeqNo( USHORT n )				{ nSeqNo = n; }	// fuer die Reader
	USHORT GetSeqRefNo() const 				{ return nSeqNo; }

	static void SetUniqueSeqRefNo( SwDoc& rDoc );
};

inline const SwTxtNode& SwTxtFtn::GetTxtNode() const
{
	ASSERT( pMyTxtNd, "SwTxtFtn:: wo ist mein TextNode?" );
	return *pMyTxtNd;
}

} //namespace binfilter
#endif

