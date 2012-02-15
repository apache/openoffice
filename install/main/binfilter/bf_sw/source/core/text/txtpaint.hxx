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


#ifndef _TXTPAINT_HXX
#define _TXTPAINT_HXX

#ifndef _SV_OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif


class SwRect;				// SwSaveClip

#ifdef VERTICAL_LAYOUT
#endif
namespace binfilter {

/*************************************************************************
 *						class SwSaveClip
 *************************************************************************/

class SwSaveClip
{
	Region	 aClip;
	const sal_Bool	   bOn;
		  sal_Bool	   bChg;
protected:
	OutputDevice *pOut;
public:
	inline SwSaveClip( OutputDevice *pOut );
	inline ~SwSaveClip();
	void Reset(){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 		   void Reset();
	inline sal_Bool IsOn()	const { return bOn; }
	inline sal_Bool IsChg() const { return bChg; }
	inline sal_Bool IsOut() const { return 0 != pOut; }
	inline OutputDevice *GetOut() { return pOut; }
};

inline SwSaveClip::SwSaveClip( OutputDevice *pOut ) :
	pOut(pOut),
	bOn( pOut && pOut->IsClipRegion() ),
	bChg( sal_False )
{}

inline SwSaveClip::~SwSaveClip()
{
	Reset();
}

#ifdef DBG_UTIL

/*************************************************************************
 *							class SwDbgOut
 *************************************************************************/

class SwDbgOut
{
protected:
		OutputDevice *pOut;
public:
		inline SwDbgOut( OutputDevice *pOutDev, const sal_Bool bOn = sal_True );
};

/*************************************************************************
 *							class DbgPen
 *************************************************************************/

//class DbgPen : public SwDbgOut
//{
//		Pen aPen;
//public:
//		inline DbgPen( OutputDevice *pOutDev, const sal_Bool bOn = sal_True,
//					   const ColorName eColor = COL_BLACK );
//		inline ~DbgPen();
//};

/*************************************************************************
 *							class DbgColor
 *************************************************************************/

class DbgColor
{
	Font *pFnt;
	Color aColor;
public:
		inline DbgColor( Font *pFont, const sal_Bool bOn = sal_True,
						 const ColorData eColor = COL_BLUE );
		inline ~DbgColor();
};

/*************************************************************************
 *							class DbgBrush
 *************************************************************************/

class DbgBackColor : public SwDbgOut
{
		Color 	aOldFillColor;
public:
		DbgBackColor( OutputDevice *pOut, const sal_Bool bOn = sal_True,
				  ColorData nColor = COL_YELLOW );
	   ~DbgBackColor();
};

/*************************************************************************
 *							class DbgRect
 *************************************************************************/

class DbgRect : public SwDbgOut
{
public:
		DbgRect( OutputDevice *pOut, const Rectangle &rRect,
				 const sal_Bool bOn = sal_True,
				 ColorData eColor = COL_LIGHTBLUE );
};

/*************************************************************************
 *						Inline-Implementierung
 *************************************************************************/

inline SwDbgOut::SwDbgOut( OutputDevice *pOutDev, const sal_Bool bOn )
			   :pOut( bOn ? pOutDev : 0 )
{ }

//inline DbgPen::DbgPen( OutputDevice *pOutDev, const sal_Bool bOn,
//			   const ColorName eColor )
//	: SwDbgOut( pOutDev, bOn)
//{
//	if( pOut )
//	{
//		const Color aColor( eColor );
//		Pen aTmpPen( aColor );
//		aPen = pOut->GetPen( );
//		pOut->SetPen( aTmpPen );
//	}
//}

//inline DbgPen::~DbgPen()
//{
//	if( pOut )
//		pOut->SetPen(aPen);
//}

inline DbgColor::DbgColor( Font *pFont, const sal_Bool bOn,
				 const ColorData eColor )
	:pFnt( bOn ? pFont : 0 )
{
	if( pFnt )
	{
		aColor = pFnt->GetColor();
		pFnt->SetColor( Color( eColor ) );
	}
}

inline DbgColor::~DbgColor()
{
	if( pFnt )
		pFnt->SetColor( aColor );
}

inline DbgBackColor::DbgBackColor( OutputDevice *pOutDev, const sal_Bool bOn,
						   ColorData eColor )
	:SwDbgOut( pOutDev, bOn )
{
	if( pOut )
	{
		aOldFillColor = pOut->GetFillColor();
		pOut->SetFillColor( Color(eColor) );
	}
}

inline DbgBackColor::~DbgBackColor()
{
	if( pOut )
	{
		pOut->SetFillColor( aOldFillColor );
	}
}

inline DbgRect::DbgRect( OutputDevice *pOutDev, const Rectangle &rRect,
						 const sal_Bool bOn,
						 ColorData eColor )
	: SwDbgOut( pOutDev, bOn )
{
	if( pOut )
	{
		const Color aColor( eColor );
		Color aLineColor = pOut->GetLineColor();
		pOut->SetLineColor( aColor );
		Color aFillColor = pOut->GetFillColor();
		pOut->SetFillColor( Color(COL_TRANSPARENT) );
		pOut->DrawRect( rRect );
		pOut->SetLineColor( aLineColor );
		pOut->SetFillColor( aFillColor );
	}
}

#endif



} //namespace binfilter
#endif
