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





#ifndef _E3D_POLYOB3D_HXX
#include "polyob3d.hxx"
#endif

#ifndef _E3D_PLIGHT3D_HXX
#include "plight3d.hxx"
#endif
namespace binfilter {

/*N*/ TYPEINIT1(E3dPointLight, E3dLight);

/*************************************************************************
|*
|* E3dPointLight-Konstruktor
|*
\************************************************************************/

/*N*/ E3dPointLight::E3dPointLight(const Vector3D& rPos, const Color& rColor,
/*N*/ 							 double fLightIntensity) :
/*N*/ 	E3dLight(rPos, rColor, fLightIntensity)
/*N*/ {
/*N*/ 	CreateLightObj();
/*N*/ }

/*************************************************************************
|*
|* E3dPointLight-Destruktor
|*
\************************************************************************/

/*N*/ E3dPointLight::~E3dPointLight()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/


/*************************************************************************
|*
|* Lichtstaerke an uebergeben Punkt bestimmen und die akkumulierte
|* Punktfarbe berechnen
|*
\************************************************************************/

/*N*/ FASTBOOL E3dPointLight::CalcLighting(Color& rNewColor,
/*N*/ 									 const Vector3D& rPnt,
/*N*/ 									 const Vector3D& rPntNormal,
/*N*/ 									 const Color& rPntColor)
/*N*/ {
/*N*/ 	double fR = 0, fG = 0, fB = 0;
/*N*/ 
/*N*/ 	if ( IsOn() )
/*N*/ 	{
/*N*/ 		double fLight;
/*N*/ 		Vector3D aPntToLight = GetTransPosition() - rPnt;
/*N*/ 
/*N*/ 		aPntToLight.Normalize();
/*N*/ 		fLight = rPntNormal.Scalar(aPntToLight);
/*N*/ 
/*N*/ 		if ( fLight > 0 )
/*N*/ 		{
/*N*/ 			fR = fLight * GetRed();
/*N*/ 			fG = fLight * GetGreen();
/*N*/ 			fB = fLight * GetBlue();
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return ImpCalcLighting(rNewColor, rPntColor, fR, fG, fB);
/*N*/ }

/*************************************************************************
|*
|* Darstellung des Lichtobjekts in der Szene ein-/ausschalten
|*
\************************************************************************/

/*N*/ void E3dPointLight::CreateLightObj()
/*N*/ {
/*N*/ 	pSub->Clear();
/*N*/ 
/*N*/ 	if ( IsLightObjVisible() )
/*N*/ 	{
/*N*/ 		Vector3D aDiff(0.5,0,0);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(0,0.5,0);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(0,0,0.5);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(0.35,0.35,0.35);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(0.35,0.35,-0.35);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(-0.35,0.35,-0.35);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 		aDiff = Vector3D(-0.35,0.35,0.35);
/*N*/ 		Insert3DObj(new E3dPolyObj(GetPosition() - aDiff, GetPosition() + aDiff));
/*N*/ 	}
/*N*/ }



}
