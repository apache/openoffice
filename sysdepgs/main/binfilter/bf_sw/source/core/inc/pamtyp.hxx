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



#ifndef _PAMTYP_HXX
#define _PAMTYP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TXTCMP_HXX //autogen
#include <bf_svtools/txtcmp.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
namespace binfilter {

class SwpHints;
struct SwPosition;
class SwPaM;
class SwTxtAttr;

// Funktions-Deklarationen fuer die Move/Find-Methoden vom SwPaM

void GoStartDoc( SwPosition*);
void GoEndDoc( SwPosition*);
void GoStartSection( SwPosition*);
void GoEndSection( SwPosition*);
FASTBOOL GoInDoc( SwPaM&, SwMoveFn);
FASTBOOL GoInSection( SwPaM&, SwMoveFn);
FASTBOOL GoInNode( SwPaM&, SwMoveFn);
FASTBOOL GoInCntnt( SwPaM&, SwMoveFn);
FASTBOOL GoInCntntCells( SwPaM&, SwMoveFn);
const SwTxtAttr* GetFrwrdTxtHint( const SwpHints&, USHORT&, xub_StrLen );
const SwTxtAttr* GetBkwrdTxtHint( const SwpHints&, USHORT&, xub_StrLen );

FASTBOOL GoNext(SwNode* pNd, SwIndex * pIdx, USHORT nMode );
FASTBOOL GoPrevious(SwNode* pNd, SwIndex * pIdx, USHORT nMode );
SwCntntNode* GoNextNds( SwNodeIndex * pIdx, FASTBOOL );
SwCntntNode* GoPreviousNds( SwNodeIndex * pIdx, FASTBOOL );

// --------- Funktionsdefinitionen fuer die SwCrsrShell --------------

FASTBOOL GoPrevPara( SwPaM&, SwPosPara);
FASTBOOL GoCurrPara( SwPaM&, SwPosPara);
FASTBOOL GoNextPara( SwPaM&, SwPosPara);
FASTBOOL GoPrevSection( SwPaM&, SwPosSection);
FASTBOOL GoCurrSection( SwPaM&, SwPosSection);
FASTBOOL GoNextSection( SwPaM&, SwPosSection);


// ------------ Typedefiniton fuer Funktionen ----------------------

typedef FASTBOOL (*GoNd)( SwNode*, SwIndex*, USHORT );
typedef SwCntntNode* (*GoNds)( SwNodeIndex*, FASTBOOL );
typedef void (*GoDoc)( SwPosition* );
typedef void (*GoSection)( SwPosition* );
typedef FASTBOOL (SwPosition:: *CmpOp)( const SwPosition& ) const;
typedef const SwTxtAttr* (*GetHint)( const SwpHints&, USHORT&, xub_StrLen );
typedef int (::utl::TextSearch:: *SearchTxt)( const String&, xub_StrLen*,
					xub_StrLen*, ::com::sun::star::util::SearchResult* );
typedef void (SwNodes:: *MvSection)( SwNodeIndex * ) const;


struct SwMoveFnCollection
{
	GoNd	  fnNd;
	GoNds	  fnNds;
	GoDoc	  fnDoc;
	GoSection fnSections;
	CmpOp	  fnCmpOp;
	GetHint   fnGetHint;
	SearchTxt fnSearch;
	MvSection fnSection;
};

// --------- Funktionsdefinitionen fuers Suchen --------------
SwCntntNode* GetNode( SwPaM&, FASTBOOL&, SwMoveFn, FASTBOOL bInReadOnly = FALSE );



} //namespace binfilter
#endif
