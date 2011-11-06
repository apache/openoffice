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


#ifndef _TXTINET_HXX
#define _TXTINET_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif
class Color; 
namespace binfilter {

class SvxFont;
class SwTxtNode;
class SwCharFmt;


// ATT_INETFMT *********************************************

class SwTxtINetFmt : public SwTxtAttrEnd, public SwClient
{
	SwTxtNode* pMyTxtNd;
	BOOL bVisited		: 1; // Besuchter Link?
	BOOL bValidVis		: 1; // Ist das bVisited-Flag gueltig?
	BOOL bColor         : 1;

public:
	SwTxtINetFmt( const SwFmtINetFmt& rAttr, xub_StrLen nStart, xub_StrLen nEnd );
	virtual ~SwTxtINetFmt();

	virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);
	virtual BOOL GetInfo( SfxPoolItem& rInfo ) const;

	// erfrage und setze den TxtNode Pointer
	const SwTxtNode* GetpTxtNode() const { return pMyTxtNd; }
	inline const SwTxtNode& GetTxtNode() const;
	void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }

		  SwCharFmt* GetCharFmt();
	const SwCharFmt* GetCharFmt() const
			{ return ((SwTxtINetFmt*)this)->GetCharFmt(); }

	BOOL IsVisited() const { return bVisited; }
	void SetVisited( BOOL bNew ) { bVisited = bNew; }

	BOOL IsValidVis() const { return bValidVis; }
	void SetValidVis( BOOL bNew ) { bValidVis = bNew; }

};
inline const SwTxtNode& SwTxtINetFmt::GetTxtNode() const
{
	ASSERT( pMyTxtNd, "SwTxtINetFmt:: wo ist mein TextNode?" );
	return *pMyTxtNd;
}

} //namespace binfilter
#endif

