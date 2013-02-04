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



#ifndef _SV_SECTCTR_HXX
#define _SV_SECTCTR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_SV_H
#include <vcl/sv.h>
#endif
#ifndef _SV_CTRL_HXX
#include <vcl/ctrl.hxx>
#endif

class ScrollBar;
class ScrollBarBox;	
namespace binfilter
{

class SvSection;
class ImplSplitWindow;

#define SECTION_APPEND		((USHORT)0xFFFF)
#define SECTION_NOTFOUND	((USHORT)0xFFFF)

#define WB_SECTION_STYLE    WB_VSCROLL | WB_HSCROLL	| WB_TABSTOP

class SvSectionControl : public Control
{
private:

	Window				aSplitWinContainer;
	ImplSplitWindow*	pSplitWin;
	ScrollBar*			pVScrollBar;
	ScrollBar*			pHScrollBar;
	ScrollBarBox*		pScrollBarBox;
	DockingWindow*		pDummy;
	long				nRealHeight;
	long				nMaxHeight;
	long				nMinWidth;
	Wallpaper			aWallpaper;

	DECL_LINK( ScrollHdl, ScrollBar* );
	DECL_LINK( EndScrollHdl, ScrollBar* );


protected:

	virtual void		Resize();
	virtual void		Paint( const Rectangle& rRect );
	virtual void		StateChanged( StateChangedType nStateChange );
	virtual void		DataChanged( const DataChangedEvent& rDCEvt );
	virtual void		Command( const CommandEvent& rCEvt );
	virtual long		PreNotify( NotifyEvent& rNEvt );
	virtual long		Notify( NotifyEvent& rNEvt );
	virtual long		KeyEventNotify( const KeyEvent& rKEvt );
    using Control::SetPosSizePixel;
	virtual void		SetPosSizePixel( long nX, long nY,long nWidth, long nHeight,USHORT nFlags);



	long				CalcMaxHeight();
	long				CalcRealHeight();
	long				CalcSectionWidth();

	void				SetScrollBars(BOOL bVert,BOOL bHorz);
	void				ShowScrollBarBox();
	void				UpdateScrollBars();
		
	BOOL				VScrollResize(Size &aSize);
	BOOL				HScrollResize(Size &aSize);
	void				SetChildPos(long nPos, BOOL bScrolling = TRUE);
	
public:
						SvSectionControl( Window* pParent,WinBits nStyle = WB_SECTION_STYLE);
						SvSectionControl( Window* pParent, const ResId& rResId );
						~SvSectionControl();


	void				InsertSection(	USHORT nSectionId,SvSection* pSection,long nSize,USHORT nPos);
	void				InsertSection(	USHORT nSectionId,SvSection* pSection,USHORT nPos);
	void				RemoveSection( USHORT nSectionId );
	void				Clear();

	USHORT				GetSectionCount() const;
	USHORT				GetSectionId( USHORT nPos ) const;
	USHORT				GetSectionPos( USHORT nSectionId ) const;
	USHORT				GetSectionId( const Point& rPos ) const;

	void				SetSectionSize( USHORT nId, long nNewSize );
	long				GetSectionSize( USHORT nId ) const;

	/*
	void				SetCurSectionId( USHORT nSectionId );
	USHORT				GetCurSectionId() const;

	void				SetFirstSectionId( USHORT nSectionId );
	USHORT				GetFirstSectionId() const { return GetSectionId( mnFirstSectionPos ); }

	void				MakeVisible( USHORT nSectionId );
	*/

	void				SetSectionWidth( USHORT nSectionId, long nWidth);
	long				GetSectionWidth( USHORT nSectionId ) const;

	void				SetSection( USHORT nSectionId, SvSection* pPage );
	SvSection*			GetSection( USHORT nSectionId ) const;

	void				SetSectionText( USHORT nSectionId, const XubString& rText );
	XubString			GetSectionText( USHORT nSectionId ) const;

	void				SetHelpText( USHORT nSectionId, const XubString& rText );
	const XubString& 	GetHelpText( USHORT nSectionId ) const;

	void				SetHelpId( USHORT nSectionId, ULONG nHelpId );
	ULONG				GetHelpId( USHORT nSectionId ) const;

	void				SetHelpText( const XubString& rText )
							{ Control::SetHelpText( rText ); }
	const XubString& 	GetHelpText() const
							{ return Control::GetHelpText(); }

	void				SetHelpId( ULONG nId )
							{ Control::SetHelpId( nId ); }
	ULONG				GetHelpId() const
							{ return Control::GetHelpId(); }

	using Control::SetBackground;
	void				SetBackground( const Wallpaper& rBackground ){aWallpaper=rBackground; }
	const Wallpaper&	GetBackground() const { return aWallpaper; }
	
};
}


#endif	
