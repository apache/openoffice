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



#ifndef _SVDMARK_HXX
#define _SVDMARK_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _CONTNR_HXX //autogen
#include <tools/contnr.hxx>
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
class Rectangle;
namespace binfilter {

class SdrPage;
class SdrObjList;
class SdrObject;
class SdrPageView;

// Ein Container fuer USHORTs (im Prinzip ein dynamisches Array)
class SdrUShortCont {
	Container aArr;
	FASTBOOL bSorted;
	FASTBOOL bDelDups;
private:
public:
	SdrUShortCont(USHORT nBlock, USHORT nInit, USHORT nResize): aArr(nBlock,nInit,nResize), bSorted(TRUE), bDelDups(TRUE) {}
	SdrUShortCont(const SdrUShortCont& rCont): aArr(rCont.aArr), bSorted(rCont.bSorted), bDelDups(rCont.bDelDups) {}
	SdrUShortCont& operator=(const SdrUShortCont& rCont)     { aArr=rCont.aArr; bSorted=rCont.bSorted; bDelDups=rCont.bDelDups; return *this; }
	FASTBOOL       operator==(const SdrUShortCont& rCont) const { return aArr==rCont.aArr; }
	FASTBOOL       operator!=(const SdrUShortCont& rCont) const { return aArr!=rCont.aArr; }
	void     Clear()                                           { aArr.Clear(); bSorted=TRUE; }
	void     Remove(ULONG nPos)                                { aArr.Remove(nPos); }
	USHORT   GetObject(ULONG nPos) const                       { return USHORT(ULONG(aArr.GetObject(nPos))); }
	ULONG    GetPos(USHORT nElem) const                        { return aArr.GetPos((void*)(ULONG)nElem); }
	ULONG    GetCount() const                                  { return aArr.Count(); }
	FASTBOOL Exist(USHORT nElem) const                         { return aArr.GetPos((void*)(ULONG)nElem)!=CONTAINER_ENTRY_NOTFOUND; }
};

class SdrMark { // Alles was eine View ueber ein markiertes Objekt wissen muss
protected:
	SdrObject*     pObj;
	SdrPageView*   pPageView;
	SdrUShortCont* pPoints;     // Markierte Punkte
	SdrUShortCont* pLines;      // Markierte Linienabschnitte
	SdrUShortCont* pGluePoints; // Markierte Klebepunkte (deren Id's)
	FASTBOOL       bCon1;       // fuer Connectoren
	FASTBOOL       bCon2;       // fuer Connectoren
	USHORT         nUser;       // z.B. fuer CopyObjects, mitkopieren von Edges
public:
	SdrMark(SdrObject* pNewObj=NULL, SdrPageView* pNewPageView=NULL): pObj(pNewObj), pPageView(pNewPageView), pPoints(NULL), pLines(NULL), pGluePoints(NULL), bCon1(FALSE), bCon2(FALSE), nUser(0) {}
	SdrMark(const SdrMark& rMark): pObj(NULL), pPageView(NULL), pPoints(NULL), pLines(NULL), pGluePoints(NULL), bCon1(FALSE), bCon2(FALSE), nUser(0) { *this=rMark; }
	~SdrMark()                                          { if (pPoints!=NULL) delete pPoints; if (pLines!=NULL) delete pLines; if (pGluePoints!=NULL) delete pGluePoints; }
	SdrObject*   GetObj() const                         { return pObj; }
	SdrPageView* GetPageView() const                    { return pPageView; }
	void         SetObj(SdrObject* pNewObj)             { pObj =pNewObj; }
	void         SetPageView(SdrPageView* pNewPageView) { pPageView=pNewPageView; }
	void         SetCon1(FASTBOOL bOn)                  { bCon1=bOn; }
	FASTBOOL     IsCon1() const                         { return bCon1; }
	void         SetCon2(FASTBOOL bOn)                  { bCon2=bOn; }
	FASTBOOL     IsCon2() const                         { return bCon2; }
	void         SetUser(USHORT nVal)                   { nUser=nVal; }
	USHORT       GetUser() const                        { return nUser; }
	const SdrUShortCont* GetMarkedPoints() const        { return pPoints;     }
	const SdrUShortCont* GetMarkedLines() const         { return pLines;      }
	const SdrUShortCont* GetMarkedGluePoints() const    { return pGluePoints; }
	SdrUShortCont*       GetMarkedPoints()              { return pPoints;     }
	SdrUShortCont*       GetMarkedLines()               { return pLines;      }
	SdrUShortCont*       GetMarkedGluePoints()          { return pGluePoints; }
	SdrUShortCont*       ForceMarkedPoints()            { if (pPoints==NULL) pPoints=new SdrUShortCont(1024,32,32); return pPoints; }
	SdrUShortCont*       ForceMarkedLines()             { if (pLines==NULL) pLines=new SdrUShortCont(1024,32,32); return pLines; }
	SdrUShortCont*       ForceMarkedGluePoints()        { if (pGluePoints==NULL) pGluePoints=new SdrUShortCont(1024,32,32); return pGluePoints; }
};

class SdrMarkList {
protected:
	Container aList;
	String    aMarkName;
	String    aPointName;
	String    aGluePointName;
	FASTBOOL  bPointNameOk;
	FASTBOOL  bGluePointNameOk;
	FASTBOOL  bNameOk;
	FASTBOOL  bSorted;
private:
	// Nicht implementiert:
	FASTBOOL operator==(const SdrMarkList& rCmpMarkList) const;
private:
	const XubString& GetPointMarkDescription(FASTBOOL bGlue) const;
protected:
public:
	SdrMarkList()                        : aList(1024,64,64),bPointNameOk(FALSE),bGluePointNameOk(FALSE),bNameOk(FALSE),bSorted(TRUE) { }
	SdrMarkList(const SdrMarkList& rLst) : aList(1024,64,64)        { *this=rLst; }
	~SdrMarkList()                                                  { Clear(); }
	void     Clear();
	// Sicherstellen, das die Liste sortiert ist:
	void     ForceSort();
	void     SetUnsorted()                                           { bSorted=FALSE; }
	ULONG    GetMarkCount() const                                    { return aList.Count(); }
	SdrMark* GetMark(ULONG nNum) const                               { return (SdrMark*)(aList.GetObject(nNum)); }

	// FindObject() sucht nach einer Markierung eines SdrObject.
	// Die Methode ist nicht const, da ein ForceSort gerufen wird!
	// Rueckgabewert ist der Index der gefundenen Mark-Entry bzw.
	// CONTAINER_ENTRY_NOTFOUND.
	ULONG FindObject(const SdrObject* pObj);

	void  InsertEntry(const SdrMark& rMark, FASTBOOL bChkSort=TRUE);

	// bReverse kann einen Performancevorteil bringen, wenn sicher ist,
	// dass die SrcList genau verkehrt herum ist.

	// Alle Entrys einer bestimmten PageView rauswerfen inkl delete
	FASTBOOL  DeletePageView (const SdrPageView& rPV);
	// Alle Objekte einer Seiteansicht markieren

	void SetNameDirty()                                              { bNameOk=FALSE; bPointNameOk=FALSE; bGluePointNameOk=FALSE; }

	// Eine verbale Beschreibung der markierten Objekte z.B.:
	// "27 Linien", "12 Objekte", "Polygon" oder auch "Kein Objekt"
	const String& GetGluePointMarkDescription() const                { return GetPointMarkDescription(TRUE); }

	// Schliesslich kann sich eine Mehrfach-Markierung
	// ueber mehrere Seiten erstrecken ...
	// pPage=NULL: Die Markierungen aller! Seiten beruecksichtigen
	FASTBOOL TakeBoundRect(SdrPageView* pPageView, Rectangle& rRect) const;
	FASTBOOL TakeSnapRect(SdrPageView* pPageView, Rectangle& rRect) const;

	// Es werden saemtliche Entries kopiert!
	void operator=(const SdrMarkList& rLst);

};
// Die Klasse kopiert alle eingefuegten MarkEntrys auf den Heap. Die Verwendung
// ist z.B. wie folgt gedacht:
//     InsertEntry(SdrMark(pObj,pPage));

}//end of namespace binfilter
#endif //_SVDMARK_HXX

