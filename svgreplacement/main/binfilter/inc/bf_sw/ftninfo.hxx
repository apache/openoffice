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


#ifndef _FTNINFO_HXX
#define _FTNINFO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#include "numrule.hxx"
namespace binfilter {

class SwTxtFmtColl;
class SwPageDesc;

class SwEndNoteInfo : public SwClient
{
	SwDepend  	aPageDescDep;
	SwDepend 	aCharFmtDep, aAnchorCharFmtDep;
	String 		sPrefix;
	String 		sSuffix;
protected:
	BOOL 	  bEndNote;
public:
	SvxNumberType aFmt;
	USHORT 	  nFtnOffset;

	void 		ChgPageDesc( SwPageDesc *pDesc );
	SwPageDesc *GetPageDesc( SwDoc &rDoc ) const;
	SwClient   *GetPageDescDep() const { return (SwClient*)&aPageDescDep; }

	void SetFtnTxtColl(SwTxtFmtColl& rColl);
	SwTxtFmtColl* GetFtnTxtColl() const { return  (SwTxtFmtColl*) GetRegisteredIn(); } // kann 0 sein

	SwCharFmt* GetCharFmt(SwDoc &rDoc) const;
	void SetCharFmt( SwCharFmt* );
	SwClient   *GetCharFmtDep() const { return (SwClient*)&aCharFmtDep; }

	SwCharFmt* GetAnchorCharFmt(SwDoc &rDoc) const;
	void SetAnchorCharFmt( SwCharFmt* );
	SwClient   *GetAnchorCharFmtDep() const { return (SwClient*)&aAnchorCharFmtDep; }

	virtual void Modify( SfxPoolItem* pOld, SfxPoolItem* pNew );

	SwEndNoteInfo & operator=(const SwEndNoteInfo&);
	BOOL operator==( const SwEndNoteInfo &rInf ) const;

	SwEndNoteInfo( SwTxtFmtColl *pTxtColl = 0);
	SwEndNoteInfo(const SwEndNoteInfo&);

	const String& GetPrefix() const 		{ return sPrefix; }
	const String& GetSuffix() const 		{ return sSuffix; }

	void SetPrefix(const String& rSet)		{ sPrefix = rSet; }
	void SetSuffix(const String& rSet)		{ sSuffix = rSet; }

	BOOL IsEndNoteInfo() const 				{ return bEndNote; }
};

enum SwFtnPos
{
	//Derzeit nur PAGE und CHAPTER. CHAPTER == Dokumentendenoten.
	FTNPOS_PAGE = 1,
	FTNPOS_CHAPTER = 8
};

enum SwFtnNum
{
	FTNNUM_PAGE, FTNNUM_CHAPTER, FTNNUM_DOC
};

class SwFtnInfo: public SwEndNoteInfo
{
public:
	String    aQuoVadis;
	String	  aErgoSum;
	SwFtnPos  ePos;
	SwFtnNum  eNum;


	SwFtnInfo& operator=(const SwFtnInfo&);
	BOOL operator==( const SwFtnInfo &rInf ) const;

	SwFtnInfo(SwTxtFmtColl* pTxtColl = 0);
	SwFtnInfo(const SwFtnInfo&);
};


} //namespace binfilter
#endif
