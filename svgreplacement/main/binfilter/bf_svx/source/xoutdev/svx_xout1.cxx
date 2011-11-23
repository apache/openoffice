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

#include "xoutx.hxx"

#ifdef JOEDEBUG
#endif
namespace binfilter {

#define GLOBALOVERFLOW

/*************************************************************************
|*
|*    XOutputDevice::SetOffset()
|*
|*    Beschreibung
|*    Ersterstellung    08.11.94
|*    Letzte Aenderung  08.11.94
|*
*************************************************************************/

/*N*/ void XOutputDevice::SetOffset( const Point& rOfs )
/*N*/ {
/*N*/ 	if (rOfs!=aOfs)
/*N*/ 	{
/*N*/ 		Point aDelta(rOfs);
/*N*/ 		Fraction aFact1(1,1);
/*N*/ 		aDelta-=aOfs;
/*N*/ #ifdef neeJOEDEBUG
/*N*/ 		DebWrite("XOutputDevice::SetOffset(");
/*N*/ 		DebOut(rOfs);
/*N*/ 		DebWrite("Alter Offset: ");
/*N*/ 		DebOut(aOfs);
/*N*/ 		DebWrite("Alter Origin: ");
/*N*/ 		DebOut(pOut->GetMapMode().GetOrigin());
/*N*/ 		DebWrite("Delta: ");
/*N*/ 		DebOut(aDelta);
/*N*/ #endif
//#ifdef JOEDEBUG
////        pOut->SetMapMode(MapMode(MAP_RELATIVE,Point(-aOfs.X(),-aOfs.Y()),aFact1,aFact1));
////        pOut->SetMapMode(MapMode(MAP_RELATIVE,rOfs,aFact1,aFact1));
/*N*/ 		pOut->SetMapMode(MapMode(MAP_RELATIVE,aDelta,aFact1,aFact1));
//#else
//        MapMode aMap(pOut->GetMapMode());
//        Point   aOrg(aMap.GetOrigin());
//        aOrg.X()+=aDelta.X();
//        aOrg.Y()+=aDelta.Y();
//        aMap.SetOrigin(aOrg);
//        pOut->SetMapMode(aMap);
//#endif
/*N*/ 		aOfs=rOfs;
/*N*/ #ifdef neeJOEDEBUG
/*N*/ 		DebWrite("Neuer Offset: ");
/*N*/ 		DebOut(aOfs);
/*N*/ 		DebWrite("Neuer Origin: ");
/*N*/ 		DebOut(pOut->GetMapMode().GetOrigin());
/*N*/ #endif
/*N*/ 	}
/*N*/ }

}
