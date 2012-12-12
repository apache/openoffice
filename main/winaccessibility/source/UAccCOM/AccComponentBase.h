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

//////////////////////////////////////////////////////////////////////
// AccComponentBase.h: interface for the CAccComponentBase class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCCOMPONENTBASE_H__946BE230_1DCB_494B_ACF6_32A2E197DD2A__INCLUDED_)
#define AFX_ACCCOMPONENTBASE_H__946BE230_1DCB_494B_ACF6_32A2E197DD2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WNT

#include <com/sun/star/uno/reference.hxx>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include "UNOXWrapper.h"

class ATL_NO_VTABLE CAccComponentBase : public CUNOXWrapper
{
public:
    CAccComponentBase();
    virtual ~CAccComponentBase();
    // IAccessibleComponent
public:
    // IAccessibleComponent

    // Returns the location of the upper left corner of the object's bounding
    // box relative to the parent.
    STDMETHOD(get_locationInParent)(long *x, long *y);

    // Returns the location of the upper left corner of the object's bounding
    // box in screen.
    STDMETHOD(get_locationOnScreen)(long *x, long *y);

    // Grabs the focus to this object.
    STDMETHOD(grabFocus)(boolean * success);

    // Returns the foreground color of this object.
    STDMETHOD(get_foreground)(IA2Color * foreground);

    // Returns the background color of this object.
    STDMETHOD(get_background)(IA2Color * background);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

protected:

    com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessibleComponent> pRXComp;

    inline com::sun::star::accessibility::XAccessibleComponent* GetXInterface()
    {
        return pRXComp.get();
    }
};

#endif // !defined(AFX_ACCCOMPONENTBASE_H__946BE230_1DCB_494B_ACF6_32A2E197DD2A__INCLUDED_)
