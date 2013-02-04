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


#ifndef _ACMPLWRD_HXX
#define _ACMPLWRD_HXX

#include <bf_svtools/bf_solar.h>


#define _SVSTDARR_STRINGSISORTDTOR
#include <bf_svtools/svstdarr.hxx>
namespace binfilter {

class SwDoc;
class SwAutoCompleteWord_Impl;
class SwAutoCompleteClient;

class SwAutoCompleteWord
{
	friend class SwAutoCompleteClient;

    SvStringsISortDtor aWordLst; // contains extended strings carrying source information
	SvPtrarr aLRULst;

    SwAutoCompleteWord_Impl* pImpl;
	USHORT nMaxCount, nMinWrdLen;
	BOOL bLockWordLst;

    void DocumentDying(const SwDoc& rDoc);
public:
	SwAutoCompleteWord( USHORT nWords = 500, USHORT nMWrdLen = 10 );
	~SwAutoCompleteWord();

    BOOL InsertWord( const String& rWord, SwDoc& rDoc );



	USHORT Count() const { return aWordLst.Count(); }

	const String& operator[]( USHORT n ) const { return *aWordLst[ n ]; }

	BOOL IsLockWordLstLocked() const 		{ return bLockWordLst; }
	void SetLockWordLstLocked( BOOL bFlag ) { bLockWordLst = bFlag; }

	USHORT GetMaxCount() const 				{ return nMaxCount; }

	USHORT GetMinWordLen() const 				{ return nMinWrdLen; }

	const SvStringsISortDtor& GetWordList() const { return aWordLst; }
};


} //namespace binfilter
#endif
