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

/**
 * AccComponent.cpp : Implementation of CUAccCOMApp and DLL registration.
 */
#include "stdafx.h"
#include "UAccCOM2.h"
#include "AccComponent.h"

/**
 * Returns the location of the upper left corner of the object's bounding 
 * box relative to the parent.
 * 
 * @param    Location    the upper left corner of the object's bounding box.
 */
STDMETHODIMP CAccComponent::get_locationInParent(long *x, long *y)
{
    
    return CAccComponentBase::get_locationInParent(x,y);
}

/**
 * Returns the foreground color of this object.
 * 
 * @param    Color    the color of foreground.
 */
STDMETHODIMP CAccComponent::get_foreground(IA2Color * foreground)
{
    
    return CAccComponentBase::get_foreground(foreground);
}

/**
 * Returns the background color of this object.
 * 
 * @param    Color    the color of background.
 */
STDMETHODIMP CAccComponent::get_background(IA2Color * background)
{
    
    return CAccComponentBase::get_background(background);
}

/**
 * Overide of IUNOXWrapper.
 *
 * @param    pXInterface    the pointer of UNO interface.
 */
STDMETHODIMP CAccComponent::put_XInterface(long pXInterface)
{
    
    return CAccComponentBase::put_XInterface(pXInterface);
}
