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



#define ITEMID_COLOR			SID_ATTR_3D_LIGHTCOLOR


#ifndef _E3D_CUBE3D_HXX
#include "cube3d.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif


namespace binfilter {

/*************************************************************************
|*
|* Klasse zum verwalten der 3D-Default Attribute
|*
\************************************************************************/

// Konstruktor
/*N*/ E3dDefaultAttributes::E3dDefaultAttributes()
/*N*/ {
/*N*/ 	Reset();
/*N*/ }

/*N*/ void E3dDefaultAttributes::Reset()
/*N*/ {
/*N*/ 	// Compound-Objekt
/*N*/ //	aDefaultFrontMaterial;
/*N*/ //	aDefaultBackMaterial;
/*N*/ 	bDefaultCreateNormals = TRUE;
/*N*/ 	bDefaultCreateTexture = TRUE;
/*N*/ 	bDefaultUseDifferentBackMaterial = FALSE;
/*N*/ 
/*N*/ 	// Cube-Objekt
/*N*/ 	aDefaultCubePos = Vector3D(-500.0, -500.0, -500.0);
/*N*/ 	aDefaultCubeSize = Vector3D(1000.0, 1000.0, 1000.0);
/*N*/ 	nDefaultCubeSideFlags = CUBE_FULL;
/*N*/ 	bDefaultCubePosIsCenter = FALSE;
/*N*/ 
/*N*/ 	// Sphere-Objekt
/*N*/ 	aDefaultSphereCenter = Vector3D(0.0, 0.0, 0.0);
/*N*/ 	aDefaultSphereSize = Vector3D(1000.0, 1000.0, 1000.0);
/*N*/ 
/*N*/ 	// Lathe-Objekt
/*N*/ 	nDefaultLatheEndAngle = 3600;
/*N*/ 	fDefaultLatheScale = 1.0;
/*N*/ 	bDefaultLatheSmoothed = TRUE;
/*N*/ 	bDefaultLatheSmoothFrontBack = FALSE;
/*N*/ 	bDefaultLatheCharacterMode = FALSE;
/*N*/ 	bDefaultLatheCloseFront = TRUE;
/*N*/ 	bDefaultLatheCloseBack = TRUE;
/*N*/ 
/*N*/ 	// Extrude-Objekt
/*N*/ 	fDefaultExtrudeScale = 1.0;
/*N*/ 	bDefaultExtrudeSmoothed = TRUE;
/*N*/ 	bDefaultExtrudeSmoothFrontBack = FALSE;
/*N*/ 	bDefaultExtrudeCharacterMode = FALSE;
/*N*/ 	bDefaultExtrudeCloseFront = TRUE;
/*N*/ 	bDefaultExtrudeCloseBack = TRUE;
/*N*/ 
/*N*/ 	// Scene-Objekt
/*N*/ //	aDefaultLightGroup;
/*N*/ 	bDefaultDither = TRUE;
/*N*/ }
}
