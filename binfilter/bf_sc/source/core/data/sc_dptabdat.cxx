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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

// INCLUDE ---------------------------------------------------------------


#include "dptabdat.hxx"
namespace binfilter {

// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------

/*N*/ ScDPTableData::ScDPTableData()
/*N*/ {
/*N*/ 	nLastDateVal = nLastHier = nLastLevel = nLastRet = -1;		// invalid
/*N*/ 
/*N*/ 	//!	reset before new calculation (in case the base date is changed)
/*N*/ }

/*N*/ ScDPTableData::~ScDPTableData()
/*N*/ {
/*N*/ }



// -----------------------------------------------------------------------




}
