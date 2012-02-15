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


#ifndef _SXMKITM_HXX
#define _SXMKITM_HXX

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
namespace binfilter {

enum SdrMeasureKind     {SDRMEASURE_STD,SDRMEASURE_RADIUS}; // n.i.

//------------------------------
// class SdrMeasureKindItem (n.i.)
//------------------------------
class SdrMeasureKindItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrMeasureKindItem(SdrMeasureKind eKind=SDRMEASURE_STD): SfxEnumItem(SDRATTR_MEASUREKIND,eKind) {}
	SdrMeasureKindItem(SvStream& rIn)                      : SfxEnumItem(SDRATTR_MEASUREKIND,rIn)    {}
	virtual SfxPoolItem*   Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem*   Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT         GetValueCount() const; // { return 2; }
			SdrMeasureKind GetValue() const { return (SdrMeasureKind)SfxEnumItem::GetValue(); }


};

}//end of namespace binfilter
#endif
