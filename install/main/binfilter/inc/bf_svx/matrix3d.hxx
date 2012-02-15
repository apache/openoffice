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



#ifndef _SVX_MATRIX3D_HXX
#define _SVX_MATRIX3D_HXX

#ifndef _B3D_HMATRIX_HXX
#include <bf_goodies/hmatrix.hxx>
#endif

namespace binfilter {

/*************************************************************************
|*
|* 3D-Transformationsmatrix, bestehend aus 3 Zeilenvektoren und
|* einem Spaltenvektor, der die Translation enthaelt:
|*
|*      |V11 V12 V13 T1|
|*  M = |V21 V22 C33 T2|
|*      |V31 V32 V33 T3|
|*
|* Vektortransformation durch Postmultiplikation mit Spaltenvektoren:
|* V' = M * V
|* Matrixverkettung per Postmultiplikation mit Transformation oder
|* anderer Matrix:
|* M' = [T,S,Rxyz] * M oder
|* M' = M2 * M
|*
\************************************************************************/

class Old_Matrix3D
{
protected:
	Vector3D					M[3];
	Vector3D					aTranslation;

 public:
	// default: Einheitsmatrix erstellen (Vektoren sind auf 0 initialisiert)
	Old_Matrix3D() { M[0][0] = M[1][1] = M[2][2] = 1; }

	// Zeilenvektor zurueckgeben
	Vector3D& operator[](int nPos) { return M[nPos]; }
	const Vector3D& operator[](int nPos) const { return M[nPos]; }

	// Spaltenvektor zurueckgeben
	Vector3D GetColumnVector(int nCol) const
		{ return Vector3D(M[0][nCol], M[1][nCol], M[2][nCol]); }

	friend SvStream& operator>>(SvStream& rIStream, Old_Matrix3D&);
	friend SvStream& operator<<(SvStream& rOStream, const Old_Matrix3D&);

	operator Matrix4D();
	Old_Matrix3D operator=(const Matrix4D&);
};

}//end of namespace binfilter
#endif
