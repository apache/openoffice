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



#ifndef _DOCFLD_HXX
#define _DOCFLD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _CALC_HXX
#include <calc.hxx>			// fuer SwHash
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
namespace binfilter {

class SwTxtFld;
class SwIndex;
class SwNodeIndex;
class SwCntntNode;
class SwCntntFrm;
class SwSectionNode;
class SwSection;
class SwTxtTOXMark;
class SwTableBox;
class SwTxtINetFmt;
class SwFlyFrmFmt;

// Update an den Expression Feldern
class _SetGetExpFld
{
	ULONG nNode;
	xub_StrLen nCntnt;
	union {
		const SwTxtFld* pTxtFld;
		const SwSection* pSection;
		const SwPosition* pPos;
		const SwTxtTOXMark* pTxtTOX;
		const SwTableBox* pTBox;
		const SwTxtINetFmt* pTxtINet;
		const SwFlyFrmFmt* pFlyFmt;
	} CNTNT;
	enum _SetGetExpFldType
		{
			TEXTFIELD, TEXTTOXMARK, SECTIONNODE, CRSRPOS, TABLEBOX,
			TEXTINET, FLYFRAME
		} eSetGetExpFldType;

public:
	_SetGetExpFld( const SwNodeIndex& rNdIdx, const SwTxtFld* pFld = 0,
					const SwIndex* pIdx = 0 );

	_SetGetExpFld( const SwSectionNode& rSectNode,
					const SwPosition* pPos = 0  );

	BOOL operator==( const _SetGetExpFld& rFld ) const
	{	return nNode == rFld.nNode && nCntnt == rFld.nCntnt &&
				( !CNTNT.pTxtFld || !rFld.CNTNT.pTxtFld ||
					CNTNT.pTxtFld == rFld.CNTNT.pTxtFld ); }
	BOOL operator<( const _SetGetExpFld& rFld ) const;

 	const SwTxtFld* GetFld() const
		{ return TEXTFIELD == eSetGetExpFldType ? CNTNT.pTxtFld : 0; }
 	const SwSection* GetSection() const
 		{ return SECTIONNODE == eSetGetExpFldType ? CNTNT.pSection : 0; }
 	ULONG GetNode() const { return nNode; }
    const void* GetPointer() const { return CNTNT.pTxtFld; }
	const SwNode* GetNodeFromCntnt() const;
	xub_StrLen GetCntPosFromCntnt() const;
};

typedef _SetGetExpFld* _SetGetExpFldPtr;
SV_DECL_PTRARR_SORT_DEL( _SetGetExpFlds, _SetGetExpFldPtr, 0, 10 )


// Struktur zum Speichern der Strings aus SetExp-String-Feldern
struct _HashStr : public SwHash
{
	String aSetStr;
	_HashStr( const String& rName, const String& rText, _HashStr* = 0 );
};

struct SwCalcFldType : public SwHash
{
	const SwFieldType* pFldType;

	SwCalcFldType( const String& rStr, const SwFieldType* pFldTyp )
		: SwHash( rStr ), pFldType( pFldTyp )
	{}
};

// Suche nach dem String, der unter dem Namen in der HashTabelle abgelegt
// wurde
void LookString( SwHash** ppTbl, USHORT nSize, const String& rName,
					String& rRet, USHORT* pPos = 0 );


// --------

const int GETFLD_ALL		= 3;		// veroderte Flags !!
const int GETFLD_CALC		= 1;
const int GETFLD_EXPAND		= 2;

class SwDocUpdtFld
{
	_SetGetExpFlds* pFldSortLst;	// akt. Field-Liste zum Calculieren
	SwCalcFldType*  aFldTypeTable[ TBLSZ ];

// noch eine weitere Optimierung - wird z.Z. nicht angesprochen!
	long nFldUpdtPos;				// ab dieser Position mit Update starten
	SwCntntNode* pCNode;			// der TxtNode zur UpdatePos.

	ULONG nNodes;					// sollte die NodesAnzahl unterschiedlich sein
	BYTE nFldLstGetMode;

	BOOL bInUpdateFlds : 1;			// zur Zeit laeuft ein UpdateFlds,
	BOOL bFldsDirty : 1;			// irgendwelche Felder sind ungueltig

	void _MakeFldList( SwDoc& pDoc, int eGetMode );
	void GetBodyNode( const SwTxtFld& , USHORT nFldWhich );
    void GetBodyNode( const SwSectionNode&);
public:
	SwDocUpdtFld();
	~SwDocUpdtFld();

	const _SetGetExpFlds* GetSortLst() const { return pFldSortLst; }

	void MakeFldList( SwDoc& rDoc, int bAll, int eGetMode )
	{
		if( !pFldSortLst || bAll || !( eGetMode & nFldLstGetMode ) ||
			rDoc.GetNodes().Count() != nNodes )
			_MakeFldList( rDoc, eGetMode );
	}

	void InsDelFldInFldLst( BOOL bIns, const SwTxtFld& rFld );

	void InsertFldType( const SwFieldType& rType );

	BOOL IsInUpdateFlds() const			{ return bInUpdateFlds; }
	void SetInUpdateFlds( BOOL b ) 		{ bInUpdateFlds = b; }

	BOOL IsFieldsDirty() const			{ return bFldsDirty; }
	void SetFieldsDirty( BOOL b ) 		{ bFldsDirty = b; }

	SwHash**	GetFldTypeTable() const { return (SwHash**)aFldTypeTable; }
};


} //namespace binfilter
#endif	// _DOCFLD_HXX

