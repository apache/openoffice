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



#ifndef _SVDUNDO_HXX
#define _SVDUNDO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _UNDO_HXX //autogen
#include <bf_svtools/undo.hxx>
#endif

#ifndef _CONTNR_HXX //autogen
#include <tools/contnr.hxx>
#endif

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#ifndef _SVDTYPES_HXX
#include <bf_svx/svdtypes.hxx> // fuer enum RepeatFuncts
#endif
#include <tools/debug.hxx>
namespace binfilter {
class SfxItemSet;
class SfxStyleSheet;
//************************************************************
//   Vorausdeklarationen
//************************************************************

class SdrView;
class SdrPageView;
class SdrModel;
class SdrObject;
class SdrPage;
class SdrObjList;
class SdrLayer;
class SdrLayerAdmin;
class SdrObjGeoData;
class OutlinerParaObject;
class SdrMasterPageDescriptor;

//************************************************************
//   SdrUndoAction
//
// Abstrakte Basisklasse fuer alle UndoActions der DrawingEngine
//
//************************************************************

class SdrUndoAction : public SfxUndoAction
{
protected:
	SdrModel&					rMod;

protected:
	SdrUndoAction(SdrModel& rNewMod)
	:	rMod(rNewMod)
	{}

public:


};

//************************************************************
//   SdrUndoGroup
//
// Zusammenfassung beliebig vieler einzelner UndoActions zu einer einzigen
// UndoAction. Beim Undo() werden die Einzelactions rueckwaets aufgerufen
// (die zuletzt angehaengte zuerst...), beim Redo() dagegen entspricht die
// Aufrufreihenfolge der Einfuegereihenfolge.
//
//************************************************************

class SdrUndoGroup : public SdrUndoAction
{
protected:
	Container					aBuf;

	// Beschreibung der Action, nicht expandiert (beinhaltet %O)
	String						aComment;
	String						aObjDescription;

	SdrRepeatFunc				eFunction;

public:
	SdrUndoGroup(SdrModel& rNewMod);
	virtual ~SdrUndoGroup();

	void Clear();
	ULONG GetActionCount() const { return aBuf.Count(); }
	SdrUndoAction* GetAction(ULONG nNum) const { return (SdrUndoAction*)(aBuf.GetObject(nNum)); }
	void AddAction(SdrUndoAction* pAct);

	void SetComment(const String& rStr) { aComment=rStr; }


};

//************************************************************
//   SdrUndoObj
//
// Abstrakte Basisklasse fuer alle UndoActions die mit Objekten zu tun haben.
//
//************************************************************

class SdrUndoObj : public SdrUndoAction
{
protected:
	SdrObject*					pObj;

protected:
	SdrUndoObj(SdrObject& rNewObj);
};

//************************************************************
//   SdrUndoAttrObj
//
// Aenderung der Objektattribute.
// Action direkt vor dem Setzen der neuen Attribute konstruieren.
// Auch fuer StyleSheets
//
//************************************************************


//************************************************************
//   SdrUndoMoveObj
//
// Blosses verschieben eines Objektes.
// Action direkt vor dem Verschieben konstruieren.
//
//************************************************************

class SdrUndoMoveObj : public SdrUndoObj
{
protected:
	Size						aDistance;     // Entfernung, um die verschoben wird

public:
	SdrUndoMoveObj(SdrObject& rNewObj, const Size& rDist): SdrUndoObj(rNewObj),aDistance(rDist) {}
};

//************************************************************
//   SdrUndoGeoObj
//
// Aenderung der Geometrie eines Objektes.
// Action direkt vor der geometrischen Transformation konstruieren.
//
//************************************************************

class SdrUndoGeoObj : public SdrUndoObj
{
protected:
	SdrObjGeoData*				pUndoGeo;
	SdrObjGeoData*				pRedoGeo;
	// Wenn sich um ein Gruppenobjekt handelt:
	SdrUndoGroup*				pUndoGroup;

public:
	SdrUndoGeoObj(SdrObject& rNewObj);
	virtual ~SdrUndoGeoObj();


};

//************************************************************
//   SdrUndoObjList
//
// Manipulationen an einer ObjList: Neues Obj, DeleteObj, SetObjZLevel, Grouping, ...
// Abstrakte Basisklasse.
//
//************************************************************

class SdrUndoObjList : public SdrUndoObj {
	FASTBOOL					bOwner;

protected:
	SdrObjList*					pObjList;
	SdrView*					pView;      // um bei ObjDel, Undo die
	SdrPageView*				pPageView;  // Selektion widerherstellen zu koennen
	UINT32						nOrdNum;
	// Bei einem Undo/Redo findet moeglicherweise Uebereignung des Objektes
	// statt. Im Dtor wird das Obj deleted, wenn bOwner==TRUE

protected:
	SdrUndoObjList(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=FALSE);
	virtual ~SdrUndoObjList();

	void SetView(SdrView* pView1, SdrPageView* pPageView1) { pView=pView1; pPageView=pPageView1; }
	BOOL IsOwner() { return bOwner; }
};

//************************************************************
//   SdrUndoRemoveObj
//
// Entfernen Objekts aus einer Objektliste. Innerhalb einer UndoGroup
// mit korrospondierenden Inserts zu verwenden.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SdrUndoRemoveObj : public SdrUndoObjList
{
public:
	SdrUndoRemoveObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=FALSE)
	: SdrUndoObjList(rNewObj,bOrdNumDirect) {}

};

//************************************************************
//   SdrUndoInsertObj
//
// Einfuegen Objekts in eine Objektliste. Innerhalb einer UndoGroup
// mit korrospondierenden Removes zu verwenden.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SdrUndoInsertObj : public SdrUndoObjList
{
public:
	SdrUndoInsertObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=FALSE)
	:	SdrUndoObjList(rNewObj,bOrdNumDirect) {}
};

//************************************************************
//   SdrUndoDelObj
//
// Loeschen eines Objekts.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SdrUndoDelObj : public SdrUndoRemoveObj
{
public:
	SdrUndoDelObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=FALSE)
	:	SdrUndoRemoveObj(rNewObj,bOrdNumDirect) {DBG_ASSERT(0, "STRIP");}//STRIP001 		:	SdrUndoRemoveObj(rNewObj,bOrdNumDirect) { SetOwner(TRUE); }

};

//************************************************************
//   SdrUndoNewObj
//
// Einfuegen eines neuen Objektes.
// Action nach dem einfuegen in die ObjList konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoReplaceObj
//
// Austausch eines Objektes.
// Action vor dem Replace an der ObjList konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoCopyObj
//
// Kopieren eines Objekts
// Action nach dem einfuegen in die ObjList konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoObjOrdNum
//************************************************************


//////////////////////////////////////////////////////////////////////////////
// #i11702#

class SdrUndoObjectLayerChange : public SdrUndoObj
{
protected:
	SdrLayerID					maOldLayer;
	SdrLayerID					maNewLayer;

public:
	SdrUndoObjectLayerChange(SdrObject& rObj, SdrLayerID aOldLayer, SdrLayerID aNewLayer);

	virtual void Undo();
	virtual void Redo();
};

//************************************************************
//   SdrUndoObjSetText
//************************************************************


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@     @@@@  @@  @@ @@@@@ @@@@@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@@@@@  @@@@  @@@@  @@@@@
//  @@    @@  @@   @@   @@    @@  @@
//  @@    @@  @@   @@   @@    @@  @@
//  @@@@@ @@  @@   @@   @@@@@ @@  @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoLayer
//
// Abstrakte Basisklasse fuer alle UndoActions die mit SdrLayer zu tun haben.
//
//************************************************************


//************************************************************
//   SdrUndoNewLayer
//
// Einfuegen eines neuen Layer. Action nach dem Einfuegen konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoDelLayer
//
// Loeschen eines Layer. Action vor dem Remove konstruieren
//
//************************************************************


//************************************************************
//   SdrUndoMoveLayer
//
// Verschieben eines Layer. Action vor dem Verschieben konstruieren.
//
//************************************************************


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@   @@@@   @@@@  @@@@@  @@@@
//  @@  @@ @@  @@ @@  @@ @@    @@  @@
//  @@  @@ @@  @@ @@     @@    @@
//  @@@@@  @@@@@@ @@ @@@ @@@@   @@@@
//  @@     @@  @@ @@  @@ @@        @@
//  @@     @@  @@ @@  @@ @@    @@  @@
//  @@     @@  @@  @@@@@ @@@@@  @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoPage
//
// Abstrakte Basisklasse fuer alle UndoActions die mit SdrPages zu tun haben.
//
//************************************************************

class SdrUndoPage : public SdrUndoAction
{
protected:
	SdrPage*					pPage;

protected:
protected:
	SdrUndoPage(SdrPage& rNewPg);
};

//************************************************************
//   SdrUndoPageList
//
// Manipulationen an einer PageList: Neue Page, DeletePage, MovePage(ChangePageNum)
// Abstrakte Basisklasse.
//
//************************************************************

class SdrUndoPageList : public SdrUndoPage
{
protected:
	USHORT						nPageNum;

	// Bei einem Undo/Redo findet moeglicherweise Uebereignung der Page
	// statt. Im Dtor wird die Page deleted, wenn bItsMine==TRUE
	FASTBOOL					bItsMine;

protected:
	SdrUndoPageList(SdrPage& rNewPg);
	virtual ~SdrUndoPageList();
};

//************************************************************
//   SdrUndoDelPage
//
// Loeschen einer Page.
// Action vor dem entfernen aus der List konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoNewPage
//
// Einfuegen einer neuen Page.
// Action nach dem einfuegen in die Liste konstruieren.
//
//************************************************************

class SdrUndoNewPage : public SdrUndoPageList
{
public:
	SdrUndoNewPage(SdrPage& rNewPg): SdrUndoPageList(rNewPg) {}
};

//************************************************************
//   SdrUndoCopyPage
//
// Kopieren einer Page
// Action nach dem einfuegen in die Liste konstruieren.
//
//************************************************************


//************************************************************
//   SdrUndoSetPageNum
//
// Verschieben der Page innerhalb der Liste
// Action vor dem Verschieben der Page konstruieren.
//
//************************************************************


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@   @@  @@@@   @@@@  @@@@@@ @@@@@ @@@@@   @@@@@   @@@@   @@@@  @@@@@  @@@@
//  @@@ @@@ @@  @@ @@  @@   @@   @@    @@  @@  @@  @@ @@  @@ @@  @@ @@    @@  @@
//  @@@@@@@ @@  @@ @@       @@   @@    @@  @@  @@  @@ @@  @@ @@     @@    @@
//  @@@@@@@ @@@@@@  @@@@    @@   @@@@  @@@@@   @@@@@  @@@@@@ @@ @@@ @@@@   @@@@
//  @@ @ @@ @@  @@     @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@        @@
//  @@   @@ @@  @@ @@  @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@    @@  @@
//  @@   @@ @@  @@  @@@@    @@   @@@@@ @@  @@  @@     @@  @@  @@@@@ @@@@@  @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoPageMasterPage
//
// Abstrakte Basisklasse fuer alle UndoActions die mit
// MasterPage-Beziehungen zu tun haben.
//
//************************************************************


//************************************************************
//   SdrUndoPageInsertMasterPage
//
// Setzen einer MasterPage an einer Zeichenseite.
// Action nach dem Einfuegen des MasterPageDescriptors erzeugen.
//
//************************************************************


//************************************************************
//   SdrUndoPageRemoveMasterPage
//
// Entfernen einer MasterPage von einer Zeichenseite.
// Action vor dem Entfernen des MasterPageDescriptors erzeugen.
//
//************************************************************


//************************************************************
//   SdrUndoPageMoveMasterPage
//
// Verschieben einer MasterPage an einer Zeichenseite (Aendern
// der Reihenfolge der MasterPageDescriptorList).
// Action vor dem Umsortieren des MasterPageDescriptors erzeugen.
//
//************************************************************


//************************************************************
//   SdrUndoPageChangeMasterPage
//
// Aenderung des MasterPageDescriptors (z.B. Aendern der VisibleLayer).
// Action vor der Aenderung am MasterPageDescriptor erzeugen.
//
//************************************************************


////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDUNDO_HXX

