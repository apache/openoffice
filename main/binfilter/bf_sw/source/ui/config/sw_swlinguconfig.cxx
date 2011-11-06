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



// #107253#
#ifndef _SWLINGUCONFIG_HXX
#include <swlinguconfig.hxx>
#endif

#ifndef _SVTOOLS_LINGUCFG_HXX_
#include <bf_svtools/lingucfg.hxx>
#endif
namespace binfilter {

// init static member
static SvtLinguConfig* mpImplLinguConfig = 0L;
static sal_uInt32 mnImplUseCount = 0L;

/*N*/ void ImplCreateOnDemand()
/*N*/ {
/*N*/ 	if(!mpImplLinguConfig && mnImplUseCount)
/*N*/ 	{
/*N*/ 		mpImplLinguConfig = new SvtLinguConfig();
/*N*/ 	}
/*N*/ }

/*N*/ SwLinguConfig::SwLinguConfig()
/*N*/ {
/*N*/ 	mnImplUseCount++;
/*N*/ }

/*N*/ SwLinguConfig::~SwLinguConfig()
/*N*/ {
/*N*/ 	mnImplUseCount--;
/*N*/ 
/*N*/ 	if(!mnImplUseCount && mpImplLinguConfig)
/*N*/ 	{
/*N*/ 		delete mpImplLinguConfig;
/*N*/ 		mpImplLinguConfig = 0L;
/*N*/ 	}
/*N*/ }


/*N*/ sal_Bool SwLinguConfig::GetOptions(SvtLinguOptions &rOptions) const
/*N*/ {
/*N*/ 	ImplCreateOnDemand();
/*N*/ 	return mpImplLinguConfig->GetOptions(rOptions);
/*N*/ }


// eof
}
