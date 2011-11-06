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



#ifndef _SVDOCIRC_HXX
#define _SVDOCIRC_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDORECT_HXX
#include <bf_svx/svdorect.hxx>
#endif
namespace binfilter {

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SdrCircSetItem;

//************************************************************
//   Hilfsklasse SdrCircObjGeoData
//************************************************************

class SdrCircObjGeoData : public SdrRectObjGeoData
{
public:
	long						nStartWink;
	long						nEndWink;
};

//************************************************************
//   SdrCircObj
//************************************************************

class SdrCircObj : public SdrRectObj
{
protected:
	SdrObjKind					eKind;
	long						nStartWink;
	long						nEndWink;
	Point						aPnt1;
	Point						aPnt2;

private:
	XPolygon ImpCalcXPoly(const Rectangle& rRect1, long nStart, long nEnd, FASTBOOL bContour=FALSE) const;
	void ImpSetAttrToCircInfo(); // Werte vom Pool kopieren
	void ImpSetCircInfoToAttr(); // Werte in den Pool kopieren

	// Liefert TRUE, wenn das Painten ein XPolygon erfordert.
	FASTBOOL PaintNeedsXPoly() const;
	virtual void RecalcXPoly();

protected:
	virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType);
	virtual void ForceDefaultAttr();

public:
	TYPEINFO();
	SdrCircObj(SdrObjKind eNewKind); // Circ, CArc, Sect oder CCut
	SdrCircObj(SdrObjKind eNewKind, const Rectangle& rRect);

	// 0=0.00Deg=3h 9000=90.00Deg=12h 18000=180.00Deg=9h 27000=270.00Deg=6h
	// Der Verlauf des Kreises von StartWink nach EndWink ist immer entgegen
	// dem Uhrzeigersinn.
	// Wenn nNewStartWink==nNewEndWink hat der Kreisbogen einen Verlaufswinkel
	// von 0 Grad. Bei nNewStartWink+36000==nNewEndWink ist der Verlaufswinkel
	// 360.00 Grad.
	SdrCircObj(SdrObjKind eNewKind, const Rectangle& rRect, long nNewStartWink, long nNewEndWink);
	virtual ~SdrCircObj();

	virtual UINT16 GetObjIdentifier() const;
	virtual void RecalcBoundRect();
	virtual void TakeUnrotatedSnapRect(Rectangle& rRect) const;
	virtual FASTBOOL Paint(ExtOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const;


	virtual void operator=(const SdrObject& rObj);
	virtual void RecalcSnapRect();
	virtual void NbcSetSnapRect(const Rectangle& rRect);
	virtual void TakeXorPoly(XPolyPolygon& rXPoly, FASTBOOL bDetail) const;
	virtual void TakeContour(XPolyPolygon& rXPoly) const;




	virtual void NbcMove(const Size& aSiz);
	virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
	virtual void NbcShear (const Point& rRef, long nWink, double tn, FASTBOOL bVShear);
	virtual SdrObject* DoConvertToPolyObj(BOOL bBezier) const;

protected:
public:
	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	long GetStartWink() const { return nStartWink; }
	long GetEndWink() const { return nEndWink; }
	virtual void NbcSetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);

	// ItemSet access
	virtual SfxItemSet* CreateNewItemSet(SfxItemPool& rPool);

	// private support routines for ItemSet access. NULL pointer means clear item.
	virtual void ItemSetChanged(const SfxItemSet& rSet);

	// pre- and postprocessing for objects for saving
	virtual void PreSave();
	virtual void PostSave();
};

}//end of namespace binfilter
#endif //_SVDOCIRC_HXX

