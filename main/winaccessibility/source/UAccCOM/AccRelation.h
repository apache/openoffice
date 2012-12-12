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

#ifndef __ACCRELATION_H_
#define __ACCRELATION_H_

#include "resource.h"       // main symbols


#include <com/sun/star/uno/reference.hxx>
#include <com/sun/star/accessibility/XAccessibleRelationSet.hpp>
#include "UNOXWrapper.h"

/**
 * CAccRelation implements IAccessibleRelation interface.
 */
class ATL_NO_VTABLE CAccRelation :
            public CComObjectRoot,
            public CComCoClass<CAccRelation, &CLSID_AccRelation>,
            public IAccessibleRelation,
            public CUNOXWrapper
{
public:
    CAccRelation()
    {
            }
    virtual ~CAccRelation()
    {
            }

    DECLARE_REGISTRY_RESOURCEID(IDR_ACCRELATION)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CAccRelation)
    COM_INTERFACE_ENTRY(IAccessibleRelation)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    END_COM_MAP()

    // IAccessibleRelation
public:
    // IAccessibleRelation

    // Gets what the type of relation is.
    STDMETHOD(get_relationType)(BSTR * relationType);

    // Gets what the type of localized relation is.
    STDMETHOD(get_localizedRelationType)(BSTR * relationType);

    // Gets how many targets this relation have.
    STDMETHOD(get_nTargets)(long * nTargets);

    // Gets one accessible relation target.
    STDMETHOD(get_target)(long targetIndex, IUnknown * * target);

    // Gets multiple accessible relation targets.
    STDMETHOD(get_targets)(long maxTargets, IUnknown * * target, long * nTargets);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XSubInterface)(long pXSubInterface);

    //static OLECHAR* getRelationTypeOLECHAR(int type);
    static BSTR getRelationTypeBSTR(int type);

private:

    com::sun::star::accessibility::AccessibleRelation relation;
};

#endif //__ACCRELATION_H_
