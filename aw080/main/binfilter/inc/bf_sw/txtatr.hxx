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


#ifndef _TXTATR_HXX
#define _TXTATR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TXATBASE_HXX
#include <txatbase.hxx>     // SwTxtAttr/SwTxtAttrEnd
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
namespace binfilter {

class SwTxtNode;	// fuer SwTxtFld
class SvxFont;
class SwCharSetCol;
class SwCharFmt;
class SvxTwoLinesItem;

// ATT_CHARFMT *********************************************

class SwTxtCharFmt : public SwTxtAttrEnd
{
    SwTxtNode* pMyTxtNd;
	BOOL bPrevNoHyph	: 1;
	BOOL bPrevBlink		: 1;
	BOOL bPrevURL		: 1;
	BOOL bColor         : 1;

public:
	SwTxtCharFmt( const SwFmtCharFmt& rAttr, xub_StrLen nStart, xub_StrLen nEnd );
	~SwTxtCharFmt( );

	// werden vom SwFmtCharFmt hierher weitergeleitet
	virtual void Modify( SfxPoolItem*, SfxPoolItem* );    // SwClient
	virtual BOOL GetInfo( SfxPoolItem& rInfo ) const;

	// erfrage und setze den TxtNode Pointer
	inline const SwTxtNode& GetTxtNode() const;
	void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }

};

// ATT_HARDBLANK ******************************


// ATT_XNLCONTAINERITEM ******************************


// ******************************

class SwTxtRuby : public SwTxtAttrEnd, public SwClient
{

public:
	SwTxtRuby( const SwFmtRuby& rAttr, xub_StrLen nStart, xub_StrLen nEnd ): SwTxtAttrEnd( (SfxPoolItem &)rAttr, nStart, nEnd ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	SwTxtRuby( const SwFmtRuby& rAttr, xub_StrLen nStart, xub_StrLen nEnd );

};

// ******************************


// --------------- Inline Implementierungen ------------------------

inline const SwTxtNode& SwTxtCharFmt::GetTxtNode() const
{
	ASSERT( pMyTxtNd, "SwTxtCharFmt:: wo ist mein TextNode?" );
	return *pMyTxtNd;
}


} //namespace binfilter
#endif
