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


#ifndef _DOCARY_HXX
#define _DOCARY_HXX

#include <bf_svtools/bf_solar.h>

namespace binfilter {//STRIP009
class SwFieldType;
class SwFrmFmt;
class SwCharFmt;
class SwBookmark;
class SwTOXType;
class SwSectionFmt;
class SwNumRule;
class SwRedline;
class SwUnoCrsr;
class SwOLENode;
} //namespace binfilter

#ifdef sun
#undef sun
#endif

namespace com { namespace sun { namespace star { namespace i18n {
	struct ForbiddenCharacters;    // comes from the I18N UNO interface
}}}}


#ifndef _SWTYPES_HXX //autogen
#include <swtypes.hxx>
#endif

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
namespace binfilter {

typedef SwFieldType* SwFldTypePtr;
#define GROW_FLDTYPES	16

//PageDescriptor-Schnittstelle
//typedef SwPageDesc * SwPageDescPtr;
//SV_DECL_PTRARR_DEL(SwPageDescs, SwPageDescPtr,1,1)//STRIP008 ;

typedef SwFrmFmt* SwFrmFmtPtr;
SV_DECL_PTRARR_DEL(SwFrmFmts,SwFrmFmtPtr,4,4)

//Spezifische Frameformate (Rahmen, DrawObjecte)
SV_DECL_PTRARR_DEL(SwSpzFrmFmts,SwFrmFmtPtr,0,4)

typedef SwCharFmt* SwCharFmtPtr;
SV_DECL_PTRARR_DEL(SwCharFmts,SwCharFmtPtr,4,4)

SV_DECL_PTRARR_DEL( SwFldTypes, SwFldTypePtr, INIT_FLDTYPES, GROW_FLDTYPES )

//Bookmarks (nach Dokumentpositionen sortiertes Array)
typedef SwBookmark* SwBookmarkPtr;
SV_DECL_PTRARR_SORT(SwBookmarks, SwBookmarkPtr,0,1)

typedef SwTOXType* SwTOXTypePtr;
SV_DECL_PTRARR_DEL( SwTOXTypes, SwTOXTypePtr, 0, 1 )

typedef SwSectionFmt* SwSectionFmtPtr;
SV_DECL_PTRARR_DEL(SwSectionFmts,SwSectionFmtPtr,0,4)


typedef SwNumRule* SwNumRulePtr;
SV_DECL_PTRARR_DEL( SwNumRuleTbl, SwNumRulePtr, 0, 5 )

typedef SwRedline* SwRedlinePtr;
SV_DECL_PTRARR_SORT_DEL( _SwRedlineTbl, SwRedlinePtr, 0, 16 )

class SwRedlineTbl : private _SwRedlineTbl
{
public:
	SwRedlineTbl( BYTE nSize = 0, BYTE nG = 16 )
		: _SwRedlineTbl( nSize, nG ) {}
	~SwRedlineTbl() {}

	BOOL SavePtrInArr( SwRedlinePtr p ) { return _SwRedlineTbl::Insert( p ); }

/*N*/ 	BOOL Insert( SwRedlinePtr& p, BOOL bIns = TRUE ); //SW50.SDW
/*N*/ 	BOOL InsertWithValidRanges( SwRedlinePtr& p, USHORT* pInsPos = 0 ); //SW50.SDW

/*N*/ 	void Remove( USHORT nP, USHORT nL = 1 ); //SW50.SDW

	// suche den naechsten oder vorherigen Redline mit dergleichen Seq.No
	// Mit dem Lookahead kann die Suche eingeschraenkt werden. 0 oder
	// USHRT_MAX suchen im gesamten Array.

	_SwRedlineTbl::Count;
	_SwRedlineTbl::operator[];
	_SwRedlineTbl::GetObject;
	_SwRedlineTbl::Seek_Entry;
	_SwRedlineTbl::GetPos;
};

typedef SwUnoCrsr* SwUnoCrsrPtr;
SV_DECL_PTRARR_DEL( SwUnoCrsrTbl, SwUnoCrsrPtr, 0, 4 )

typedef SwOLENode* SwOLENodePtr;
SV_DECL_PTRARR(SwOLENodes,SwOLENodePtr,16,16)


} //namespace binfilter
#endif	//_DOCARY_HXX

