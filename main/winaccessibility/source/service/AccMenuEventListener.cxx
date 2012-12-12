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

#include "AccMenuEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccMenuEventListener::AccMenuEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccComponentEventListener(pAcc, Agent)
{}
AccMenuEventListener::~AccMenuEventListener()
{
}

/**
 *	Uno's event notifier when event is captured
 *	@param AccessibleEventObject	the event object which contains information about event
 */
void  AccMenuEventListener::notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    switch (aEvent.EventId)
    {
    case AccessibleEventId::CHILD:
        handleChildChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;
    case AccessibleEventId::SELECTION_CHANGED:
        //don't need to process anything,just same as word behavior
        //handleSelectionChangedEvent();
        break;
    default:
        AccComponentEventListener::notifyEvent(aEvent);
        break;
    }
}

/**
 *	handle the CHILD event
 *	@param	oldValue	the child to be deleted
 *	@param	newValue	the child to be added
 */
void AccMenuEventListener::handleChildChangedEvent(Any oldValue, Any newValue)
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
        //delete an existing child
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            pAgent->NotifyAccEvent(UM_EVENT_CHILD_REMOVED, pAcc);
            //delete all oldValue's existing children
            pAgent->DeleteChildrenAccObj( pAcc );
            //delete this child
            pAgent->DeleteAccObj( pAcc );
        }
        else
        {}
    }

}

/**
 *	handle the SELECTION_CHANGED event
 */
void AccMenuEventListener::handleSelectionChangedEvent()
{
    pAgent->NotifyAccEvent(UM_EVENT_SELECTION_CHANGED, pAccessible);
}

/**
 *	handle the Menu_popup event
 */
void AccMenuEventListener::fireStatePropertyChange(short state, bool set)
{
    if( set )
    {
        // new value
        switch(state)
        {
            //for sub menu is popup, there is a menu selected event.
        case AccessibleStateType::SELECTED:
            pAgent->IncreaseState( pAccessible, state);

            pAgent->UpdateChildState(pAccessible);
            break;
        default:
            AccComponentEventListener::fireStatePropertyChange(state, set
                                                                  );
            break;
        }
    }
    else
    {
        switch(state)
        {
            //for sub menu is popup, there is a menu selected event.
        case AccessibleStateType::SELECTED:
            pAgent->DecreaseState( pAccessible, state );

            break;
        default:
            AccComponentEventListener::fireStatePropertyChange(state, set
                                                                  );
            break;
        }
    }
}
