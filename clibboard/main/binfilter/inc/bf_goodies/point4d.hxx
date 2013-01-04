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



#ifndef _B3D_POINT4D_HXX
#define _B3D_POINT4D_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SVX_VECTOR3D_HXX //autogen
#include "vector3d.hxx"
#endif

#ifndef _SVX_POINT3D_HXX
#include "point3d.hxx"
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif


/*************************************************************************
|*
|* homogener 4D-Punkt (x,y,z,w)
|*
\************************************************************************/

namespace binfilter {
class Point4D
{
protected:
	// 4 Dimensinen, X,Y,Z und W
	double V[4];

	void	ImplHomogenize();

public:
	Point4D(double fX = 0.0, double fY = 0.0, double fZ = 0.0, double fW = 1.0)
		{ V[0] = fX; V[1] = fY; V[2] = fZ; V[3] = fW; }
	Point4D(const Vector3D& rVec, double fW = 1.0);

	const double& X() const { return V[0]; }
	const double& Y() const { return V[1]; }
	const double& Z() const { return V[2]; }
	const double& W() const { return V[3]; }

	double& X() { return V[0]; }
	double& Y() { return V[1]; }
	double& Z() { return V[2]; }
	double& W() { return V[3]; }

	const double& operator[] (int nPos) const { return V[nPos]; }
	double& operator[] (int nPos) { return V[nPos]; }

	void	Homogenize() { if(V[3] != 1.0) ImplHomogenize(); }
	const Vector3D& GetVector3D() const;

	void CalcInBetween(Point4D& rOld1, Point4D& rOld2, double t);
	void CalcMiddle(Point4D& rOld1, Point4D& rOld2);

	// Rechnen untereinander
	Point4D&	operator+=	(const Point4D&);
	Point4D&	operator-=	(const Point4D&);
	Point4D		operator+ 	(const Point4D&) const;
	Point4D		operator-	(const Point4D&) const;
	Point4D		operator-	(void) const;
	Point4D&	operator*=	(const Point4D&);
	Point4D&	operator/=	(const Point4D&);
	Point4D		operator* 	(const Point4D&) const;
	Point4D		operator/	(const Point4D&) const;

	// Verschiebung um einen Vektor
	Point4D&	operator+=	(const Vector3D&);
	Point4D&	operator-=	(const Vector3D&);
	Point4D		operator+ 	(const Vector3D&) const;
	Point4D		operator-	(const Vector3D&) const;

	// Multiplikation, Division um einen Faktor
	Point4D&	operator*=	(double);
	Point4D		operator*	(double) const;
	Point4D&	operator/=	(double);
	Point4D		operator/	(double) const;

	BOOL		operator==	(const Point4D&) const;
	BOOL		operator!=	(const Point4D&) const;

	friend SvStream& operator>>(SvStream& rIStream, Point4D&);
	friend SvStream& operator<<(SvStream& rOStream, const Point4D&);
};
}//end of namespace binfilter


#endif          // _B3D_POINT4D_HXX
