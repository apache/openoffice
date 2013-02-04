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


#ifndef _SXCTITM_HXX
#define _SXCTITM_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

enum SdrCaptionType   {SDRCAPT_TYPE1,SDRCAPT_TYPE2,SDRCAPT_TYPE3,SDRCAPT_TYPE4};

//------------------------------
// class SdrCaptionTypeItem
//------------------------------
class SdrCaptionTypeItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrCaptionTypeItem(SdrCaptionType eStyle=SDRCAPT_TYPE3): SfxEnumItem(SDRATTR_CAPTIONTYPE,eStyle) {}
	SdrCaptionTypeItem(SvStream& rIn)                      : SfxEnumItem(SDRATTR_CAPTIONTYPE,rIn)    {}
	virtual SfxPoolItem*    Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem*    Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT          GetValueCount() const; // { return 4; }
			SdrCaptionType  GetValue() const      { return (SdrCaptionType)SfxEnumItem::GetValue(); }
};

}//end of namespace binfilter
#endif
