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


#ifndef SDSHSITM_HXX
#define SDSHSITM_HXX

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

/*************************************************************************/
//---------------------
// class SdrShadowSetItem
//---------------------
class SdrShadowSetItem: public SfxSetItem {
public:
	TYPEINFO();
	SdrShadowSetItem(SfxItemSet* pItemSet);
	SdrShadowSetItem(SfxItemPool* pItemPool);
	SdrShadowSetItem(const SdrShadowSetItem& rAttr, SfxItemPool* pItemPool=NULL): SfxSetItem(rAttr,pItemPool) {}
	virtual SfxPoolItem*    Clone(SfxItemPool* pToPool) const;
	virtual SfxPoolItem*    Create(SvStream& rStream, USHORT nVersion) const;
};

}//end of namespace binfilter
#endif
