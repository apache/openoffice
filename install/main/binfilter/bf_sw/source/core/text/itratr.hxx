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


#ifndef _ITRATR_HXX
#define _ITRATR_HXX


#include <bf_svtools/bf_solar.h>
#ifndef _ATRHNDL_HXX
#include <atrhndl.hxx>
#endif

#include "txttypes.hxx"
#include "swfont.hxx"
#include "porlay.hxx"

#define _SVSTDARR_XUB_STRLEN
#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>
class OutputDevice; 
namespace binfilter {


class SwFont;
class SwpHints;
class SwTxtAttr;
class SwAttrSet;
class SwTxtNode;
class SwRedlineItr;
class ViewShell;

#ifdef VERTICAL_LAYOUT
class SwTxtFrm;
#endif

/*************************************************************************
 *						class SwAttrIter
 *************************************************************************/

class SwAttrIter
{
	friend class SwFontSave;
protected:

    SwAttrHandler aAttrHandler;
    ViewShell *pShell;
	SwFont *pFnt;
	SwpHints  *pHints;
	const SwAttrSet* pAttrSet;		 // das Char-Attribut-Set
	SwScriptInfo* pScriptInfo;

private:
	OutputDevice *pLastOut;
	MSHORT nChgCnt;
	SwRedlineItr *pRedln;
    xub_StrLen nStartIndex, nEndIndex, nPos;
	BYTE nPropFont;
	void SeekFwd( const xub_StrLen nPos );
	inline void SetFnt( SwFont* pNew ) { pFnt = pNew; }
	const void* aMagicNo[ SW_SCRIPTS ];
	MSHORT aFntIdx[ SW_SCRIPTS ];

protected:
	void Chg( SwTxtAttr *pHt );
	void Rst( SwTxtAttr *pHt );
#ifdef VERTICAL_LAYOUT
    void CtorInit( SwTxtNode& rTxtNode, SwScriptInfo& rScrInf, SwTxtFrm* pFrm = 0 );
#else
    void CtorInit( SwTxtNode& rTxtNode, SwScriptInfo& rScrInf );
#endif
    inline SwAttrIter()
		: pFnt(0), pLastOut(0),	nChgCnt(0), nPropFont(0), pShell(0), pRedln(0){}

public:
	// Konstruktor, Destruktor
    inline SwAttrIter( SwTxtNode& rTxtNode, SwScriptInfo& rScrInf )
		: pFnt(0), pLastOut(0), nChgCnt(0), nPropFont(0), pShell(0), pRedln(0)
        { CtorInit( rTxtNode, rScrInf ); }

	virtual ~SwAttrIter();

	inline SwRedlineItr *GetRedln() { return pRedln; }
	// Liefert im Parameter die Position des naechsten Wechsels vor oder an
	// der uebergebenen Characterposition zurueck. Liefert sal_False, wenn vor
	// oder an dieser Position kein Wechsel mehr erfolgt, sal_True sonst.
	xub_StrLen GetNextAttr( ) const;
	// Macht die an der Characterposition i gueltigen Attribute im
	// logischen Font wirksam.
	sal_Bool Seek( const xub_StrLen nPos );
	// Bastelt den Font an der gew. Position via Seek und fragt ihn,
	// ob er ein Symbolfont ist.
	sal_Bool IsSymbol( const xub_StrLen nPos );

	// Fuehrt ChgPhysFnt aus, wenn Seek() sal_True zurueckliefert.
	sal_Bool SeekAndChg( const xub_StrLen nPos, OutputDevice *pOut );
	sal_Bool SeekStartAndChg( OutputDevice *pOut, const sal_Bool bParaFont = sal_False );

	// Gibt es ueberhaupt Attributwechsel ?
	inline sal_Bool HasHints() const { return 0 != pHints; }

	// liefert fuer eine Position das Attribut
	SwTxtAttr *GetAttr( const xub_StrLen nPos ) const;

	inline const SwAttrSet* GetAttrSet() const { return pAttrSet; }

	inline const SwpHints *GetHints() const { return pHints; }

	inline SwFont *GetFnt() { return pFnt; }
	inline const SwFont *GetFnt() const { return pFnt; }

	inline BYTE GetPropFont() const { return nPropFont; }
	inline void SetPropFont( const BYTE nNew ) { nPropFont = nNew; }

    inline SwAttrHandler& GetAttrHandler() { return aAttrHandler; }
};

} //namespace binfilter
#endif
