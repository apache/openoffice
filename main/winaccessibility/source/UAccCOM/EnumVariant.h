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

#ifndef __ENUMVARIANT_H_
#define __ENUMVARIANT_H_

#define WNT

#include "resource.h"       // main symbols
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include "UAccCOM2.h"
#include <AccObjectManagerAgent.hxx>

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;

#include <vector>
#include <algorithm>
using namespace std;

/**
 * CEnumVariant implements IEnumVARIANT interface.
 */
class ATL_NO_VTABLE CEnumVariant :
            public CComObjectRootEx<CComMultiThreadModel>,
            public CComCoClass<CEnumVariant, &CLSID_EnumVariant>,
            public IDispatchImpl<IEnumVariant, &IID_IEnumVariant, &LIBID_UACCCOMLib>
{
public:
    CEnumVariant()
            :m_lLBound(0),
            pUNOInterface(NULL),
            m_pXAccessibleSelection(NULL)
    {
        m_lCurrent = m_lLBound;
    }

	virtual ~CEnumVariant() {};

    DECLARE_REGISTRY_RESOURCEID(IDR_ENUMVARIANT)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CEnumVariant)
    COM_INTERFACE_ENTRY(IEnumVariant)
    COM_INTERFACE_ENTRY(IEnumVARIANT)
    END_COM_MAP()

    // IEnumVariant
public:

    STDMETHOD(ClearEnumeration)();

    // IEnumVARIANT

    //
    HRESULT STDMETHODCALLTYPE Next(ULONG cElements,VARIANT __RPC_FAR *pvar,ULONG __RPC_FAR *pcElementFetched);

    //
    HRESULT STDMETHODCALLTYPE Skip(ULONG cElements);

    //
    HRESULT STDMETHODCALLTYPE Reset( void);

    //
    HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT __RPC_FAR *__RPC_FAR *ppenum);

    // IEnumVariant

    //
    HRESULT STDMETHODCALLTYPE PutSelection(long pXSelection);

    //
    static HRESULT STDMETHODCALLTYPE Create(CEnumVariant __RPC_FAR *__RPC_FAR *ppenum);

    //
    long GetCountOfElements();

private:

    long m_lCurrent;
    long m_lLBound;
    XAccessible* pUNOInterface;						// XAccessible.
    Reference<XAccessibleSelection> m_pXAccessibleSelection;	// Selection.
};

#endif //__ENUMVARIANT_H_
