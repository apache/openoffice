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



// include ---------------------------------------------------------------

#include <limits.h>

#include "dialogs.hrc"

#include "dlgutil.hxx"
namespace binfilter {

/*N*/ FieldUnit GetModuleFieldUnit( const SfxItemSet* pSet )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	FieldUnit eUnit = FUNIT_INCH;
/*N*/ 	return eUnit;
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ long CalcToUnit( float nIn, SfxMapUnit eUnit )
/*N*/ {
/*N*/ 	// nIn ist in Points
/*N*/ 
/*N*/ 	DBG_ASSERT( eUnit == SFX_MAPUNIT_TWIP 		||
/*N*/ 				eUnit == SFX_MAPUNIT_100TH_MM 	||
/*N*/ 				eUnit == SFX_MAPUNIT_10TH_MM 	||
/*N*/ 				eUnit == SFX_MAPUNIT_MM 		||
/*N*/ 				eUnit == SFX_MAPUNIT_CM, "this unit is not implemented" );
/*N*/ 
/*N*/ 	float nTmp = nIn;
/*N*/ 
/*N*/ 	if ( SFX_MAPUNIT_TWIP != eUnit )
/*N*/ 		nTmp = nIn * 10 / 567;
/*N*/ 
/*N*/ 	switch ( eUnit )
/*N*/ 	{
/*N*/ 		case SFX_MAPUNIT_100TH_MM:	nTmp *= 100; break;
/*?*/ 		case SFX_MAPUNIT_10TH_MM:	nTmp *= 10;	 break;
/*?*/ 		case SFX_MAPUNIT_MM:					 break;
/*?*/ 		case SFX_MAPUNIT_CM:		nTmp /= 10;	 break;
/*N*/ 	}
/*N*/ 
/*N*/ 	nTmp *= 20;
/*N*/ 	long nRet = (long)nTmp;
/*N*/ 	return nRet;
/*N*/ //!	return (long)(nTmp * 20);
/*N*/ }

// -----------------------------------------------------------------------

/*?*/ long ItemToControl( long nIn, SfxMapUnit eItem, SfxFieldUnit eCtrl )
/*?*/ {
/*?*/ DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 //STRIP001 	long nOut = 0;
/*?*/ }

// -----------------------------------------------------------------------

/*N*/ long CalcToPoint( long nIn, SfxMapUnit eUnit, USHORT nFaktor )
/*N*/ {
/*N*/ 	DBG_ASSERT( eUnit == SFX_MAPUNIT_TWIP 		||
/*N*/ 				eUnit == SFX_MAPUNIT_100TH_MM 	||
/*N*/ 				eUnit == SFX_MAPUNIT_10TH_MM 	||
/*N*/ 				eUnit == SFX_MAPUNIT_MM 		||
/*N*/ 				eUnit == SFX_MAPUNIT_CM, "this unit is not implemented" );
/*N*/ 
/*N*/ 	long nRet = 0;
/*N*/ 
/*N*/ 	if ( SFX_MAPUNIT_TWIP == eUnit )
/*N*/ 		nRet = nIn;
/*N*/ 	else
/*N*/ 		nRet = nIn * 567;
/*N*/ 
/*N*/ 	switch ( eUnit )
/*N*/ 	{
/*N*/ 		case SFX_MAPUNIT_100TH_MM:	nRet /= 100; break;
/*?*/ 		case SFX_MAPUNIT_10TH_MM:	nRet /= 10;	 break;
/*?*/ 		case SFX_MAPUNIT_MM:					 break;
/*?*/ 		case SFX_MAPUNIT_CM:		nRet *= 10;	 break;
/*N*/ 	}
/*N*/ 
/*N*/ 	// ggf. aufrunden
/*N*/ 	if ( SFX_MAPUNIT_TWIP != eUnit )
/*N*/ 	{
/*N*/ 		long nMod = 10;
/*N*/ 		long nTmp = nRet % nMod;
/*N*/ 
/*N*/ 		if ( nTmp >= 4 )
/*N*/ 			nRet += 10 - nTmp;
/*N*/ 		nRet /= 10;
/*N*/ 	}
/*N*/ 	return nRet * nFaktor / 20;
/*N*/ }

}
