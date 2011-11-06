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



#ifndef _SVDVMARK_HXX
#define _SVDVMARK_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif

namespace binfilter {

//************************************************************
//   Vorausdeklarationen
//************************************************************

class XPolygon;
class XPolyPolygon;
class SdrObject;
class SdrPageView;
class SdrPaintView;

//************************************************************
//   Defines
//************************************************************

enum SdrViewUserMarkerAlign
{
	SDRMARKER_ALIGNCENTER,
	SDRMARKER_ALIGNINNER,
	SDRMARKER_ALIGNOUTER
};

//************************************************************
//   SdrViewUserMarker
//************************************************************

class SdrViewUserMarker
{
	friend class				SdrPaintView;

	SdrPaintView*				pView;
	OutputDevice*				pForcedOutDev;
	Point*						pPoint;
	Rectangle*					pRect;
	Polygon*					pPoly;
	PolyPolygon*				pPolyPoly;
	XPolygon*					pXPoly;
	XPolyPolygon*				pXPolyPoly;

	SdrViewUserMarkerAlign		eAlign;
	USHORT						nPixelDistance;      // Abstand des Rahmen zum eigentlichen pRect
	USHORT						nLineWdt;            // Strichstaerke
	USHORT						nCrossSize;          // fuer CrossHair und Stripes. 0=FullWindow
	FASTBOOL					bLineWdtLog;
	FASTBOOL					bCrossSizeLog;
	FASTBOOL					bSolidArea;
	FASTBOOL					bDashed;
	FASTBOOL					bCrossHair;          // Bei pPoint: Fadenkreuz. Sonst Dot (Rect oder Circle)
	FASTBOOL					bStripes;            // Bei pRect
	FASTBOOL					bEllipse;            // Bei pRect oder pPoint
	FASTBOOL					bPolyLine;           // Bei pPoly oder pXPoly
	FASTBOOL					bAnimate;
	FASTBOOL					bVisible;

	USHORT						nAnimateDelay;
	USHORT						nAnimateSpeed;
	USHORT						nAnimateAnz;
	FASTBOOL					bAnimateBwd;
	FASTBOOL					bAnimateToggle;
	USHORT						nAnimateDelayCountDown;
	USHORT						nAnimateSpeedCountDown;
	USHORT						nAnimateNum;

	FASTBOOL					bHasPointer;
	FASTBOOL					bMouseMovable;

protected:
	void ImpDelGeometrics();

public:
	SdrViewUserMarker(SdrPaintView* pView);

	// Jede Ableitung, die ImpDraw ueberladen hat muss
	// im Dtor IsVisible() fragen und ggf. Hiden!
	virtual ~SdrViewUserMarker();

	// Falls der Marker nur in einem bestimmten Window der View
	// dargestellt werden soll. Default ist NULL= alle Windows der View.
	void SetOutputDevice(OutputDevice* pOut) { pForcedOutDev=pOut; }
	OutputDevice* GetOutputDevice() const { return pForcedOutDev; }

	// Der Marker kann durchaus auch in einen andere View gesteckt werden.
	// (Beim Destruieren der View wird die View am Marker automatisch auf NULL gesetzt.)

	void SetLineWidth(USHORT nWdt);

	void SetAnimateDelay(USHORT nTime) { nAnimateDelay=(nTime+25)/50; }
	void SetAnimateSpeed(USHORT nTime) { nAnimateSpeed=(nTime+25)/50; if (nAnimateSpeed>0) nAnimateSpeed--; }
	void SetAnimateCount(USHORT nAnz) { nAnimateAnz=nAnz; }
	void SetAnimateBackward(FASTBOOL bOn) { bAnimateBwd=bOn; }
	void SetAnimateToggle(FASTBOOL bOn) { bAnimateToggle=bOn; }

	const Point* GetPoint() const { return pPoint; }
	const Rectangle* GetRectangle() const { return pRect; }
	const Polygon* GetPolygon() const { return pPoly; }
	const PolyPolygon* GetPolyPolygon() const { return pPolyPoly; }
	const XPolygon* GetXPolygon() const { return pXPoly; }
	const XPolyPolygon* GetXPolyPolygon() const { return pXPolyPoly; }

	FASTBOOL IsDot() const { return pPoint!=NULL && !bCrossHair; }
	FASTBOOL IsCrossHair() const { return pPoint!=NULL && bCrossHair; }
	FASTBOOL IsRectangle() const { return pRect!=NULL && !bEllipse && !bStripes; }
	FASTBOOL IsStripes() const { return pRect!=NULL && bStripes; }
	FASTBOOL IsEllipse() const { return pRect!=NULL && bEllipse; }
	FASTBOOL IsPolyLine() const { return (pPoly!=NULL || pPolyPoly!=NULL || pXPoly!=NULL || pXPolyPoly!=NULL) && bPolyLine; }

	FASTBOOL IsSolidArea() const { return bSolidArea && pPoint==NULL && !bPolyLine; }
	FASTBOOL IsDashed() const { return bDashed; }
	USHORT GetPixelDistance() const { return nPixelDistance; }
	USHORT GetLineWidth() const { return nLineWdt; }
	FASTBOOL IsLineWidthIsLogic() const { return bLineWdtLog; }
	USHORT GetCrossHairSize() const { return nCrossSize; }
	FASTBOOL IsCrossHairSizeIsLogic() const { return bCrossSizeLog; }

	FASTBOOL IsAnimate() const { return bAnimate; }
	USHORT GetAnimateDelay() const { return nAnimateDelay*50; }
	USHORT GetAnimateSpeed() const { return (nAnimateSpeed+1)*50; }
	USHORT GetAnimateCount() const { return nAnimateAnz; }
	FASTBOOL IsAnimateBackward() const { return bAnimateBwd; }
	FASTBOOL IsAnimateToggle() const { return bAnimateToggle; }

	void Show();
	void Hide();
	FASTBOOL IsVisible() const { return bVisible; }

	FASTBOOL HasPointer() const { return bHasPointer; }
	FASTBOOL IsMouseMovable() const { return bMouseMovable; }
};

}//end of namespace binfilter
#endif //_SVDVMARK_HXX

