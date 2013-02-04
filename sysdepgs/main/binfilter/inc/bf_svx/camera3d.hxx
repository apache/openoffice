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



#ifndef _CAMERA3D_HXX
#define _CAMERA3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _VIEWPT3D_HXX
#include <bf_svx/viewpt3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Kamera-Ableitung
|*
\************************************************************************/

class Camera3D : public Viewport3D
{
 protected:
	Vector3D	aResetPos;
	Vector3D	aResetLookAt;
	double		fResetFocalLength;
	double		fResetBankAngle;

	Vector3D	aPosition;
	Vector3D	aLookAt;
	double		fFocalLength;
	double		fBankAngle;

	FASTBOOL	bAutoAdjustProjection;

 public:
	Camera3D(const Vector3D& rPos, const Vector3D& rLookAt,
			 double fFocalLen = 35.0, double fBankAng = 0);
	Camera3D();

	// Anfangswerte wieder herstellen
	void Reset();

	void SetDefaults(const Vector3D& rPos, const Vector3D& rLookAt,
					 double fFocalLen = 35.0, double fBankAng = 0);

	void SetViewWindow(double fX, double fY, double fW, double fH);

	void SetPosition(const Vector3D& rNewPos);
	const Vector3D& GetPosition() const { return aPosition; }
	void SetLookAt(const Vector3D& rNewLookAt);
	const Vector3D& GetLookAt() const { return aLookAt; }
	void SetPosAndLookAt(const Vector3D& rNewPos, const Vector3D& rNewLookAt);

	// Brennweite in mm
	void	SetFocalLength(double fLen);
	double	GetFocalLength() const { return fFocalLength; }

	// Neigung links/rechts
	void	SetBankAngle(double fAngle);
	double	GetBankAngle() const { return fBankAngle; }

	// Um den Blickpunkt drehen, Position wird dabei veraendert
	void RotateAroundLookAt(double fHAngle, double fVAngle);

	void SetAutoAdjustProjection(FASTBOOL bAdjust = TRUE)
		{ bAutoAdjustProjection = bAdjust; }
	FASTBOOL IsAutoAdjustProjection() const { return bAutoAdjustProjection; }

	// Die Umstellung von Filerevision 12 auf 13 erfolgte zur 355
	// 5.2.1997 Franz Gotsis, ReadData31 kann also auch die
	// zur 4.0 gehoerigen Filrevisionen bis zu 12 lesen.
	virtual void WriteData31(SvStream& rOut) const;
	virtual void ReadData31(SvStream& rIn);

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);
};

SvStream& operator<<(SvStream& rOStream, const Viewport3D&);
SvStream& operator>>(SvStream& rIStream, Viewport3D&);

}//end of namespace binfilter
#endif		// _CAMERA3D_HXX
