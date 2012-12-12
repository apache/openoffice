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

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "UAccCOM2.h"

#include "UAccCOM_i.c"
#include "Accessible2_i.c"
#include "AccessibleComponent_i.c"
#include "AccessibleText_i.c"
#include "AccessibleEditableText_i.c"
#include "AccessibleImage_i.c"
#include "AccessibleTable_i.c"
#include "AccessibleAction_i.c"
#include "AccessibleValue_i.c"
#include "AccessibleHyperText_i.c"
#include "AccessibleRelation_i.c"
#include "AccessibleHyperLink_i.c"

#include "MAccessible.h"
#include "EnumVariant.h"
#include "UNOXWrapper.h"
#include "AccComponent.h"
#include "AccRelation.h"
#include "AccAction.h"
#include "AccText.h"
#include "AccEditableText.h"
#include "AccImage.h"
#include "AccValue.h"
#include "AccTable.h"
#include "AccHyperLink.h"
#include "AccHyperText.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_MAccessible, CMAccessible)
OBJECT_ENTRY(CLSID_EnumVariant, CEnumVariant)
OBJECT_ENTRY(CLSID_AccComponent, CAccComponent)
OBJECT_ENTRY(CLSID_AccRelation, CAccRelation)
OBJECT_ENTRY(CLSID_AccAction, CAccAction)
OBJECT_ENTRY(CLSID_AccText, CAccText)
OBJECT_ENTRY(CLSID_AccEditableText, CAccEditableText)
OBJECT_ENTRY(CLSID_AccImage, CAccImage)
OBJECT_ENTRY(CLSID_AccValue, CAccValue)
OBJECT_ENTRY(CLSID_AccTable, CAccTable)
OBJECT_ENTRY(CLSID_AccHyperLink, CAccHyperLink)
OBJECT_ENTRY(CLSID_AccHypertext, CAccHypertext)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
    BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_UACCCOMLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}
