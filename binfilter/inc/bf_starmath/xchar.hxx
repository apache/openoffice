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



#ifndef XCHAR_HXX
#define XCHAR_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SV_GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
#ifndef _SV_OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _XPOLY_HXX //autogen
#include <bf_svx/xpoly.hxx>
#endif


#include "rect.hxx"
namespace binfilter {




////////////////////////////////////////
// SmPolygon
//

class SmPolygonLoader;

class SmPolygon
{
	friend class SmPolygonLoader;

	XPolyPolygon	aPoly;
	Rectangle	  	aBoundRect;	// das umgebende Rechteck (ohne Rand!)
								// wie von OutputDevice::GetBoundRect
	Size			aFontSize;  // vom Originalzeichen (Width != 0 aus FontMetric)
	Point			aPos;
	Size		  	aOrigSize;	// Originalgröße (des BoundRect)
	Point			aOrigPos;	// Offset des BoundRect im Originalzeichen

	double		  	fScaleX,    // Skalierungsfaktoren der aktuellen (gewollten)
					fScaleY,	// Größe gegenüber der Originalgröße

					fDelayedFactorX,	// dienen zum sammeln der Änderungen
					fDelayedFactorY;	// bis tatsächlich skaliert wird

    sal_Unicode     cChar;
	BOOL			bDelayedScale,
					bDelayedBoundRect;

	void			Scale();

public:
	SmPolygon();
    SmPolygon(sal_Unicode cChar);
	// default copy-constructor verwenden

	const Size &  	GetOrigFontSize() const { return aFontSize; };
	const Size &	GetOrigSize() const { return aOrigSize; }
	const Point	&	GetOrigPos() const { return aOrigPos; }

	void 			ScaleBy(double fFactorX, double fFactorY);
	double			GetScaleX() const { return fScaleX * fDelayedFactorX; }
	double			GetScaleY() const { return fScaleY * fDelayedFactorY; }

	void 			AdaptToX(const OutputDevice &rDev, ULONG nWidth);
	void 			AdaptToY(const OutputDevice &rDev, ULONG nHeight);

	void 			Move  (const Point &rPoint);
	void 			MoveTo(const Point &rPoint) { Move(rPoint - aPos); }

    const sal_Unicode       GetChar() const { return cChar; }
	const Rectangle & 		GetBoundRect(const OutputDevice &rDev) const;


	// default assignment-operator verwenden

	friend SvStream & operator >> (SvStream &rIStream, SmPolygon &rPoly);
	friend SvStream & operator << (SvStream &rOStream, const SmPolygon &rPoly);
};



} //namespace binfilter
#endif
