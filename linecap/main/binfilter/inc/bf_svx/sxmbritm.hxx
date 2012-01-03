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


#ifndef _SXMBRITM_HXX
#define _SXMBRITM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif

#ifndef SXMBRITM_HXX
#include <bf_svx/sdynitm.hxx>
#endif
namespace binfilter {

// Die Masslinie unterhalb der Bezugskante
// Zur Bemassung einer Objektunterkante
// (Redundant zum drehen der Bezugskante um 180deg +
// TextUpsideDown, jedoch besser bedienbar)
// laesst sich auch draggen, indem man den Masslinienabstand
// (SdrMeasureLineDistItem) ueber die Bezugskante hinwegdraggt
class SdrMeasureBelowRefEdgeItem: public SdrYesNoItem {
public:
	SdrMeasureBelowRefEdgeItem(FASTBOOL bOn=FALSE): SdrYesNoItem(SDRATTR_MEASUREBELOWREFEDGE,bOn) {}
	SdrMeasureBelowRefEdgeItem(SvStream& rIn): SdrYesNoItem(SDRATTR_MEASUREBELOWREFEDGE,rIn) {}
};

}//end of namespace binfilter
#endif
