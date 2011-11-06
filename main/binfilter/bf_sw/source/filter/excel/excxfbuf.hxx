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


#ifndef _EXCXFBUF_HXX
#define _EXCXFBUF_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
namespace binfilter {
class SfxItemSet; 

class SvxBorderLine;



enum ExcHorizAlign { EHA_General = 0, EHA_Left = 1, EHA_Center = 2,
	EHA_Right = 3, EHA_Fill = 4, EHA_Justify = 5, EHA_CentAcrSel = 6,
	EHA_Parent };

enum ExcVertAlign { EVA_Top = 0, EVA_Center = 1, EVA_Bottom = 2,
	EVA_Justify = 3, EVA_Parent };

enum ExcTextOrient { ETO_NoRot = 0, ETO_TopBottom = 1, ETO_90ccw = 2,
	ETO_90cw = 3, ETO_Parent };

enum ExcWrapText { EWT_NoWrap = 0, EWT_Wrap = 1, EWT_Parent };

struct CellBorder
	{
	BYTE nTopLine;
	BYTE nLeftLine;
	BYTE nBottomLine;
	BYTE nRightLine;
	USHORT nTopColor;
	USHORT nLeftColor;
	USHORT nBottomColor;
	USHORT nRightColor;
	};

struct CellFill
	{
	BYTE nPattern;
	USHORT nForeColor;
	USHORT nBackColor;
	};

// -----------------------------------------------------------------------
class XF_Data
	{
	private:
		USHORT			nFont;			// Index auf zugehoerigen Font
		ULONG			nValForm;		// Id fuer Value-Format
		ExcWrapText		eWrap;
		ExcHorizAlign	eHoriz;
		USHORT			nParent;		// Index zum Parent Style-XF
		CellFill		*pFill;
		CellBorder		*pBord;
		BOOL			bCellXF : 1;	// Cell- oder Style-XF
		BOOL			bFontValid : 1;	// nFont <> Font vom Parent?
		BOOL			bFormValid : 1;
	public:
						XF_Data();

						~XF_Data();

		BOOL			IsCellXF() const { return bCellXF; }
		void			SetCellXF() { bCellXF = TRUE; }
		void			SetCellXF( BOOL bNewCellXF ) { bCellXF = bNewCellXF; }
		void			SetStyleXF() { bCellXF = FALSE; }

		BOOL			HasFont() const { return bFontValid; }
		USHORT			Font() const { return nFont; }
		void			SetFont( USHORT nNew )
							{ nFont = nNew; bFontValid = TRUE; }

		BOOL			HasValueFormat() const { return bFormValid; }
		ULONG			ValueFormat() const { return nValForm; }
		void			SetValueFormat( ULONG nNew )
							{ nValForm=nNew; bFormValid=TRUE; }

		BOOL			HasWrap() const { return ( eWrap != EWT_Parent ); }
		void			SetWrap() { eWrap = EWT_Wrap; }
		void			SetWrap( ExcWrapText eNew ) { eWrap = eNew; }
		ExcWrapText		Wrap() const { return eWrap; }

		BOOL			HasHorizAlign() const
							{ return ( eHoriz != EHA_Parent ); }
		void			SetAlign( ExcHorizAlign eNew ) { eHoriz = eNew; }
		ExcHorizAlign	HorizAlign() const { return eHoriz; }

		void			SetParent( USHORT nNew ) { nParent = nNew; }
		USHORT			Parent() const { return nParent; }

		BOOL			HasFill() const { return ( pFill != NULL ); }
		inline void		SetFill( CellFill *pNew );
		inline void		SetFill( const CellFill &rNew );
		inline void		SetFill( BYTE nPattern, USHORT nForeColor,
							USHORT nBackColor );
		const CellFill	*Fill() const { return pFill; }

		BOOL			HasBorder() const { return ( pBord != NULL ); }

		inline void		SetBorder( CellBorder *pNew );

		void			SetBorder( const CellBorder &rNew );
#if 0
		void			SetBorder( BYTE nTopLine, BYTE nLeftLine,
							BYTE nBottomLine, BYTE nRightLine,
							USHORT nTopColor, USHORT nLeftColor,
							USHORT nBottomColor, USHORT nRightColor );
#endif
		const			CellBorder *Border() const { return pBord; }
	};

inline void XF_Data::SetFill( CellFill *pNew )
	{
	// PREC: Rufender newed pNew, deleted wird in XF_Data!
	//			!Es wird keine Kopie angelegt!
	if( pFill != NULL ) delete pFill;
	pFill = pNew;
	}

inline void XF_Data::SetFill( const CellFill &rNew )
	{
	if( pFill == NULL ) pFill = new CellFill;
	DBG_ASSERT( pFill != NULL,
		"-XF_Data::SetFill(): pFill == NULL!" );
	pFill->nPattern = rNew.nPattern;
	pFill->nForeColor = rNew.nForeColor;
	pFill->nBackColor = rNew.nBackColor;
	}

inline void XF_Data::SetFill( BYTE nPattern, USHORT nForeColor,
	USHORT nBackColor )
	{
	if( pFill == NULL ) pFill = new CellFill;
	DBG_ASSERT( pFill != NULL,
		"-XF_Data::SetFill(): pFill == NULL!" );
	pFill->nPattern = nPattern;
	pFill->nForeColor = nForeColor;
	pFill->nBackColor = nBackColor;
	}
inline void XF_Data::SetBorder( CellBorder *pNew )
	{
	// PREC: Rufender newed pNew, deleted wird in XF_Data!
	//			!Es wird keine Kopie angelegt!
	if( pBord != NULL ) delete pBord;
	pBord = pNew;
	}


// -----------------------------------------------------------------------
class XF_Buffer
	{
	private:
		SfxItemSet		**ppTxtAttr;	// Array mit Text-Attributes
		SfxItemSet		**ppBoxAttr;	// Array mit Box-Attributes
		XF_Data			**ppData;		// Array mit XF-Daten
		USHORT nMax;					// Groesse des Arrays
		USHORT nCount;					// akt. Speichermarke im pPattern-Array
		SfxItemSet		*pDefTxtAttr;	// Array mit Text-Attributes
		SfxItemSet		*pDefBoxAttr;	// Array mit Box-Attributes
		XF_Data			*pDefaultData;	// im Fehlerfall als Daten
		//----------------------------------------------------------------
		void			ExcToSwBorderLine( BYTE nLine, USHORT nColor,
											SvxBorderLine *&rpBorderLine );
		void			CellXF( XF_Data &rD );
		void			StyleXF( XF_Data &rD );
		void			CreateItemSets( USHORT nIndex );
	public:
						XF_Buffer( USHORT nNewMax = 2048 );
						~XF_Buffer();

		inline void			NewXF( XF_Data *pD );

		inline void		GetItemSets( USHORT nIndex, const SfxItemSet *pTxtAttr,
							const SfxItemSet *pBoxAttr );

		sal_uInt32		GetNumFormat( USHORT nIndex );
		void			SetItemSets( USHORT nCol, USHORT nSR, USHORT nER,
							USHORT nXF );
#ifdef USED
		void			Reset();
#endif
	};

inline void XF_Buffer::NewXF( XF_Data *pD )
	{
	// PREC: pD muss in rufender Routine ge-'new'-t werden,
	//			delete im ~XF_Buffer
	DBG_ASSERT( nCount < nMax,
		"+XF_Buffer::NewXF(): Puffer fuer XFs voll!" );
	if( nCount < nMax )
		ppData[ nCount++ ] = pD;
	}

inline void XF_Buffer::GetItemSets( USHORT nIndex, const SfxItemSet *pTxtAttr,
					const SfxItemSet *pBoxAttr )
	{
	DBG_ASSERT( nIndex < nCount ,
		"+XF_Buffer::GetItemSets(): das ist zuviel des Guten!" );
	if( nIndex >= nCount )
		{
		pTxtAttr = pDefTxtAttr;			// nicht im Puffer
		pBoxAttr = pDefBoxAttr;
		}
	else
		{
		if( ppTxtAttr[ nIndex ] == NULL )
			CreateItemSets( nIndex );		// erste Nutzung

		pTxtAttr = ppTxtAttr[ nIndex ];
		pBoxAttr = ppBoxAttr[ nIndex ];
		}
	}



} //namespace binfilter
#endif

