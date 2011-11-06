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


#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif

#define _SVSTDARR_STRINGS
#include <bf_svtools/svstdarr.hxx>

#include "app.hxx"
#include "appdata.hxx"
#include "doctempl.hxx"
#include "docfile.hxx"
#include "imestatuswindow.hxx"

#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {

/*N*/ SfxAppData_Impl::SfxAppData_Impl( SfxApplication* pApp ) :
/*N*/ 		pProgress(0),
/*N*/ 		pPool(0),
/*N*/ 		pEventConfig(0),
/*N*/ 		nBasicCallLevel(0),
/*N*/ 		nRescheduleLocks(0),
/*N*/ 		nInReschedule(0),
/*N*/ 		pInitLinkList(0),
			pSfxFrameObjectFactoryPtr(0),
			pThisDocument(0),
/*N*/ 		pMatcher( 0 ),
/*N*/ 		pCancelMgr( 0 ),
/*N*/ 		pMiscConfig(0),
/*N*/ 		bInQuit(sal_False),
/*N*/ 		bInException( sal_False ),
/*N*/       m_xImeStatusWindow(new sfx2::appl::ImeStatusWindow(
/*N*/                                *pApp, ::legacy_binfilters::getLegacyProcessServiceFactory()))
/*N*/ {
/*N*/ }

/*N*/ SfxAppData_Impl::~SfxAppData_Impl()
/*N*/ {
/*N*/ 	delete pCancelMgr;
/*N*/ }
}
