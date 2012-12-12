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

#ifndef __ACCEVENTLISTENER_HXX
#define __ACCEVENTLISTENER_HXX

#include <com/sun/star/accessibility/XAccessibleEventListener.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <cppuhelper/weak.hxx>
#include <vos/mutex.hxx>

class AccObjectManagerAgent;
using namespace ::com::sun::star::uno;
/**
 * AccEventListener is the general event listener for all controls. It defines the
 * procedure of all the event handling and provides the basic support for some simple
 * methods. 
 */
class AccEventListener:
            public com::sun::star::accessibility::XAccessibleEventListener,
            public ::cppu::OWeakObject
{
private:
    oslInterlockedCount m_refcount;
protected:
    //accessible owner's pointer
    com::sun::star::accessibility::XAccessible* pAccessible;
    //agent pointer for objects' manager
    AccObjectManagerAgent* pAgent;
    //disposed state indicator
    bool  m_isDisposed;
    mutable ::vos::OMutex aRemoveMutex;
public:
    AccEventListener( com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent);
    virtual ~AccEventListener();

    //AccessibleEventListener
    virtual void SAL_CALL notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException);

    //for name changed event
    virtual void SAL_CALL handleNameChangedEvent(Any name);

    //for description changed event
    virtual void SAL_CALL handleDescriptionChangedEvent(Any desc);

    //for state changed event
    virtual void SAL_CALL handleStateChangedEvent (Any oldValue, Any newValue);
    virtual void SAL_CALL setComponentState(short state, bool enable);
    virtual void SAL_CALL fireStatePropertyChange(short state, bool set
                                                     );
    virtual void SAL_CALL fireStateFocusdChange(bool enable);

    //for bound rect changed event
    virtual void SAL_CALL handleBoundrectChangedEvent();

    //for visible data changed event
    virtual void SAL_CALL handleVisibleDataChangedEvent();

    //for interface
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire() throw ();
    virtual void SAL_CALL release() throw ();
    //get the accessible role of pAccessible
    virtual short SAL_CALL getRole();
    //get the accessible parent's role
    virtual short SAL_CALL getParentRole();
public:
    void removeMeFromBroadcaster();
};

#endif
