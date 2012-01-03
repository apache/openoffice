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



#ifndef _B2D_MATRIX3D_HXX
#define _B2D_MATRIX3D_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _B2D_POINT3D_HXX
#include "point3d.hxx"
#endif

/*************************************************************************
|*
|* homogene 4x4 matrix
|*
\************************************************************************/

namespace binfilter {
class Matrix3D
{
protected:
	Point3D						M[3];

public:
	// default: Einheitsmatrix erstellen (Vektoren sind auf 0
	// initialisiert, der recte Spaltenvektor auf 1)
	Matrix3D() { M[0][0] = M[1][1] = 1.0;
				 M[0][2] = M[1][2] = 0.0; }

	// Zeilenvektor zurueckgeben
	Point3D& operator[](int nPos) { return M[nPos]; }
	const Point3D& operator[](int nPos) const { return M[nPos]; }

	// Spaltenvektor zurueckgeben
	Point3D GetColumnVector(int nCol) const
		{ return Point3D(M[0][nCol], M[1][nCol], M[2][nCol]); }

	// Auf Einheitsmatrix zuruecksetzen
	void Identity(void);

	// Rotation
	void Rotate(double fAngle);
	void Rotate(double fSin, double fCos );

	// Translation
	void Translate(double fX, double fY);
	void Translate(const Vector2D& aTrans);

	// Skalierung
	void Scale(double fX, double fY);
	void Scale(const Vector2D& aScale);

	// Shearing-Matrices
	void ShearX(double fSx);
	void ShearY(double fSy);

	// Addition, Subtraktion
	Matrix3D&	operator+=	(const Matrix3D&);
	Matrix3D&	operator-=	(const Matrix3D&);
	Matrix3D	operator+ 	(const Matrix3D&) const;
	Matrix3D	operator-	(const Matrix3D&) const;

	// Vergleichsoperatoren
	BOOL		operator==	(const Matrix3D&) const;
	BOOL		operator!=	(const Matrix3D&) const;

	// Multiplikation, Division mit Konstante
	Matrix3D&	operator*=	(double);
	Matrix3D	operator*	(double) const;
	Matrix3D&	operator/=	(double);
	Matrix3D	operator/	(double) const;

	// Matritzenmultiplikation von links auf die lokale
	Matrix3D&	operator*=	(const Matrix3D&);
	Matrix3D	operator*	(const Matrix3D&) const;

	// Operatoren zur Punkttransformation
	friend Point3D	operator*	(const Matrix3D&, const Point3D&);
	friend Point3D&	operator*=	(Point3D& rPnt, const Matrix3D& rMat)
		{ return (rPnt = rMat * rPnt); }

	// Operatoren zur Vektortransformation
	friend Vector2D		operator*	(const Matrix3D&, const Vector2D&);
	friend Vector2D&	operator*=	(Vector2D& rVec, const Matrix3D& rMat)
		{ return (rVec = rMat * rVec); }

	// Streamoperatoren
	friend SvStream& operator>>(SvStream& rIStream, Matrix3D&);
	friend SvStream& operator<<(SvStream& rOStream, const Matrix3D&);

	// Help routine to decompose given homogen 3x3 matrix to components. A correction of
	// the components is done to avoid inaccuracies.
	BOOL DecomposeAndCorrect(Vector2D& rScale, double& rShear, double& rRotate, Vector2D& rTranslate) const;
};
}//end of namespace binfilter

#endif          // _B2D_MATRIX3D_HXX
