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



#ifndef _E3D_CUBE3D_HXX
#define _E3D_CUBE3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _E3D_OBJ3D_HXX
#include <bf_svx/obj3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|*                                                              |
|* 3D-Quader erzeugen; aPos: Zentrum oder links, unten, hinten  |__
|*                           (abhaengig von bPosIsCenter)      /
|* Mit nSideFlags kann angegeben werden, ob nur ein Teil der
|* Quaderflaechen erzeugt werden kann; die entsprechenden Bits
|* sind in dem enum definiert. Das Flag bDblSided legt fest,
|* ob die erzeugten Flaechen doppelseitig sind (nur sinnvoll,
|* wenn nicht alle Flaechen erzeugt wurden).
|*
\************************************************************************/

enum { CUBE_BOTTOM = 0x0001, CUBE_BACK = 0x0002, CUBE_LEFT = 0x0004,
	   CUBE_TOP = 0x0008, CUBE_RIGHT = 0x0010, CUBE_FRONT = 0x0020,
	   CUBE_FULL = 0x003F, CUBE_OPEN_TB = 0x0036, CUBE_OPEN_LR = 0x002B,
	   CUBE_OPEN_FB = 0x001D };

class E3dCubeObj : public E3dCompoundObject
{
private:
	// Zur Geometrieerzeugung eines Cubes notwendige
	// Parameter
	Vector3D			aCubePos;
	Vector3D			aCubeSize;
	UINT16				nSideFlags;

	// BOOLeans
	unsigned			bPosIsCenter			: 1;

protected:
	void SetDefaultAttributes(E3dDefaultAttributes& rDefault);

public:
	TYPEINFO();
	E3dCubeObj();

	virtual UINT16 GetObjIdentifier() const;
	virtual SdrObject* DoConvertToPolyObj(BOOL bBezier) const;

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	// Geometrieerzeugung
	virtual void CreateGeometry();

	// Give out simple line geometry

	// Lokale Parameter setzen mit Geometrieneuerzeugung
	void SetCubePos(const Vector3D& rNew);
	const Vector3D& GetCubePos() { return aCubePos; }

	void SetCubeSize(const Vector3D& rNew);
	const Vector3D& GetCubeSize() { return aCubeSize; }

	void SetPosIsCenter(BOOL bNew);
	BOOL GetPosIsCenter() { return (BOOL)bPosIsCenter; }

	UINT16 GetSideFlags() { return nSideFlags; }

	// TakeObjName...() ist fuer die Anzeige in der UI, z.B. "3 Rahmen selektiert".
};

}//end of namespace binfilter
#endif			// _E3D_CUBE3D_HXX
