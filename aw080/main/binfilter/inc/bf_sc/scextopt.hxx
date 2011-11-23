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



#ifndef _SCEXTOPT_HXX
#define _SCEXTOPT_HXX

#include <bf_svtools/bf_solar.h>


#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {




// ACHTUNG1: einzelne Einstellungen sollten stimmen
//				-> Absicherung nur ueber Assertions
// ACHTUNG2: Plausibilitaet der Einstellungen untereinander ist nicht
//				gewaehrleistet

struct ScExtTabOptions
{
	// Split -Info
	UINT16						nTabNum;
	UINT16						nSplitX;			// horiz. pos. in twips, 0 = no split
	UINT16						nSplitY;			// vert. pos. ~
	UINT16						nLeftCol;			// leftmost column visible
	UINT16						nTopRow;			// topmost row visible
	UINT16						nLeftSplitCol;		// leftmost column after horizontal split
	UINT16						nTopSplitRow;		// topmost row after vertical split
	UINT16						nActPane;			// 0: br, 1: tr, 2: bl, 3: tl

	ScRange						aLastSel;			// last selection
	BOOL						bValidSel;
	ScRange						aDim;				// original Excel size
	BOOL						bValidDim;

	BOOL						bSelected;
	BOOL						bFrozen;			// = TRUE -> nSplitX / nSplitY contain
													// count of visible columns/rows

	inline						ScExtTabOptions( void );
	inline						ScExtTabOptions( const ScExtTabOptions& rCpy );
	inline void					operator =( const ScExtTabOptions& rCpy );

	void						SetSelection( const ScRange& rSelection );
	void						SetDimension( const ScRange& rDim );
};




class CodenameList : protected List
{
	// Code: colrowst.cxx
private:
protected:
public:
	inline						CodenameList( void );
								CodenameList( const CodenameList& );
	virtual						~CodenameList();

	inline void					Append( const String& );
};




class ScExtDocOptions
{
// Code: colrowst.cxx
private:
	// Window -Info
	ScExtTabOptions**		ppExtTabOpts;

	String*					pCodenameWB;
	CodenameList*			pCodenames;

    BOOL                    bChanged;       // for import: copy data only first time to doc
	bool					bWinProtection;  // Excel Workbook Windows protection flag

public:
	UINT32					nLinkCnt;		// Zaehlt die Rekursionstufe beim Laden
											//	von externen Dokumenten
	UINT16					nActTab;		// aktuelle Tabelle
    ScRange*                pOleSize;       // visible range if embedded
	UINT16					nSelTabs;		// count of selected sheets
	Color*					pGridCol;		// Farbe Grid und Row-/Col-Heading
	UINT16					nZoom;			// in %
	// Cursor
	UINT16					nCurCol;		// aktuelle Cursor-Position
	UINT16					nCurRow;
	// -------------------------------------------------------------------
							ScExtDocOptions( void );
							~ScExtDocOptions();

	inline const ScExtTabOptions*	GetExtTabOptions( const UINT16 nTabNum ) const;
    inline ScExtTabOptions* GetExtTabOptions( const UINT16 nTabNum );
	inline const String*	GetCodename( void ) const;		// for Workbook globals
	inline CodenameList*	GetCodenames( void );			// for tables

	inline void				SetWinProtection(bool bImportWinProtection) {bWinProtection = bImportWinProtection; }
	inline bool				IsWinProtected()         { return bWinProtection; }
};




inline CodenameList::CodenameList( void )
{
}


inline void CodenameList::Append( const String& r )
{
	List::Insert( new String( r ), LIST_APPEND );
}










inline ScExtTabOptions::ScExtTabOptions( void )
{
	nTabNum = nSplitX = nSplitY = nLeftCol = nTopRow = nLeftSplitCol = nTopSplitRow = 0;
	nActPane = 3;
	bSelected = bFrozen = bValidSel = bValidDim = FALSE;
}


inline ScExtTabOptions::ScExtTabOptions( const ScExtTabOptions& rCpy )
{
	nTabNum = rCpy.nTabNum;
	nSplitX = rCpy.nSplitX;
	nSplitY = rCpy.nSplitY;
	nLeftCol = rCpy.nLeftCol;
	nTopRow = rCpy.nTopRow;
	nLeftSplitCol = rCpy.nLeftSplitCol;
	nTopSplitRow = rCpy.nTopSplitRow;
	nActPane = rCpy.nActPane;
	aLastSel = rCpy.aLastSel;
	aDim = rCpy.aDim;
	bSelected = rCpy.bSelected;
	bFrozen = rCpy.bFrozen;
	bValidSel = rCpy.bValidSel;
	bValidDim = rCpy.bValidDim;
}


inline void ScExtTabOptions::operator =( const ScExtTabOptions& rCpy )
{
	nTabNum = rCpy.nTabNum;
	nSplitX = rCpy.nSplitX;
	nSplitY = rCpy.nSplitY;
	nLeftCol = rCpy.nLeftCol;
	nTopRow = rCpy.nTopRow;
	nLeftSplitCol = rCpy.nLeftSplitCol;
	nTopSplitRow = rCpy.nTopSplitRow;
	nActPane = rCpy.nActPane;
	aLastSel = rCpy.aLastSel;
	aDim = rCpy.aDim;
	bSelected = rCpy.bSelected;
	bFrozen = rCpy.bFrozen;
	bValidSel = rCpy.bValidSel;
	bValidDim = rCpy.bValidDim;
}




inline const ScExtTabOptions* ScExtDocOptions::GetExtTabOptions( const UINT16 nTab ) const
{
    return (nTab <= MAXTAB) ? ppExtTabOpts[ nTab ] : NULL;
}


inline ScExtTabOptions* ScExtDocOptions::GetExtTabOptions( const UINT16 nTab )
{
    return (nTab <= MAXTAB) ? ppExtTabOpts[ nTab ] : NULL;
}


inline const String* ScExtDocOptions::GetCodename( void ) const
{
	return pCodenameWB;
}


inline CodenameList* ScExtDocOptions::GetCodenames( void )
{
	return pCodenames;
}


} //namespace binfilter
#endif

