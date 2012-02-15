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



#ifndef _IPWIN_HXX
#define _IPWIN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#include <bf_so3/so2ref.hxx>
#include <bf_so3/iface.hxx>

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

namespace binfilter {

/********************** SvResizeHelper ***********************************
*************************************************************************/
class SO3_DLLPUBLIC SvResizeHelper
{
private:
	Size        aBorder;
	Rectangle   aOuter;
	short       nGrab; // -1 kein Grab,  0 - 7, 8 = Move, siehe FillHandle...
	Point       aSelPos;
	BOOL		bResizeable;
public:
				SvResizeHelper();

	void		SetResizeable( BOOL b ) { bResizeable = b; }
	short       GetGrab() const { return nGrab; }
	void        SetBorderPixel( const Size & rBorderP )
				{ aBorder = rBorderP; }
	const Size & GetBorderPixel() const { return aBorder; }
	const Rectangle & GetOuterRectPixel() const
				{ return aOuter; }
	void        SetOuterRectPixel( const Rectangle & rRect )
				{ aOuter = rRect; }
	Rectangle   GetInnerRectPixel() const
				{
					Rectangle aRect( aOuter );
					aRect.Top()    += aBorder.Height();
					aRect.Left()   += aBorder.Width();
					aRect.Bottom() -= aBorder.Height();
					aRect.Right()  -= aBorder.Width();
					return aRect;
				}
				// Im Uhrzeigersinn, beginnend bei Linksoben
	void        FillHandleRectsPixel( Rectangle aRects[ 8 ] ) const;
	void        FillMoveRectsPixel( Rectangle aRects[ 4 ] ) const;
	void        Draw( OutputDevice * );
	void        InvalidateBorder( Window * );
	BOOL        SelectBegin( Window *, const Point & rPos );
	short       SelectMove( Window * pWin, const Point & rPos );
	Point       GetTrackPosPixel( const Rectangle & rRect ) const;
	Rectangle	GetTrackRectPixel( const Point & rTrackPos ) const;
	void        ValidateRect( Rectangle & rValidate ) const;
	BOOL        SelectRelease( Window *, const Point & rPos, Rectangle & rOutPosSize );
	void        Release( Window * pWin );
};

/********************** SvResizeWindow ***********************************
*************************************************************************/
class SO3_DLLPUBLIC SvResizeWindow : public Window
{
private:
	Pointer         aOldPointer;
	short           nMoveGrab;  // Letzer Pointertyp
	SvBorder        aBorder;
	Point           aPosCorrection;
	Window *        pObjWin;
protected:
	SvResizeHelper  aResizer;
	SO3_DLLPRIVATE void            AdjustObjWin();
public:
			SvResizeWindow( Window * pParent );

	void    SetObjWin( Window * pNewWin );
	Window * GetObjWin() const
			{ return pObjWin; }

	void    SetHatchBorderPixel( const Size & rSize )
			{
				 aResizer.SetBorderPixel( rSize );
				 AdjustObjWin();
			}
	const Size & GetHatchBorderPixel() const
			{ return aResizer.GetBorderPixel(); }

	const SvBorder & GetBorderPixel() const
			{ return aBorder; }

	SvBorder GetAllBorderPixel() const
			{
				SvBorder aB( aBorder );
				return aB += GetHatchBorderPixel();
			}

	void    SetPosCorrectionPixel( const Point & rNewCorr )
			{ aPosCorrection = rNewCorr; }
	const Point & GetPosCorrectionPixel() const
			{ return aPosCorrection; }

	Rectangle CalcInnerRectPixel( const Point & rPos, const Size & rSize ) const;
	Rectangle GetInnerRectPixel() const;

	void    SetInnerPosSizePixel( const Point &, const Size & );

	void    TerminateResizing() { aResizer.Release( this ); }

	void    SelectMouse( const Point & rPos );
	void    MouseButtonUp( const MouseEvent & rEvt );
	void    MouseMove( const MouseEvent & rEvt );
	void    MouseButtonDown( const MouseEvent & rEvt );
	void    KeyInput( const KeyEvent & rEvt );
	void    Resize();
	void    Paint( const Rectangle & );
	virtual void	QueryObjAreaPixel( Rectangle & );
	virtual void    RequestObjAreaPixel( const Rectangle & );
};

/********************** SvInPlaceWindow **********************************
*************************************************************************/
class SvInPlaceEnvironment;
class SO3_DLLPUBLIC SvInPlaceWindow : public SvResizeWindow
{
private:
	SvInPlaceEnvironment *    pIPEnv;
public:
					SvInPlaceWindow( Window * pParent, SvInPlaceEnvironment * );
	void            KeyInput( const KeyEvent & rEvt );
	virtual void	QueryObjAreaPixel( Rectangle & );
	virtual void    RequestObjAreaPixel( const Rectangle & );
};

/********************** SvInPlaceClipWindow ******************************
*************************************************************************/
class SO3_DLLPUBLIC SvInPlaceClipWindow : public Window
{
private:
	SvResizeWindow * pResizeWin;
	Rectangle        aMaxClip;
public:
			SvInPlaceClipWindow( Window * pParent );

	void    SetResizeWin( SvResizeWindow * pNewWin )
			{ pResizeWin = pNewWin; }
	SvResizeWindow * GetResizeWin() const
			{ return pResizeWin; }

	void    SetRectsPixel( const Rectangle & rObjRect,
						   const Rectangle & rInPlaceWinMaxRect );
};

}

#endif // _IPWIN_HXX
