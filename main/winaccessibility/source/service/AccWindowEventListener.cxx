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

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>

#include "AccWindowEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccWindowEventListener::AccWindowEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccEventListener(pAcc, Agent)
{}
AccWindowEventListener::~AccWindowEventListener()
{
}

/**
 *	Uno's event notifier when event is captured
 *	@param AccessibleEventObject: the event object which contains information about event
 */
void  AccWindowEventListener::notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException)
{
    switch (aEvent.EventId)
    {
    case AccessibleEventId::CHILD:
        handleChildChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;
    case AccessibleEventId::VISIBLE_DATA_CHANGED:
        handleVisibleDataChangedEvent();
        break;
    case AccessibleEventId::BOUNDRECT_CHANGED:
        handleBoundrectChangedEvent();
        break;
    default:
        AccEventListener::notifyEvent(aEvent);
        break;
    }
}

/**
 *	handle the VISIBLE_DATA_CHANGED event
 */
void AccWindowEventListener::handleVisibleDataChangedEvent()
{
    AccEventListener::handleVisibleDataChangedEvent();
}

/**
 *	handle the BOUNDRECT_CHANGED event
 */
void AccWindowEventListener::handleBoundrectChangedEvent()
{
    AccEventListener::handleBoundrectChangedEvent();
}

/**
 *	handle the CHILD event
 *	@param	oldValue	the child to be deleted
 *	@param	newValue	the child to be added
 */
void AccWindowEventListener::handleChildChangedEvent(Any oldValue, Any newValue)
{
    Reference< XAccessible > xChild;
    if( newValue >>= xChild)
    {
        //create a new child
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            //add this child
            pAgent->InsertAccObj( pAcc,pAccessible);
            //add all oldValue's existing children
            pAgent->InsertChildrenAccObj(pAcc);
            pAgent->NotifyAccEvent(UM_EVENT_CHILD_ADDED, pAcc);
        }
        else
        {}
    }
    else if (oldValue >>= xChild)
    {
        //delete a existing child
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            pAgent->NotifyAccEvent(UM_EVENT_CHILD_REMOVED, pAcc);
            pAgent->DeleteChildrenAccObj( pAcc );
            //delete this child
            pAgent->DeleteAccObj( pAcc );
        }
        else
        {}
    }
}

/**
 *	set the new state and fire the MSAA event
 *	@param state	new state id
 *	@param enable	true if state is set, false if state is unset
 */
void AccWindowEventListener::setComponentState(short state, bool enable )
{
    // only the following state can be fired state event.
    switch (state)
    {
    case AccessibleStateType::ICONIFIED:
        // no msaa state
        break;
    case AccessibleStateType::VISIBLE:
        // UNO !VISIBLE == MSAA INVISIBLE
        if( enable )
            pAgent->IncreaseState( pAccessible, AccessibleStateType::VISIBLE );
        else
            pAgent->DecreaseState( pAccessible, AccessibleStateType::VISIBLE );
        break;
    case AccessibleStateType::SHOWING:
        // UNO !SHOWING == MSAA OFFSCREEN
        if( enable )
        {
            pAgent->IncreaseState( pAccessible, AccessibleStateType::SHOWING );
        }
        else
            pAgent->DecreaseState( pAccessible, AccessibleStateType::SHOWING );
        break;
    default:
        break;
    }
}
