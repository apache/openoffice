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

#if !defined(AFX_ACCVALUE_H__CBA4972C_4188_4A18_A3CD_4A1FA3DDED38__INCLUDED_)
#define AFX_ACCVALUE_H__CBA4972C_4188_4A18_A3CD_4A1FA3DDED38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"           // main symbols


#include <com/sun/star/uno/reference.hxx>
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include "UNOXWrapper.h"

/**
 * CAccValue implements IAccessibleValue interface.
 */
class CAccValue :
            public CComObjectRoot,
            public CComCoClass<CAccValue,&CLSID_AccValue>,
            public IAccessibleValue,
            public CUNOXWrapper
{
public:
    CAccValue()
    {
            }
    virtual ~CAccValue()
    {
            }

    BEGIN_COM_MAP(CAccValue)
    COM_INTERFACE_ENTRY(IAccessibleValue)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    COM_INTERFACE_ENTRY_FUNC_BLIND(NULL,_SmartQI)
    END_COM_MAP()

    static HRESULT WINAPI _SmartQI(void* pv,
                                   REFIID iid, void** ppvObject, DWORD)
    {
        return ((CAccValue*)pv)->SmartQI(iid,ppvObject);
    }

    HRESULT SmartQI(REFIID iid, void** ppvObject)
    {
        if( m_pOuterUnknown )
            return OuterQueryInterface(iid,ppvObject);
        return E_FAIL;
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_AccValue)

    // IAccessibleValue
public:
    // IAccessibleValue

    // Returns the value of this object as a number.
    STDMETHOD(get_currentValue)(VARIANT *currentValue);

    // Sets the value of this object to the given number.
    STDMETHOD(setCurrentValue)(VARIANT value);

    // Returns the maximal value that can be represented by this object.
    STDMETHOD(get_maximumValue)(VARIANT *maximumValue);

    // Returns the minimal value that can be represented by this object.
    STDMETHOD(get_minimumValue)(VARIANT *mininumValue);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

private:

    com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessibleValue> pRXVal;

    inline com::sun::star::accessibility::XAccessibleValue* GetXInterface()
    {
        return pRXVal.get();
    }

};

#endif // !defined(AFX_ACCVALUE_H__CBA4972C_4188_4A18_A3CD_4A1FA3DDED38__INCLUDED_)
