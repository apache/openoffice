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



#include "svdovirt.hxx"
#include "svdio.hxx"
#include "svdhdl.hxx"
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ TYPEINIT1(SdrVirtObj,SdrObject);

/*N*/ SdrVirtObj::SdrVirtObj(SdrObject& rNewObj):
/*N*/ 	rRefObj(rNewObj)
/*N*/ {
/*N*/ 	bVirtObj=TRUE; // Ja, ich bin ein virtuelles Objekt
/*N*/ 	rRefObj.AddReference(*this);
/*N*/ 	bClosedObj=rRefObj.IsClosedObj();
/*N*/ }

/*N*/ SdrVirtObj::~SdrVirtObj()
/*N*/ {
/*N*/ 	rRefObj.DelReference(*this);
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ const SdrObject& SdrVirtObj::GetReferencedObj() const
/*N*/ {
/*N*/ 	return rRefObj;
/*N*/ }

/*N*/ SdrObject& SdrVirtObj::ReferencedObj()
/*N*/ {
/*N*/ 	return rRefObj;
/*N*/ }

/*N*/ void __EXPORT SdrVirtObj::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
/*N*/ {
/*N*/ 	bClosedObj=rRefObj.IsClosedObj();
/*N*/ 	SetRectsDirty(); // hier noch Optimieren.
/*N*/ 	SendRepaintBroadcast();
/*N*/ }


////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrVirtObj::SetModel(SdrModel* pNewModel)
/*N*/ {
/*N*/ 	SdrObject::SetModel(pNewModel);
/*N*/ 	rRefObj.SetModel(pNewModel);
/*N*/ }


/*N*/ UINT32 SdrVirtObj::GetObjInventor() const
/*N*/ {
/*N*/ 	return rRefObj.GetObjInventor();
/*N*/ }

/*N*/ UINT16 SdrVirtObj::GetObjIdentifier() const
/*N*/ {
/*N*/ 	return rRefObj.GetObjIdentifier();
/*N*/ }

/*N*/ SdrObjList* SdrVirtObj::GetSubList() const
/*N*/ {
/*N*/ 	return rRefObj.GetSubList();
/*N*/ }



/*N*/ void SdrVirtObj::SetChanged()
/*N*/ {
/*N*/ 	SdrObject::SetChanged();
/*N*/ }










////////////////////////////////////////////////////////////////////////////////////////////////////













////////////////////////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////////////////////////


/*N*/ void SdrVirtObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
/*N*/ {
/*N*/ 	rRefObj.NbcResize(rRef-aAnchor,xFact,yFact);
/*N*/ 	SetRectsDirty();
/*N*/ }




////////////////////////////////////////////////////////////////////////////////////////////////////


/*N*/ void SdrVirtObj::Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
/*N*/ {
/*N*/ 	if (xFact.GetNumerator()!=xFact.GetDenominator() || yFact.GetNumerator()!=yFact.GetDenominator()) {
/*N*/ 		Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetBoundRect();
/*N*/ 		rRefObj.Resize(rRef-aAnchor,xFact,yFact);
/*N*/ 		SetRectsDirty();
/*N*/ 		SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
/*N*/ 	}
/*N*/ }




////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrVirtObj::RecalcSnapRect()
/*N*/ {
/*N*/ 	aSnapRect=rRefObj.GetSnapRect();
/*N*/ 	aSnapRect+=aAnchor;
/*N*/ }




////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ long SdrVirtObj::GetRotateAngle() const
/*N*/ {
/*N*/ 	return rRefObj.GetRotateAngle();
/*N*/ }

/*N*/ long SdrVirtObj::GetShearAngle(FASTBOOL bVertical) const
/*N*/ {
/*N*/ 	return rRefObj.GetShearAngle(bVertical);
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////






/*N*/ void SdrVirtObj::NbcSetPoint(const Point& rPnt, USHORT i)
/*N*/ {
/*N*/ 	Point aP(rPnt);
/*N*/ 	aP-=aAnchor;
/*N*/ 	rRefObj.SetPoint(aP,i);
/*N*/ 	SetRectsDirty();
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrObjGeoData* SdrVirtObj::NewGeoData() const
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001 
/*N*/ }

/*N*/ void SdrVirtObj::SaveGeoData(SdrObjGeoData& rGeo) const
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*N*/ void SdrVirtObj::RestGeoData(const SdrObjGeoData& rGeo)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// ItemSet access

/*N*/ const SfxItemSet& SdrVirtObj::GetItemSet() const
/*N*/ {
/*N*/ 	return rRefObj.GetItemSet();
/*N*/ }

/*N*/ SfxItemSet* SdrVirtObj::CreateNewItemSet(SfxItemPool& rPool)
/*N*/ {
/*N*/ 	return rRefObj.CreateNewItemSet(rPool);
/*N*/ }





////////////////////////////////////////////////////////////////////////////////////////////////////
// private support routines for ItemSet access




////////////////////////////////////////////////////////////////////////////////////////////////////
// pre- and postprocessing for objects for saving

/*N*/ void SdrVirtObj::PreSave()
/*N*/ {
/*N*/ }

/*N*/ void SdrVirtObj::PostSave()
/*N*/ {
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////


/*N*/ void SdrVirtObj::SetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*N*/ void SdrVirtObj::NbcSetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr)
/*N*/ {
/*N*/ 	rRefObj.NbcSetStyleSheet(pNewStyleSheet,bDontRemoveHardAttr);
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////////////////////////

/*?*/ void SdrVirtObj::WriteData(SvStream& rOut) const
/*?*/ {
/*?*/ 	SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 	aCompat.SetID("SdrVirtObj");
/*?*/ #endif
/*?*/ 	// fehlende Implementation
/*?*/ 	rOut<<aAnchor;
/*?*/ }

/*?*/ void SdrVirtObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
/*?*/ {
/*?*/ 	if (rIn.GetError()!=0) return;
/*?*/ 	if (rHead.GetVersion()>=4) {
/*?*/ 		SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 		aCompat.SetID("SdrVirtObj");
/*?*/ #endif
/*?*/ 		// fehlende Implementation
/*?*/ 		rIn>>aAnchor;
/*?*/ 	} else {
/*?*/ 		rIn>>aAnchor;
/*?*/ 	}
/*?*/ }

/*?*/ void SdrVirtObj::AfterRead()
/*?*/ {
/*?*/ 	// fehlende Implementation
/*?*/ }

/*?*/ const Point SdrVirtObj::GetOffset() const
/*?*/ {
/*?*/     return Point(0,0);
/*?*/ }
}
