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



#ifndef _SVDTRANS_HXX
#define _SVDTRANS_HXX

#include <bf_svtools/bf_solar.h>






#include <vcl/field.hxx>
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

// Winkelangaben der DrawingEngine sind 1/100 Degree
// const nWinkDiv=100;
// Um Winkel der DrawingEngine mit den Trigonometrischen Funktionen
// verarbeiten zu koennen, muessen sie zunaest ins Bogenmass umgerechnet
// werden. Dies gestaltet sich recht einfach mit der folgenden Konstanten
// nPi180. Sei nWink ein Winkel in 1/100 Deg so schreibt man z.B.:
//   double nSin=sin(nWink*nPi180);
// Rueckwandlung entsprechend durch Teilen.
const double nPi=3.14159265358979323846;
const double nPi180=0.000174532925199432957692222; // Bei zuweing Stellen ist tan(4500*nPi180)!=1.0

// Der maximale Shearwinkel
#define SDRMAXSHEAR 8900

class XPolygon;
class XPolyPolygon;

inline long Round(double a) { return a>0.0 ? (long)(a+0.5) : -(long)((-a)+0.5); }

inline void MoveRect(Rectangle& rRect, const Size& S)    { rRect.Move(S.Width(),S.Height()); }
inline void MovePoint(Point& rPnt, const Size& S)        { rPnt.X()+=S.Width(); rPnt.Y()+=S.Height(); }
inline void MovePoly(Polygon& rPoly, const Size& S)      { rPoly.Move(S.Width(),S.Height()); }
inline void MovePoly(PolyPolygon& rPoly, const Size& S)  { rPoly.Move(S.Width(),S.Height()); }
void MoveXPoly(XPolygon& rPoly, const Size& S);
void MoveXPoly(XPolyPolygon& rPoly, const Size& S);

void ResizeRect(Rectangle& rRect, const Point& rRef, const Fraction& xFact, const Fraction& yFact, FASTBOOL bNoJustify=FALSE);
inline void ResizePoint(Point& rPnt, const Point& rRef, Fraction xFact, Fraction yFact);
void ResizePoly(Polygon& rPoly, const Point& rRef, const Fraction& xFact, const Fraction& yFact);
void ResizeXPoly(XPolygon& rPoly, const Point& rRef, const Fraction& xFact, const Fraction& yFact);
void ResizeXPoly(XPolyPolygon& rPoly, const Point& rRef, const Fraction& xFact, const Fraction& yFact);

inline void RotatePoint(Point& rPnt, const Point& rRef, double sn, double cs);
void RotatePoly(Polygon& rPoly, const Point& rRef, double sn, double cs);
void RotateXPoly(XPolygon& rPoly, const Point& rRef, double sn, double cs);
void RotateXPoly(XPolyPolygon& rPoly, const Point& rRef, double sn, double cs);

// MirrorRect macht nur Sinn bei Spiegelachsen
// mit einem durch 45 Degree teilbaren Winkel!

inline void ShearPoint(Point& rPnt, const Point& rRef, double tn, FASTBOOL bVShear=FALSE);
void ShearPoly(Polygon& rPoly, const Point& rRef, double tn, FASTBOOL bVShear=FALSE);
void ShearXPoly(XPolygon& rPoly, const Point& rRef, double tn, FASTBOOL bVShear=FALSE);
void ShearXPoly(XPolyPolygon& rPoly, const Point& rRef, double tn, FASTBOOL bVShear=FALSE);

// rPnt.X bzw rPnt.Y wird auf rCenter.X bzw. rCenter.Y gesetzt!
// anschliessend muss rPnt nur noch um rCenter gedreht werden.
// Der Rueckgabewinkel ist ausnahmsweise in Rad.
inline double GetCrookAngle(Point& rPnt, const Point& rCenter, const Point& rRad, FASTBOOL bVertical);
// Die folgenden Methoden behandeln einen Punkt eines XPolygons, wobei die
// benachbarten Kontrollpunkte des eigentlichen Punktes ggf. in pC1/pC2
// uebergeben werden. Ueber rSin/rCos wird gleichzeitig sin(nWink) und cos(nWink)
// zurueckgegeben.
// Der Rueckgabewinkel ist hier ebenfalls in Rad.



/**************************************************************************************************/
/*  Inline                                                                                        */
/**************************************************************************************************/

inline void ResizePoint(Point& rPnt, const Point& rRef, Fraction xFact, Fraction yFact)
{
	if (xFact.GetDenominator()==0) xFact=Fraction(xFact.GetNumerator(),1); // DivZero abfangen
	if (yFact.GetDenominator()==0) yFact=Fraction(yFact.GetNumerator(),1); // DivZero abfangen
	rPnt.X()=rRef.X()+ Round(((double)(rPnt.X()-rRef.X())*xFact.GetNumerator())/xFact.GetDenominator());
	rPnt.Y()=rRef.Y()+ Round(((double)(rPnt.Y()-rRef.Y())*yFact.GetNumerator())/yFact.GetDenominator());
}

inline void RotatePoint(Point& rPnt, const Point& rRef, double sn, double cs)
{
	long dx=rPnt.X()-rRef.X();
	long dy=rPnt.Y()-rRef.Y();
	rPnt.X()=Round(rRef.X()+dx*cs+dy*sn);
	rPnt.Y()=Round(rRef.Y()+dy*cs-dx*sn);
}

inline void ShearPoint(Point& rPnt, const Point& rRef, double tn, FASTBOOL bVShear)
{
	if (!bVShear) { // Horizontal
		if (rPnt.Y()!=rRef.Y()) { // sonst nicht noetig
			rPnt.X()-=Round((rPnt.Y()-rRef.Y())*tn);
		}
	} else { // ansonsten vertikal
		if (rPnt.X()!=rRef.X()) { // sonst nicht noetig
			rPnt.Y()-=Round((rPnt.X()-rRef.X())*tn);
		}
	}
}

inline double GetCrookAngle(Point& rPnt, const Point& rCenter, const Point& rRad, FASTBOOL bVertical)
{
	double nWink;
	if (bVertical) {
		long dy=rPnt.Y()-rCenter.Y();
		nWink=(double)dy/(double)rRad.Y();
		rPnt.Y()=rCenter.Y();
	} else {
		long dx=rCenter.X()-rPnt.X();
		nWink=(double)dx/(double)rRad.X();
		rPnt.X()=rCenter.X();
	}
	return nWink;
}

/**************************************************************************************************/
/**************************************************************************************************/

// Y-Achse zeigt nach unten! Die Funktion negiert bei der
// Winkelberechnung die Y-Achse, sodass GetAngle(Point(0,-1))=90.00?
// GetAngle(Point(0,0)) liefert 0.
// Der Rueckgabewert liegt im Bereich -180.00..179.99 Degree und
// ist in 1/100 Degree angegeben.
long GetAngle(const Point& rPnt);
long NormAngle180(long a); // Winkel normalisieren auf -180.00..179.99
long NormAngle360(long a); // Winkel normalisieren auf    0.00..359.99
// Berechnet die Laenge von (0,0) via a^2 + b^2 = c^2
// Zur Vermeidung von Ueberlaeufen werden ggf. einige Stellen ignoriert.
long GetLen(const Point& rPnt);

/*
  Transformation eines Rechtecks in ein Polygon unter            ?---------?
  Anwendung der Winkelparameter aus GeoStat.                    /1        2/
  Referenzpunkt ist stets der Punkt 0, also die linke          /          /
  obere Ecke des Ausgangsrects.                               /          /
  Bei der Berechnung des Polygons ist die Reihenfolge        /          /
  (erst Shear, dann Rotation vorgegeben).                   /          / \
                                                           /          /   |
  A) Ausgangsrechteck aRect  B) Nach Anwendung von Shear  /0        3/  Rot|
  ?-----------------?      ?-----------------?       ?---------? ------
  |0                1|        \0                1\       C) Nach Anwendung
  |                  |         \                  \      von Rotate
  |                  |       |  \                  \
  |3                2|       |   \3                2\
  ?-----------------?      |    ?-----------------?
							 |Shr |
  Bei Rueckkonvertierung des        Polygons in ein Rect ist die Reihenfolge
  zwangslaeufig umgekehrt:
  - Berechnung des Drehwinkels: Winkel der Strecke 0-1 aus Abb. C) zum Horizont
  - Rueckdrehung des geshearten Rects (man erhaelt Abb B))
  - Bestimmung der Breite des Rects=Laenge der Strecke 0-1 aus Abb. B)
  - Bestimmung der Hoehe des Rects=vertikaler Abstand zwischen den Punkten
	0 und 3 aus Abb. B)
  - Bestimmung des Shear-Winkels aus der Strecke 0-3 zur Senkrechten.
  Es ist darauf zu achten, dass das Polygon bei einer zwischenzeitlichen
  Transformation evtl. gespiegelt wurde (Mirror oder Resize mit neg. Faktor).
  In diesem Fall muss zunaecht eine Normalisierung durch Vertauschung der
  Punkte (z.B. 0 mit 3 und 1 mit 2) durchgefuehrt werden, damit der
  Richtungssinn im Polygon wieder stimmig ist.
  Hinweis: Positiver Shear-Winkel bedeutet Shear mit auf dem Bildschirm
  sichtbarer positiver Kursivierung. Mathematisch waere dass eine negative
  Kursivierung, da die Y-Achse auf dem Bildschirm von oben nach unten verlaeuft.
  Drehwinkel: Positiv bedeutet auf dem Bildschirm sichtbare Linksdrehung.
*/

class GeoStat { // Geometrischer Status fuer ein Rect
public:
	long     nDrehWink;
	long     nShearWink;
	double   nTan;      // tan(nShearWink)
	double   nSin;      // sin(nDrehWink)
	double   nCos;      // cos(nDrehWink)
	FASTBOOL bMirrored; // Horizontal gespiegelt? (ni)
public:
	GeoStat(): nDrehWink(0),nShearWink(0),nTan(0.0),nSin(0.0),nCos(1.0),bMirrored(FALSE) {}
	void RecalcSinCos();
	void RecalcTan();
};

Polygon Rect2Poly(const Rectangle& rRect, const GeoStat& rGeo);
void Poly2Rect(const Polygon& rPol, Rectangle& rRect, GeoStat& rGeo);

// Multiplikation und anschliessende Division.
// Rechnung und Zwischenergebnis sind BigInt.
long BigMulDiv(long nVal, long nMul, long nDiv);

// Fehlerbehaftetes Kuerzen einer Fraction.
// nDigits gibt an, wieviele signifikante Stellen in
// Zaehler/Nenner mindestens erhalten bleiben sollen.
void Kuerzen(Fraction& rF, unsigned nDigits);

class FrPair {
	Fraction aX;
	Fraction aY;
public:
	FrPair()                                          : aX(0,1),aY(0,1)             {}
	FrPair(const Fraction& rBoth)                     : aX(rBoth),aY(rBoth)         {}
	FrPair(const Fraction& rX, const Fraction& rY)    : aX(rX),aY(rY)               {}
	FrPair(long nMul, long nDiv)                      : aX(nMul,nDiv),aY(nMul,nDiv) {}
	FrPair(long xMul, long xDiv, long yMul, long yDiv): aX(xMul,xDiv),aY(yMul,yDiv) {}
	const Fraction& X() const { return aX; }
	const Fraction& Y() const { return aY; }
	Fraction& X()             { return aX; }
	Fraction& Y()             { return aY; }
};

// Fuer die Umrechnung von Masseinheiten
FrPair GetMapFactor(FieldUnit eS, FieldUnit eD);

inline FASTBOOL IsMetric(MapUnit eU) {
	return (eU==MAP_100TH_MM || eU==MAP_10TH_MM || eU==MAP_MM || eU==MAP_CM);
}

inline FASTBOOL IsInch(MapUnit eU) {
	return (eU==MAP_1000TH_INCH || eU==MAP_100TH_INCH || eU==MAP_10TH_INCH || eU==MAP_INCH ||
			eU==MAP_POINT       || eU==MAP_TWIP);
}

inline FASTBOOL IsMetric(FieldUnit eU) {
	return (eU==FUNIT_MM || eU==FUNIT_CM || eU==FUNIT_M || eU==FUNIT_KM || eU==FUNIT_100TH_MM);
}

inline FASTBOOL IsInch(FieldUnit eU) {
	return (eU==FUNIT_TWIP || eU==FUNIT_POINT || eU==FUNIT_PICA ||
			eU==FUNIT_INCH || eU==FUNIT_FOOT || eU==FUNIT_MILE);
}

class SdrFormatter {
	Fraction  aScale;
	long      nMul_;
	long      nDiv_;
	short     nKomma_;
	FASTBOOL  bSrcFU;
	FASTBOOL  bDstFU;
	FASTBOOL  bDirty;
	MapUnit   eSrcMU;
	MapUnit   eDstMU;
	FieldUnit eSrcFU;
	FieldUnit eDstFU;
private:
public:
	SdrFormatter(MapUnit eSrc, MapUnit eDst)     { eSrcMU=eSrc; bSrcFU=FALSE; eDstMU=eDst; bDstFU=FALSE; bDirty=TRUE; }
	SdrFormatter(MapUnit eSrc, FieldUnit eDst)   { eSrcMU=eSrc; bSrcFU=FALSE; eDstFU=eDst; bDstFU=TRUE;  bDirty=TRUE; }
	SdrFormatter(FieldUnit eSrc, MapUnit eDst)   { eSrcFU=eSrc; bSrcFU=TRUE;  eDstMU=eDst; bDstFU=FALSE; bDirty=TRUE; }
	SdrFormatter(FieldUnit eSrc, FieldUnit eDst) { eSrcFU=eSrc; bSrcFU=TRUE;  eDstFU=eDst; bDstFU=TRUE;  bDirty=TRUE; }
	void SetSourceUnit(MapUnit eSrc)        { eSrcMU=eSrc; bSrcFU=FALSE; bDirty=TRUE; }
	void SetSourceUnit(FieldUnit eSrc)      { eSrcFU=eSrc; bSrcFU=TRUE;  bDirty=TRUE; }
	void SetDestinationUnit(MapUnit eDst)   { eDstMU=eDst; bDstFU=FALSE; bDirty=TRUE; }
	void SetDestinationUnit(FieldUnit eDst) { eDstFU=eDst; bDstFU=TRUE;  bDirty=TRUE; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDTRANS_HXX

