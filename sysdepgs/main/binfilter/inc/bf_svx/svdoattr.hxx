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



#ifndef _SVDOATTR_HXX
#define _SVDOATTR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SVX_XFILLIT0_HXX //autogen
#include <bf_svx/xfillit0.hxx>
#endif

#ifndef _SVX_XFLASIT_HXX //autogen
#include <bf_svx/xflasit.hxx>
#endif

#ifndef _SVX_XLINEIT0_HXX //autogen
#include <bf_svx/xlineit0.hxx>
#endif

#ifndef _SVX_XLNASIT_HXX //autogen
#include <bf_svx/xlnasit.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include <bf_svx/svdobj.hxx>
#endif

#ifndef _SVDATTR_HXX
#include <bf_svx/svdattr.hxx>
#endif
namespace binfilter {

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SfxPoolItem; 
class SfxSetItem;
class SfxItemSet;
class SfxItemPool;

class SdrOutliner;

//************************************************************
//   SdrAttrObj
//************************************************************

class SdrAttrObj : public SdrObject
{
	friend class				SdrOutliner;

protected:
	Rectangle					maSnapRect;
	SfxStyleSheet				*mpStyleSheet;
	SfxItemSet					*mpObjectItemSet;

	void ImpDeleteItemSet();
	void ImpForceItemSet();

protected:
	// Strichstaerke ermitteln. Keine Linie -> 0.
	INT32 ImpGetLineWdt() const;

	// Feststellen, wieviel wegen der Linienenden auf's BoundRect draufaddiert werden muss.
	INT32 ImpGetLineEndAdd() const;

	// HitTest auf's Linienende
	// ueber nSin/nCos wird der Winkel vorgegeben

	// Schattenabstand ermitteln. FALSE=Kein Schatten.
	FASTBOOL ImpGetShadowDist(sal_Int32& nXDist, sal_Int32& nYDist) const;

	// ggf. Schattenversatz zum BoundRect draufaddieren
	void ImpAddShadowToBoundRect();

	// Line und Fill Attribute fuer Schatten setzen.
	// Return=FALSE: kein Schatten attributiert.
    FASTBOOL ImpSetShadowAttributes( const SfxItemSet& rSet, SfxItemSet& rShadowSet ) const;

	// Zuhoeren, ob sich ein StyleSheet aendert
	virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType);
	virtual void RemoveStyleSheet();
	virtual void AddStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);

	// aus NULL-AttrPointern Pointer auf defaults machen
	virtual void ForceDefaultAttr();
	SdrAttrObj();
	virtual ~SdrAttrObj();

public:
	TYPEINFO();

	// Feststellen, ob bFilledObj && Fuellung!=FillNone
	BOOL HasFill() const;

	// Feststellen, ob Linie!=LineNone
	BOOL HasLine() const;

	virtual const Rectangle& GetSnapRect() const;
	virtual void operator=(const SdrObject& rObj);
	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);
	virtual void SetModel(SdrModel* pNewModel);

	// pre- and postprocessing for objects for saving
	virtual void PreSave();
	virtual void PostSave();

	// ItemSet access
	virtual const SfxItemSet& GetItemSet() const;
	virtual SfxItemSet* CreateNewItemSet(SfxItemPool& rPool);

	// syntactical sugar for ItemSet accesses
	const SfxItemSet& GetUnmergedItemSet() const;

	// t support routines for ItemSet access. NULL pointer means clear item.
	virtual void ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0);
	virtual void ItemSetChanged(const SfxItemSet& rSet);

	virtual SfxStyleSheet* GetStyleSheet() const;
	virtual void SetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);
	virtual void NbcSetStyleSheet(SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr);


	// ItemPool fuer dieses Objekt wechseln
	virtual void MigrateItemPool(SfxItemPool* pSrcPool, SfxItemPool* pDestPool, SdrModel* pNewModel = NULL );
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDOATTR_HXX

