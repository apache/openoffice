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


#ifndef _SXCECITM_HXX
#define _SXCECITM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif

#ifndef _SDYNITM_HXX
#include <bf_svx/sdynitm.hxx>
#endif

#ifndef _SDMETITM_HXX
#include <bf_svx/sdmetitm.hxx>
#endif
namespace binfilter {

enum SdrCaptionEscDir {SDRCAPT_ESCHORIZONTAL,SDRCAPT_ESCVERTICAL,SDRCAPT_ESCBESTFIT};

//------------------------------
// class SdrCaptionEscDirItem
//------------------------------
class SdrCaptionEscDirItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrCaptionEscDirItem(SdrCaptionEscDir eDir=SDRCAPT_ESCHORIZONTAL): SfxEnumItem(SDRATTR_CAPTIONESCDIR,eDir) {}
	SdrCaptionEscDirItem(SvStream& rIn)                              : SfxEnumItem(SDRATTR_CAPTIONESCDIR,rIn)  {}
	virtual SfxPoolItem*     Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem*     Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT           GetValueCount() const; // { return 3; }
	SdrCaptionEscDir GetValue() const      { return (SdrCaptionEscDir)SfxEnumItem::GetValue(); }


};

//------------------------------------
// class SdrCaptionEscIsRelItem
// TRUE=Linienaustrittsposition relativ
// FALSE=Linienaustrittsposition absolut
//------------------------------------
class SdrCaptionEscIsRelItem: public SdrYesNoItem {
public:
	SdrCaptionEscIsRelItem(BOOL bRel=TRUE): SdrYesNoItem(SDRATTR_CAPTIONESCISREL,bRel) {}
	SdrCaptionEscIsRelItem(SvStream& rIn) : SdrYesNoItem(SDRATTR_CAPTIONESCISREL,rIn)  {}
};

//------------------------------------
// class SdrCaptionEscRelItem
// Relativer Linienaustritt
//     0 =   0.00% = oben bzw. links,
// 10000 = 100.00% = rechts bzw. unten
// nur wenn SdrCaptionEscIsRelItem=TRUE
//------------------------------------
class SdrCaptionEscRelItem: public SfxInt32Item {
public:
	SdrCaptionEscRelItem(long nEscRel=5000): SfxInt32Item(SDRATTR_CAPTIONESCREL,nEscRel) {}
	SdrCaptionEscRelItem(SvStream& rIn)    : SfxInt32Item(SDRATTR_CAPTIONESCREL,rIn)     {}
};

//------------------------------------
// class SdrCaptionEscAbsItem
// Absoluter Linienaustritt
// 0  = oben bzw. links,
// >0 = in Richtung rechts bzw. unten
// nur wenn SdrCaptionEscIsRelItem=FALSE
//------------------------------------
class SdrCaptionEscAbsItem: public SdrMetricItem {
public:
	SdrCaptionEscAbsItem(long nEscAbs=0): SdrMetricItem(SDRATTR_CAPTIONESCABS,nEscAbs) {}
	SdrCaptionEscAbsItem(SvStream& rIn) : SdrMetricItem(SDRATTR_CAPTIONESCABS,rIn)     {}
};

}//end of namespace binfilter
#endif
