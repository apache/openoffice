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



#ifndef _B3D_B3DTRANS_HXX
#define _B3D_B3DTRANS_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _B3D_HMATRIX_HXX
#include "hmatrix.hxx"
#endif

// Zu verwendender DephRange des Z-Buffers
#define ZBUFFER_DEPTH_RANGE			(256.0 * 256.0 * 256.0)

namespace binfilter {

// Vorausdeklarationen
//class Base3D;
class B3dVolume;

/*************************************************************************
|*
|* Unterstuetzte Methoden, um das Seitenverhaeltnis einzuhalten
|*
\************************************************************************/

enum Base3DRatio
{
	Base3DRatioGrow = 1,
	Base3DRatioShrink,
	Base3DRatioMiddle
};

/*************************************************************************
|*
|* Typ der Projektion
|*
\************************************************************************/

enum Base3DProjectionType
{
	Base3DProjectionTypeParallel = 1,
	Base3DProjectionTypePerspective
};

/*************************************************************************
|*
|* Transformationen fuer alle 3D Ausgaben
|*
\************************************************************************/

class B3dTransformationSet
{
private:
	// Object Matrix Object -> World
	Matrix4D				aObjectTrans;
	Matrix4D				aInvObjectTrans;

	// Orientation Matrix
	Matrix4D				aOrientation;
	Matrix4D				aInvOrientation;

	// Projection Matrix
	Matrix4D				aProjection;
	Matrix4D				aInvProjection;

	// Texture Matrices
	Matrix4D				aTexture;

	// Speziell zum Umwandeln von Punkten Objekt -> Device
	Matrix4D				aObjectToDevice;

	// Transponierte Inverse fuer Vectortransformationen
	Matrix4D				aInvTransObjectToEye;

	// Transformation World->View
	Matrix4D				aMatFromWorldToView;
	Matrix4D				aInvMatFromWorldToView;

	// Parameters for ViewportTransformation
	Vector3D				aScale;
	Vector3D				aTranslate;

	// ViewPlane DeviceRectangle (vom Benutzer gesetzt)
	double					fLeftBound;
	double					fRightBound;
	double					fBottomBound;
	double					fTopBound;

	// Near and far clipping planes
	double					fNearBound;
	double					fFarBound;

	// Seitenverhaeltnis der 3D Abbildung (Y / X)
	// default ist 1:1 -> 1.0
	// Deaktivieren mit 0.0 als Wert
	double					fRatio;

	// Der gesetzte Ausgabebereich (in logischen Koordinaten)
	// und der dazugehoerige sichtbare Bereich
	Rectangle				aViewportRectangle;
	Rectangle				aVisibleRectangle;

	// Die tatsaechlich von CalcViewport gesetzten Abmessungen
	// des sichtbaren Bereichs (in logischen Koordinaten)
	Rectangle				aSetBound;

	// Methode zur Aufrechterhaltung des Seitenverhaeltnisses
	// default ist Base3DRatioGrow
	Base3DRatio				eRatio;

	// Flags
	unsigned				bPerspective				: 1;
	unsigned				bWorldToViewValid			: 1;
	unsigned				bInvTransObjectToEyeValid	: 1;
	unsigned				bObjectToDeviceValid		: 1;
	unsigned				bProjectionValid			: 1;

public:
	B3dTransformationSet();

	// Zurueck auf Standard
	void Reset();

	// ObjectTrans
	void SetObjectTrans(Matrix4D& rObj);
	const Matrix4D& GetObjectTrans() { return aObjectTrans; }
	const Matrix4D& GetInvObjectTrans() { return aInvObjectTrans; }

	// Orientation
#if ! defined ICC && ! defined __GNUC__
	void SetOrientation(Vector3D& aVRP = Vector3D(0.0,0.0,1.0),
		Vector3D& aVPN = Vector3D(0.0,0.0,1.0),
		Vector3D& aVUP = Vector3D(0.0,1.0,0.0));
#else
	void SetOrientation(Vector3D aVRP = Vector3D(0.0,0.0,1.0),
		Vector3D aVPN = Vector3D(0.0,0.0,1.0),
		Vector3D aVUP = Vector3D(0.0,1.0,0.0));
#endif
	const Matrix4D& GetOrientation() { return aOrientation; }
	const Matrix4D& GetInvOrientation() { return aInvOrientation; }

	// Projection
	void SetProjection(Matrix4D& mProject);
	const Matrix4D& GetProjection();
	const Matrix4D& GetInvProjection();

	// Texture
	const Matrix4D& GetTexture() { return aTexture; }

	// Seitenverhaeltnis und Modus zu dessen Aufrechterhaltung
	double GetRatio() { return fRatio; }
	void SetRatio(double fNew=1.0);
	Base3DRatio GetRatioMode() { return eRatio; }

	// Parameter der ViewportTransformation
	void SetDeviceRectangle(double fL=-1.0, double fR=1.0, double fB=-1.0, double fT=1.0,
		BOOL bBroadCastChange=TRUE);
	void SetDeviceVolume(const B3dVolume& rVol, BOOL bBroadCastChange=TRUE);
	double GetDeviceRectangleWidth() const { return fRightBound - fLeftBound; }
	double GetDeviceRectangleHeight() const { return fTopBound - fBottomBound; }
	void SetFrontClippingPlane(double fF=0.0);
	double GetFrontClippingPlane() { return fNearBound; }
	void SetBackClippingPlane(double fB=1.0);
	double GetBackClippingPlane() { return fFarBound; }
	void SetPerspective(BOOL bNew);
	BOOL GetPerspective() { return bPerspective; }
	void SetViewportRectangle(Rectangle& rRect, Rectangle& rVisible);
	void SetViewportRectangle(Rectangle& rRect) { SetViewportRectangle(rRect, rRect); }
	const Rectangle& GetViewportRectangle() { return aViewportRectangle; }
	void CalcViewport();

	// Speziell zum Umwandeln von Punkten World -> View
	const Matrix4D& GetMatFromWorldToView();
	const Matrix4D& GetInvMatFromWorldToView();

	// Bounds des Viewports lesen
	const Vector3D& GetScale();
	const Vector3D& GetTranslate();

	// Direkter Zugriff auf verschiedene Transformationen
	const Vector3D WorldToEyeCoor(const Vector3D& rVec);
	const Vector3D EyeToWorldCoor(const Vector3D& rVec);
	const Vector3D ViewToEyeCoor(const Vector3D& rVec);
	const Vector3D WorldToViewCoor(const Vector3D& rVec);
	const Vector3D ObjectToWorldCoor(const Vector3D& rVec);
	const Vector3D ViewToObjectCoor(const Vector3D& rVec);
	const Vector3D ObjectToEyeCoor(const Vector3D& rVec);
protected:
	void PostSetObjectTrans();
	void PostSetOrientation();
	void PostSetProjection();
	void PostSetViewport();

	void CalcMatFromWorldToView();

	virtual void DeviceRectangleChange();
};

/*************************************************************************
|*
|* Viewport fuer B3D
|*
|* Verwendet wird hier ein vereinfachtes System, bei dem der abzubildende
|* Punkt durch VRP repraesentiert wird
|*
\************************************************************************/

class B3dViewport : public B3dTransformationSet
{
private:
	Vector3D				aVRP;			// View Reference Point
	Vector3D				aVPN;			// View Plane Normal
	Vector3D				aVUV;			// View Up Vector

public:
	B3dViewport();

	void SetVUV(const Vector3D& rNewVUV);
	void SetViewportValues(
		const Vector3D& rNewVRP,
		const Vector3D& rNewVPN,
		const Vector3D& rNewVUV);

	const Vector3D&	GetVRP() const	{ return aVRP; }
	const Vector3D&	GetVPN() const	{ return aVPN; }
	const Vector3D&	GetVUV() const	{ return aVUV; }

protected:
	void CalcOrientation();
};

/*************************************************************************
|*
|* Kamera fuer B3D
|*
\************************************************************************/

class B3dCamera : public B3dViewport
{
private:
	Vector3D				aPosition;
	Vector3D				aCorrectedPosition;
	Vector3D				aLookAt;
	double					fFocalLength;
	double					fBankAngle;

	unsigned				bUseFocalLength			: 1;

public:
	B3dCamera(const Vector3D& rPos = Vector3D(0.0, 0.0, 1.0),
		const Vector3D& rLkAt = Vector3D(0.0, 0.0, 0.0),
		double fFocLen = 35.0, double fBnkAng = 0.0,
		BOOL bUseFocLen = FALSE);

	// Positionen
	const Vector3D& GetPosition() const { return aPosition; }
	const Vector3D& GetLookAt() const { return aLookAt; }

	// Brennweite in mm
	double GetFocalLength() const { return fFocalLength; }

	// Neigung links/rechts
	double GetBankAngle() const { return fBankAngle; }

	// FocalLength Flag
	BOOL GetUseFocalLength() const { return (BOOL)bUseFocalLength; }

protected:
	void CalcNewViewportValues();
	BOOL CalcFocalLength();

	virtual void DeviceRectangleChange();
};
}//end of namespace binfilter

#endif          // _B3D_B3DTRANS_HXX
