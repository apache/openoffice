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



#include <bf_svx/svdobj.hxx>
#include <bf_svtools/moduleoptions.hxx>

#include "bf_sd/grdocsh.hxx"
#include "sdresid.hxx"
#include "sdobjfac.hxx"
#include "cfgids.hxx"

namespace binfilter {

void SdDLL::Init()
{
	// the SdModule must be created
	SdModuleDummy** ppShlPtr = (SdModuleDummy**) GetAppData(BF_SHL_DRAW);
	SvFactory* pDrawFact    = (SvFactory*)(*ppShlPtr)->pSdDrawDocShellFactory;
	SvFactory* pGraphicFact = (SvFactory*)(*ppShlPtr)->pSdGraphicDocShellFactory;
	delete (*ppShlPtr);
	(*ppShlPtr) = new SdModule(pDrawFact, pGraphicFact);
	(*ppShlPtr)->pSdDrawDocShellFactory    = pDrawFact;
	(*ppShlPtr)->pSdGraphicDocShellFactory = pGraphicFact;

	SdrObjFactory::InsertMakeUserDataHdl(LINK(&aSdObjectFactory, SdObjectFactory, MakeUserData));
}

void SdDLL::Exit()
{
	SdrObjFactory::RemoveMakeUserDataHdl(LINK(&aSdObjectFactory, SdObjectFactory, MakeUserData));

	SdModuleDummy** ppShlPtr = (SdModuleDummy**) GetAppData(BF_SHL_DRAW);
	delete (*ppShlPtr);
	(*ppShlPtr) = NULL;
}
}
