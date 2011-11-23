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



#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif

#ifndef _E3D_GLOBL3D_HXX
#include "globl3d.hxx"
#endif

#ifndef _E3D_POLYOB3D_HXX
#include "polyob3d.hxx"
#endif

#ifndef _E3D_DLIGHT3D_HXX
#include "dlight3d.hxx"
#endif

namespace binfilter {

/*N*/ TYPEINIT1(E3dDistantLight, E3dLight);

/*************************************************************************
|*
|* E3dDistantLight-Konstruktor
|*
\************************************************************************/

/*N*/ E3dDistantLight::E3dDistantLight(const Vector3D& rPos, const Vector3D& rDir,
/*N*/ 								 const Color& rColor, double fLightIntensity) :
/*N*/ 	E3dLight(rPos, rColor, fLightIntensity)
/*N*/ {
/*N*/ 	SetDirection(rDir);
/*N*/ 	CreateLightObj();
/*N*/ }

/*************************************************************************
|*
|* E3dDistantLight-Destruktor
|*
\************************************************************************/

/*N*/ E3dDistantLight::~E3dDistantLight()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

/*N*/ UINT16 E3dDistantLight::GetObjIdentifier() const
/*N*/ {
/*N*/ 	return E3D_DISTLIGHT_ID;
/*N*/ }

/*************************************************************************
|*
|* Richtung ZUR Lichtquelle setzen
|*
\************************************************************************/

/*N*/ void E3dDistantLight::SetDirection(const Vector3D& rNewDir)
/*N*/ {
/*N*/ 	if ( aDirection != rNewDir )
/*N*/ 	{
/*N*/ 		aDirection = rNewDir;
/*N*/ 		aDirection.Normalize();
/*N*/ 		CreateLightObj();
/*N*/ 		StructureChanged(this);
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* Lichtstaerke an uebergeben Punkt bestimmen und die akkumulierte
|* Punktfarbe berechnen
|*
\************************************************************************/

/*?*/ FASTBOOL E3dDistantLight::CalcLighting(Color& rNewColor,
/*?*/ 										const Vector3D& rPnt,
/*?*/ 										const Vector3D& rPntNormal,
/*?*/ 										const Color& rPntColor)
/*?*/ {
/*?*/ 	double fR = 0, fG = 0, fB = 0;
/*?*/ 
/*?*/ 	if ( IsOn() )
/*?*/ 	{
/*?*/ 		double fLight = rPntNormal.Scalar(aDirection);
/*?*/ 
/*?*/ 		if ( fLight > 0 )
/*?*/ 		{
/*?*/ 			fR = fLight * GetRed();
/*?*/ 			fG = fLight * GetGreen();
/*?*/ 			fB = fLight * GetBlue();
/*?*/ 		}
/*?*/ 	}
/*?*/ 	return ImpCalcLighting(rNewColor, rPntColor, fR, fG, fB);
/*?*/ }

/*************************************************************************
|*
|* Darstellung des Lichtobjekts in der Szene ein-/ausschalten
|*
\************************************************************************/

/*N*/ void E3dDistantLight::CreateLightObj()
/*N*/ {
/*N*/ 	pSub->Clear();
/*N*/ 
/*N*/ 	if ( IsLightObjVisible() )
/*N*/ 	{
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() + aDirection, GetPosition()));
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* sichern
|*
\************************************************************************/

/*N*/ void E3dDistantLight::WriteData(SvStream& rOut) const
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	E3dLight::WriteData(rOut);
/*N*/ 
/*N*/ #ifdef E3D_STREAMING
/*N*/ 	SdrDownCompat aCompat(rOut, STREAM_WRITE);
/*N*/ #ifdef DBG_UTIL
/*N*/ 	aCompat.SetID("E3dDistantLight");
/*N*/ #endif
/*N*/ 
/*N*/ 	rOut << aDirection;
/*N*/ #endif
/*N*/ #endif	// #ifndef SVX_LIGHT
/*N*/ }

/*************************************************************************
|*
|* laden
|*
\************************************************************************/

/*N*/ void E3dDistantLight::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
/*N*/ {
/*N*/ 	if (ImpCheckSubRecords (rHead, rIn))
/*N*/ 	{
/*N*/ 		E3dLight::ReadData(rHead, rIn);
/*N*/ 
/*N*/ 		SdrDownCompat aCompat(rIn, STREAM_READ);
/*N*/ #ifdef DBG_UTIL
/*N*/ 		aCompat.SetID("E3dDistantLight");
/*N*/ #endif
/*N*/ 
/*N*/ 		rIn >> aDirection;
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* Zuweisungsoperator
|*
\************************************************************************/



}
