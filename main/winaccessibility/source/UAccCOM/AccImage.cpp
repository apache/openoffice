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
#include "UAccCOM2.h"
#include "AccImage.h"
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;

/**
   * Get description.
   * @param description Variant to get description.
   * @return Result.
*/
STDMETHODIMP CAccImage::get_description(BSTR * description)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK#
    if (description == NULL)
        return E_INVALIDARG;
    if( !pRXImg.is() )
        return E_FAIL;

    ::rtl::OUString ouStr = GetXInterface()->getAccessibleImageDescription();
    SAFE_SYSFREESTRING(*description);
    *description = SysAllocString((OLECHAR*)ouStr.getStr());

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

STDMETHODIMP CAccImage::get_imagePosition(
    /* [in] */ enum IA2CoordinateType,
    /* [out] */ long __RPC_FAR *,
    /* [retval][out] */ long __RPC_FAR *)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAccImage::get_imageSize(
    /* [out] */ long __RPC_FAR *,
    /* [retval][out] */ long __RPC_FAR *)
{
    return E_NOTIMPL;
}

/**
   * Put UNO interface.
   * @param pXInterface UNO interface.
   * @return Result.
*/
STDMETHODIMP CAccImage::put_XInterface(long pXInterface)
{
    

    ENTER_PROTECTED_BLOCK

    CUNOXWrapper::put_XInterface(pXInterface);
    //special query.
    if(pUNOInterface == NULL)
        return E_FAIL;

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();
    if( !pRContext.is() )
    {
        return E_FAIL;
    }
    Reference<XAccessibleImage> pRXI(pRContext,UNO_QUERY);
    if( !pRXI.is() )
        pRXImg = NULL;
    else
        pRXImg = pRXI.get();
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}
