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



// MARKER(update_precomp.py): autogen include statement, do not remove


#include <map>

#ifndef _TOOLS_RESMGR_HXX
#include <tools/resmgr.hxx>
#endif
#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _VOS_PROCESS_HXX_
#include <vos/process.hxx>
#endif

#ifndef _SVTOOLS_SVTDATA_HXX
#include <bf_svtools/svtdata.hxx>
#endif

namespace binfilter
{

ImpSvtData::~ImpSvtData()
{
	delete pResMgr;
}

ResMgr * ImpSvtData::GetResMgr(const ::com::sun::star::lang::Locale aLocale)
{
	if (!pResMgr)
		pResMgr = ResMgr::CreateResMgr(CREATEVERSIONRESMGR_NAME(bf_svt), aLocale );
	return pResMgr;
}

ImpSvtData & ImpSvtData::GetSvtData()
{
	void ** pAppData = GetAppData(BF_SHL_SVT);
	if (!*pAppData)
		*pAppData= new ImpSvtData;
	return *static_cast<ImpSvtData *>(*pAppData);
}

}
