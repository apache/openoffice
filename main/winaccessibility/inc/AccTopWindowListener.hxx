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

#ifndef __ACCTOPWINDOWLISTENER_HXX
#define __ACCTOPWINDOWLISTENER_HXX

#include <com/sun/star/awt/XTopWindowListener.hpp>
#include <com/sun/star/awt/XExtendedToolkit.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <cppuhelper/weak.hxx>

#include  "AccObjectManagerAgent.hxx"

/**
 * AccEventListener is the general event listener for all top windows. The top windows defined
 * in UNO are: FRAME, WINDOW, DIALOG, MENU, TOOLTIP.
 * It implements the methods of XTopWindowListener and the most important method is windowOpened().
 * In this method, all the accessible objects (including COM object and Uno objects) are created and
 * cached into bridge managers, and they are monitored by listeners for later accessible evnet handling.
 */
class AccTopWindowListener:
            public com::sun::star::awt::XTopWindowListener,
            public ::cppu::OWeakObject

{
private:
    AccObjectManagerAgent accManagerAgent;
    oslInterlockedCount m_refcount;
public:
    AccTopWindowListener();
    virtual ~AccTopWindowListener();
    virtual void SAL_CALL windowOpened( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowClosing( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowClosed( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowMinimized( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowNormalized( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowActivated( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL windowDeactivated( const ::com::sun::star::lang::EventObject& e ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  ) throw ();
    virtual void SAL_CALL release(  ) throw ();
    virtual void AddAllListeners(com::sun::star::accessibility::XAccessible* pAccessible,com::sun::star::accessibility::XAccessible* pParentXAcc,HWND pWND );
    //for On-Demand load.
    virtual void handleWindowOpened( com::sun::star::accessibility::XAccessible* pAccessible );
};

#endif
