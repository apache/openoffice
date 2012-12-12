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

#include "AccListEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccListEventListener::AccListEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccDescendantManagerEventListener(pAcc, Agent),
        shouldDeleteChild(true)
{
}

AccListEventListener::~AccListEventListener()
{
}

/**
 *	Uno's event notifier when event is captured
 *	@param AccessibleEventObject	the event object which contains information about event
 */
void  AccListEventListener::notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException)
{
    switch (aEvent.EventId)
    {
    case AccessibleEventId::ACTIVE_DESCENDANT_CHANGED:
        handleActiveDescendantChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;
    case AccessibleEventId::INVALIDATE_ALL_CHILDREN:
        // Since List items a transient a child events are mostly used
        // to attach/detach listeners, it is save to ignore it here
        //TODO: investigate again
        break;
    case AccessibleEventId::VALUE_CHANGED:
        handleValueChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;
    default:
        AccDescendantManagerEventListener::notifyEvent(aEvent);
        break;
    }
}

/**
 *	handle the ACTIVE_DESCENDANT_CHANGED event
 *  @param	oldValue	the child to lose active
 *  @param	newValue	the child to get active
 */
void AccListEventListener::handleActiveDescendantChangedEvent(Any oldValue, Any newValue)
{
    Reference< XAccessible > xChild;

    if(newValue >>= xChild )
    {
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();

            // Valueset has cache the child item xacc,Update state if no insert obj
            sal_Bool bHasCache = pAgent->InsertAccObj(pAcc,pAccessible);
            if (!bHasCache)
            {
                pAgent->UpdateState(pAcc);
            }

            pAgent->IncreaseState( pAcc, AccessibleStateType::FOCUSED);

            pAgent->NotifyAccEvent(UM_EVENT_ACTIVE_DESCENDANT_CHANGED, pAcc);
            pActiveDescendant= pAcc;
        }
    }
    if (oldValue >>= xChild)
    {
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            pAgent->DeleteAccObj( pAcc );
        }
    }
}

/**
 * handle the VALUE_CHANGED event
 *
 * @param	oldValue	the old value of the source of event
 * @param	newValue	the new value of the source of event
 */
void AccListEventListener::handleValueChangedEvent(Any oldValue, Any newValue)
{
    //to enable value changed event
    if(getParentRole() == AccessibleRole::COMBO_BOX)
    {
        XAccessible* pParentAcc = pAgent->GetParentXAccessible(pAccessible);
        pAgent->UpdateValue(pParentAcc);
        pAgent->NotifyAccEvent(UM_EVENT_OBJECT_VALUECHANGE, pParentAcc);
    }
}
