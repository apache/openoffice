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



#include <algorithm>

#include <stdlib.h>
//#include <math.h>
#include <vcl/virdev.hxx>
#include <vcl/metaact.hxx>

#ifndef _XDEF_HXX
#include <bf_svx/xdef.hxx>
#endif

#include "xoutx.hxx"
namespace binfilter {

// -----------
// - Statics -
// -----------

#ifdef MAC
	const BOOL XIMP_bDrawRasterOps = FALSE;
#else
	const BOOL XIMP_bDrawRasterOps = TRUE;
#endif

// -----------
// - Defines -
// -----------

#define FILL_TILE 200

// ----------------
// - XOuputDevice -
// ----------------

/*N*/ void XOutputDevice::DrawFillPolyPolygon( const PolyPolygon& rPolyPoly, BOOL bRect )
/*N*/ {
/*N*/ 	if( ( eFillStyle != XFILL_NONE ) && iRotTransGradient() &&
/*N*/ 		( eFillStyle != XFILL_SOLID || iRotTransGradient()->GetStartColor() != iRotTransGradient()->GetEndColor() ) )
/*N*/ 	{
/*?*/ 		XGradient&		rTransGradient = *iRotTransGradient();
/*?*/ 		Gradient		aVCLGradient;
/*?*/ 		GDIMetaFile		aMtf;
/*?*/ 		VirtualDevice	aVDev;
/*?*/ 		OutputDevice*	pOldOut = pOut;
/*?*/ 		const Rectangle	aBound( rPolyPoly.GetBoundRect() );
/*?*/ 		MapMode			aMap( pOldOut->GetMapMode() );
/*?*/ 
/*?*/ 		pOut = &aVDev;
/*?*/ 		aVDev.EnableOutput( FALSE );
/*?*/ 		aVDev.SetMapMode( pOldOut->GetMapMode() );
/*?*/ 		aMtf.Record( &aVDev );
/*?*/ 		aVDev.SetLineColor( pOldOut->GetLineColor() );
/*?*/ 		aVDev.SetFillColor( pOldOut->GetFillColor() );
/*?*/ 		aVDev.SetFont( pOldOut->GetFont() );
/*?*/ 		aVDev.SetDrawMode( pOldOut->GetDrawMode() );
/*?*/ 		aVDev.SetRefPoint( pOldOut->GetRefPoint() );
/*?*/ 		ImpDrawFillPolyPolygon( rPolyPoly, bRect, pOldOut->GetOutDevType() == OUTDEV_PRINTER );
/*?*/ 		aMtf.Stop();
/*?*/ 		aMtf.WindStart();
/*?*/ 		aMap.SetOrigin( aBound.TopLeft() );
/*?*/ 		aMtf.SetPrefMapMode( aMap );
/*?*/ 		aMtf.SetPrefSize( aBound.GetSize() );
/*?*/ 		pOut = pOldOut;
/*?*/ 
/*?*/ 		aVCLGradient.SetStyle((GradientStyle)rTransGradient.GetGradientStyle());
/*?*/ 		aVCLGradient.SetStartColor(rTransGradient.GetStartColor());
/*?*/ 		aVCLGradient.SetEndColor(rTransGradient.GetEndColor());
/*?*/ 		aVCLGradient.SetAngle((USHORT)rTransGradient.GetAngle());
/*?*/ 		aVCLGradient.SetBorder(rTransGradient.GetBorder());
/*?*/ 		aVCLGradient.SetOfsX(rTransGradient.GetXOffset());
/*?*/ 		aVCLGradient.SetOfsY(rTransGradient.GetYOffset());
/*?*/ 		aVCLGradient.SetStartIntensity(rTransGradient.GetStartIntens());
/*?*/ 		aVCLGradient.SetEndIntensity(rTransGradient.GetEndIntens());
/*?*/ 		aVCLGradient.SetSteps(rTransGradient.GetSteps());
/*?*/ 
/*?*/ 		pOut->DrawTransparent( aMtf, aBound.TopLeft(), aBound.GetSize(), aVCLGradient );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		ImpDrawFillPolyPolygon( rPolyPoly, bRect, pOut->GetOutDevType() == OUTDEV_PRINTER );
/*N*/ }

// ------------------------------------------------------------------------

/*N*/ void XOutputDevice::ImpDrawFillPolyPolygon( const PolyPolygon& rPolyPoly, BOOL bRect, BOOL bPrinter )
/*N*/ {
/*N*/ 	if( eFillStyle != XFILL_NONE )
/*N*/ 	{
/*?*/ 		const Color aOldLineColor( pOut->GetLineColor() );
/*?*/ 		const ULONG	nDrawMode = pOut->GetDrawMode();
/*?*/ 
/*?*/ 		pOut->SetLineColor();
/*?*/ 
/*?*/ 		if( eFillStyle == XFILL_SOLID )
/*?*/ 		{
/*?*/ 			if( nFillTransparence )
/*?*/ 				pOut->DrawTransparent( rPolyPoly, nFillTransparence );
/*?*/ 			else
/*?*/ 				pOut->DrawPolyPolygon( rPolyPoly );
/*?*/ 		}
/*?*/ 		else if( eFillStyle == XFILL_HATCH )
/*?*/ 		{
/*?*/ 			long		nAngle10 = aHatch.GetAngle() % 3600;
/*?*/ 			HatchStyle	eStyle;
/*?*/ 
/*?*/ 			if( nAngle10 < 0 )
/*?*/ 				nAngle10 += 3600;
/*?*/ 
/*?*/ 			switch( aHatch.GetHatchStyle() )
/*?*/ 			{
/*?*/ 				case( XHATCH_TRIPLE ): eStyle = HATCH_TRIPLE; break;
/*?*/ 				case( XHATCH_DOUBLE ): eStyle = HATCH_DOUBLE; break;
/*?*/ 
/*?*/ 				default:
/*?*/ 					eStyle = HATCH_SINGLE;
/*?*/ 				break;
/*?*/ 			}
/*?*/ 
/*?*/ 			if( iSolidHatch() )
/*?*/ 				pOut->DrawPolyPolygon( rPolyPoly );
/*?*/ 
/*?*/ 			pOut->DrawHatch( rPolyPoly, Hatch( eStyle, aHatch.GetColor(), aHatch.GetDistance(), (USHORT) nAngle10 ) );
/*?*/ 		}
/*?*/ 		else if( eFillStyle == XFILL_GRADIENT )
/*?*/ 		{
/*?*/ 			GDIMetaFile*	pMtf = pOut->GetConnectMetaFile();
/*?*/ 			Gradient		aVCLGradient;
/*?*/ 
/*?*/ 
/*?*/ 			aVCLGradient.SetStyle((GradientStyle)aGradient.GetGradientStyle());
/*?*/ 			aVCLGradient.SetStartColor(aGradient.GetStartColor());
/*?*/ 			aVCLGradient.SetEndColor(aGradient.GetEndColor());
/*?*/ 			aVCLGradient.SetAngle((USHORT)aGradient.GetAngle());
/*?*/ 			aVCLGradient.SetBorder(aGradient.GetBorder());
/*?*/ 			aVCLGradient.SetOfsX(aGradient.GetXOffset());
/*?*/ 			aVCLGradient.SetOfsY(aGradient.GetYOffset());
/*?*/ 			aVCLGradient.SetStartIntensity(aGradient.GetStartIntens());
/*?*/ 			aVCLGradient.SetEndIntensity(aGradient.GetEndIntens());
/*?*/ 			aVCLGradient.SetSteps(aGradient.GetSteps());
/*?*/ 
/*?*/ 			if( bRect )
/*?*/ 				pOut->DrawGradient( rPolyPoly.GetBoundRect(), aVCLGradient );
/*?*/ 			else
/*?*/ 				pOut->DrawGradient( rPolyPoly, aVCLGradient );
/*?*/ 		}
/*?*/ 		else if( eFillStyle == XFILL_BITMAP )
/*?*/ 		{
/*?*/ 			if( nDrawMode & DRAWMODE_WHITEFILL )
/*?*/ 			{
/*?*/ 				const Color aOldFillColor( pOut->GetFillColor() );
/*?*/ 
/*?*/ 				pOut->SetFillColor( COL_WHITE );
/*?*/ 				pOut->DrawPolyPolygon( rPolyPoly );
/*?*/ 				pOut->SetFillColor( aOldFillColor );
/*?*/ 			}
/*?*/ 			else
/*?*/ 			{
/*?*/ 				Rectangle       aPolyRect( rPolyPoly.GetBoundRect() );
/*?*/ 				GDIMetaFile*    pMtf = pOut->GetConnectMetaFile();
/*?*/ 
/*?*/ 				pOut->Push();
/*?*/ 				pOut->SetRasterOp( ROP_OVERPAINT );
/*?*/ 
/*?*/ 				if( pMtf && !bPrinter )
/*?*/ 				{
/*?*/ 					if( !pOut->GetPDFWriter() && XIMP_bDrawRasterOps )
/*?*/ 					{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 					}
/*?*/ 					else
/*?*/ 					{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 					}
/*?*/ 				}
/*?*/ 				else if( bRect || bPrinter )
/*?*/ 				{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 				}
/*?*/ 				else
/*?*/ 				{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ 				}
/*?*/ 
/*?*/ 				pOut->Pop();
/*?*/ 			}
/*?*/ 		}
/*?*/ 
/*?*/ 		pOut->SetLineColor( aOldLineColor );
/*N*/ 	}
/*N*/ }

// ------------------------------------------------------------------------


// ------------------------------------------------------------------------


// ------------------------------------------------------------------------

} //namespace binfilter
