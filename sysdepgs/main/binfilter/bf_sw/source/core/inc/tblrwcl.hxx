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


#ifndef _TBLRWCL_HXX
#define _TBLRWCL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _TBLSEL_HXX
#include <tblsel.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
namespace binfilter {

class SfxPoolItem;
class SvxBorderLine;
class SwDoc;
class SwTableNode;
class _FndLine;
class _FndBox;
class SwTableLine;
class SwTableBox;
class SwTableBoxFmt;
class SwTableLineFmt;
class SwHistory;
class SwCntntNode;

class SwShareBoxFmts;
class SwFmtFrmSize;

// Funktions Deklarationen:
BOOL lcl_CopyRow( const _FndLine*& rpFndLine, void* pPara );
BOOL lcl_CopyCol( const _FndBox*& rpFndBox, void* pPara );

BOOL lcl_MergeGCBox( const SwTableBox*& rpBox, void* pPara );
BOOL lcl_MergeGCLine( const SwTableLine*& rpLine, void* pPara );

BOOL lcl_Merge_MoveBox( const _FndBox*& rpFndBox, void* pPara );
BOOL lcl_Merge_MoveLine( const _FndLine*& rpFndLine, void* pPara );

BOOL lcl_CopyBoxToDoc( const _FndBox*& rpFndBox, void* pPara );
BOOL lcl_CopyLineToDoc( const _FndLine*& rpFndLn, void* pPara );

BOOL lcl_BoxSetHeadCondColl( const SwTableBox*& rpBox, void* pPara );
BOOL lcl_LineSetHeadCondColl( const SwTableLine*& rpLine, void* pPara );


#ifdef DBG_UTIL
#endif

void _InsTblBox( SwDoc* pDoc, SwTableNode* pTblNd,
				SwTableLine* pLine, SwTableBoxFmt* pBoxFrmFmt,
				SwTableBox* pBox, USHORT nInsPos, USHORT nCnt = 1 );

void _DeleteBox( SwTable& rTbl, SwTableBox* pBox,
				BOOL bCalcNewSize = TRUE, const BOOL bCorrBorder = TRUE,
				SwShareBoxFmts* pShareFmts = 0 );

// Klasse fuers SplitTable
// sammelt von einer Line die Boxen aller obersten oder untersten Lines
// in einem Array. Zusaetzlich werden die Positionen vermerkt.
// ( die Implementierung steht im ndtbl.cxx)

 class SwCollectTblLineBoxes
 {
    SvUShorts aPosArr;
    SwSelBoxes_SAR aBoxes;
    SwHistory* pHst;
    USHORT nMode, nWidth;
    BOOL bGetFromTop : 1;
    BOOL bGetValues : 1;

 public:
    SwCollectTblLineBoxes( BOOL bTop, USHORT nMd = 0, SwHistory* pHist=0 )
        : aPosArr( 16, 16 ), aBoxes( 16, 16 ),
        bGetFromTop( bTop ), bGetValues( TRUE ), nMode( nMd ),
        nWidth( 0 ), pHst( pHist )
    {}

    void AddBox( const SwTableBox& rBox );
    const SwTableBox* GetBoxOfPos( const SwTableBox& rBox );

    USHORT Count() const                { return aBoxes.Count(); }
    const SwTableBox& GetBox( USHORT nPos, USHORT* pWidth = 0 ) const
        {
            // hier wird die EndPos der Spalte benoetigt!
            if( pWidth )
                *pWidth = nPos+1 == aPosArr.Count() ? nWidth
                                                    : aPosArr[ nPos+1 ];
            return *aBoxes[ nPos ];
        }

    BOOL IsGetFromTop() const           { return bGetFromTop; }
    BOOL IsGetValues() const            { return bGetValues; }

    USHORT GetMode() const              { return nMode; }
    void SetValues( BOOL bFlag )        { bGetValues = FALSE; nWidth = 0;
                                          bGetFromTop = bFlag; }
    FASTBOOL Resize( USHORT nOffset, USHORT nWidth );
 };

BOOL lcl_Box_CollectBox( const SwTableBox*& rpBox, void* pPara );
BOOL lcl_Line_CollectBox( const SwTableLine*& rpLine, void* pPara );

BOOL lcl_BoxSetSplitBoxFmts( const SwTableBox*& rpBox, void* pPara );

struct _SwGCLineBorder
{
	const SwTableLines* pLines;
	SwShareBoxFmts* pShareFmts;
	USHORT nLinePos;

	_SwGCLineBorder( const SwTable& rTable )
		: pLines( &rTable.GetTabLines() ), nLinePos( 0 ), pShareFmts(0) {}

	_SwGCLineBorder( const SwTableBox& rBox )
		: pLines( &rBox.GetTabLines() ), nLinePos( 0 ), pShareFmts(0) {}
	BOOL IsLastLine() const { return nLinePos + 1 >= pLines->Count(); }
};

class _SwGCBorder_BoxBrd
{
	const SvxBorderLine* pBrdLn;
	BOOL bAnyBorderFnd;
public:
	_SwGCBorder_BoxBrd() : pBrdLn( 0 ), bAnyBorderFnd( FALSE ) {}

	void SetBorder( const SvxBorderLine& rBorderLine )
		{ pBrdLn = &rBorderLine; bAnyBorderFnd = FALSE; }

	// checke, ob die linke Border dieselbe wie die gesetzte ist
	// returnt FALSE falls gar keine Border gesetzt ist
	BOOL CheckLeftBorderOfFormat( const SwFrmFmt& rFmt );

	BOOL IsAnyBorderFound() const { return bAnyBorderFnd; }
};

BOOL lcl_GC_Line_Border( const SwTableLine*& , void* pPara );
BOOL lcl_GC_Box_Border( const SwTableBox*& , void* pPara );

BOOL lcl_GCBorder_ChkBoxBrd_L( const SwTableLine*& , void* pPara );
BOOL lcl_GCBorder_ChkBoxBrd_B( const SwTableBox*& , void* pPara );

BOOL lcl_GCBorder_GetLastBox_L( const SwTableLine*& , void* pPara );
BOOL lcl_GCBorder_GetLastBox_B( const SwTableBox*& , void* pPara );


class SwShareBoxFmt
{
	const SwFrmFmt* pOldFmt;
	SvPtrarr aNewFmts;

public:
	SwShareBoxFmt( const SwFrmFmt& rFmt )
		: pOldFmt( &rFmt ), aNewFmts( 1, 4 )
	{}

	const SwFrmFmt& GetOldFormat() const { return *pOldFmt; }

    SwFrmFmt* GetFormat( long nWidth ) const;
    SwFrmFmt* GetFormat( const SfxPoolItem& rItem ) const;
    void AddFormat( const SwFrmFmt& rFmt );
//STRIP001 	// returnt TRUE, wenn geloescht werden kann
    FASTBOOL RemoveFormat( const SwFrmFmt& rFmt );
};


SV_DECL_PTRARR_DEL( _SwShareBoxFmts, SwShareBoxFmt*, 8, 8 )

class SwShareBoxFmts
{
	_SwShareBoxFmts aShareArr;
	BOOL Seek_Entry( const SwFrmFmt& rFmt, USHORT* pPos ) const;

    void ChangeFrmFmt( SwTableBox* pBox, SwTableLine* pLn, SwFrmFmt& rFmt );
public:
	SwShareBoxFmts() {}
	~SwShareBoxFmts();

    SwFrmFmt* GetFormat( const SwFrmFmt& rFmt, long nWidth ) const;
    SwFrmFmt* GetFormat( const SwFrmFmt& rFmt, const SfxPoolItem& ) const;
    void AddFormat( const SwFrmFmt& rOld, const SwFrmFmt& rNew );
    void SetSize( SwTableBox& rBox, const SwFmtFrmSize& rSz );
    void SetAttr( SwTableBox& rBox, const SfxPoolItem& rItem );
    void RemoveFormat( const SwFrmFmt& rFmt );
};



} //namespace binfilter
#endif
