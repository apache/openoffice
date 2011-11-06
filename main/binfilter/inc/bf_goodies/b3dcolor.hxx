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



#ifndef _B3D_B3DCOLOR_HXX
#define _B3D_B3DCOLOR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

/*************************************************************************
|*
|* Farbklasse fuer 3D. Besitzt einige Funktionen mehr, als die normale
|* Farbe
|*
\************************************************************************/

namespace binfilter {
class B3dColor : public Color
{
public:
	B3dColor() : Color() {}
	B3dColor( ColorData nColor ) : Color(nColor) {}
	B3dColor( UINT8 nRed, UINT8 nGreen, UINT8 nBlue )
		: Color(nRed, nGreen, nBlue) {}
	B3dColor( UINT8 nTransparency, UINT8 nRed, UINT8 nGreen, UINT8 nBlue )
		: Color(nTransparency, nRed, nGreen, nBlue) {}
	B3dColor( const ResId& rResId ) : Color(rResId) {}
	B3dColor( const Color& rCol ) : Color(rCol) {}

	void CalcInBetween(Color& rOld1, Color& rOld2, double t);
	void CalcMiddle(Color& rOld1, Color& rOld2);

	// Addition, Subtraktion mit clamping
	B3dColor&	operator+=	(const B3dColor&);
	B3dColor&	operator-=	(const B3dColor&);
	B3dColor	operator+ 	(const B3dColor&) const;
	B3dColor	operator-	(const B3dColor&) const;

	// Multiplikation als Gewichtung, Anwendung einer Lampe
	// auf eine Farbe, Lampe als 2.Faktor
	B3dColor&	operator*=	(const B3dColor&);
	B3dColor	operator* 	(const B3dColor&) const;

	// Multiplikation mit Faktor im Bereich [0.0 .. 1.0]
	B3dColor&	operator*=	(const double);
	B3dColor	operator* 	(const double) const;

	// Zuweisung
	void operator=(const Color& rCol) { mnColor = rCol.GetColor(); }
};
}//end of namespace binfilter

#endif          // _B3D_B3DCOLOR_HXX
