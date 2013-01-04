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



#ifndef SC_HDRCONT_HXX
#define SC_HDRCONT_HXX

#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SELENG_HXX //autogen
#include <vcl/seleng.hxx>
#endif
namespace binfilter {

// ---------------------------------------------------------------------------


#define HDR_HORIZONTAL		0
#define HDR_VERTICAL		1

#define HDR_SIZE_OPTIMUM	0xFFFF


									// Groesse des Sliders
#define HDR_SLIDERSIZE		2

class ScHeaderControl : public Window
{
private:
	SelectionEngine*	pSelEngine;
	Font				aNormFont;
	Font				aBoldFont;
	BOOL				bBoldSet;

	USHORT			nFlags;
	BOOL			bVertical;				// Vertikal = Zeilenheader

	long			nWidth;
	long			nSmallWidth;
	long			nBigWidth;

	USHORT			nSize;

	USHORT			nMarkStart;
	USHORT			nMarkEnd;
	BOOL			bMarkRange;

	BOOL			bDragging;				// Groessen aendern
	USHORT			nDragNo;
	long			nDragStart;
	long			nDragPos;
	BOOL			bDragMoved;

	BOOL			bIgnoreMove;

	long			GetScrPos( USHORT nEntryNo );


	void			DoPaint( USHORT nStart, USHORT nEnd );

protected:
					//	von Window ueberladen




					//	neue Methoden

	virtual USHORT	GetPos() = 0;								// aktuelle Position (Scrolling)
	virtual USHORT	GetEntrySize( USHORT nEntryNo ) = 0;		// Breite / Hoehe (Pixel)
	virtual String	GetEntryText( USHORT nEntryNo ) = 0;


	virtual void	SetEntrySize( USHORT nPos, USHORT nNewWidth ) = 0;
	virtual void	HideEntries( USHORT nStart, USHORT nEnd ) = 0;

	virtual void	SetMarking( BOOL bSet );


public:
			ScHeaderControl( Window* pParent, SelectionEngine* pSelectionEngine,
								USHORT nNewSize, USHORT nNewFlags );
			~ScHeaderControl();

	void	SetIgnoreMove(BOOL bSet)			{ bIgnoreMove = bSet; }

 	void	StopMarking();

	void	SetMark( BOOL bNewSet, USHORT nNewStart, USHORT nNewEnd );

	long	GetWidth() const					{ return nWidth; }
	long	GetSmallWidth() const				{ return nSmallWidth; }
	long	GetBigWidth() const					{ return nBigWidth; }
	void	SetWidth( long nNew );
};



} //namespace binfilter
#endif

