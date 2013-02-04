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



#ifndef _SVDOVIRT_HXX
#define _SVDOVIRT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDOBJ_HXX
#include <bf_svx/svdobj.hxx>
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   SdrVirtObj
//
// Achtung! Das virtuelle Objekt ist noch nicht bis in alle Feinheiten
// durchprogrammiert und getestet. Z.Zt. kommt es nur in abgeleiteter
// beim Writer zum Einsatz.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrVirtObj : public SdrObject
{
protected:
	SdrObject&					rRefObj; // Referenziertes Zeichenobjekt
	Rectangle					aSnapRect;
	Point						aHack;   // f. GetPoint()

protected:
	virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType);

	virtual SdrObjGeoData* NewGeoData() const;
	virtual void SaveGeoData(SdrObjGeoData& rGeo) const;
	virtual void RestGeoData(const SdrObjGeoData& rGeo);

public:
	TYPEINFO();
	SdrVirtObj(SdrObject& rNewObj);
	virtual ~SdrVirtObj();
	virtual SdrObject& ReferencedObj();
	virtual const SdrObject& GetReferencedObj() const;
	virtual void SetModel(SdrModel* pNewModel);

	virtual UINT32 GetObjInventor() const;
	virtual UINT16 GetObjIdentifier() const;
	virtual SdrObjList* GetSubList() const;

	virtual void SetChanged();






	virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);

	virtual void Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);

	virtual void RecalcSnapRect();


	virtual long GetRotateAngle() const;
	virtual long GetShearAngle(FASTBOOL bVertical=FALSE) const;


	virtual void NbcSetPoint(const Point& rPnt, USHORT i);


	// ItemSet access
	virtual const SfxItemSet& GetItemSet() const;
	virtual SfxItemSet* CreateNewItemSet(SfxItemPool& rPool);

	// private support routines for ItemSet access. NULL pointer means clear item.

	// pre- and postprocessing for objects for saving
	virtual void PreSave();
	virtual void PostSave();

	virtual void NbcSetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);
	virtual void SetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);



	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);
	virtual void AfterRead();

    // OD 30.06.2003 #108784# - virtual <GetOffset()> returns Point(0,0)
    virtual const Point GetOffset() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDOVIRT_HXX

