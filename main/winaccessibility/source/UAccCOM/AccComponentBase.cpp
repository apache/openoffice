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
#include "AccComponentBase.h"
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include "MAccessible.h"

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccComponentBase::CAccComponentBase()
{}

CAccComponentBase::~CAccComponentBase()
{}


/**
 * Returns the location of the upper left corner of the object's bounding 
 * box relative to the parent.
 * 
 * @param    Location    the upper left corner of the object's bounding box.
 */
STDMETHODIMP CAccComponentBase::get_locationInParent(long *x, long *y)
{
    
	CHECK_ENABLE_INF

    try
    {
        if (x == NULL || y == NULL)
            return E_INVALIDARG;
        // #CHECK XInterface#
        if(!pRXComp.is())
            return E_FAIL;

        const ::com::sun::star::awt::Point& pt = GetXInterface()->getLocation();
        *x = pt.X;
        *y = pt.Y;
        return S_OK;
    }
    catch(...)
    {
        return E_FAIL;
    }
}

/**
 * Returns the location of the upper left corner of the object's bounding 
 * box in screen.
 * 
 * @param    Location    the upper left corner of the object's bounding 
 *                       box in screen coordinates.
 */
STDMETHODIMP CAccComponentBase::get_locationOnScreen(long *x, long *y)
{
    
	CHECK_ENABLE_INF

    try
    {
        if (x == NULL || y == NULL)
            return E_INVALIDARG;
        // #CHECK XInterface#
        if(!pRXComp.is())
            return E_FAIL;

        const ::com::sun::star::awt::Point& pt = GetXInterface()->getLocationOnScreen();
        *x = pt.X;
        *y = pt.Y;
        return S_OK;

    }
    catch(...)
    {
        return E_FAIL;
    }
}

/**
 * Grabs the focus to this object.
 * 
 * @param    success    the boolean result to be returned.
 */
STDMETHODIMP CAccComponentBase::grabFocus(boolean * success)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (success == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXComp.is())
    {
        return E_FAIL;
    }
    GetXInterface()->grabFocus();
    *success = TRUE;

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
 * Returns the foreground color of this object.
 * 
 * @param    Color    the color of foreground.
 */
STDMETHODIMP CAccComponentBase::get_foreground(IA2Color * foreground)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (foreground == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXComp.is())
    {
        return E_FAIL;
    }
    *foreground = (long)GetXInterface()->getForeground();

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
 * Returns the background color of this object.
 * 
 * @param    Color    the color of background.
 */
STDMETHODIMP CAccComponentBase::get_background(IA2Color * background)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (background == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXComp.is())
    {
        return E_FAIL;
    }
    *background = (long)GetXInterface()->getBackground();

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
 * Overide of IUNOXWrapper.
 *
 * @param    pXInterface    the pointer of UNO interface.
 */
STDMETHODIMP CAccComponentBase::put_XInterface(long pXInterface)
{
    
	CHECK_ENABLE_INF

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
    Reference<XAccessibleComponent> pRXI(pRContext,UNO_QUERY);
    if( !pRXI.is() )
        pRXComp = NULL;
    else
        pRXComp = pRXI.get();

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}
