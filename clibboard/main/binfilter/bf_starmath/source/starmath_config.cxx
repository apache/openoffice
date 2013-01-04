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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif
#ifndef _SFXSIDS_HRC //autogen
#include <bf_sfx2/sfxsids.hrc>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX //autogen
#include <bf_svtools/itempool.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif


#ifndef CONFIG_HXX
#include "config.hxx"
#endif
#ifndef _STARMATH_HRC
#include "starmath.hrc"
#endif
namespace binfilter {

/////////////////////////////////////////////////////////////////

/*N*/ SmConfig::SmConfig()
/*N*/ {
/*N*/ }


/*N*/ SmConfig::~SmConfig()
/*N*/ {
/*N*/ }




/*N*/ void SmConfig::ConfigToItemSet(SfxItemSet &rSet) const
/*N*/ {
/*N*/ 	const SfxItemPool *pPool = rSet.GetPool();
/*N*/
/*N*/ 	rSet.Put(SfxUInt16Item(pPool->GetWhich(SID_PRINTSIZE),
/*N*/                            (UINT16) GetPrintSize()));
/*N*/ 	rSet.Put(SfxUInt16Item(pPool->GetWhich(SID_PRINTZOOM),
/*N*/                            (UINT16) GetPrintZoomFactor()));
/*N*/
/*N*/     rSet.Put(SfxBoolItem(pPool->GetWhich(SID_PRINTTITLE), IsPrintTitle()));
/*N*/     rSet.Put(SfxBoolItem(pPool->GetWhich(SID_PRINTTEXT),  IsPrintFormulaText()));
/*N*/     rSet.Put(SfxBoolItem(pPool->GetWhich(SID_PRINTFRAME), IsPrintFrame()));
/*N*/     rSet.Put(SfxBoolItem(pPool->GetWhich(SID_AUTOREDRAW), IsAutoRedraw()));
/*N*/     rSet.Put(SfxBoolItem(pPool->GetWhich(SID_NO_RIGHT_SPACES), IsIgnoreSpacesRight()));
/*N*/ }


/////////////////////////////////////////////////////////////////


}
