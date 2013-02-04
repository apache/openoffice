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



#ifndef _VOLUME3D_HXX
#define _VOLUME3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _B3D_VOLUM_HXX
#include <bf_goodies/b3dvolum.hxx>
#endif
namespace binfilter {
class Matrix4D;

//************************************************************
//   Vorausdeklarationen
//************************************************************

class Polygon3D;
class XPolygon;
class XPolyPolygon;
class Vol3DPointIterator;

/*************************************************************************
|*
|* dreidimensionales Volumen, symmetrisch zu den Koordinatenachsen
|*
\************************************************************************/

class Volume3D : public B3dVolume
{
public:
	Volume3D(const Vector3D& rPos, const Vector3D& r3DSize, BOOL bPosIsCenter = TRUE);
	Volume3D();

	// Zuweisungsoperator, um B3dVolumes an Volume3D zuweisen zu koennen
	void operator=(const B3dVolume& rVol)
	{
		aMinVec = rVol.MinVec();
		aMaxVec = rVol.MaxVec();
	}
	Volume3D	GetTransformVolume(const Matrix4D& rTfMatrix) const;
};

/*************************************************************************
|*
|* Iterator, der die Eckpunkte eines Volumens berechnet; wenn eine Matrix
|* uebergeben wird, werden die Punkte damit transformiert
|*
\************************************************************************/

class Vol3DPointIterator
{
protected:
	const Volume3D&				rVolume;
	const Matrix4D*				pTransform;
	Vector3D					a3DExtent;
	UINT16						nIndex;

public:
	Vol3DPointIterator(const Volume3D& rVol, const Matrix4D* pTf = NULL);

	BOOL Next(Vector3D& rVec);
	void Reset() { nIndex = 0; }
};

}//end of namespace binfilter
#endif			// _VOLUME3D_HXX
