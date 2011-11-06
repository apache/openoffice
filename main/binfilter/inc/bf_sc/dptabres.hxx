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



#ifndef SC_DPTABRES_HXX
#define SC_DPTABRES_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_MEMBERRESULT_HPP_
#include <com/sun/star/sheet/MemberResult.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATARESULT_HPP_
#include <com/sun/star/sheet/DataResult.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"		// enum ScSubTotalFunc
#endif
namespace binfilter {


class ScAddress;
class ScDocument;
class ScDPSource;
class ScDPDimension;
class ScDPLevel;
class ScDPMember;
class ScDPAggData;

struct ScDPValueData;

//
//	aggregated data
//!	separate header file?
//





// --------------------------------------------------------------------
//
//	results for a hierarchy dimension
//

#define SC_DP_RES_GROW	16

class ScDPResultDimension;
class ScDPDataDimension;
class ScDPDataMember;
struct ScDPItemData;


#define SC_DPMEASURE_ALL	-1
#define SC_DPMEASURE_ANY	-2

class ScDPResultData
{
public:
	ScDPResultData( ScDPSource* pSrc ) {DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ScDPResultData( ScDPSource* pSrc );		//! Ref
						~ScDPResultData(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ~ScDPResultData();





};


class ScDPResultMember
{

public:
						ScDPResultMember( ScDPResultData* pData, ScDPDimension* pDim,
											ScDPLevel* pLev, ScDPMember* pDesc,
											BOOL bForceSub ) {DBG_BF_ASSERT(0, "STRIP");} //STRIP001 BOOL bForceSub );	//! Ref
						~ScDPResultMember(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 ~ScDPResultMember();



//	BOOL				SubTotalEnabled() const;






						//!	this will be removed!
};









						//!	this will be removed!

//!	replace PtrArr with 32-bit array ????




//	result dimension contains only members



						//	allocates new members



						//	modifies existing members, allocates data dimensions



						//	for ScDPDataDimension::InitFrom






} //namespace binfilter
#endif

