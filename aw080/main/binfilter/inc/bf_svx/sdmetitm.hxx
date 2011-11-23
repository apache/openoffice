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


#ifndef _SDMETITM_HXX
#define _SDMETITM_HXX

#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif
namespace binfilter {

//------------------------------------------------------------
// class SdrAngleItem
// Fuer alle Metriken. GetPresentation liefert dann bei
// Value=2500 z.B. ein "25mm".
//------------------------------------------------------------
class SdrMetricItem: public SfxInt32Item {
public:
	TYPEINFO();
	SdrMetricItem(): SfxInt32Item() {}
	SdrMetricItem(USHORT nId, INT32 nVal=0):  SfxInt32Item(nId,nVal) {}
	SdrMetricItem(USHORT nId, SvStream& rIn): SfxInt32Item(nId,rIn) {}
	virtual SfxPoolItem* Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem* Create(SvStream& rIn, USHORT nVer) const;


#ifdef SDR_ISPOOLABLE
	virtual int IsPoolable() const;
#endif
};

}//end of namespace binfilter
#endif
