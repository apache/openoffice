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



#include "frm_resource.hxx"

#ifndef _TOOLS_SIMPLERESMGR_HXX_
#include <tools/simplerm.hxx>
#endif

// ---- needed as long as we have no contexts for components ---
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
//---------------------------------------------------

#include <bf_svtools/bf_solar.h>
namespace binfilter {

//.........................................................................
namespace frm
{

	//==================================================================
	//= ResourceManager
	//==================================================================
	SimpleResMgr* ResourceManager::m_pImpl = NULL;

	//------------------------------------------------------------------
	ResourceManager::EnsureDelete::~EnsureDelete()
	{
		delete ResourceManager::m_pImpl;
	}

	//------------------------------------------------------------------
	void ResourceManager::ensureImplExists()
	{
		if (m_pImpl)
			return;

		ByteString sFileName("bf_frm");	//STRIP005

		m_pImpl = SimpleResMgr::Create(sFileName.GetBuffer(), Application::GetSettings().GetUILocale());

		if (m_pImpl)
		{
			// no that we have a impl class make sure it's deleted on unloading the library
			static ResourceManager::EnsureDelete	s_aDeleteTheImplClas;
		}
	}

	//------------------------------------------------------------------
	::rtl::OUString ResourceManager::loadString(sal_uInt16 _nResId)
	{
		::rtl::OUString sReturn;

		ensureImplExists();
		if (m_pImpl)
			sReturn = m_pImpl->ReadString(_nResId);

		return sReturn;
	}

//.........................................................................
}
//.........................................................................

}
