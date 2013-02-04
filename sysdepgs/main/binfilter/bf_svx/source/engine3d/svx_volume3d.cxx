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




#ifndef _POLY3D_HXX
#include "poly3d.hxx"
#endif


#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Konstruktor 1:                                               |
|* rPos: Zentrum oder minimale Koordinate links, unten, hinten  |__
|*       (abhaengig von bPosIsCenter)                          /
|*
\************************************************************************/

/*N*/ Volume3D::Volume3D(const Vector3D& rPos, const Vector3D& r3DSize, BOOL bPosIsCenter)
/*N*/ :	B3dVolume(rPos, r3DSize, bPosIsCenter)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Konstruktor 2 - leeres Volumen, Werte als ungueltig markieren
|*
\************************************************************************/

/*N*/ Volume3D::Volume3D() : B3dVolume()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Transformation des Volumens berechnen und als neues Volumen
|* zurueckgeben
|*
\************************************************************************/

/*N*/ Volume3D Volume3D::GetTransformVolume(const Matrix4D& rTfMatrix) const
/*N*/ {
/*N*/ 	Volume3D aTfVol;
/*N*/ 
/*N*/ 	if(IsValid())
/*N*/ 	{
/*N*/ 		Vector3D aTfVec;
/*N*/ 		Vol3DPointIterator aIter(*this, &rTfMatrix);
/*N*/ 
/*N*/ 		while(aIter.Next(aTfVec))
/*N*/ 			aTfVol.Union(aTfVec);
/*N*/ 	}
/*N*/ 	return aTfVol;
/*N*/ }

/*************************************************************************
|*
|* Drahtgitter-Linien fuer das Volumen berechnen und in rPoly3D ablegen
|*
\************************************************************************/


/*************************************************************************
|*
|* Konstruktor des Point-Iterators
|*
\************************************************************************/

/*N*/ Vol3DPointIterator::Vol3DPointIterator(const Volume3D& rVol, const Matrix4D* pTf)
/*N*/ :	rVolume(rVol),
/*N*/ 	pTransform(pTf),
/*N*/ 	nIndex(0)
/*N*/ {
/*N*/ 	DBG_ASSERT(rVol.IsValid(), "Vol3DPointIterator-Aufruf mit ungueltigem Volume3D!");
/*N*/ 	a3DExtent = rVolume.aMaxVec - rVolume.aMinVec;
/*N*/ }

/*************************************************************************
|*
|* Gibt die einzelnen Punkte des (ggf. transformierten) Volumens zurueck
|*
|*    4---5  -> Reihenfolge der Punktausgabe (untransformiert)
|*   /|  /|
|*  7---6 |
|*  | 0-|-1
|*  |/  |/
|*  3---2
|*
\************************************************************************/

/*N*/ BOOL Vol3DPointIterator::Next(Vector3D& rVec)
/*N*/ {
/*N*/ 	if(nIndex > 7)
/*N*/ 	{
/*N*/ 		return FALSE;
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		rVec = rVolume.aMinVec;
/*N*/ 
/*N*/ 		if(nIndex >= 4)
/*N*/ 			rVec.Y() += a3DExtent.Y();
/*N*/ 
/*N*/ 		switch(nIndex)
/*N*/ 		{
/*N*/ 			case 6:
/*N*/ 			case 2: rVec.Z() += a3DExtent.Z();
/*N*/ 			case 5:
/*N*/ 			case 1: rVec.X() += a3DExtent.X();
/*N*/ 					break;
/*N*/ 			case 7:
/*N*/ 			case 3: rVec.Z() += a3DExtent.Z();
/*N*/ 					break;
/*N*/ 		}
/*N*/ 		nIndex++;
/*N*/ 
/*N*/ 		if(pTransform)
/*N*/ 			rVec *= *pTransform;
/*N*/ 
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ }


}
