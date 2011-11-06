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


#ifndef _TXTFLD_HXX
#define _TXTFLD_HXX

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
namespace binfilter {

class SwTxtNode;

// ATT_FLD ***********************************

class SwTxtFld : public SwTxtAttr
{
	String     aExpand;
	SwTxtNode  *pMyTxtNd;
public:
	SwTxtFld( const SwFmtFld& rAttr, xub_StrLen nStart );
	~SwTxtFld();

	void Expand();
	inline void ExpandAlways();

	// erfrage und setze den TxtNode Pointer
	const SwTxtNode* GetpTxtNode() const { return pMyTxtNd; }
	inline const SwTxtNode& GetTxtNode() const;
	void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }
    // enable notification that field content has changed and needs reformatting
    void NotifyContentChange(SwFmtFld& rFmtFld);
};

inline const SwTxtNode& SwTxtFld::GetTxtNode() const
{
	ASSERT( pMyTxtNd, "SwTxtFld:: wo ist mein TextNode?" );
	return *pMyTxtNd;
}

inline void SwTxtFld::ExpandAlways()
{
	aExpand += ' ';
	Expand();
}

} //namespace binfilter
#endif

