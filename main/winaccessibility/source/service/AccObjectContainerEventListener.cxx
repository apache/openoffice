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

// AccObjectContainerEventListener.cpp: implementation of the AccContainerEventListener class.
//
//////////////////////////////////////////////////////////////////////

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>

#include "AccObjectContainerEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccObjectContainerEventListener::AccObjectContainerEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccContainerEventListener(pAcc, Agent)
{}
AccObjectContainerEventListener::~AccObjectContainerEventListener()
{
}

/**
 *	handle the STATE_CHANGED event
 */
void AccObjectContainerEventListener::handleStateChangedEvent(Any oldValue, Any newValue)
{
    //set the accessible name before process for there is no NAME_CHANGED event when change
    //the text in drawing objects.
    short newV;
    if( newValue >>= newV)
    {
        if (newV == AccessibleStateType::FOCUSED)
        {
            pAgent->UpdateAccName(pAccessible);

            pAgent->UpdateDescription(pAccessible);
        }
    }
    AccContainerEventListener::handleStateChangedEvent(oldValue, newValue);
}
/**
 *	handle the VISIBLE_DATA_CHANGED event
 *  For SHAPES, the visiable_data_changed event should be mapped to LOCATION_CHANGED event
  */
void AccObjectContainerEventListener::handleVisibleDataChangedEvent()
{
    AccContainerEventListener::handleBoundrectChangedEvent();
}
