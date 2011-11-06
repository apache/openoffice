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


#ifndef _SDPRCITM_HXX
#define _SDPRCITM_HXX

#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif
namespace binfilter {

//------------------------------------------------------------
// class SdrPercentItem
// Ganze Prozente von 0
//------------------------------------------------------------

class SdrPercentItem : public SfxUInt16Item
{
public:
	TYPEINFO();
	SdrPercentItem(): SfxUInt16Item() {}
	SdrPercentItem(USHORT nId, UINT16 nVal=0): SfxUInt16Item(nId,nVal) {}
	SdrPercentItem(USHORT nId, SvStream& rIn):  SfxUInt16Item(nId,rIn) {}
	virtual SfxPoolItem* Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem* Create(SvStream& rIn, USHORT nVer) const;


#ifdef SDR_ISPOOLABLE
	virtual int IsPoolable() const;
#endif
};

//------------------------------------------------------------
// class SdrSignedPercentItem
// Ganze Prozente von +/-
//------------------------------------------------------------

class SdrSignedPercentItem : public SfxInt16Item
{
public:
	TYPEINFO();
	SdrSignedPercentItem(): SfxInt16Item() {}
	SdrSignedPercentItem( USHORT nId, INT16 nVal = 0 ) : SfxInt16Item( nId,nVal ) {}
	SdrSignedPercentItem( USHORT nId, SvStream& rIn ) : SfxInt16Item( nId,rIn ) {}


#ifdef SDR_ISPOOLABLE
	virtual int IsPoolable() const;
#endif
};

}//end of namespace binfilter
#endif
