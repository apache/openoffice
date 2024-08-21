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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#include "winmtf.hxx"
#include <osl/endian.h>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <vcl/dibtools.hxx>

//=========================== GDI-Array ===================================

#define EMR_HEADER                      1
#define EMR_POLYBEZIER                  2
#define EMR_POLYGON                     3
#define EMR_POLYLINE                    4
#define EMR_POLYBEZIERTO                5
#define EMR_POLYLINETO                  6
#define EMR_POLYPOLYLINE                7
#define EMR_POLYPOLYGON                 8
#define EMR_SETWINDOWEXTEX              9
#define EMR_SETWINDOWORGEX              10
#define EMR_SETVIEWPORTEXTEX            11
#define EMR_SETVIEWPORTORGEX            12
#define EMR_SETBRUSHORGEX               13
#define EMR_EOF                         14
#define EMR_SETPIXELV                   15
#define EMR_SETMAPPERFLAGS              16
#define EMR_SETMAPMODE                  17
#define EMR_SETBKMODE                   18
#define EMR_SETPOLYFILLMODE             19
#define EMR_SETROP2                     20
#define EMR_SETSTRETCHBLTMODE           21
#define EMR_SETTEXTALIGN                22
#define EMR_SETCOLORADJUSTMENT          23
#define EMR_SETTEXTCOLOR                24
#define EMR_SETBKCOLOR                  25
#define EMR_OFFSETCLIPRGN               26
#define EMR_MOVETOEX                    27
#define EMR_SETMETARGN                  28
#define EMR_EXCLUDECLIPRECT             29
#define EMR_INTERSECTCLIPRECT           30
#define EMR_SCALEVIEWPORTEXTEX          31
#define EMR_SCALEWINDOWEXTEX            32
#define EMR_SAVEDC                      33
#define EMR_RESTOREDC                   34
#define EMR_SETWORLDTRANSFORM           35
#define EMR_MODIFYWORLDTRANSFORM        36
#define EMR_SELECTOBJECT                37
#define EMR_CREATEPEN                   38
#define EMR_CREATEBRUSHINDIRECT         39
#define EMR_DELETEOBJECT                40
#define EMR_ANGLEARC                    41
#define EMR_ELLIPSE                     42
#define EMR_RECTANGLE                   43
#define EMR_ROUNDRECT                   44
#define EMR_ARC                         45
#define EMR_CHORD                       46
#define EMR_PIE                         47
#define EMR_SELECTPALETTE               48
#define EMR_CREATEPALETTE               49
#define EMR_SETPALETTEENTRIES           50
#define EMR_RESIZEPALETTE               51
#define EMR_REALIZEPALETTE              52
#define EMR_EXTFLOODFILL                53
#define EMR_LINETO                      54
#define EMR_ARCTO                       55
#define EMR_POLYDRAW                    56
#define EMR_SETARCDIRECTION             57
#define EMR_SETMITERLIMIT               58
#define EMR_BEGINPATH                   59
#define EMR_ENDPATH                     60
#define EMR_CLOSEFIGURE                 61
#define EMR_FILLPATH                    62
#define EMR_STROKEANDFILLPATH           63
#define EMR_STROKEPATH                  64
#define EMR_FLATTENPATH                 65
#define EMR_WIDENPATH                   66
#define EMR_SELECTCLIPPATH              67
#define EMR_ABORTPATH                   68

#define EMR_GDICOMMENT                  70
#define EMR_FILLRGN                     71
#define EMR_FRAMERGN                    72
#define EMR_INVERTRGN                   73
#define EMR_PAINTRGN                    74
#define EMR_EXTSELECTCLIPRGN            75
#define EMR_BITBLT                      76
#define EMR_STRETCHBLT                  77
#define EMR_MASKBLT                     78
#define EMR_PLGBLT                      79
#define EMR_SETDIBITSTODEVICE           80
#define EMR_STRETCHDIBITS               81
#define EMR_EXTCREATEFONTINDIRECTW      82
#define EMR_EXTTEXTOUTA                 83
#define EMR_EXTTEXTOUTW                 84
#define EMR_POLYBEZIER16                85
#define EMR_POLYGON16                   86
#define EMR_POLYLINE16                  87
#define EMR_POLYBEZIERTO16              88
#define EMR_POLYLINETO16                89
#define EMR_POLYPOLYLINE16              90
#define EMR_POLYPOLYGON16               91
#define EMR_POLYDRAW16                  92
#define EMR_CREATEMONOBRUSH             93
#define EMR_CREATEDIBPATTERNBRUSHPT     94
#define EMR_EXTCREATEPEN                95
#define EMR_POLYTEXTOUTA                96
#define EMR_POLYTEXTOUTW                97

// WINDOWS VERSION >= 0x400
#define EMR_SETICMMODE                  98
#define EMR_CREATECOLORSPACE            99
#define EMR_SETCOLORSPACE              100
#define EMR_DELETECOLORSPACE           101
#define EMR_GLSRECORD                  102
#define EMR_GLSBOUNDEDRECORD	       103
#define EMR_PIXELFORMAT				   104

// WINDOWS VERSION >= 0x500
#define EMR_DRAWESCAPE                 105
#define EMR_EXTESCAPE                  106
#define EMR_STARTDOC                   107
#define EMR_SMALLTEXTOUT               108
#define EMR_FORCEUFIMAPPING            109
#define EMR_NAMEDESCAPE                110
#define EMR_COLORCORRECTPALETTE        111
#define EMR_SETICMPROFILEA             112
#define EMR_SETICMPROFILEW             113
#define EMR_ALPHABLEND                 114
#define EMR_ALPHADIBBLEND              115
#define EMR_TRANSPARENTBLT             116
#define EMR_TRANSPARENTDIB             117
#define EMR_GRADIENTFILL               118
#define EMR_SETLINKEDUFIS              119
#define EMR_SETTEXTJUSTIFICATION       120


//-----------------------------------------------------------------------------------

#ifdef OSL_BIGENDIAN
// currently unused
static float GetSwapFloat( SvStream& rSt )
{
	float	fTmp;
	sal_Int8* pPtr = (sal_Int8*)&fTmp;
	rSt >> pPtr[3] >> pPtr[2] >> pPtr[1] >> pPtr[0];	// Little Endian <-> Big Endian switch
	return fTmp;
}
#endif

struct BLENDFUNCTION{
	unsigned char aBlendOperation;
	unsigned char aBlendFlags;
	unsigned char aSrcConstantAlpha;
	unsigned char aAlphaFormat;

	friend SvStream& operator>>( SvStream& rIn, BLENDFUNCTION& rBlendFun );
};

SvStream& operator>>( SvStream& rIn, BLENDFUNCTION& rBlendFun )
{
	rIn >> rBlendFun.aBlendOperation >> rBlendFun.aBlendFlags >>
		     rBlendFun.aSrcConstantAlpha >> rBlendFun.aAlphaFormat;
    return rIn;
}
SvStream& operator>>( SvStream& rIn, XForm& rXForm )
{
	if ( sizeof( float ) != 4 )
	{
        DBG_ERROR( "EnhWMFReader::sizeof( float ) != 4" );
		rXForm = XForm();
	}
	else
	{
#ifdef OSL_BIGENDIAN
	rXForm.eM11 = GetSwapFloat( rIn );
	rXForm.eM12 = GetSwapFloat( rIn );
	rXForm.eM21 = GetSwapFloat( rIn );
	rXForm.eM22 = GetSwapFloat( rIn );
	rXForm.eDx = GetSwapFloat( rIn );
	rXForm.eDy = GetSwapFloat( rIn );
#else
	rIn >> rXForm.eM11 >> rXForm.eM12 >> rXForm.eM21 >> rXForm.eM22
			>> rXForm.eDx >> rXForm.eDy;
#endif
	}
    return rIn;
}

static sal_Bool ImplReadRegion( PolyPolygon& rPolyPoly, SvStream& rSt, sal_uInt32 nLen )
{
    sal_Bool bOk = sal_False;
    if ( nLen )
    {
        sal_uInt32 nHdSize, nType, nCount, nRgnSize, i;
        rSt >> nHdSize
            >> nType
            >> nCount
            >> nRgnSize;

        if ( nCount && ( nType == RDH_RECTANGLES ) &&
                ( nLen >= ( ( nCount << 4 ) + ( nHdSize - 16 ) ) ) )
        {
            sal_Int32 nx1, ny1, nx2, ny2;

            for ( i = 0; i < nCount; i++ )
            {
				rSt >> nx1 >> ny1 >> nx2 >> ny2;

                Rectangle aRect( Point( nx1, ny1 ), Point( nx2, ny2 ) );
        		Polygon aPolygon( aRect );
        		PolyPolygon aPolyPolyOr1( aPolygon );
		        PolyPolygon aPolyPolyOr2( rPolyPoly );
		        rPolyPoly.GetUnion( aPolyPolyOr1, aPolyPolyOr2 );
		        rPolyPoly = aPolyPolyOr2;
            }
            bOk = sal_True;
        }
    }
    return bOk;
}

sal_Bool EnhWMFReader::ReadEnhWMF()
{
	sal_uInt32  nStretchBltMode = 0;
	sal_uInt32  nRecType, nRecSize, nNextPos,
	            nW, nH, nPoints, nColor, nIndex,
	            nDat32, nNom1, nDen1, nNom2, nDen2;
	sal_Int32   nX32, nY32, nx32, ny32;
	sal_Int16   nX16, nY16;

	sal_Bool	bFlag, bStatus = ReadHeader();

	while( bStatus && nRecordCount-- )
	{
		*pWMF >> nRecType >> nRecSize;

		if ( ( nRecSize < 8 ) || ( nRecSize & 3 ) )		// Parameter sind immer durch 4 teilbar
		{
			bStatus = sal_False;
			break;
		}

		nNextPos = pWMF->Tell() + ( nRecSize - 8 );

		if ( nNextPos > nEndPos )
		{
			bStatus = sal_False;
			break;
		}

		if( aBmpSaveList.Count() && ( nRecType != EMR_STRETCHBLT ) && ( nRecType != EMR_STRETCHDIBITS ) )
				pOut->ResolveBitmapActions( aBmpSaveList );

		bFlag = sal_False;

		switch( nRecType )
		{
			case EMR_POLYBEZIERTO :
				bFlag = sal_True;
			case EMR_POLYBEZIER :
			{
				pWMF->SeekRel( 16 );
				*pWMF >> nPoints;
				sal_uInt16 i = 0;
				if ( bFlag )
				{
					i++;
					nPoints++;
				}
				Polygon aPoly( (sal_uInt16)nPoints );
				for( ; i < (sal_uInt16)nPoints; i++ )
				{
					*pWMF >> nX32 >> nY32;
					aPoly[ i ] = Point( nX32, nY32 );
				}
				pOut->DrawPolyBezier( aPoly, bFlag, bRecordPath );
			}
			break;

			case EMR_POLYGON :
			{
				pWMF->SeekRel( 16 );
				*pWMF >> nPoints;
				Polygon aPoly( (sal_uInt16)nPoints );
				for( sal_uInt16 k = 0; k < (sal_uInt16)nPoints; k++ )
				{
					*pWMF >> nX32 >> nY32;
					aPoly[ k ] = Point( nX32, nY32 );
				}
				pOut->DrawPolygon( aPoly, bRecordPath );
			}
			break;

			case EMR_POLYLINETO :
				bFlag = sal_True;
			case EMR_POLYLINE :
			{
				pWMF->SeekRel( 0x10 );
				*pWMF >> nPoints;
				sal_uInt16 i = 0;
				if ( bFlag )
				{
					i++;
					nPoints++;
				}
				Polygon aPolygon( (sal_uInt16)nPoints );
				for ( ; i < (sal_uInt16)nPoints; i++ )
				{
					*pWMF >> nX32 >> nY32;
					aPolygon[ i ] = Point( nX32, nY32 );
				}
				pOut->DrawPolyLine( aPolygon, bFlag, bRecordPath );
			}
			break;

			case EMR_POLYPOLYLINE :
			{
				sal_Int32	i, nPoly;
				pWMF->SeekRel( 0x10 );

				// Anzahl der Polygone:
				*pWMF >> nPoly >> i;

				// taking the amount of points of each polygon, retrieving the total number of points
				if ( static_cast< sal_uInt32 >(nPoly) < SAL_MAX_UINT32 / sizeof(sal_uInt16) )
				{
					if ( ( static_cast< sal_uInt32 >( nPoly ) * sizeof(sal_uInt16) ) <= ( nEndPos - pWMF->Tell() ) )
					{
						sal_uInt16*	pnPoints = new sal_uInt16[ nPoly ];

						for ( i = 0; i < nPoly && !pWMF->IsEof(); i++ )
						{
							*pWMF >> nPoints;
							pnPoints[ i ] = (sal_uInt16)nPoints;
						}

						// Polygonpunkte holen:

						for ( i = 0; ( i < nPoly ) && !pWMF->IsEof(); i++ )
						{
							Polygon aPoly( pnPoints[ i ] );
							for( sal_uInt16 k = 0; k < pnPoints[ i ]; k++ )
							{
								*pWMF >> nX32 >> nY32;
								aPoly[ k ] = Point( nX32, nY32 );
							}
							pOut->DrawPolyLine( aPoly, sal_False, bRecordPath );
						}
						delete[] pnPoints;
					}
				}
			}
			break;

			case EMR_POLYPOLYGON :
			{
				sal_uInt32 nPoly(0);
				sal_uInt32 nGesPoints(0);
                sal_uInt32 nReadPoints(0);
				pWMF->SeekRel( 0x10 );

				// Anzahl der Polygone:
				*pWMF >> nPoly >> nGesPoints;

				if ( ( nGesPoints < SAL_MAX_UINT32 / sizeof(Point) ) && ( nPoly < SAL_MAX_UINT32 / sizeof(sal_uInt16) )  && !pWMF->IsEof() )
				{
					if ( ( nPoly * sizeof(sal_uInt16) ) <= ( nEndPos - pWMF->Tell() ) )
					{
        				sal_uInt32 i(0);
						sal_uInt16*	pnPoints = new sal_uInt16[ nPoly ];

						for ( i = 0; i < nPoly && !pWMF->IsEof(); i++ )
						{
							*pWMF >> nPoints;
							pnPoints[ i ] = (sal_uInt16)nPoints;
						}

						if ( ( nGesPoints * (sizeof(sal_uInt32)+sizeof(sal_uInt32)) ) <= ( nEndPos - pWMF->Tell() ) && !pWMF->IsEof())
						{
                            PolyPolygon aPolyPoly(nPoly, nPoly);

						    for ( i = 0; i < nPoly && !pWMF->IsEof(); i++ )
						    {
                                const sal_uInt16 nPointCount(pnPoints[i]);
                				Point* pPtAry = new Point[nPointCount];

                                for(sal_uInt16 j(0); j < nPointCount && !pWMF->IsEof(); j++)
                                {
								    *pWMF >> nX32 >> nY32;
								    pPtAry[ j ] = Point( nX32, nY32 );
                                    nReadPoints++;
                                }

                                aPolyPoly.Insert(Polygon(nPointCount, pPtAry));
                                delete[] pPtAry;
                            }

                            pOut->DrawPolyPolygon( aPolyPoly, bRecordPath );
						}

                        delete[] pnPoints;
					}
				}

                OSL_ENSURE(nReadPoints == nGesPoints, "The number Points processed from EMR_POLYPOLYGON is unequal imported number (!)");
			}
			break;

			case EMR_SETWINDOWEXTEX :
			{														// #75383#
				*pWMF >> nW >> nH;
				pOut->SetWinExt( Size( nW, nH ) );
			}
			break;

			case EMR_SETWINDOWORGEX :
			{
				*pWMF >> nX32 >> nY32;
				pOut->SetWinOrg( Point( nX32, nY32 ) );
			}
			break;

			case EMR_SCALEWINDOWEXTEX :
			{
				*pWMF >> nNom1 >> nDen1 >> nNom2 >> nDen2;
				pOut->ScaleWinExt( (double)nNom1 / nDen1, (double)nNom2 / nDen2 );
			}
			break;

			case EMR_SETVIEWPORTORGEX :
			{
				*pWMF >> nX32 >> nY32;
				pOut->SetDevOrg( Point( nX32, nY32 ) );
			}
			break;

			case EMR_SCALEVIEWPORTEXTEX :
			{
				*pWMF >> nNom1 >> nDen1 >> nNom2 >> nDen2;
				pOut->ScaleDevExt( (double)nNom1 / nDen1, (double)nNom2 / nDen2 );
			}
			break;

			case EMR_SETVIEWPORTEXTEX :
			{
				*pWMF >> nW >> nH;
				pOut->SetDevExt( Size( nW, nH ) );
			}
			break;

			case EMR_EOF :
				nRecordCount = 0;			// #76846#
			break;

			case EMR_SETPIXELV :
			{
				*pWMF >> nX32 >> nY32;
				pOut->DrawPixel( Point( nX32, nY32 ), ReadColor() );
			}
			break;

			case EMR_SETMAPMODE :
			{
				sal_uInt32 nMapMode;
				*pWMF >> nMapMode;
				pOut->SetMapMode( nMapMode );
			}
			break;

			case EMR_SETBKMODE :
			{
				*pWMF >> nDat32;
				pOut->SetBkMode( nDat32 );
			}
			break;

			case EMR_SETPOLYFILLMODE :
			break;

			case EMR_SETROP2 :
			{
				*pWMF >> nDat32;
				pOut->SetRasterOp( nDat32 );
			}
			break;

			case EMR_SETSTRETCHBLTMODE :
			{
				*pWMF >> nStretchBltMode;
			}
			break;

			case EMR_SETTEXTALIGN :
			{
				*pWMF >> nDat32;
				pOut->SetTextAlign( nDat32 );
			}
			break;

			case EMR_SETTEXTCOLOR :
			{
				pOut->SetTextColor( ReadColor() );
			}
			break;

			case EMR_SETBKCOLOR :
			{
				pOut->SetBkColor( ReadColor() );
			}
			break;

			case EMR_OFFSETCLIPRGN :
			{
				*pWMF >> nX32 >> nY32;
				pOut->MoveClipRegion( Size( nX32, nY32 ) );
			}
			break;

			case EMR_MOVETOEX :
			{
				*pWMF >> nX32 >> nY32;
				pOut->MoveTo( Point( nX32, nY32 ), bRecordPath );
			}
			break;

			case EMR_INTERSECTCLIPRECT :
			{
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32;
				pOut->IntersectClipRect( ReadRectangle( nX32, nY32, nx32, ny32 ) );
			}
			break;

			case EMR_SAVEDC :
			{
				pOut->Push();
			}
			break;

			case EMR_RESTOREDC :
			{
				pOut->Pop();
			}
			break;

			case EMR_SETWORLDTRANSFORM :
			{
				XForm aTempXForm;
				*pWMF >> aTempXForm;
				pOut->SetWorldTransform( aTempXForm );
			}
			break;

			case EMR_MODIFYWORLDTRANSFORM :
			{
				sal_uInt32 	nMode;
				XForm	aTempXForm;
				*pWMF >> aTempXForm >> nMode;
				pOut->ModifyWorldTransform( aTempXForm, nMode );
			}
			break;

			case EMR_SELECTOBJECT :
			{
				*pWMF >> nIndex;
				pOut->SelectObject( nIndex );
			}
			break;

			case EMR_CREATEPEN :
			{
				*pWMF >> nIndex;
				if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
				{

					LineInfo	aLineInfo;
					sal_uInt32		nStyle;
					Size		aSize;

					*pWMF >> nStyle >> aSize.Width() >> aSize.Height();

					if ( aSize.Width() )
						aLineInfo.SetWidth( aSize.Width() );

					sal_Bool bTransparent = sal_False;
					sal_uInt16 nDashCount = 0;
					sal_uInt16 nDotCount = 0;
					switch( nStyle )
					{
						case PS_DASHDOTDOT :
							nDotCount++;
						case PS_DASHDOT :
							nDashCount++;
						case PS_DOT :
							nDotCount++;
						break;
						case PS_DASH :
							nDashCount++;
						break;
						case PS_NULL :
							bTransparent = sal_True;
							aLineInfo.SetStyle( LINE_NONE );
						break;
						default :
						case PS_INSIDEFRAME :
						case PS_SOLID :
							aLineInfo.SetStyle( LINE_SOLID );
					}
					if ( nDashCount | nDotCount )
					{
						aLineInfo.SetStyle( LINE_DASH );
						aLineInfo.SetDashCount( nDashCount );
						aLineInfo.SetDotCount( nDotCount );
					}
					pOut->CreateObject( nIndex, GDI_PEN, new WinMtfLineStyle( ReadColor(), aLineInfo, bTransparent ) );
				}
			}
			break;

			case EMR_EXTCREATEPEN :
            {
                sal_Int32   elpHatch;
                sal_uInt32  offBmi, cbBmi, offBits, cbBits, nStyle, nWidth, nBrushStyle, elpNumEntries;
                Color       aColorRef;

                *pWMF >> nIndex;
                if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
                {
                    *pWMF >> offBmi >> cbBmi >> offBits >> cbBits >>  nStyle >> nWidth >> nBrushStyle;
                     aColorRef = ReadColor();
                     *pWMF >> elpHatch >> elpNumEntries;

					LineInfo	aLineInfo;
					if ( nWidth )
						aLineInfo.SetWidth( nWidth );

					sal_Bool bTransparent = sal_False;
					sal_uInt16 nDashCount = 0;
					sal_uInt16 nDotCount = 0;

					switch( nStyle & PS_STYLE_MASK )
					{
						case PS_DASHDOTDOT :
							nDotCount++;
						case PS_DASHDOT :
							nDashCount++;
						case PS_DOT :
							nDotCount++;
						break;
						case PS_DASH :
							nDashCount++;
						break;
						case PS_NULL :
							bTransparent = sal_True;
							aLineInfo.SetStyle( LINE_NONE );
						break;

						default :
						case PS_INSIDEFRAME :
						case PS_SOLID :
							aLineInfo.SetStyle( LINE_SOLID );
					}
					if ( nDashCount | nDotCount )
					{
						aLineInfo.SetStyle( LINE_DASH );
						aLineInfo.SetDashCount( nDashCount );
						aLineInfo.SetDotCount( nDotCount );
					}
					pOut->CreateObject( nIndex, GDI_PEN, new WinMtfLineStyle( aColorRef, aLineInfo, bTransparent ) );
                }
            }
            break;

			case EMR_CREATEBRUSHINDIRECT :
			{
				sal_uInt32	nStyle;
				*pWMF >> nIndex;
				if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
				{
					*pWMF >> nStyle;
					pOut->CreateObject( nIndex, GDI_BRUSH, new WinMtfFillStyle( ReadColor(), ( nStyle == BS_HOLLOW ) ? sal_True : sal_False ) );
				}
			}
			break;

			case EMR_DELETEOBJECT :
			{
				*pWMF >> nIndex;
				if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
					pOut->DeleteObject( nIndex );
			}
			break;

			case EMR_ELLIPSE :
			{
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32;
				pOut->DrawEllipse( ReadRectangle( nX32, nY32, nx32, ny32 ) );
			}
			break;

			case EMR_RECTANGLE :
			{
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32;
				pOut->DrawRect( ReadRectangle( nX32, nY32, nx32, ny32 ) );
			}
			break;

			case EMR_ROUNDRECT :
			{
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32 >> nW >> nH;
				Size aSize( Size( nW, nH ) );
				pOut->DrawRoundRect( ReadRectangle( nX32, nY32, nx32, ny32 ), aSize );
			}
			break;

			case EMR_ARC :
			{
				sal_uInt32 nStartX, nStartY, nEndX, nEndY;
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32 >> nStartX >> nStartY >> nEndX >> nEndY;
				pOut->DrawArc( ReadRectangle( nX32, nY32, nx32, ny32 ), Point( nStartX, nStartY ), Point( nEndX, nEndY ) );
			}
			break;

			case EMR_CHORD :
			{
				sal_uInt32 nStartX, nStartY, nEndX, nEndY;
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32 >> nStartX >> nStartY >> nEndX >> nEndY;
				pOut->DrawChord( ReadRectangle( nX32, nY32, nx32, ny32 ), Point( nStartX, nStartY ), Point( nEndX, nEndY ) );
			}
			break;

			case EMR_PIE :
			{
				sal_uInt32 nStartX, nStartY, nEndX, nEndY;
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32 >> nStartX >> nStartY >> nEndX >> nEndY;
                const Rectangle aRect( ReadRectangle( nX32, nY32, nx32, ny32 ));

                // #i73608# OutputDevice deviates from WMF
                // semantics. start==end means full ellipse here.
                if( nStartX == nEndX && nStartY == nEndY )
                    pOut->DrawEllipse( aRect );
                else
                    pOut->DrawPie( aRect, Point( nStartX, nStartY ), Point( nEndX, nEndY ) );
			}
			break;

			case EMR_LINETO :
			{
				*pWMF >> nX32 >> nY32;
				pOut->LineTo( Point( nX32, nY32 ), bRecordPath );
			}
			break;

			case EMR_ARCTO :
			{
				sal_uInt32 nStartX, nStartY, nEndX, nEndY;
				*pWMF >> nX32 >> nY32 >> nx32 >> ny32 >> nStartX >> nStartY >> nEndX >> nEndY;
				pOut->DrawArc( ReadRectangle( nX32, nY32, nx32, ny32 ), Point( nStartX, nStartY ), Point( nEndX, nEndY ), sal_True );
			}
			break;

			case EMR_BEGINPATH :
			{
				pOut->ClearPath();
				bRecordPath = sal_True;
			}
			break;

			case EMR_ABORTPATH :
				pOut->ClearPath();
			case EMR_ENDPATH :
				bRecordPath = sal_False;
			break;

			case EMR_CLOSEFIGURE :
				pOut->ClosePath();
			break;

			case EMR_FILLPATH :
				pOut->StrokeAndFillPath( sal_False, sal_True );
			break;

			case EMR_STROKEANDFILLPATH :
				pOut->StrokeAndFillPath( sal_True, sal_True );
			break;

			case EMR_STROKEPATH :
				pOut->StrokeAndFillPath( sal_True, sal_False );
			break;

			case EMR_SELECTCLIPPATH :
			{
				sal_Int32 nClippingMode;
				*pWMF >> nClippingMode;
				pOut->SetClipPath( pOut->GetPathObj(), nClippingMode, sal_True );
			}
			break;

			case EMR_EXTSELECTCLIPRGN :
            {
                sal_Int32 iMode, cbRgnData;
                *pWMF >> cbRgnData
                      >> iMode;

                PolyPolygon aPolyPoly;
                if ( cbRgnData )
					ImplReadRegion( aPolyPoly, *pWMF, nRecSize );
				pOut->SetClipPath( aPolyPoly, iMode, sal_False );
	        }
            break;
			case EMR_ALPHABLEND:
			{
			   sal_Int32	xDest, yDest, cxDest, cyDest;

			   BLENDFUNCTION aFunc;
			   sal_Int32  xSrc, ySrc;
			   XForm	xformSrc;
			   sal_uInt32	BkColorSrc,iUsageSrc ,offBmiSrc,cbBmiSrc,offBitsSrc,cbBitsSrc ,cxSrc,cySrc ;

			   sal_uInt32	nStart = pWMF->Tell() - 8;
				pWMF->SeekRel( 0x10 );

				*pWMF >> xDest >> yDest >> cxDest >> cyDest >> aFunc >> xSrc >> ySrc
						>> xformSrc >> BkColorSrc >> iUsageSrc >> offBmiSrc >> cbBmiSrc
							>> offBitsSrc >> cbBitsSrc >>cxSrc>>cySrc ;

				sal_uInt32	dwRop = SRCAND|SRCINVERT;

				Bitmap		aBitmap;
				Rectangle	aRect( Point( xDest, yDest ), Size( cxDest+1, cyDest+1 ) );

				if ( (cbBitsSrc > (SAL_MAX_UINT32 - 14)) || ((SAL_MAX_UINT32 - 14) - cbBitsSrc < cbBmiSrc) )
			        bStatus = sal_False;
				else
				{
					sal_uInt32 nSize = cbBmiSrc + cbBitsSrc + 14;
					if ( nSize <= ( nEndPos - nStartPos ) )
					{
						char* pBuf = new char[ nSize ];
						SvMemoryStream aTmp( pBuf, nSize, STREAM_READ | STREAM_WRITE );
						aTmp.ObjectOwnsMemory( sal_True );
						aTmp << (sal_uInt8)'B'
							 << (sal_uInt8)'M'
							 << (sal_uInt32)cbBitsSrc
							 << (sal_uInt16)0
							 << (sal_uInt16)0
							 << (sal_uInt32)cbBmiSrc + 14;
						pWMF->Seek( nStart + offBmiSrc );
						pWMF->Read( pBuf + 14, cbBmiSrc );
						pWMF->Seek( nStart + offBitsSrc );
						pWMF->Read( pBuf + 14 + cbBmiSrc, cbBitsSrc );
						aTmp.Seek( 0 );
						ReadDIB(aBitmap, aTmp, true);

                        // test if it is sensible to crop
						if ( ( cxSrc > 0 ) && ( cySrc > 0 ) &&
							( xSrc >= 0 ) && ( ySrc >= 0 ) &&
                                ( xSrc + static_cast< sal_Int32 >(cxSrc) <= static_cast< sal_Int32 >(aBitmap.GetSizePixel().Width()) ) &&
                                    ( ySrc + static_cast< sal_Int32 >(cySrc) <= static_cast< sal_Int32 >(aBitmap.GetSizePixel().Height()) ) )
						{
							Rectangle aCropRect( Point( xSrc, ySrc ), Size( cxSrc, cySrc ) );
							aBitmap.Crop( aCropRect );
						}
 						aBmpSaveList.Insert( new BSaveStruct( aBitmap, aRect, dwRop ), LIST_APPEND );
					}
				}
			}
			break;
			case EMR_BITBLT :	// PASSTHROUGH INTENDED
			case EMR_STRETCHBLT :
			{
				sal_Int32	xDest, yDest, cxDest, cyDest, xSrc, ySrc, cxSrc, cySrc;
				sal_uInt32	dwRop, iUsageSrc, offBmiSrc, cbBmiSrc, offBitsSrc, cbBitsSrc;
				XForm	xformSrc;

				sal_uInt32	nStart = pWMF->Tell() - 8;

				pWMF->SeekRel( 0x10 );
				*pWMF >> xDest >> yDest >> cxDest >> cyDest >> dwRop >> xSrc >> ySrc
						>> xformSrc >> nColor >> iUsageSrc >> offBmiSrc >> cbBmiSrc
							>> offBitsSrc >> cbBitsSrc;

				if ( nRecType == EMR_STRETCHBLT )
					*pWMF >> cxSrc >> cySrc;
				else
					cxSrc = cySrc = 0;

				Bitmap		aBitmap;
				Rectangle	aRect( Point( xDest, yDest ), Size( cxDest+1, cyDest+1 ) );

				cxDest = abs( (int)cxDest );		// sj: i37894, size can be negative
				cyDest = abs( (int)cyDest );		// and also 122889

				if ( (cbBitsSrc > (SAL_MAX_UINT32 - 14)) || ((SAL_MAX_UINT32 - 14) - cbBitsSrc < cbBmiSrc) )
			        bStatus = sal_False;
				else
				{
					sal_uInt32 nSize = cbBmiSrc + cbBitsSrc + 14;
					if ( nSize <= ( nEndPos - nStartPos ) )
					{
						char* pBuf = new char[ nSize ];
						SvMemoryStream aTmp( pBuf, nSize, STREAM_READ | STREAM_WRITE );
						aTmp.ObjectOwnsMemory( sal_True );
						aTmp << (sal_uInt8)'B'
							 << (sal_uInt8)'M'
							 << (sal_uInt32)cbBitsSrc
							 << (sal_uInt16)0
							 << (sal_uInt16)0
							 << (sal_uInt32)cbBmiSrc + 14;
						pWMF->Seek( nStart + offBmiSrc );
						pWMF->Read( pBuf + 14, cbBmiSrc );
						pWMF->Seek( nStart + offBitsSrc );
						pWMF->Read( pBuf + 14 + cbBmiSrc, cbBitsSrc );
						aTmp.Seek( 0 );
						ReadDIB(aBitmap, aTmp, true);

						// test if it is sensible to crop
						if ( ( cxSrc > 0 ) && ( cySrc > 0 ) &&
							( xSrc >= 0 ) && ( ySrc >= 0 ) &&
								( xSrc + cxSrc <= aBitmap.GetSizePixel().Width() ) &&
									( ySrc + cySrc <= aBitmap.GetSizePixel().Height() ) )
						{
							Rectangle aCropRect( Point( xSrc, ySrc ), Size( cxSrc, cySrc ) );
							aBitmap.Crop( aCropRect );
						}
 						aBmpSaveList.Insert( new BSaveStruct( aBitmap, aRect, dwRop ), LIST_APPEND );
					}
				}
			}
			break;

			case EMR_STRETCHDIBITS :
			{
				sal_Int32	xDest, yDest, xSrc, ySrc, cxSrc, cySrc, cxDest, cyDest;
				sal_uInt32	offBmiSrc, cbBmiSrc, offBitsSrc, cbBitsSrc, iUsageSrc, dwRop;
				sal_uInt32	nStart = pWMF->Tell() - 8;

				pWMF->SeekRel( 0x10 );
				*pWMF >> xDest >> yDest >> xSrc >> ySrc >> cxSrc >> cySrc >> offBmiSrc >> cbBmiSrc >> offBitsSrc
					>> cbBitsSrc >> iUsageSrc >> dwRop >> cxDest >> cyDest;

				Bitmap		aBitmap;
				Rectangle	aRect( Point( xDest, yDest ), Size( cxDest+1, cyDest+1 ) );

				cxDest = abs( (int)cxDest );		// sj: i37894, size can be negative
				cyDest = abs( (int)cyDest );		// and also 122889

				if ( (cbBitsSrc > (SAL_MAX_UINT32 - 14)) || ((SAL_MAX_UINT32 - 14) - cbBitsSrc < cbBmiSrc) )
			        bStatus = sal_False;
				else
				{
					sal_uInt32 nSize = cbBmiSrc + cbBitsSrc + 14;
					if ( nSize <= ( nEndPos - nStartPos ) )
					{
						char* pBuf = new char[ nSize ];
						SvMemoryStream aTmp( pBuf, nSize, STREAM_READ | STREAM_WRITE );
						aTmp.ObjectOwnsMemory( sal_True );
						aTmp << (sal_uInt8)'B'
							<< (sal_uInt8)'M'
							<< (sal_uInt32)cbBitsSrc
							<< (sal_uInt16)0
							<< (sal_uInt16)0
							<< (sal_uInt32)cbBmiSrc + 14;
						pWMF->Seek( nStart + offBmiSrc );
						pWMF->Read( pBuf + 14, cbBmiSrc );
						pWMF->Seek( nStart + offBitsSrc );
						pWMF->Read( pBuf + 14 + cbBmiSrc, cbBitsSrc );
						aTmp.Seek( 0 );
						ReadDIB(aBitmap, aTmp, true);

						// test if it is sensible to crop
						if ( ( cxSrc > 0 ) && ( cySrc > 0 ) &&
							( xSrc >= 0 ) && ( ySrc >= 0 ) &&
								( xSrc + cxSrc <= aBitmap.GetSizePixel().Width() ) &&
									( ySrc + cySrc <= aBitmap.GetSizePixel().Height() ) )
						{
							Rectangle aCropRect( Point( xSrc, ySrc ), Size( cxSrc, cySrc ) );
							aBitmap.Crop( aCropRect );
						}
						aBmpSaveList.Insert( new BSaveStruct( aBitmap, aRect, dwRop ), LIST_APPEND );
					}
				}
			}
			break;

			case EMR_EXTCREATEFONTINDIRECTW :
			{
				*pWMF >> nIndex;
				if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
				{
					LOGFONTW aLogFont;
					*pWMF >> aLogFont.lfHeight >> aLogFont.lfWidth >> aLogFont.lfEscapement >> aLogFont.lfOrientation >> aLogFont.lfWeight >> aLogFont.lfItalic
								>> aLogFont.lfUnderline >> aLogFont.lfStrikeOut	>> aLogFont.lfCharSet >> aLogFont.lfOutPrecision >> aLogFont.lfClipPrecision
									>> aLogFont.lfQuality >> aLogFont.lfPitchAndFamily;

					sal_Unicode lfFaceName[ LF_FACESIZE ];

					for ( int i = 0; i < LF_FACESIZE; i++ )
					{
						sal_uInt16 nChar;
						*pWMF >> nChar;
						lfFaceName[ i ] = nChar;
					}
					aLogFont.alfFaceName = UniString( lfFaceName );

                    // #123216# Not used in the test case of #121382# (always identity in XForm), also
                    // no hints in ms docu if FontSize should be scaled with WT. Using with the example
                    // from #123216# creates errors, so removing.
                    //
                    // // #121382# Need to apply WorldTransform to FontHeight/Width; this should be completely
                    // // changed to basegfx::B2DHomMatrix instead of 'struct XForm', but not now due to time
                    // // constraints and dangers
                    // const XForm& rXF = pOut->GetWorldTransform();
                    // const basegfx::B2DHomMatrix aWT(rXF.eM11, rXF.eM21, rXF.eDx, rXF.eM12, rXF.eM22, rXF.eDy);
                    // const basegfx::B2DVector aTransVec(aWT * basegfx::B2DVector(aLogFont.lfWidth, aLogFont.lfHeight));
                    // aLogFont.lfWidth = aTransVec.getX();
                    // aLogFont.lfHeight = aTransVec.getY();

					pOut->CreateObject( nIndex, GDI_FONT, new WinMtfFontStyle( aLogFont ) );
				}
			}
			break;

			case EMR_EXTTEXTOUTA :
				bFlag = sal_True;
			case EMR_EXTTEXTOUTW :
			{
				sal_Int32	nLeft, nTop, nRight, nBottom, ptlReferenceX, ptlReferenceY, nGfxMode, nXScale, nYScale;
				sal_uInt32	nCurPos, nLen, nOffString, nOptions, offDx;
				sal_Int32*	pDX = NULL;

				nCurPos = pWMF->Tell() - 8;

				*pWMF >> nLeft >> nTop >> nRight >> nBottom >> nGfxMode >> nXScale >> nYScale
					>> ptlReferenceX >> ptlReferenceY >> nLen >> nOffString >> nOptions;

				pWMF->SeekRel( 0x10 );
				*pWMF >> offDx;

				sal_Int32 nTextLayoutMode = TEXT_LAYOUT_DEFAULT;
				if ( nOptions & ETO_RTLREADING )
					nTextLayoutMode = TEXT_LAYOUT_BIDI_RTL | TEXT_LAYOUT_TEXTORIGIN_LEFT;
				pOut->SetTextLayoutMode( nTextLayoutMode );
				DBG_ASSERT( ( nOptions & ( ETO_PDY | ETO_GLYPH_INDEX ) ) == 0, "SJ: ETO_PDY || ETO_GLYPH_INDEX in EMF" );

				Point aPos( ptlReferenceX, ptlReferenceY );
				if ( nLen && ( nLen < SAL_MAX_UINT32 / sizeof(sal_Int32) ) )
				{
					if ( offDx && (( nCurPos + offDx + nLen * 4 ) <= nNextPos ) )
					{
						pWMF->Seek( nCurPos + offDx );
						if ( ( nLen * sizeof(sal_uInt32) ) <= ( nEndPos - pWMF->Tell() ) )
						{
							pDX = new sal_Int32[ nLen ];
							sal_uInt32 i;
							for ( i = 0; i < nLen; i++ )
								*pWMF >> pDX[ i ];
						}
					}
					pWMF->Seek( nCurPos + nOffString );
					String aText;
					if ( bFlag )
					{
						if ( nLen <= ( nEndPos - pWMF->Tell() ) )
						{
							sal_Char* pBuf = new sal_Char[ nLen ];
							pWMF->Read( pBuf, nLen );
							aText = String( pBuf, (sal_uInt16)nLen, pOut->GetCharSet() );
							delete[] pBuf;

							if ( aText.Len() != nLen )
							{
								sal_uInt16 i, j, k;
								sal_Int32* pOldDx = pDX;
								pDX = new sal_Int32[ aText.Len() ];
								for ( i = 0, j = 0; i < aText.Len(); i++ )
								{
									ByteString aCharacter( aText.GetChar( i ), pOut->GetCharSet() );
									pDX[ i ] = 0;
									for ( k = 0; ( k < aCharacter.Len() ) && ( j < nLen ) && ( i < aText.Len() ); k++ )
										pDX[ i ] += pOldDx[ j++ ];
								}
								delete[] pOldDx;
							}
						}
					}
					else
					{
						if ( ( nLen * sizeof(sal_Unicode) ) <= ( nEndPos - pWMF->Tell() ) )
						{
							sal_Unicode* pBuf = new sal_Unicode[ nLen ];
							pWMF->Read( pBuf, nLen << 1 );
#ifdef OSL_BIGENDIAN
							sal_Char nTmp, *pTmp = (sal_Char*)( pBuf + nLen );
							while ( pTmp-- != (sal_Char*)pBuf )
							{
								nTmp = *pTmp--;
								pTmp[ 1 ] = *pTmp;
								*pTmp = nTmp;
							}
#endif
							aText = String( pBuf, (xub_StrLen)nLen );
							delete[] pBuf;
						}
					}
					pOut->DrawText( aPos, aText, pDX, bRecordPath, nGfxMode );
				}
				delete[] pDX;
			}
			break;

			case EMR_POLYBEZIERTO16 :
				bFlag = sal_True;
			case EMR_POLYBEZIER16 :
			{
				pWMF->SeekRel( 16 );
				*pWMF >> nPoints;
				sal_uInt16 i = 0;
				if ( bFlag )
				{
					i++;
					nPoints++;
				}
				Polygon aPoly( (sal_uInt16)nPoints );
				for( ; i < (sal_uInt16)nPoints; i++ )
				{
					*pWMF >> nX16 >> nY16;
					aPoly[ i ] = Point( nX16, nY16 );
				}
				pOut->DrawPolyBezier( aPoly, bFlag, bRecordPath );	// Line( aPoly, bFlag );
			}
			break;

			case EMR_POLYGON16 :
			{
				pWMF->SeekRel( 16 );
				*pWMF >> nPoints;
				Polygon aPoly( (sal_uInt16)nPoints );
				for( sal_uInt16 k = 0; k < (sal_uInt16)nPoints; k++ )
				{
					*pWMF >> nX16 >> nY16;
					aPoly[ k ] = Point( nX16, nY16 );
				}
				pOut->DrawPolygon( aPoly, bRecordPath );
			}
			break;

			case EMR_POLYLINETO16 :
				bFlag = sal_True;
			case EMR_POLYLINE16 :
			{
				pWMF->SeekRel( 16 );
				*pWMF >> nPoints;
				sal_uInt16 i = 0;
				if ( bFlag )
				{
					i++;
					nPoints++;
				}

				Polygon aPoly( (sal_uInt16)nPoints );
				for( ; i < (sal_uInt16)nPoints; i++ )
				{
					*pWMF >> nX16 >> nY16;
					aPoly[ i ] = Point( nX16, nY16 );
				}
				pOut->DrawPolyLine( aPoly, bFlag, bRecordPath );
			}
			break;

			case EMR_POLYPOLYLINE16 :
			{
				sal_uInt16*	pnPoints;

				sal_Int32	i, nPoly, nGesPoints;
				pWMF->SeekRel( 0x10 );
				// Anzahl der Polygone:
				*pWMF >> nPoly >> nGesPoints;

				// taking the amount of points of each polygon, retrieving the total number of points
				if ( static_cast< sal_uInt32 >(nPoly) < SAL_MAX_UINT32 / sizeof(sal_uInt16) )
				{
					if ( ( static_cast< sal_uInt32 >( nPoly ) * sizeof(sal_uInt16) ) <= ( nEndPos - pWMF->Tell() ) )
					{
						pnPoints = new sal_uInt16[ nPoly ];
						for ( i = 0; i < nPoly; i++ )
						{
							*pWMF >> nPoints;
							pnPoints[ i ] = (sal_uInt16)nPoints;
						}
						// Polygonpunkte holen:
						for ( i = 0; ( i < nPoly ) && !pWMF->IsEof(); i++ )
						{
							Polygon aPolygon( pnPoints[ i ] );
							for ( sal_uInt16 k = 0; k < pnPoints[ i ]; k++ )
							{
								*pWMF >> nX16 >> nY16;
								aPolygon[ k ] = Point( nX16, nY16 );
							}
							pOut->DrawPolyLine( aPolygon, sal_False, bRecordPath );
						}
						delete[] pnPoints;
					}
				}
			}
			break;

			case EMR_POLYPOLYGON16 :
			{
				sal_uInt32 nPoly(0);
                sal_uInt32 nGesPoints(0);
				pWMF->SeekRel( 0x10 );
				// Anzahl der Polygone:
				*pWMF >> nPoly >> nGesPoints;
                sal_uInt32 nReadPoints(0);

                if ( ( nGesPoints < SAL_MAX_UINT32 / sizeof(Point) ) && ( nPoly < SAL_MAX_UINT32 / sizeof(sal_uInt16) )  && !pWMF->IsEof() )
				{
					if ( ( static_cast< sal_uInt32 >( nPoly ) * sizeof( sal_uInt16 ) ) <= ( nEndPos - pWMF->Tell() ) )
					{
                        sal_uInt32 i(0);
						sal_uInt16*	pnPoints = new sal_uInt16[ nPoly ];

                        for ( i = 0; i < nPoly && !pWMF->IsEof(); i++ )
						{
							*pWMF >> nPoints;
							pnPoints[ i ] = (sal_uInt16)nPoints;
						}

                        if ( ( nGesPoints * (sizeof(sal_uInt16)+sizeof(sal_uInt16)) ) <= ( nEndPos - pWMF->Tell() )  && !pWMF->IsEof() )
						{
                            PolyPolygon aPolyPoly(nPoly, nPoly);

						    for ( i = 0; i < nPoly && !pWMF->IsEof(); i++ )
						    {
                                const sal_uInt16 nPointCount(pnPoints[i]);
                				Point* pPtAry = new Point[nPointCount];

                                for(sal_uInt16 b(0); b < nPointCount && !pWMF->IsEof(); b++)
                                {
								    *pWMF >> nX16 >> nY16;
								    pPtAry[b] = Point( nX16, nY16 );
                                    nReadPoints++;
                                }

                                aPolyPoly.Insert(Polygon(nPointCount, pPtAry));
                                delete[] pPtAry;
                            }

							// create PolyPolygon actions
							pOut->DrawPolyPolygon( aPolyPoly, bRecordPath );
						}

                        delete[] pnPoints;
					}
				}

                OSL_ENSURE(nReadPoints == nGesPoints, "The number Points processed from EMR_POLYPOLYGON16 is unequal imported number (!)");
			}
			break;

			case EMR_FILLRGN :
            {
                sal_uInt32 nLen;
                PolyPolygon aPolyPoly;
                pWMF->SeekRel( 0x10 );
                *pWMF >> nLen >> nIndex;

                if ( ImplReadRegion( aPolyPoly, *pWMF, nRecSize ) )
                {
                    pOut->Push();
                    pOut->SelectObject( nIndex );
				    pOut->DrawPolyPolygon( aPolyPoly, sal_False );
                    pOut->Pop();
                }
            }
            break;
			case EMR_CREATEDIBPATTERNBRUSHPT :
			{
				sal_uInt32	nTmp32;
				sal_uInt32  nOffset;
				*pWMF >> nIndex;
				Bitmap	aBmp;
				BitmapReadAccess* pBmp;
				sal_uInt32	nRed = 0, nGreen = 0, nBlue = 0, nCount = 1;

				*pWMF >> nTmp32;
				*pWMF >> nOffset;
				for ( sal_uInt32 i = 0; i < (nOffset - 20)/4; i ++ )
				{
					*pWMF >> nTmp32;
				}

				ReadDIB(aBmp, *pWMF, false);
				pBmp = aBmp.AcquireReadAccess();
				if ( pBmp )
				{
					for ( sal_Int32 y = 0; y < pBmp->Height(); y++ )
					{
						for ( sal_Int32 x = 0; x < pBmp->Width(); x++ )
						{
							const BitmapColor aColor( pBmp->GetColor( y, x ) );

							nRed += aColor.GetRed();
							nGreen += aColor.GetGreen();
							nBlue += aColor.GetBlue();
						}
					}
					nCount = pBmp->Height() * pBmp->Width();
					if ( !nCount )
						nCount++;
					aBmp.ReleaseAccess( pBmp );
				}
				Color aColor( (sal_Char)( nRed / nCount ), (sal_Char)( nGreen / nCount ), (sal_Char)( nBlue / nCount ) );
				pOut->CreateObject( nIndex, GDI_BRUSH, new WinMtfFillStyle( aColor, sal_False ) );
			}
			break;

#ifdef WIN_MTF_ASSERT
            default :                           WinMtfAssertHandler( "Unknown Meta Action" );       break;
			case EMR_MASKBLT :                  WinMtfAssertHandler( "MaskBlt" );                   break;
			case EMR_PLGBLT :                   WinMtfAssertHandler( "PlgBlt" );                    break;
			case EMR_SETDIBITSTODEVICE :        WinMtfAssertHandler( "SetDIBitsToDevice" );         break;
			case EMR_FRAMERGN :                 WinMtfAssertHandler( "FrameRgn" );                  break;
			case EMR_INVERTRGN :                WinMtfAssertHandler( "InvertRgn" );                 break;
            case EMR_PAINTRGN :                 WinMtfAssertHandler( "PaintRgn" );                  break;
            case EMR_FLATTENPATH :		        WinMtfAssertHandler( "FlattenPath" );               break;
            case EMR_WIDENPATH :                WinMtfAssertHandler( "WidenPath" );                 break;
			case EMR_POLYDRAW :			        WinMtfAssertHandler( "Polydraw" );                  break;
			case EMR_SETARCDIRECTION :	        WinMtfAssertHandler( "SetArcDirection" );           break;
			case EMR_SETPALETTEENTRIES :        WinMtfAssertHandler( "SetPaletteEntries" );         break;
			case EMR_RESIZEPALETTE :	        WinMtfAssertHandler( "ResizePalette" );             break;
			case EMR_EXTFLOODFILL :		        WinMtfAssertHandler( "ExtFloodFill" );              break;
			case EMR_ANGLEARC :     	        WinMtfAssertHandler( "AngleArc" );                  break;
			case EMR_SETCOLORADJUSTMENT :       WinMtfAssertHandler( "SetColorAdjustment" );	    break;
			case EMR_POLYDRAW16 :		        WinMtfAssertHandler( "PolyDraw16" );                break;
			case EMR_POLYTEXTOUTA : 		    WinMtfAssertHandler( "PolyTextOutA" );              break;
			case EMR_POLYTEXTOUTW :			    WinMtfAssertHandler( "PolyTextOutW" );              break;
			case EMR_CREATECOLORSPACE :         WinMtfAssertHandler( "CreateColorSpace" );	        break;
            case EMR_SETCOLORSPACE :		    WinMtfAssertHandler( "SetColorSpace" );             break;
			case EMR_DELETECOLORSPACE :		    WinMtfAssertHandler( "DeleteColorSpace" );          break;
			case EMR_GLSRECORD :    		    WinMtfAssertHandler( "GlsRecord" );                 break;
			case EMR_GLSBOUNDEDRECORD :		    WinMtfAssertHandler( "GlsBoundRecord" );            break;
			case EMR_PIXELFORMAT :  		    WinMtfAssertHandler( "PixelFormat" );               break;
            case EMR_DRAWESCAPE :               WinMtfAssertHandler( "DrawEscape" );                break;
            case EMR_EXTESCAPE :                WinMtfAssertHandler( "ExtEscape" );                 break;
            case EMR_STARTDOC :                 WinMtfAssertHandler( "StartDoc" );                  break;
            case EMR_SMALLTEXTOUT :             WinMtfAssertHandler( "SmallTextOut" );              break;
            case EMR_FORCEUFIMAPPING :          WinMtfAssertHandler( "ForceUFIMapping" );           break;
            case EMR_NAMEDESCAPE :              WinMtfAssertHandler( "NamedEscape" );               break;
            case EMR_COLORCORRECTPALETTE :      WinMtfAssertHandler( "ColorCorrectPalette" );       break;
            case EMR_SETICMPROFILEA :           WinMtfAssertHandler( "SetICMProfileA" );            break;
            case EMR_SETICMPROFILEW :           WinMtfAssertHandler( "SetICMProfileW" );            break;
            case EMR_TRANSPARENTBLT :           WinMtfAssertHandler( "TransparenBlt" );             break;
            case EMR_TRANSPARENTDIB :           WinMtfAssertHandler( "TransparenDib" );             break;
            case EMR_GRADIENTFILL :             WinMtfAssertHandler( "GradientFill" );              break;
            case EMR_SETLINKEDUFIS :            WinMtfAssertHandler( "SetLinkedUFIS" );             break;

			case EMR_SETMAPPERFLAGS :	        WinMtfAssertHandler( "SetMapperFlags", 0 );         break;
			case EMR_SETICMMODE :			    WinMtfAssertHandler( "SetICMMode", 0 );             break;
			case EMR_CREATEMONOBRUSH :          WinMtfAssertHandler( "CreateMonoBrush", 0 );        break;
            case EMR_SETBRUSHORGEX :	        WinMtfAssertHandler( "SetBrushOrgEx", 0 );          break;
            case EMR_SETMETARGN :		        WinMtfAssertHandler( "SetMetArgn", 0 );             break;
			case EMR_SETMITERLIMIT :	        WinMtfAssertHandler( "SetMiterLimit", 0 );          break;
			case EMR_EXCLUDECLIPRECT :          WinMtfAssertHandler( "ExcludeClipRect", 0 );        break;
			case EMR_REALIZEPALETTE :	        WinMtfAssertHandler( "RealizePalette", 0 );         break;
			case EMR_SELECTPALETTE :	        WinMtfAssertHandler( "SelectPalette", 0 );          break;
			case EMR_CREATEPALETTE :	        WinMtfAssertHandler( "CreatePalette", 0 );          break;
            case EMR_ALPHADIBBLEND :            WinMtfAssertHandler( "AlphaDibBlend", 0 );          break;
            case EMR_SETTEXTJUSTIFICATION :     WinMtfAssertHandler( "SetTextJustification", 0 );   break;

			case EMR_GDICOMMENT :
			case EMR_HEADER :	            // has already been read at ReadHeader()
            break;
#endif
		}
		pWMF->Seek( nNextPos );
	}
	if( aBmpSaveList.Count() )
		pOut->ResolveBitmapActions( aBmpSaveList );

	if ( bStatus )
		pWMF->Seek(nEndPos);

	return bStatus;
};

//-----------------------------------------------------------------------------------

sal_Bool EnhWMFReader::ReadHeader()
{
	sal_uInt32		nsal_uInt32, nHeaderSize, nPalEntries;
	sal_Int32		nLeft, nTop, nRight, nBottom;

	// METAFILEHEADER SPARE ICH MIR HIER
	// Einlesen des METAHEADER
	*pWMF >> nsal_uInt32 >> nHeaderSize;
	if ( nsal_uInt32 != 1 )			// Typ
		return sal_False;

	// bound size
	Rectangle rclBounds;	// rectangle in logical units 1/100th mm
	*pWMF >> nLeft >> nTop >> nRight >> nBottom;
	rclBounds.Left() = nLeft;
	rclBounds.Top() = nTop;
	rclBounds.Right() = nRight;
	rclBounds.Bottom() = nBottom;

	// picture frame size
	Rectangle rclFrame;		// rectangle in device units
	*pWMF >> nLeft >> nTop >> nRight >> nBottom;
	rclFrame.Left() = nLeft;
	rclFrame.Top() = nTop;
	rclFrame.Right() = nRight;
	rclFrame.Bottom() = nBottom;

	*pWMF >> nsal_uInt32;									// signature

	if ( nsal_uInt32 != 0x464d4520 )
		return sal_False;

	*pWMF >> nsal_uInt32;									// nVersion
	*pWMF >> nEndPos;									// size of metafile
	nEndPos += nStartPos;

	sal_uInt32 nStrmPos = pWMF->Tell();					// checking if nEndPos is valid
	pWMF->Seek( STREAM_SEEK_TO_END );
	if ( pWMF->Tell() < nEndPos )
		nEndPos = pWMF->Tell();
	pWMF->Seek( nStrmPos );

	*pWMF >> nRecordCount;

	if ( !nRecordCount )
		return sal_False;

	pWMF->SeekRel( 0xc );

	sal_Int32 nPixX, nPixY, nMillX, nMillY;
	*pWMF >> nPalEntries >> nPixX >> nPixY >> nMillX >> nMillY;

	pOut->SetrclFrame( rclFrame );
	pOut->SetrclBounds( rclBounds );
	pOut->SetRefPix( Size( nPixX, nPixY ) );
	pOut->SetRefMill( Size( nMillX, nMillY ) );

	pWMF->Seek( nStartPos + nHeaderSize );
	return sal_True;
}

//-----------------------------------------------------------------------------------

Rectangle EnhWMFReader::ReadRectangle( sal_Int32 x1, sal_Int32 y1, sal_Int32 x2, sal_Int32 y2 )
{
	Point aTL ( Point( x1, y1 ) );
	Point aBR( Point( --x2, --y2 ) );
	return Rectangle( aTL, aBR );
}

EnhWMFReader::~EnhWMFReader()
{

};

/* vim: set noet sw=4 ts=4: */
