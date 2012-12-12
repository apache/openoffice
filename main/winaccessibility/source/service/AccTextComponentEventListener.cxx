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

#include "AccTextComponentEventListener.hxx"
#include "AccObjectManagerAgent.hxx"
#include "unomsaaevent.hxx"

using namespace com::sun::star::uno;
using namespace com::sun::star::accessibility;

AccTextComponentEventListener::AccTextComponentEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent)
        :AccComponentEventListener(pAcc, Agent)
{}
AccTextComponentEventListener::~AccTextComponentEventListener()
{
}

/**
 *	set the new state and fire the MSAA event
 *	@param state	new state id
 *	@param enable	true if state is set, false if state is unset
 */
void AccTextComponentEventListener::setComponentState(short state, bool enable )
{
    // only the following state can be fired state event.
    switch (state)
    {
    case AccessibleStateType::EDITABLE:
        // no msaa state mapping
        break;
    case AccessibleStateType::MULTI_LINE:
        // no msaa state mapping
        break;
    case AccessibleStateType::SINGLE_LINE:
        // no msaa state mapping
        break;
    default:
        AccComponentEventListener::setComponentState(state, enable);
        break;
    }
}
