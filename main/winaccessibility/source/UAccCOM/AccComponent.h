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

#if !defined(AFX_ACCCOMPONENT_H__626D760C_3944_4B0E_BB4D_F0D20AEDF7DC__INCLUDED_)
#define AFX_ACCCOMPONENT_H__626D760C_3944_4B0E_BB4D_F0D20AEDF7DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

#include "AccessibleComponent.h"


#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include "UNOXWrapper.h"
#include "AccComponentBase.h"

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;

/**
 * CAccComponent implements IAccessibleComponent interface.
 */
class ATL_NO_VTABLE CAccComponent :
            public CComObjectRoot,
            public CComCoClass<CAccComponent,&CLSID_AccComponent>,
            public IAccessibleComponent,
            public CAccComponentBase
{
public:
    CAccComponent()
    {
            }
    ~CAccComponent()
    {
            }

    BEGIN_COM_MAP(CAccComponent)
    COM_INTERFACE_ENTRY(IAccessibleComponent)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    COM_INTERFACE_ENTRY_FUNC_BLIND(NULL,_SmartQI)
    END_COM_MAP()

    static HRESULT WINAPI _SmartQI(void* pv,
                                   REFIID iid, void** ppvObject, DWORD)
    {
        return ((CAccComponent*)pv)->SmartQI(iid,ppvObject);
    }

    HRESULT SmartQI(REFIID iid, void** ppvObject)
    {
        if( m_pOuterUnknown )
            return OuterQueryInterface(iid,ppvObject);
        return E_FAIL;
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_AccComponent)

    // IAccessibleComponent
public:
    // IAccessibleComponent

    // Returns the location of the upper left corner of the object's bounding
    // box relative to the parent.
    STDMETHOD(get_locationInParent)(long *x, long *y);

    // Returns the foreground color of this object.
    STDMETHOD(get_foreground)(IA2Color * foreground);

    // Returns the background color of this object.
    STDMETHOD(get_background)(IA2Color * background);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

};

#endif // !defined(AFX_ACCCOMPONENT_H__626D760C_3944_4B0E_BB4D_F0D20AEDF7DC__INCLUDED_)
