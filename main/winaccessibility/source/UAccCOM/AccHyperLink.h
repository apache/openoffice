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

#if !defined(AFX_ACCHYPERLINK_H__59DA79A5_A895_43DB_9495_2B2049CF5C65__INCLUDED_)
#define AFX_ACCHYPERLINK_H__59DA79A5_A895_43DB_9495_2B2049CF5C65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols


#include <com/sun/star/accessibility/XAccessibleHyperlink.hpp>
#include "AccActionBase.h"
#include "UNOXWrapper.h"

/**
 * CAccHyperLink implements IAccessibleHyperlink interface.
 */
class ATL_NO_VTABLE CAccHyperLink :
            public CComObjectRoot,
            public CComCoClass<CAccHyperLink,&CLSID_AccHyperLink>,
            public IAccessibleHyperlink,
            public CAccActionBase
{
public:
    CAccHyperLink()
    {
            }
    ~CAccHyperLink()
    {
            }

    BEGIN_COM_MAP(CAccHyperLink)
    COM_INTERFACE_ENTRY(IAccessibleAction)
    COM_INTERFACE_ENTRY(IAccessibleHyperlink)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    END_COM_MAP()

    DECLARE_REGISTRY_RESOURCEID(IDR_AccHyperLink)

    static HRESULT WINAPI _SmartQI(void* pv,
                                   REFIID iid, void** ppvObject, DWORD)
    {
        return ((CAccHyperLink*)pv)->SmartQI(iid,ppvObject);
    }

    HRESULT SmartQI(REFIID iid, void** ppvObject)
    {
        if( m_pOuterUnknown )
            return OuterQueryInterface(iid,ppvObject);
        return E_FAIL;
    }

    // IAccessibleHyperlink
public:
    // IAccessibleAction

    // Returns the number of action.
    STDMETHOD(nActions)(/*[out,retval]*/long* nActions);

    // Performs specified action on the object.
    STDMETHOD(doAction)(/* [in] */ long actionIndex);

    // get the action name
    STDMETHOD(get_name)( long actionIndex, BSTR __RPC_FAR *name);

    // get the localized action name
    STDMETHOD(get_localizedName)( long actionIndex, BSTR __RPC_FAR *localizedName);

    // Gets description of specified action.
    STDMETHOD(get_description)(long actionIndex,BSTR __RPC_FAR *description);

    // Returns key binding object (if any) associated with specified action
    // key binding is string.
    // e.g. "alt+d" (like IAccessible::get_accKeyboardShortcut).
    STDMETHOD(get_keyBinding)(
        /* [in] */ long actionIndex,
        /* [in] */ long nMaxBinding,
        /* [length_is][length_is][size_is][size_is][out] */ BSTR __RPC_FAR *__RPC_FAR *keyBinding,
        /* [retval][out] */ long __RPC_FAR *nBinding);

    // IAccessibleHyperlink

    // get an object, e.g. BSTR or image object, that is overloaded with link behavior
    STDMETHOD(get_anchor)(/* [in] */ long index,
                                     /* [retval][out] */ VARIANT __RPC_FAR *anchor);

    // get an object representing the target of the link, usually a BSTR of the URI
    STDMETHOD(get_anchorTarget)(/* [in] */ long index,
                                           /* [retval][out] */ VARIANT __RPC_FAR *anchorTarget);

    // Returns the index at which the textual representation of the
    // hyperlink (group) starts.
    STDMETHOD(get_startIndex)(/* [retval][out] */ long __RPC_FAR *index);

    // Returns the index at which the textual rerpesentation of the
    // hyperlink (group) ends.
    STDMETHOD(get_endIndex)(/* [retval][out] */ long __RPC_FAR *index);

    // Returns whether the document referenced by this links is still valid.
    STDMETHOD(get_valid)(/* [retval][out] */ boolean __RPC_FAR *valid);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XSubInterface)(long pXSubInterface);

private:

    com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessibleHyperlink> pRXLink;

    inline com::sun::star::accessibility::XAccessibleHyperlink* GetXInterface()
    {
        return pRXLink.get();
    }

};

#endif // !defined(AFX_ACCHYPERLINK_H__59DA79A5_A895_43DB_9495_2B2049CF5C65__INCLUDED_)
