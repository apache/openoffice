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



#ifndef _E3D_EXTRUD3D_HXX
#define _E3D_EXTRUD3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _E3D_OBJ3D_HXX
#include <bf_svx/obj3d.hxx>
#endif

#ifndef _POLY3D_HXX
#include <bf_svx/poly3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* 3D-Extrusionsobjekt aus uebergebenem 2D-Polygon erzeugen
|*
\************************************************************************/

class E3dExtrudeObj : public E3dCompoundObject
{
private:
	// Geometrie, die dieses Objekt bestimmt
	PolyPolygon3D			aExtrudePolygon;
	double					fExtrudeScale;

	// #78972#
	PolyPolygon3D			maLinePolyPolygon;

	// #107245# unsigned				bExtrudeSmoothed			: 1;
	// #107245# unsigned				bExtrudeSmoothFrontBack		: 1;
	// #107245# unsigned				bExtrudeCharacterMode		: 1;
	// #107245# unsigned				bExtrudeCloseFront			: 1;
	// #107245# unsigned				bExtrudeCloseBack			: 1;

protected:
	void SetDefaultAttributes(E3dDefaultAttributes& rDefault);
	PolyPolygon3D GetFrontSide();
	PolyPolygon3D GetBackSide(const PolyPolygon3D& rFrontSide);

public:
	TYPEINFO();

	E3dExtrudeObj(E3dDefaultAttributes& rDefault, const PolyPolygon& rPP, double fDepth);
	// es wird keine Bezier-Konvertierung des XPolygon durchgefuehrt,
	// sondern es werden nur die Punkte uebernommen!
	E3dExtrudeObj(E3dDefaultAttributes& rDefault, const XPolyPolygon& rXPP, double fDepth);
	E3dExtrudeObj();

	// PercentDiagonal: 0..100, before 0.0..0.5
	sal_uInt16 GetPercentDiagonal() const 
		{ return ((const Svx3DPercentDiagonalItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_PERCENT_DIAGONAL)).GetValue(); }

	// BackScale: 0..100, before 0.0..1.0
	sal_uInt16 GetPercentBackScale() const 
		{ return ((const Svx3DBackscaleItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_BACKSCALE)).GetValue(); }

	// BackScale: 0..100, before 0.0..1.0
	sal_uInt32 GetExtrudeDepth() const 
		{ return ((const Svx3DDepthItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_DEPTH)).GetValue(); }

	// #107245# GetSmoothNormals() for bExtrudeSmoothed
	sal_Bool GetSmoothNormals() const 
		{ return ((const Svx3DSmoothNormalsItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_SMOOTH_NORMALS)).GetValue(); }

	// #107245# GetSmoothLids() for bExtrudeSmoothFrontBack
	sal_Bool GetSmoothLids() const 
		{ return ((const Svx3DSmoothLidsItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_SMOOTH_LIDS)).GetValue(); }

	// #107245# GetCharacterMode() for bExtrudeCharacterMode
	sal_Bool GetCharacterMode() const 
		{ return ((const Svx3DCharacterModeItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_CHARACTER_MODE)).GetValue(); }

	// #107245# GetCloseFront() for bExtrudeCloseFront
	sal_Bool GetCloseFront() const 
		{ return ((const Svx3DCloseFrontItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_CLOSE_FRONT)).GetValue(); }

	// #107245# GetCloseBack() for bExtrudeCloseBack
	sal_Bool GetCloseBack() const 
		{ return ((const Svx3DCloseBackItem&)GetUnmergedItemSet().Get(SDRATTR_3DOBJ_CLOSE_BACK)).GetValue(); }

	virtual UINT16 GetObjIdentifier() const;

	virtual void CreateWireframe(Polygon3D& rWirePoly, const Matrix4D* pTf = NULL,
		E3dDragDetail eDetail = E3DDETAIL_DEFAULT);

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	// TakeObjName...() ist fuer die Anzeige in der UI, z.B. "3 Rahmen selektiert".

	// Geometrieerzeugung
	virtual void CreateGeometry();

	// Give out simple line geometry

	// Lokale Parameter setzen/lesen mit Geometrieneuerzeugung
	void SetExtrudePolygon(const PolyPolygon3D &rNew);
	const PolyPolygon3D &GetExtrudePolygon() { return aExtrudePolygon; }


	// #107245# 
	// void SetExtrudeSmoothed(BOOL bNew);
	// BOOL GetExtrudeSmoothed() const { return bExtrudeSmoothed; }
	// void SetExtrudeSmoothFrontBack(BOOL bNew);
	// BOOL GetExtrudeSmoothFrontBack() const { return bExtrudeSmoothFrontBack; }
	// void SetExtrudeCharacterMode(BOOL bNew);
	// BOOL GetExtrudeCharacterMode() const { return bExtrudeCharacterMode; }
	// void SetExtrudeCloseFront(BOOL bNew);
	// BOOL GetExtrudeCloseFront() const { return bExtrudeCloseFront; }
	// void SetExtrudeCloseBack(BOOL bNew);
	// BOOL GetExtrudeCloseBack() const { return bExtrudeCloseBack; }

	// private support routines for ItemSet access. NULL pointer means clear item.
	virtual void PostItemChange(const sal_uInt16 nWhich);

	// Aufbrechen
};

}//end of namespace binfilter
#endif			// _E3D_EXTRUD3D_HXX

