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



#ifndef _B3D_B3DENTITY_HXX
#include "b3dentty.hxx"
#endif

//#ifndef _B3D_B3DCOMMN_HXX
//#include "b3dcommn.hxx"
//#endif

#ifndef _B3D_B3DTRANS_HXX
#include "b3dtrans.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

namespace binfilter {
/*************************************************************************
|*
|* Flags auf Ausgangsposition
|*
\************************************************************************/

void B3dEntity::Reset()
{
	bValid = bNormalUsed = bTexCoorUsed = bDeviceCoor = FALSE;
	bEdgeFlag = TRUE;
}

/*************************************************************************
|*
|* Neuen Punkt an der stelle t des parametrisierten Vektors rOld1, rOld2
|* berechnen und fuellen
|*
\************************************************************************/

void B3dEntity::CalcInBetween(B3dEntity& rOld1, B3dEntity& rOld2, double t)
{
	// DeviceCoor der ersten Quelle benutzen, die Basis sollte
	// vorher abgeglichen sein
	SetDeviceCoor(rOld1.IsDeviceCoor());

	// Punktkoordinaten berechnen
	aPoint.CalcInBetween(rOld1.Point(), rOld2.Point(), t);
	SetValid();

	// PlaneNormal Koordinaten berechnen
	rOld1.PlaneNormal().Normalize();
	rOld2.PlaneNormal().Normalize();
	aPlaneNormal.CalcInBetween(rOld1.PlaneNormal(), rOld2.PlaneNormal(), t);
	aPlaneNormal.Normalize();

	// Vektor berechnen
	if(rOld1.IsNormalUsed() && rOld2.IsNormalUsed())
	{
		rOld1.Normal().Normalize();
		rOld2.Normal().Normalize();
		aNormal.CalcInBetween(rOld1.Normal(), rOld2.Normal(), t);
		aNormal.Normalize();
		SetNormalUsed();
	}

	// Texturkoordinaten berechnen
	if(rOld1.IsTexCoorUsed() && rOld2.IsTexCoorUsed())
	{
		aTexCoor.CalcInBetween(rOld1.TexCoor(), rOld2.TexCoor(), t);
		SetTexCoorUsed();
	}

	// EdgeVisible berechnen
	SetEdgeVisible(rOld1.IsEdgeVisible());

	// Farbe berechnen
	aColor.CalcInBetween(rOld1.Color(), rOld2.Color(), t);
}

/*************************************************************************
|*
|* Neuen Punkt in der Mitte des parametrisierten Vektors rOld1, rOld2
|* berechnen und fuellen
|*
\************************************************************************/

void B3dEntity::CalcMiddle(B3dEntity& rOld1, B3dEntity& rOld2)
{
	// DeviceCoor der ersten Quelle benutzen, die Basis sollte
	// vorher abgeglichen sein
	SetDeviceCoor(rOld1.IsDeviceCoor());

	// Punktkoordinaten berechnen
	aPoint.CalcMiddle(rOld1.Point(), rOld2.Point());
	SetValid();

	// PlaneNormal Koordinaten berechnen
	rOld1.PlaneNormal().Normalize();
	rOld2.PlaneNormal().Normalize();
	aPlaneNormal.CalcMiddle(rOld1.PlaneNormal(), rOld2.PlaneNormal());
	aPlaneNormal.Normalize();

	// Vektor berechnen
	if(rOld1.IsNormalUsed() && rOld2.IsNormalUsed())
	{
		rOld1.Normal().Normalize();
		rOld2.Normal().Normalize();
		aNormal.CalcMiddle(rOld1.Normal(), rOld2.Normal());
		aNormal.Normalize();
		SetNormalUsed();
	}

	// Texturkoordinaten berechnen
	if(rOld1.IsTexCoorUsed() && rOld2.IsTexCoorUsed())
	{
		aTexCoor.CalcMiddle(rOld1.TexCoor(), rOld2.TexCoor());
		SetTexCoorUsed();
	}

	// EdgeVisible berechnen
	SetEdgeVisible(rOld1.IsEdgeVisible());

	// Farbe berechnen
	aColor.CalcMiddle(rOld1.Color(), rOld2.Color());
}

/*************************************************************************
|*
|* Bucket fuer geometrische Daten
|*
\************************************************************************/

BASE3D_IMPL_BUCKET(B3dEntity, Bucket)

}//end of namespace binfilter

// eof
