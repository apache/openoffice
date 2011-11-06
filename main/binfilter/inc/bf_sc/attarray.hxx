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



#ifndef SC_ATRARR_HXX
#define SC_ATRARR_HXX

#include <bf_svtools/bf_solar.h>


#ifndef SC_SCATTR_HXX
#include "attrib.hxx"
#endif
class Rectangle;
namespace binfilter {

class SfxItemPoolCache;
class SfxStyleSheetBase;

class ScDocument;
class ScMarkArray;
class ScPatternAttr;
class ScStyleSheet;

class SvxBorderLine;
class SvxBoxItem;
class SvxBoxInfoItem;

#define SC_LINE_EMPTY			0
#define SC_LINE_SET				1
#define SC_LINE_DONTCARE		2

#define SC_ATTRARRAY_DELTA      4

struct ScLineFlags
{
	BYTE	nLeft;
	BYTE	nRight;
	BYTE	nTop;
	BYTE	nBottom;
	BYTE	nHori;
	BYTE	nVert;

	ScLineFlags() : nLeft(SC_LINE_EMPTY),nRight(SC_LINE_EMPTY),nTop(SC_LINE_EMPTY),
					nBottom(SC_LINE_EMPTY),nHori(SC_LINE_EMPTY),nVert(SC_LINE_EMPTY) {}
};

struct ScAttrEntry
{
	USHORT					nRow;
	const ScPatternAttr*	pPattern;
};


class ScAttrArray
{
private:
	USHORT			nCol;
	USHORT			nTab;
	ScDocument*		pDocument;

	USHORT			nCount;
    USHORT          nLimit;
	ScAttrEntry*	pData;

friend class ScDocument;				// fuer FillInfo
friend class ScAttrIterator;
friend void lcl_IterGetNumberFormat( ULONG& nFormat,
		const ScAttrArray*& rpArr, USHORT& nAttrEndRow,
		const ScAttrArray* pNewArr, USHORT nRow, ScDocument* pDoc );


public:
			ScAttrArray( USHORT nNewCol, USHORT nNewTab, ScDocument* pDoc );
			~ScAttrArray();

	void	SetTab(USHORT nNewTab)	{ nTab = nNewTab; }
	void	SetCol(USHORT nNewCol)	{ nCol = nNewCol; }
#ifdef DBG_UTIL
	void	TestData() const;
#endif
	void	Reset( const ScPatternAttr* pPattern, BOOL bAlloc = TRUE );
	BOOL	Concat(USHORT nPos);

	const ScPatternAttr* GetPattern( USHORT nRow ) const;
	void	MergePatternArea( USHORT nStartRow, USHORT nEndRow, SfxItemSet** ppSet, BOOL bDeep ) const;


	void	SetPattern( USHORT nRow, const ScPatternAttr* pPattern, BOOL bPutToPool = FALSE );
	void	SetPatternArea( USHORT nStartRow, USHORT nEndRow, const ScPatternAttr* pPattern, BOOL bPutToPool = FALSE);
	void	ApplyStyleArea( USHORT nStartRow, USHORT nEndRow, ScStyleSheet* pStyle );
	void	ApplyCacheArea( USHORT nStartRow, USHORT nEndRow, SfxItemPoolCache* pCache );

/*N*/ 	void	ClearItems( USHORT nStartRow, USHORT nEndRow, const USHORT* pWhich );



	BOOL	ApplyFlags( USHORT nStartRow, USHORT nEndRow, INT16 nFlags );
/*N*/ 	BOOL	RemoveFlags( USHORT nStartRow, USHORT nEndRow, INT16 nFlags );

	BOOL 	Search( USHORT nRow, short& nIndex ) const;

	BOOL	HasAttrib( USHORT nRow1, USHORT nRow2, USHORT nMask ) const;
	BOOL	ExtendMerge( USHORT nThisCol, USHORT nStartRow, USHORT nEndRow,
								USHORT& rPaintCol, USHORT& rPaintRow,
								BOOL bRefresh, BOOL bAttrs );
	BOOL	RemoveAreaMerge( USHORT nStartRow, USHORT nEndRow );

	void	FindStyleSheet( const SfxStyleSheetBase* pStyleSheet, BOOL* pUsed, BOOL bReset );

	void	DeleteAreaSafe(USHORT nStartRow, USHORT nEndRow);

	BOOL	IsEmpty() const;


	BOOL	HasVisibleAttr( USHORT& rFirstRow, USHORT& rLastRow, BOOL bSkipFirst ) const;
	BOOL	IsVisibleEqual( const ScAttrArray& rOther,
							USHORT nStartRow, USHORT nEndRow ) const;
	BOOL	IsAllEqual( const ScAttrArray& rOther, USHORT nStartRow, USHORT nEndRow ) const;

/*N*/ 	BOOL	TestInsertCol( USHORT nStartRow, USHORT nEndRow) const;
/*N*/ 	BOOL	TestInsertRow( USHORT nSize ) const;
/*N*/ 	void	InsertRow( USHORT nStartRow, USHORT nSize );
/*N*/ 	void	DeleteRow( USHORT nStartRow, USHORT nSize );
/*N*/ 	void	DeleteRange( USHORT nStartIndex, USHORT nEndIndex );
	void	DeleteArea( USHORT nStartRow, USHORT nEndRow );
/*N*/ 	void	MoveTo( USHORT nStartRow, USHORT nEndRow, ScAttrArray& rAttrArray );
	void	CopyArea( USHORT nStartRow, USHORT nEndRow, short nDy, ScAttrArray& rAttrArray,
						INT16 nStripFlags = 0 );

	void	DeleteHardAttr( USHORT nStartRow, USHORT nEndRow );

	void	Save( SvStream& rStream ) const;
	void	Load( SvStream& rStream );
    void    ConvertFontsAfterLoad();     // old binary file format
};


//	------------------------------------------------------------------------------
//								Iterator fuer Attribute
//	------------------------------------------------------------------------------

/*N*/ class ScAttrIterator
/*N*/ {
/*N*/ 	const ScAttrArray*	pArray;
/*N*/ 	short				nPos;
/*N*/ 	USHORT				nRow;
/*N*/ 	USHORT				nEndRow;
/*N*/ public:
/*N*/ 	inline				ScAttrIterator( const ScAttrArray* pNewArray, USHORT nStart, USHORT nEnd );
/*N*/ 	inline const ScPatternAttr*	Next( USHORT& rTop, USHORT& rBottom );
/*N*/ 	USHORT				GetNextRow() const { return nRow; }
/*N*/ };
/*N*/ 
/*N*/ 
/*N*/ inline ScAttrIterator::ScAttrIterator( const ScAttrArray* pNewArray, USHORT nStart, USHORT nEnd ) :
/*N*/ 	pArray( pNewArray ),
/*N*/ 	nRow( nStart ),
/*N*/ 	nEndRow( nEnd )
/*N*/ {
/*N*/ 	if ( nStart )
/*N*/ 		pArray->Search( nStart, nPos );
/*N*/ 	else
/*N*/ 		nPos = 0;
/*N*/ }
/*N*/ 
/*N*/ inline const ScPatternAttr*	ScAttrIterator::Next( USHORT& rTop, USHORT& rBottom )
/*N*/ {
/*N*/ 	const ScPatternAttr* pRet;
/*N*/ 	if ( nPos < (short) pArray->nCount && nRow <= nEndRow )
/*N*/ 	{
/*N*/ 		rTop = nRow;
/*N*/ 		rBottom = Min( pArray->pData[nPos].nRow, nEndRow );
/*N*/ 		pRet = pArray->pData[nPos].pPattern;
/*N*/ 		nRow = rBottom + 1;
/*N*/ 		++nPos;
/*N*/ 	}
/*N*/ 	else
/*N*/ 		pRet = NULL;
/*N*/ 	return pRet;
/*N*/ }



} //namespace binfilter
#endif


