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
#include "precompiled_filter.hxx"

#include <string.h>
#include <math.h>
#include <dxfreprd.hxx>


//------------------DXFBoundingBox--------------------------------------------


void DXFBoundingBox::Union(const DXFVector & rVector)
{
	if (bEmpty==sal_True) {
		fMinX=rVector.fx;
		fMinY=rVector.fy;
		fMinZ=rVector.fz;
		fMaxX=rVector.fx;
		fMaxY=rVector.fy;
		fMaxZ=rVector.fz;
		bEmpty=sal_False;
	}
	else {
		if (fMinX>rVector.fx) fMinX=rVector.fx;
		if (fMinY>rVector.fy) fMinY=rVector.fy;
		if (fMinZ>rVector.fz) fMinZ=rVector.fz;
		if (fMaxX<rVector.fx) fMaxX=rVector.fx;
		if (fMaxY<rVector.fy) fMaxY=rVector.fy;
		if (fMaxZ<rVector.fz) fMaxZ=rVector.fz;
	}
}


//------------------DXFPalette------------------------------------------------


DXFPalette::DXFPalette()
{
	short i,j,nHue,nNSat,nVal,nC[3],nmax,nmed,nmin;
	sal_uInt8 nV;

	pRed  =new sal_uInt8[256];
	pGreen=new sal_uInt8[256];
	pBlue =new sal_uInt8[256];

	// Farben 0 - 9 (normale Farben)
	SetColor(0, 0x00, 0x00, 0x00); // eigentlich nie benutzt
	SetColor(1, 0xff, 0x00, 0x00);
	SetColor(2, 0xff, 0xff, 0x00);
	SetColor(3, 0x00, 0xff, 0x00);
	SetColor(4, 0x00, 0xff, 0xff);
	SetColor(5, 0x00, 0x00, 0xff);
	SetColor(6, 0xff, 0x00, 0xff);
	SetColor(7, 0x0f, 0x0f, 0x0f); // eigentlich weiss ???
	SetColor(8, 0x80, 0x80, 0x80);
	SetColor(9, 0xc0, 0xc0, 0xc0);

	// Farben 10 - 249
	// (Universal-Palette: 24 Farbtoene * 5 Helligkeiten * 2 Saettigungen )
	i=10;
	for (nHue=0; nHue<24; nHue++) {
		for (nVal=5; nVal>=1; nVal--) {
			for (nNSat=0; nNSat<2; nNSat++) {
				nmax=((nHue+3)>>3)%3;
				j=nHue-(nmax<<3); if (j>4) j=j-24;
				if (j>=0) {
					nmed=(nmax+1)%3;
					nmin=(nmax+2)%3;
				}
				else {
					nmed=(nmax+2)%3;
					nmin=(nmax+1)%3;
					j=-j;
				}
				nC[nmin]=0;
				nC[nmed]=255*j/4;
				nC[nmax]=255;
				if (nNSat!=0) {
					for (j=0; j<3; j++) nC[j]=(nC[j]>>1)+128;
				}
				for (j=0; j<3; j++) nC[j]=nC[j]*nVal/5;
				SetColor((sal_uInt8)(i++),(sal_uInt8)nC[0],(sal_uInt8)nC[1],(sal_uInt8)nC[2]);
			}
		}
	}

	// Farben 250 - 255 (Grautoenne)
	for (i=0; i<6; i++) {
		nV=(sal_uInt8)(i*38+65);
		SetColor((sal_uInt8)(250+i),nV,nV,nV);
	}
}


DXFPalette::~DXFPalette()
{
	delete[] pBlue;
	delete[] pGreen;
	delete[] pRed;
}


void DXFPalette::SetColor(sal_uInt8 nIndex, sal_uInt8 nRed, sal_uInt8 nGreen, sal_uInt8 nBlue)
{
	pRed[nIndex]=nRed;
	pGreen[nIndex]=nGreen;
	pBlue[nIndex]=nBlue;
}


//------------------DXFRepresentation-----------------------------------------


DXFRepresentation::DXFRepresentation()
{
	setTextEncoding(RTL_TEXTENCODING_IBM_437);
        setGlobalLineTypeScale(1.0);
}


DXFRepresentation::~DXFRepresentation()
{
}


sal_Bool DXFRepresentation::Read( SvStream & rIStream, sal_uInt16 nMinPercent, sal_uInt16 nMaxPercent)
{
	DXFGroupReader * pDGR;
	sal_Bool bRes;

	aTables.Clear();
	aBlocks.Clear();
	aEntities.Clear();

	pDGR = new DXFGroupReader( rIStream, nMinPercent, nMaxPercent );

	pDGR->Read();
	while (pDGR->GetG()!=0 || strcmp(pDGR->GetS(),"EOF")!=0) {
		if (pDGR->GetG()==0 && strcmp(pDGR->GetS(),"SECTION")==0) {
			if (pDGR->Read()!=2) {
				pDGR->SetError();
				break;
			}
			if      (strcmp(pDGR->GetS(),"HEADER"  )==0) ReadHeader(*pDGR);
			else if (strcmp(pDGR->GetS(),"TABLES"  )==0) aTables.Read(*pDGR);
			else if (strcmp(pDGR->GetS(),"BLOCKS"  )==0) aBlocks.Read(*pDGR);
			else if (strcmp(pDGR->GetS(),"ENTITIES")==0) aEntities.Read(*pDGR);
			else pDGR->Read();
		}
		else pDGR->Read();
	}

	bRes=pDGR->GetStatus();

	delete pDGR;

	if (bRes==sal_True)
	{
		// The header $EXTMIN/$EXTMAX that ReadHeader collected into aBoundingBox
		// are written by AutoCAD from the current view/regen state, not from a
		// tight geometry fit, so they drift with the zoom level in effect when
		// the file was saved (issue 58347: same drawing, different zoom, wrong
		// import scale). Measure the real geometry extent and use that for the
		// page-fit scaling instead. CalcBoundingBox now measures every entity
		// type DrawEntities actually renders (HATCH included), so a non-empty
		// geometry box is authoritative. The header extents are kept only as a
		// last-resort fallback for the degenerate case where nothing measurable
		// was drawn at all (empty geometry box).
		DXFBoundingBox aGeometryBox;
		CalcBoundingBox(aEntities,aGeometryBox);
		if (aGeometryBox.bEmpty==sal_False)
			aBoundingBox=aGeometryBox;
	}

	return bRes;
}


void DXFRepresentation::ReadHeader(DXFGroupReader & rDGR)
{

         while (rDGR.GetG()!=0 || (strcmp(rDGR.GetS(),"EOF")!=0 &&  strcmp(rDGR.GetS(),"ENDSEC")!=0) )
         {
                 if (rDGR.GetG()==9) {
                         if (strcmp(rDGR.GetS(),"$EXTMIN")==0 ||
                                 strcmp(rDGR.GetS(),"$EXTMAX")==0)
                         {
                                 DXFVector aVector;
                                 rDGR.SetF(10,0.0);
                                 rDGR.SetF(20,0.0);
                                 rDGR.SetF(30,0.0);
                                 do {
                                         rDGR.Read();
                                 } while (rDGR.GetG()!=9 && rDGR.GetG()!=0);
                                 aVector.fx=rDGR.GetF(10);
                                 aVector.fy=rDGR.GetF(20);
                                 aVector.fz=rDGR.GetF(30);
                                 aBoundingBox.Union(aVector);
                         } else {
                                 if (strcmp(rDGR.GetS(),"$DWGCODEPAGE")==0)
                                 {
                                         rDGR.Read();

                                         // FIXME: we really need a whole table of
                                         // $DWGCODEPAGE to encodings mappings
                                         if ( (strcmp(rDGR.GetS(),"ANSI_932")==0) ||
					      (strcmp(rDGR.GetS(),"ansi_932")==0) ||
                                              (strcmp(rDGR.GetS(),"DOS932")==0) ||
                                              (strcmp(rDGR.GetS(),"dos932")==0) )
                                         {
                                                 setTextEncoding(RTL_TEXTENCODING_MS_932);
                                         }
                                 }
				 else if (strcmp(rDGR.GetS(),"$LTSCALE")==0)
                                 {
                                         rDGR.Read();
                                         setGlobalLineTypeScale(getGlobalLineTypeScale() * rDGR.GetF());
                                 }
                                 else rDGR.Read();
                         }
                 }
                 else rDGR.Read();
         }
}


// Map a single OCS point of an entity to WCS via its extrusion, then add it to
// the bounding box. The renderer (DXF2GDIMetaFile::DrawEntities) applies the
// same ECS->WCS transform per entity, so the extent has to be measured in WCS
// as well — otherwise drawings that use a non-default (e.g. negative-Z)
// extrusion come out mis-scaled and offset (issue 16564: PL1CAFE.dxf reaches
// OCS X=-10.88 on mirrored polylines that actually render near WCS X=+10.88).
static inline void UnionOCS(DXFBoundingBox & rBox, const DXFTransform & rE2W,
							const DXFVector & rP)
{
	DXFVector aW;
	rE2W.Transform(rP, aW);
	rBox.Union(aW);
}


// Approximate the world-space extent of a single line of text and add it to
// the bounding box, so labels sitting near the drawing edge are not cropped.
// AOO renders TEXT/ATTRIB left-aligned on the baseline at the insertion point,
// growing up and to the right (the justification codes are ignored by the
// renderer, see DXF2GDIMetaFile::DrawTextEntity), so bound that rectangle,
// rotated by the text angle. The per-character advance and descent are
// deliberately generous fractions of the text height so the label never clips.
// rE2W maps the (OCS) text rectangle to WCS so text on an extruded entity is
// measured where it is drawn.
static void UnionTextExtent(DXFBoundingBox & rBox, const DXFTransform & rE2W,
							const DXFVector & rInsert,
							double fHeight, double fXScale, double fRotAngle,
							const char * pText)
{
	if (fHeight <= 0.0) {
		UnionOCS(rBox, rE2W, rInsert);
		return;
	}
	if (fXScale <= 0.0)
		fXScale = 1.0;

	const size_t nLen = (pText != NULL) ? strlen(pText) : 0;
	const double fWidth   = (double)nLen * 0.85 * fHeight * fXScale;
	const double fAscent  = fHeight;
	const double fDescent = 0.25 * fHeight;

	const double fRad = fRotAngle * 3.14159265359 / 180.0;
	const DXFVector aDir( cos(fRad),  sin(fRad), 0.0); // text direction
	const DXFVector aUp (-sin(fRad),  cos(fRad), 0.0); // baseline normal (up)

	for (int a = 0; a < 2; a++) {
		const double fAlong = a ? fWidth : 0.0;
		for (int b = 0; b < 2; b++) {
			const double fPerp = b ? fAscent : -fDescent;
			UnionOCS(rBox, rE2W, rInsert + aDir * fAlong + aUp * fPerp);
		}
	}
}


void DXFRepresentation::CalcBoundingBox(const DXFEntities & rEntities,
										DXFBoundingBox & rBox)
{
	DXFBasicEntity * pBE=rEntities.pFirst;
	while (pBE!=NULL) {
		// The renderer builds an ECS->WCS transform from the entity's extrusion
		// whenever it is not the default (DrawEntities checks fz != 1.0); measure
		// through the SAME transform so the box is in WCS, not raw OCS. Use the same
		// WCS-vs-OCS rule as DrawEntities (DXFCoordsAreWCS): entities whose coords
		// are already WCS (LINE/POINT/3DFACE/3D-polyline) must NOT be extruded, or
		// the box is measured over scattered positions and comes out far too large
		// (issue 99893/70273 — geometry then sits in a corner of an inflated page).
		DXFTransform aE2W;                        // identity by default
		if (pBE->aExtrusion.fz != 1.0 && !DXFCoordsAreWCS(*pBE))
			aE2W = DXFTransform(pBE->aExtrusion);
		switch (pBE->eType) {
			case DXF_LINE: {
				const DXFLineEntity * pE = (DXFLineEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				UnionOCS(rBox, aE2W, pE->aP1);
				break;
			}
			case DXF_POINT: {
				const DXFPointEntity * pE = (DXFPointEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				break;
			}
			case DXF_CIRCLE: {
				const DXFCircleEntity * pE = (DXFCircleEntity*)pBE;
				// radius is invariant under the extrusion (a rotation/reflection),
				// so map the centre to WCS and take +/-radius there.
				DXFVector aC;
				aE2W.Transform(pE->aP0, aC);
				DXFVector aP=aC; aP.fx-=pE->fRadius; aP.fy-=pE->fRadius; rBox.Union(aP);
				aP=aC; aP.fx+=pE->fRadius; aP.fy+=pE->fRadius; rBox.Union(aP);
				break;
			}
			case DXF_ARC: {
				const DXFArcEntity * pE = (DXFArcEntity*)pBE;
				// Measure the arc's SWEPT extent, not its full circle. A
				// large-radius arc (nearly straight, common in CAD) has its
				// centre far outside the drawing; unioning centre +/- radius
				// blew the box up enormously and collapsed the real geometry
				// (issue 122565). Sample along the sweep exactly as the renderer
				// does (CCW from fStart by fdA, normalised to (0,360]) and map
				// each point through the same ECS->WCS transform.
				double fA1 = pE->fStart;
				double fdA = pE->fEnd - fA1;
				while (fdA>=360.0) fdA-=360.0;
				while (fdA<=0.0)   fdA+=360.0;
				sal_uInt16 nSeg = (sal_uInt16)(fdA/4.0 + 0.5);
				if (nSeg<1) nSeg=1;
				for (sal_uInt16 nA=0; nA<=nSeg; nA++) {
					double fAng = 3.14159265359/180.0 *
								  (fA1 + fdA*(double)nA/(double)nSeg);
					UnionOCS(rBox, aE2W, pE->aP0 +
							 DXFVector(pE->fRadius*cos(fAng),
									   pE->fRadius*sin(fAng), 0.0));
				}
				break;
			}
			case DXF_TRACE: {
				const DXFTraceEntity * pE = (DXFTraceEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				UnionOCS(rBox, aE2W, pE->aP1);
				UnionOCS(rBox, aE2W, pE->aP2);
				UnionOCS(rBox, aE2W, pE->aP3);
				break;
			}
			case DXF_SOLID: {
				const DXFSolidEntity * pE = (DXFSolidEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				UnionOCS(rBox, aE2W, pE->aP1);
				UnionOCS(rBox, aE2W, pE->aP2);
				UnionOCS(rBox, aE2W, pE->aP3);
				break;
			}
			case DXF_TEXT: {
				const DXFTextEntity * pE = (DXFTextEntity*)pBE;
				UnionTextExtent(rBox, aE2W, pE->aP0, pE->fHeight, pE->fXScale,
								pE->fRotAngle, pE->sText);
				break;
			}
			case DXF_SHAPE: {
				// SHAPE is not rendered (DrawEntities' dispatch has no case for
				// it, so DrawEntities falls through to default:break), therefore
				// it deliberately does not contribute to the extent — measuring
				// invisible geometry would only reserve empty margin and shrink
				// the visible drawing on the page.
				break;
			}
			case DXF_INSERT: {
				const DXFInsertEntity * pE = (DXFInsertEntity*)pBE;
				DXFBlock * pB;
				DXFBoundingBox aBox;
				DXFVector aP;
				pB=aBlocks.Search(pE->sName);
				if (pB==NULL) break;
				CalcBoundingBox(*pB,aBox);
				if (aBox.bEmpty==sal_True) break;
				aP.fx=(aBox.fMinX-pB->aBasePoint.fx)*pE->fXScale+pE->aP0.fx;
				aP.fy=(aBox.fMinY-pB->aBasePoint.fy)*pE->fYScale+pE->aP0.fy;
				aP.fz=(aBox.fMinZ-pB->aBasePoint.fz)*pE->fZScale+pE->aP0.fz;
				UnionOCS(rBox, aE2W, aP);
				aP.fx=(aBox.fMaxX-pB->aBasePoint.fx)*pE->fXScale+pE->aP0.fx;
				aP.fy=(aBox.fMaxY-pB->aBasePoint.fy)*pE->fYScale+pE->aP0.fy;
				aP.fz=(aBox.fMaxZ-pB->aBasePoint.fz)*pE->fZScale+pE->aP0.fz;
				UnionOCS(rBox, aE2W, aP);
				break;
			}
			case DXF_ATTDEF: {
				// ATTDEF (attribute definition) is likewise not rendered by
				// DrawEntities, so — like SHAPE — it is intentionally left out
				// of the extent to stay consistent with what actually gets
				// drawn.
				break;
			}
			case DXF_ATTRIB: {
				const DXFAttribEntity * pE = (DXFAttribEntity*)pBE;
				UnionTextExtent(rBox, aE2W, pE->aP0, pE->fHeight, pE->fXScale,
								pE->fRotAngle, pE->sText);
				break;
			}
			case DXF_VERTEX: {
				const DXFVertexEntity * pE = (DXFVertexEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				break;
			}
			case DXF_3DFACE: {
				const DXF3DFaceEntity * pE = (DXF3DFaceEntity*)pBE;
				UnionOCS(rBox, aE2W, pE->aP0);
				UnionOCS(rBox, aE2W, pE->aP1);
				UnionOCS(rBox, aE2W, pE->aP2);
				UnionOCS(rBox, aE2W, pE->aP3);
				break;
			}
			case DXF_DIMENSION: {
				const DXFDimensionEntity * pE = (DXFDimensionEntity*)pBE;
				DXFBlock * pB;
				DXFBoundingBox aBox;
				DXFVector aP;
				pB=aBlocks.Search(pE->sPseudoBlock);
				if (pB==NULL) break;
				CalcBoundingBox(*pB,aBox);
				if (aBox.bEmpty==sal_True) break;
				aP.fx=aBox.fMinX-pB->aBasePoint.fx;
				aP.fy=aBox.fMinY-pB->aBasePoint.fy;
				aP.fz=aBox.fMinZ-pB->aBasePoint.fz;
				UnionOCS(rBox, aE2W, aP);
				aP.fx=aBox.fMaxX-pB->aBasePoint.fx;
				aP.fy=aBox.fMaxY-pB->aBasePoint.fy;
				aP.fz=aBox.fMaxZ-pB->aBasePoint.fz;
				UnionOCS(rBox, aE2W, aP);
				break;
			}
			case DXF_POLYLINE: {
				// The old-style POLYLINE header carries no coordinates itself;
				// its geometry lives in the VERTEX entities that follow it in
				// the entity list (each handled by the DXF_VERTEX case above),
				// so there is nothing to union here.
				break;
			}
			case DXF_SEQEND: {
				//const DXFAttribEntity * pE = (DXFAttribEntity*)pBE;
				//???
				break;
			}
			case DXF_HATCH : {
				// HATCH is rendered (DrawHatchEntity), so its extent has to be
				// measured or a hatch reaching past the rest of the drawing gets
				// cropped. Union exactly the boundary geometry the renderer
				// actually draws: the points of a polyline boundary path, and
				// the endpoints of straight (type 1) edges. Circular-arc,
				// elliptical-arc and spline edges are NOT drawn by
				// DrawHatchEntity (their code is commented out / empty), so they
				// are intentionally not unioned here — matching the render.
				const DXFHatchEntity * pE = (DXFHatchEntity*)pBE;
				for (sal_Int32 nPath = 0;
					 pE->pBoundaryPathData != NULL && nPath < pE->nBoundaryPathCount;
					 nPath++)
				{
					const DXFBoundaryPathData & rPath = pE->pBoundaryPathData[nPath];
					if (rPath.bIsPolyLine == sal_True) {
						if (rPath.pP != NULL) {
							for (sal_Int32 i = 0; i < rPath.nPointCount; i++)
								UnionOCS(rBox, aE2W, rPath.pP[i]);
						}
					}
					else {
						for (size_t i = 0; i < rPath.aEdges.size(); i++) {
							const DXFEdgeType * pEdge = rPath.aEdges[i];
							if (pEdge != NULL && pEdge->nEdgeType == 1) {
								const DXFEdgeTypeLine * pLine = (DXFEdgeTypeLine*)pEdge;
								UnionOCS(rBox, aE2W, pLine->aStartPoint);
								UnionOCS(rBox, aE2W, pLine->aEndPoint);
							}
						}
					}
				}
				break;
			}
			case DXF_LWPOLYLINE : {
				// LWPOLYLINE keeps its vertices inline in pP[] (not as
				// separate VERTEX entities), so they must be unioned here
				// or the drawing extent misses the polyline and the graphic
				// is cropped. Walk the same [0,nCount) range that
				// DrawLWPolyLineEntity renders.
				const DXFLWPolyLineEntity * pE = (DXFLWPolyLineEntity*)pBE;
				if (pE->pP != NULL) {
					for (sal_Int32 i = 0; i < pE->nCount; i++)
						UnionOCS(rBox, aE2W, pE->pP[i]);
				}
				break;
			}
			case DXF_ELLIPSE: {
				// Conservative full-ellipse AABB (like ARC uses the full-circle
				// box): centre +/- the axis-projected radii. Over-approximates a
				// partial ellipse, which is fine for a bounding box.
				const DXFEllipseEntity * pE = (DXFEllipseEntity*)pBE;
				const DXFVector & aU = pE->aP1;
				DXFVector aV(-aU.fy*pE->fRatio, aU.fx*pE->fRatio, 0.0);
				double fRx = sqrt(aU.fx*aU.fx + aV.fx*aV.fx);
				double fRy = sqrt(aU.fy*aU.fy + aV.fy*aV.fy);
				DXFVector aC;
				aE2W.Transform(pE->aP0, aC);
				DXFVector aP=aC; aP.fx-=fRx; aP.fy-=fRy; rBox.Union(aP);
				aP=aC; aP.fx+=fRx; aP.fy+=fRy; rBox.Union(aP);
				break;
			}
			case DXF_SPLINE: {
				// The control polygon's convex hull contains the B-spline curve,
				// so unioning the control points bounds it (conservatively).
				const DXFSplineEntity * pE = (DXFSplineEntity*)pBE;
				if (pE->pControlPts != NULL)
					for (long i = 0; i < pE->nCtrlCount; i++)
						UnionOCS(rBox, aE2W, pE->pControlPts[i]);
				break;
			}
		}
		pBE=pBE->pSucc;
	}
}
