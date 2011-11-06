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



#ifndef _SVDOPAGE_HXX
#define _SVDOPAGE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDOBJ_HXX
#include <bf_svx/svdobj.hxx>
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrPageObj : public SdrObject
{
	SfxItemSet*				mpPageItemSet;
	sal_uInt16				nPageNum;
	BOOL					bPainting;  // Verriegelungsflag
	BOOL					bNotifying; // Verriegelungsflag

protected:
	virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType);

public:
	TYPEINFO();
	SdrPageObj(USHORT nNewPageNum=0);
	~SdrPageObj();

	virtual void SetModel(SdrModel* pNewModel);
	USHORT GetPageNum() const { return nPageNum; }
	void SetPageNum(USHORT nNewPageNum) 
		{ NbcSetPageNum(nNewPageNum); SetChanged(); SendRepaintBroadcast(GetBoundRect()); }
	void NbcSetPageNum(USHORT nNewPageNum) { nPageNum=nNewPageNum; }

	virtual UINT16 GetObjIdentifier() const;


	virtual const Rectangle& GetBoundRect() const;
	virtual const Rectangle& GetSnapRect() const;
	virtual const Rectangle& GetLogicRect() const;
	virtual void NbcSetSnapRect(const Rectangle& rRect);
	virtual void NbcSetLogicRect(const Rectangle& rRect);
	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	// ItemSet access
	virtual SfxItemSet* CreateNewItemSet(SfxItemPool& rPool);
	virtual const SfxItemSet& GetItemSet() const;
	// #86481# simply ignore item setting on page objects
	virtual void ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDOPAGE_HXX

