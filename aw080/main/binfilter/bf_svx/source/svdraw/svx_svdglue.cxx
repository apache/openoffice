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




#include "svdobj.hxx"
#include "svdio.hxx"
#include "svdtrans.hxx"
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////


/*N*/ Point SdrGluePoint::GetAbsolutePos(const SdrObject& rObj) const
/*N*/ {
/*N*/ 	if (bReallyAbsolute) return aPos;
/*N*/ 	Rectangle aSnap(rObj.GetSnapRect());
/*N*/ 	Rectangle aBound(rObj.GetSnapRect());
/*N*/ 	Point aPt(aPos);
/*N*/ 
/*N*/ 	Point aOfs(aSnap.Center());
/*N*/ 	switch (GetHorzAlign()) {
/*?*/ 		case SDRHORZALIGN_LEFT  : aOfs.X()=aSnap.Left(); break;
/*?*/ 		case SDRHORZALIGN_RIGHT : aOfs.X()=aSnap.Right(); break;
/*N*/ 	}
/*N*/ 	switch (GetVertAlign()) {
/*?*/ 		case SDRVERTALIGN_TOP   : aOfs.Y()=aSnap.Top(); break;
/*?*/ 		case SDRVERTALIGN_BOTTOM: aOfs.Y()=aSnap.Bottom(); break;
/*N*/ 	}
/*N*/ 	if (!bNoPercent) {
/*?*/ 		long nXMul=aSnap.Right()-aSnap.Left();
/*?*/ 		long nYMul=aSnap.Bottom()-aSnap.Top();
/*?*/ 		long nXDiv=10000;
/*?*/ 		long nYDiv=10000;
/*?*/ 		if (nXMul!=nXDiv) {
/*?*/ 			aPt.X()*=nXMul;
/*?*/ 			aPt.X()/=nXDiv;
/*?*/ 		}
/*?*/ 		if (nYMul!=nYDiv) {
/*?*/ 			aPt.Y()*=nYMul;
/*?*/ 			aPt.Y()/=nYDiv;
/*?*/ 		}
/*N*/ 	}
/*N*/ 	aPt+=aOfs;
/*N*/ 	// Und nun auf's BoundRect des Objekts begrenzen
/*N*/ 	if (aPt.X()<aBound.Left  ()) aPt.X()=aBound.Left  ();
/*N*/ 	if (aPt.X()>aBound.Right ()) aPt.X()=aBound.Right ();
/*N*/ 	if (aPt.Y()<aBound.Top   ()) aPt.Y()=aBound.Top   ();
/*N*/ 	if (aPt.Y()>aBound.Bottom()) aPt.Y()=aBound.Bottom();
/*N*/ 	return aPt;
/*N*/ }













/*?*/ SvStream& operator<<(SvStream& rOut, const SdrGluePoint& rGP)
/*?*/ {
/*?*/ 	if (rOut.GetError()!=0) return rOut;
/*?*/ 	SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 	aCompat.SetID("SdrGluePoint");
/*?*/ #endif
/*?*/ 	rOut<<rGP.aPos;
/*?*/ 	rOut<<rGP.nEscDir;
/*?*/ 	rOut<<rGP.nId;
/*?*/ 	rOut<<rGP.nAlign;
/*?*/ 	BOOL bTmp=rGP.bNoPercent; // ueber bTmp, weil sonst (beim casting) im Falle
/*?*/ 	rOut<<bTmp;               // TRUE nicht 01 sondern FF geschrieben wird.
/*?*/ 	return rOut;
/*?*/ }

/*?*/ SvStream& operator>>(SvStream& rIn, SdrGluePoint& rGP)
/*?*/ {
/*?*/ 	if (rIn.GetError()!=0) return rIn;
/*?*/ 	SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 	aCompat.SetID("SdrGluePoint");
/*?*/ #endif
/*?*/ 	BOOL bTmpBool;
/*?*/ 	rIn>>rGP.aPos;
/*?*/ 	rIn>>rGP.nEscDir;
/*?*/ 	rIn>>rGP.nId;
/*?*/ 	rIn>>rGP.nAlign;
/*?*/ 	rIn>>bTmpBool; rGP.bNoPercent=(bTmpBool!=0);
/*?*/ 	return rIn;
/*?*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrGluePointList::Clear()
/*N*/ {
/*N*/ 	USHORT nAnz=GetCount();
/*N*/ 	for (USHORT i=0; i<nAnz; i++) {
/*N*/ 		delete GetObject(i);
/*N*/ 	}
/*N*/ 	aList.Clear();
/*N*/ }


// Die Id's der Klebepunkte in der Liste sind stets streng monoton steigend!
// Ggf. wird dem neuen Klebepunkt eine neue Id zugewiesen (wenn diese bereits
// vergeben ist). Die Id 0 ist reserviert.
/*N*/ USHORT SdrGluePointList::Insert(const SdrGluePoint& rGP)
/*N*/ {
/*N*/ 	SdrGluePoint* pGP=new SdrGluePoint(rGP);
/*N*/ 	USHORT nId=pGP->GetId();
/*N*/ 	USHORT nAnz=GetCount();
/*N*/ 	USHORT nInsPos=nAnz;
/*N*/ 	USHORT nLastId=nAnz!=0 ? GetObject(nAnz-1)->GetId() : 0;
/*N*/ 	DBG_ASSERT(nLastId>=nAnz,"SdrGluePointList::Insert(): nLastId<nAnz");
/*N*/ 	FASTBOOL bHole=nLastId>nAnz;
/*N*/ 	if (nId<=nLastId) {
/*N*/ 		if (!bHole || nId==0) {
/*N*/ 			nId=nLastId+1;
/*N*/ 		} else {
/*N*/ 			FASTBOOL bBrk=FALSE;
/*N*/ 			for (USHORT nNum=0; nNum<nAnz && !bBrk; nNum++) {
/*N*/ 				const SdrGluePoint* pGP=GetObject(nNum);
/*N*/ 				USHORT nTmpId=pGP->GetId();
/*N*/ 				if (nTmpId==nId) {
/*N*/ 					nId=nLastId+1; // bereits vorhanden
/*N*/ 					bBrk=TRUE;
/*N*/ 				}
/*N*/ 				if (nTmpId>nId) {
/*N*/ 					nInsPos=nNum; // Hier einfuegen (einsortieren)
/*N*/ 					bBrk=TRUE;
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/ 		pGP->SetId(nId);
/*N*/ 	}
/*N*/ 	aList.Insert(pGP,nInsPos);
/*N*/ 	return nInsPos;
/*N*/ }



/*N*/ USHORT SdrGluePointList::FindGluePoint(USHORT nId) const
/*N*/ {
/*N*/ 	// Hier noch einen optimaleren Suchalgorithmus implementieren.
/*N*/ 	// Die Liste sollte stets sortiert sein!!!!
/*N*/ 	USHORT nAnz=GetCount();
/*N*/ 	USHORT nRet=SDRGLUEPOINT_NOTFOUND;
/*N*/ 	for (USHORT nNum=0; nNum<nAnz && nRet==SDRGLUEPOINT_NOTFOUND; nNum++) {
/*N*/ 		const SdrGluePoint* pGP=GetObject(nNum);
/*N*/ 		if (pGP->GetId()==nId) nRet=nNum;
/*N*/ 	}
/*N*/ 	return nRet;
/*N*/ }







/*?*/ SvStream& operator<<(SvStream& rOut, const SdrGluePointList& rGPL)
/*?*/ {
/*?*/ 	if (rOut.GetError()!=0) return rOut;
/*?*/ 	SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 	aCompat.SetID("SdrGluePointList");
/*?*/ #endif
/*?*/ 	UINT16 nAnz=rGPL.GetCount();
/*?*/ 	rOut<<(UINT16)nAnz;
/*?*/ 	for (USHORT nNum=0; nNum<nAnz; nNum++) {
/*?*/ 		rOut<<rGPL[nNum];
/*?*/ 	}
/*?*/ 	return rOut;
/*?*/ }

/*?*/ SvStream& operator>>(SvStream& rIn, SdrGluePointList& rGPL)
/*?*/ {
/*?*/ 	if (rIn.GetError()!=0) return rIn;
/*?*/ 	SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
/*?*/ #ifdef DBG_UTIL
/*?*/ 	aCompat.SetID("SdrGluePointList");
/*?*/ #endif
/*?*/ 	rGPL.Clear();
/*?*/ 	UINT16 nAnz=0;
/*?*/ 	rIn>>nAnz;
/*?*/ 	for (USHORT nNum=0; nNum<nAnz; nNum++) {
/*?*/ 		SdrGluePoint aGP;
/*?*/ 		rIn>>aGP;
/*?*/ 		rGPL.Insert(aGP);
/*?*/ 	}
/*?*/ 	return rIn;
/*?*/ }

}
