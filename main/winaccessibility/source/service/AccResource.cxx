/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2010.
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "AccResource.hxx"

#include <tools/simplerm.hxx>

// ---- needed as long as we have no contexts for components ---
#include <vcl/svapp.hxx>

//2009.04. Comment the following code because the acc resource is not necessary now. Can open them if necessary
//#ifndef _TOOLS_INTN_HXX
//#include <tools/intn.hxx>
//#endif
//#ifndef _SOLAR_HRC
//#include <svtools/solar.hrc>
//#endif

//.........................................................................

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
	//Comment the following code because the acc resource is not necessary now. Can open them if necessary
	/* 
    if (m_pImpl)
        return;

    LanguageType nType = Application::GetSettings().GetUILanguage();

    ByteString sFileName("winaccessibility");
    sFileName += ByteString::CreateFromInt32( (sal_Int32)SOLARUPD );

    m_pImpl = SimpleResMgr::Create(sFileName.GetBuffer(), nType);

    if (m_pImpl)
    {
        // no that we have a impl class make sure it's deleted on unloading the library
        static ResourceManager::EnsureDelete	s_aDeleteTheImplClas;
    }
  */
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


