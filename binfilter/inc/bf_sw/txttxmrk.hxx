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


#ifndef _TXTTXMRK_HXX
#define _TXTTXMRK_HXX

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
namespace binfilter {

class SwTxtNode;
class SwDoc;

// SWTXT_TOXMARK *******************************
// Attribut fuer die Verzeichnisse

class SwTxtTOXMark : public SwTxtAttrEnd
{
	xub_StrLen* pEnd;		// wenn AlternativText vom SwTOXMark dann 0 !!
	SwTxtNode* pMyTxtNd;
public:
	SwTxtTOXMark( const SwTOXMark& rAttr,
					xub_StrLen nStart, xub_StrLen * pEnd = 0 );
	virtual ~SwTxtTOXMark();

	virtual xub_StrLen *GetEnd();					// SwTxtAttr
	inline const xub_StrLen* GetEnd() const { return pEnd; }

	void CopyTOXMark( SwDoc* pDestDoc );

	// erfrage und setze den TxtNode Pointer
	inline const SwTxtNode* GetpTxtNd() const { return pMyTxtNd; }
	inline const SwTxtNode& GetTxtNode() const;
	void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }
};

inline const SwTxtNode& SwTxtTOXMark::GetTxtNode() const
{
	ASSERT( pMyTxtNd, "SwTxtTOXMark:: wo ist mein TextNode?" );
	return *pMyTxtNd;
}

} //namespace binfilter
#endif

