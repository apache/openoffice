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



#ifndef _SVX_VECTOR3D_HXX
#include "vector3d.hxx"
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

#ifndef _INC_FLOAT
#include <float.h>
#endif

namespace binfilter {
/*************************************************************************
|*
|* Konstruktor aus Point
|*
\************************************************************************/

Vector3D::Vector3D(const Point& rPnt, double fZ)
{
	V[0] = rPnt.X();
	V[1] = rPnt.Y();
	V[2] = fZ;
}

/*************************************************************************
|*
|* Vektorlaenge berechnen
|*
\************************************************************************/

double Vector3D::GetLength(void) const
{
	double fSum = V[0] * V[0] + V[1] * V[1] + V[2] * V[2];
	if(fSum == 0.0 || fSum == 1.0)
		return fSum;
	return sqrt(fSum);
}

/*************************************************************************
|*
|* Laenge der Projektion des Vektors auf die XZ-Ebene berechnen
|*
\************************************************************************/

double Vector3D::GetXZLength(void) const
{
	double fSum = V[0] * V[0] + V[2] * V[2];
	if(fSum == 0.0 || fSum == 1.0)
		return fSum;
	return sqrt(fSum);
}

/*************************************************************************
|*
|* Laenge der Projektion des Vektors auf die YZ-Ebene berechnen
|*
\************************************************************************/

double Vector3D::GetYZLength(void) const
{
	double fSum = V[1] * V[1] + V[2] * V[2];
	if(fSum == 0.0 || fSum == 1.0)
		return fSum;
	return sqrt(fSum);
}

/*************************************************************************
|*
|* Vektor Normalisieren
|*
\************************************************************************/

void Vector3D::Normalize(void)
{
	double fLen = V[0] * V[0] + V[1] * V[1] + V[2] * V[2];

	// nur wirklich rechnen (und Wurzel ziehen), wenn != 0.0
	// und der Faktor nicht schon 1.0, denn dann ist der Vektor
	// bereits normalisiert
	if(fLen == 0.0 || fLen == 1.0)
		return;

	fLen = sqrt(fLen);

	if(fLen == 0.0)
		return;

	V[0] /= fLen;
	V[1] /= fLen;
	V[2] /= fLen;
}

/*************************************************************************
|*
|* Minimum aus diesem und dem uebergebenen Vektor bilden
|*
\************************************************************************/

void Vector3D::Min(const Vector3D& rVec)
{
	if ( V[0] > rVec.V[0] ) V[0] = rVec.V[0];
	if ( V[1] > rVec.V[1] ) V[1] = rVec.V[1];
	if ( V[2] > rVec.V[2] ) V[2] = rVec.V[2];
}

/*************************************************************************
|*
|* Maximum aus diesem und dem uebergebenen Vektor bilden
|*
\************************************************************************/

void Vector3D::Max(const Vector3D& rVec)
{
	if ( V[0] < rVec.V[0] ) V[0] = rVec.V[0];
	if ( V[1] < rVec.V[1] ) V[1] = rVec.V[1];
	if ( V[2] < rVec.V[2] ) V[2] = rVec.V[2];
}

/*************************************************************************
|*
|* Absolutwert
|*
\************************************************************************/

void Vector3D::Abs()
{
	if ( V[0] < 0 ) V[0] = - V[0];
	if ( V[1] < 0 ) V[1] = - V[1];
	if ( V[2] < 0 ) V[2] = - V[2];
}

/*************************************************************************
|*
|* Neue Position an der Stelle t im Parametergebiet des Vektors berechnen
|*
\************************************************************************/

void Vector3D::CalcInBetween(const Vector3D& rOld1, const Vector3D& rOld2, double t)
{
	for(UINT16 i=0;i<3;i++)
	{
		if(rOld2[i] == rOld1[i])
		{
			V[i] = rOld1[i];
		}
		else
		{
			V[i] = ((rOld2[i] - rOld1[i]) * t) + rOld1[i];
		}
	}
}

/*************************************************************************
|*
|* Neue Position in der Mitte des Vektors berechnen
|*
\************************************************************************/

void Vector3D::CalcMiddle(const Vector3D& rOld1, const Vector3D& rOld2)
{
	for(UINT16 i=0;i<3;i++)
	{
		if(rOld2[i] == rOld1[i])
		{
			V[i] = rOld1[i];
		}
		else
		{
			V[i] = (rOld1[i] + rOld2[i]) / 2.0;
		}
	}
}

/*************************************************************************
|*
|* Vektoraddition
|*
\************************************************************************/

Vector3D& Vector3D::operator+= (const Vector3D& rVec)
{
	V[0] += rVec.V[0];
	V[1] += rVec.V[1];
	V[2] += rVec.V[2];
	return *this;
}

/*************************************************************************
|*
|* Vektorsubtraktion
|*
\************************************************************************/

Vector3D& Vector3D::operator-= (const Vector3D& rVec)
{
	V[0] -= rVec.V[0];
	V[1] -= rVec.V[1];
	V[2] -= rVec.V[2];
	return *this;
}

/*************************************************************************
|*
|* Vektoraddition, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator+ (const Vector3D& rVec) const
{
	Vector3D aSum = *this;

	aSum += rVec;
	return aSum;
}

/*************************************************************************
|*
|* Vektorsubtraktion, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator- (const Vector3D& rVec) const
{
	Vector3D aSub = *this;

	aSub -= rVec;
	return aSub;
}

/*************************************************************************
|*
|* Vektornegation
|*
\************************************************************************/

Vector3D Vector3D::operator- (void) const
{
	Vector3D aVec;

	aVec[0] = -V[0];
	aVec[1] = -V[1];
	aVec[2] = -V[2];
	return aVec;
}

/*************************************************************************
|*
|* Kreuzprodukt
|*
\************************************************************************/

Vector3D& Vector3D::operator|=(const Vector3D& rVec)
{
	// this sichern, da Elemente ueberschrieben werden
	Vector3D aVec = *this;
	V[0] = aVec.V[1] * rVec.V[2] - aVec.V[2] * rVec.V[1];
	V[1] = aVec.V[2] * rVec.V[0] - aVec.V[0] * rVec.V[2];
	V[2] = aVec.V[0] * rVec.V[1] - aVec.V[1] * rVec.V[0];
	return *this;
}

Vector3D Vector3D::operator|(const Vector3D& rVec) const
{
	Vector3D aCross = *this;
	aCross |= rVec;
	return aCross;
}

/*************************************************************************
|*
|* Skalarprodukt
|*
\************************************************************************/

double Vector3D::Scalar(const Vector3D& rVec) const
{
	return ((V[0] * rVec.V[0]) + (V[1] * rVec.V[1]) + (V[2] * rVec.V[2]));
}

/*************************************************************************
|*
|* Vektormultiplikation
|*
\************************************************************************/

Vector3D& Vector3D::operator*= (const Vector3D& rVec)
{
	V[0] *= rVec.V[0];
	V[1] *= rVec.V[1];
	V[2] *= rVec.V[2];
	return *this;
}

/*************************************************************************
|*
|* Vektormultiplikation, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator* (const Vector3D& rVec) const
{
	Vector3D aMul = *this;

	aMul *= rVec;
	return aMul;
}

/*************************************************************************
|*
|* Vektordivision
|*
\************************************************************************/

Vector3D& Vector3D::operator/= (const Vector3D& rVec)
{
	if(rVec.V[0] != 0.0)
		V[0] /= rVec.V[0];
	else
		V[0] = DBL_MAX;

	if(rVec.V[1] != 0.0)
		V[1] /= rVec.V[1];
	else
		V[1] = DBL_MAX;

	if(rVec.V[2] != 0.0)
		V[2] /= rVec.V[2];
	else
		V[2] = DBL_MAX;
	return *this;
}

/*************************************************************************
|*
|* Vektordivision, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator/ (const Vector3D& rVec) const
{
	Vector3D aDiv = *this;

	aDiv /= rVec;
	return aDiv;
}

/*************************************************************************
|*
|* Skalarmultiplikation
|*
\************************************************************************/

Vector3D& Vector3D::operator*= (double fFactor)
{
	V[0] *= fFactor;
	V[1] *= fFactor;
	V[2] *= fFactor;
	return *this;
}

/*************************************************************************
|*
|* Skalarmultiplikation, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator* (double fFactor) const
{
	Vector3D aNewVec = *this;

	aNewVec *= fFactor;
	return aNewVec;
}

/*************************************************************************
|*
|* Skalardivision
|*
\************************************************************************/

Vector3D& Vector3D::operator/= (double fDiv)
{
	if (fDiv != 0.0)
	{
		V[0] /= fDiv;
		V[1] /= fDiv;
		V[2] /= fDiv;
	}
	return *this;
}

/*************************************************************************
|*
|* Skalardivision, neuen Vektor erzeugen
|*
\************************************************************************/

Vector3D Vector3D::operator/ (double fDiv) const
{
	Vector3D aNewVec = *this;

	if (fDiv != 0.0) aNewVec /= fDiv;
	return aNewVec;
}

/*************************************************************************
|*
|* Skalarmultiplikation, neuen Vektor erzeugen
|*
\************************************************************************/

BOOL Vector3D::operator==(const Vector3D& rVec) const
{
	return (V[0] == rVec.V[0] && V[1] == rVec.V[1] && V[2] == rVec.V[2] );
}

/*************************************************************************
|*
|* Skalarmultiplikation, neuen Vektor erzeugen
|*
\************************************************************************/

BOOL Vector3D::operator!=(const Vector3D& rVec) const
{
	return (V[0] != rVec.V[0] || V[1] != rVec.V[1] || V[2] != rVec.V[2] );
}

/*************************************************************************
|*
|* Stream-In-Operator fuer Vector3D
|*
\************************************************************************/

SvStream& operator>>(SvStream& rIStream, Vector3D& rVector3D)
{
	for (int i = 0; i < 3; i++)
		rIStream >> rVector3D.V[i];

	return rIStream;
}

/*************************************************************************
|*
|* Stream-Out-Operator fuer Vector3D
|*
\************************************************************************/

SvStream& operator<<(SvStream& rOStream, const Vector3D& rVector3D)
{
	for (int i = 0; i < 3; i++)
		rOStream << rVector3D.V[i];

	return rOStream;
}

}//end of namespace binfilter

// eof



