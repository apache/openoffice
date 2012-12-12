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

#if !defined(AFX_ACCIMAGE_H__D5E55275_CCD4_497F_8E77_F9B391D2F4A8__INCLUDED_)
#define AFX_ACCIMAGE_H__D5E55275_CCD4_497F_8E77_F9B391D2F4A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"        // main symbols


#include <com/sun/star/uno/reference.hxx>
#include <com/sun/star/accessibility/XAccessibleImage.hpp>
#include "UNOXWrapper.h"

/**
 * CAccImage implements IAccessibleImage interface.
 */
class ATL_NO_VTABLE CAccImage :
            public CComObjectRoot,
            public CComCoClass<CAccImage,&CLSID_AccImage>,
            public IAccessibleImage,
            public CUNOXWrapper
{
public:
    CAccImage()
    {
            }
    virtual ~CAccImage()
    {
            }
    BEGIN_COM_MAP(CAccImage)
    COM_INTERFACE_ENTRY(IAccessibleImage)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    COM_INTERFACE_ENTRY_FUNC_BLIND(NULL,_SmartQI)
    END_COM_MAP()

    static HRESULT WINAPI _SmartQI(void* pv,
                                   REFIID iid, void** ppvObject, DWORD)
    {
        return ((CAccImage*)pv)->SmartQI(iid,ppvObject);
    }

    HRESULT SmartQI(REFIID iid, void** ppvObject)
    {
        if( m_pOuterUnknown )
            return OuterQueryInterface(iid,ppvObject);
        return E_FAIL;
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_AccImage)

    // IAccessibleImage
public:
    // IAccessibleImage

    // Gets the description of the image.
    STDMETHOD(get_description)(BSTR * description);

    STDMETHOD(get_imagePosition)( enum IA2CoordinateType coordinateType,
                                  long __RPC_FAR *x,
                                  long __RPC_FAR *y);

    STDMETHOD(get_imageSize)(
        long __RPC_FAR *height,
        long __RPC_FAR *width);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

private:

    com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessibleImage> pRXImg;

    inline com::sun::star::accessibility::XAccessibleImage* GetXInterface()
    {
        return pRXImg.get();
    }
};

#endif // !defined(AFX_ACCIMAGE_H__D5E55275_CCD4_497F_8E77_F9B391D2F4A8__INCLUDED_)
