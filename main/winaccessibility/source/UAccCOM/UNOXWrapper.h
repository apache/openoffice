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

#ifndef __UNOXWRAPPER_H_
#define __UNOXWRAPPER_H_

#define WNT

#include "resource.h"       // main symbols


#include <com/sun/star/accessibility/XAccessible.hpp>
#include "UAccCOM2.h"

/**
 * CUNOXWrapper implements IUNOXWrapper interface.
 */
class ATL_NO_VTABLE CUNOXWrapper : public IUNOXWrapper
{
public:
    CUNOXWrapper()
    {	}

public:
    // IUNOXWrapper

    STDMETHOD(put_XInterface)(long pXInterface)
    {
        pUNOInterface = (com::sun::star::accessibility::XAccessible*)pXInterface;
        return S_OK;
    }

    STDMETHOD(put_XSubInterface)(long)
    {
        return S_OK;
    }

    STDMETHOD(get_XInterface)(/*[out,retval]*/long *pXInterface)
    {
        *pXInterface = (long)pUNOInterface;//.get();
        return S_OK;
    }

    STDMETHOD(get_XSubInterface)(/*[out,retval]*/long *)
    {
        return S_OK;
    }

protected:

    com::sun::star::accessibility::XAccessible* pUNOInterface;
};

#endif //__UNOXWRAPPER_H_
