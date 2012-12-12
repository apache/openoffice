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
#include <com/sun/star/accessibility/AccessibleTableModelChange.hpp>

#include "AccTableEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccTableEventListener::AccTableEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccDescendantManagerEventListener(pAcc, Agent)
{}
AccTableEventListener::~AccTableEventListener()
{
}

/**
 *	Uno's event notifier when event is captured
 *	@param AccessibleEventObject	the event object which contains information about event
 */
void  AccTableEventListener::notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    switch (aEvent.EventId)
    {
    case AccessibleEventId::ACTIVE_DESCENDANT_CHANGED:
        handleActiveDescendantChangedEvent(aEvent.OldValue, aEvent.NewValue);
        break;

    case AccessibleEventId::TABLE_CAPTION_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_CAPTION_CHANGED, pAccessible);
            break;
        }
    case AccessibleEventId::TABLE_COLUMN_DESCRIPTION_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_COLUMN_DESCRIPTION_CHANGED, pAccessible);
            break;
        }
    case AccessibleEventId::TABLE_COLUMN_HEADER_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_COLUMN_HEADER_CHANGED, pAccessible);
            break;
        }
    case AccessibleEventId::TABLE_ROW_HEADER_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_ROW_HEADER_CHANGED, pAccessible);
            break;
        }
    case AccessibleEventId::TABLE_MODEL_CHANGED:
        {

            handleTableModelChangeEvent(aEvent.NewValue);
            break;
        }
    case AccessibleEventId::TABLE_SUMMARY_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_SUMMARY_CHANGED, pAccessible);
            break;
        }
    case AccessibleEventId::TABLE_ROW_DESCRIPTION_CHANGED:
        {

            pAgent->NotifyAccEvent(UM_EVENT_TABLE_ROW_DESCRIPTION_CHANGED, pAccessible);
            break;
        }
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
void AccTableEventListener::handleActiveDescendantChangedEvent(Any oldValue, Any newValue)
{
    Reference< XAccessible > xChild;
    if(newValue >>= xChild )
    {
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            pAgent->InsertAccObj(pAcc,pAccessible);
            pAgent->NotifyAccEvent(UM_EVENT_ACTIVE_DESCENDANT_CHANGED, pAcc);
        }
    }
    else if (oldValue >>= xChild)
    {
        //delete an existing child
        if(xChild.is())
        {
            XAccessible* pAcc = xChild.get();
            pAgent->DeleteAccObj( pAcc );
        }
    }

}
void AccTableEventListener::handleTableModelChangeEvent(Any newValue)
{
	AccessibleTableModelChange aModelChange;
	if (newValue >>= aModelChange)
	{
	    if( pAccessible )
	    {
	        //delete all oldValue's existing children
	        pAgent->DeleteChildrenAccObj( pAccessible );
	        //add all oldValue's existing children
	        pAgent->InsertChildrenAccObj( pAccessible );
	    }		
		pAgent->NotifyAccEvent(UM_EVENT_TABLE_MODEL_CHANGED, pAccessible);
	}
}
