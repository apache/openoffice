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

#include "AccParagraphEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccParagraphEventListener::AccParagraphEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccContainerEventListener(pAcc, Agent)
{}
AccParagraphEventListener::~AccParagraphEventListener()
{
}

/**
 *	Uno's event notifier when event is captured
 *	@param AccessibleEventObject	the event object which contains information about event
 */
void  AccParagraphEventListener::notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    switch (aEvent.EventId)
    {
    case AccessibleEventId::CARET_CHANGED:
        handleCaretChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;
    case AccessibleEventId::VISIBLE_DATA_CHANGED:
        handleVisibleDataChangedEvent();
        break;
    case AccessibleEventId::BOUNDRECT_CHANGED:
        handleBoundrectChangedEvent();
        break;
        //Added for paragraph selected state.
    case AccessibleEventId::STATE_CHANGED:
        {
            short State;
            if( (aEvent.NewValue >>= State) && (State == AccessibleStateType::SELECTED) )
            {
                pAgent->IncreaseState( pAccessible, State);
                break;
            }
            else if( (aEvent.OldValue >>= State) && (State == AccessibleStateType::SELECTED) )
            {
                pAgent->DecreaseState( pAccessible, State);
                break;
            }

            AccContainerEventListener::notifyEvent(aEvent);
            break;
        }

    case AccessibleEventId::TEXT_SELECTION_CHANGED:
        handleTextSelectionChangedEvent();
        break;

    default:
        AccContainerEventListener::notifyEvent(aEvent);
        break;
    }
}

/**
 *	handle the CARET_CHANGED event
 *	@param oldValue		in UNO, this parameter is always NULL
 *	@param newValue		in UNO, this parameter is always NULL
 */
void AccParagraphEventListener::handleCaretChangedEvent(Any oldValue, Any newValue)
{
    pAgent->UpdateLocation(pAccessible);
    pAgent->NotifyAccEvent(UM_EVENT_OBJECT_CARETCHANGE, pAccessible);
}

/**
 *	handle the VISIBLE_DATA_CHANGED event
 */
void AccParagraphEventListener::handleVisibleDataChangedEvent()
{
    AccEventListener::handleVisibleDataChangedEvent();
}

/**
 *	handle the BOUNDRECT_CHANGED event
 */
void AccParagraphEventListener::handleBoundrectChangedEvent()
{
    AccEventListener::handleBoundrectChangedEvent();
}

/**
 *	set the new state and fire the MSAA event
 *	@param state	new state id
 *	@param enable	true if state is set, false if state is unset
 */
void AccParagraphEventListener::setComponentState(short state, bool enable )
{
    // only the following state can be fired state event.
    switch (state)
    {
    case AccessibleStateType::EDITABLE:
        // no msaa state
        break;
    case AccessibleStateType::MULTI_LINE:
        // no msaa state mapping
        break;
    case AccessibleStateType::SINGLE_LINE:
        // no msaa state mapping
        break;
    default:
        AccContainerEventListener::setComponentState(state, enable);
        break;
    }
}

void AccParagraphEventListener::handleTextSelectionChangedEvent()
{
    pAgent->NotifyAccEvent(UM_EVENT_TEXT_SELECTION_CHANGED, pAccessible);
}

