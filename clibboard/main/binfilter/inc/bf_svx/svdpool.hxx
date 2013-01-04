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



#ifndef _SVDPOOL_HXX
#define _SVDPOOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _XPOOL_HXX
#include <bf_svx/xpool.hxx>
#endif

#ifndef _SVDDEF_HXX
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

class XLineAttrSetItem;
class XFillAttrSetItem;
class XTextAttrSetItem;
class SdrShadowSetItem;
class SdrOutlinerSetItem;
class SdrMiscSetItem;

////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrItemPool: public XOutdevItemPool {
//-/#ifdef SDRDEFITEMCACHE
//-/friend class SdrAttrObj;
//-/	const XLineAttrSetItem*   pDefLineAttr;
//-/	const XFillAttrSetItem*   pDefFillAttr;
//-/	const XTextAttrSetItem*   pDefTextAttr;
//-/	const SdrShadowSetItem*   pDefShadAttr;
//-/	const SdrOutlinerSetItem* pDefOutlAttr;
//-/	const SdrMiscSetItem*     pDefMiscAttr;
//-/	const XLineAttrSetItem*   pDefTextFrameLineAttr;
//-/	const XFillAttrSetItem*   pDefTextFrameFillAttr;
//-/	const XTextAttrSetItem*   pDefTextFrameTextAttr;
//-/	const SdrShadowSetItem*   pDefTextFrameShadAttr;
//-/	const SdrOutlinerSetItem* pDefTextFrameOutlAttr;
//-/	const SdrMiscSetItem*     pDefTextFrameMiscAttr;
//-/#endif
private:
	void Ctor(SfxItemPool* pMaster, USHORT nAttrStart, USHORT nAttrEnd);
//-/	void ImpMakeDefItems();
public:
	// Kontstruktion des Pools als MasterPool
	SdrItemPool(USHORT nAttrStart=SDRATTR_START, USHORT nAttrEnd=SDRATTR_END,
		FASTBOOL bLoadRefCounts = TRUE);

	// Damit meine SetItems mit dem MasterPool konstruiert werden koennen.
	// Der SdrItemPool wird dabei automatisch als Secondary an den
	// uebergebenen MasterPool bzw. an's Ende einer bestehenden Pool-Kette
	// angehaengt.
	SdrItemPool(SfxItemPool* pMaster, USHORT nAttrStart=SDRATTR_START, USHORT nAttrEnd=SDRATTR_END,
		FASTBOOL bLoadRefCounts = TRUE);

	SdrItemPool(const SdrItemPool& rPool);
	virtual ~SdrItemPool();
	virtual SfxItemPool* Clone() const;

	// liefert nur einen String, wenn die DLL mit DBG_UTIL uebersetzt ist
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif // _SVDPOOL_HXX

