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

/**
 * AccAction.cpp : Implementation of CAccAction
 */
#include "stdafx.h"
#include "UAccCOM2.h"
#include "AccAction.h"

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;

/**
 * Returns the number of action.
 *
 * @param    nActions    the number of action.
 */
STDMETHODIMP CAccAction::nActions(/*[out,retval]*/long* nActions)
{
    
    return CAccActionBase::nActions(nActions);
}

/**
 * Performs specified action on the object.
 *
 * @param    actionIndex    the index of action.
 */
STDMETHODIMP CAccAction::doAction(/* [in] */ long actionIndex)
{
    
    return CAccActionBase::doAction(actionIndex);
}

/**
 * Gets description of specified action.
 *
 * @param    actionIndex    the index of action.
 * @param    description    the description string of the specified action.
 */
STDMETHODIMP CAccAction::get_description(long actionIndex,BSTR __RPC_FAR *description)
{
    
    return CAccActionBase::get_description(actionIndex, description);
}

STDMETHODIMP CAccAction::get_name( long actionIndex, BSTR __RPC_FAR *name)
{
    
    return CAccActionBase::get_name(actionIndex, name);
}

STDMETHODIMP CAccAction::get_localizedName( long actionIndex, BSTR __RPC_FAR *localizedName)
{
    
    return CAccActionBase::get_localizedName(actionIndex, localizedName);
}

/**
 * Returns key binding object (if any) associated with specified action 
 * key binding is string.
 * e.g. "alt+d" (like IAccessible::get_accKeyboardShortcut).
 *
 * @param    actionIndex    the index of action.
 * @param    nMaxBinding    the max number of key binding.
 * @param    keyBinding     the key binding array.
 * @param    nBinding       the actual number of key binding returned.
 */
STDMETHODIMP CAccAction::get_keyBinding(
    /* [in] */ long actionIndex,
    /* [in] */ long nMaxBinding,
    /* [length_is][length_is][size_is][size_is][out] */ BSTR __RPC_FAR *__RPC_FAR *keyBinding,
    /* [retval][out] */ long __RPC_FAR *nBinding)
{
    
    return CAccActionBase::get_keyBinding(actionIndex, nMaxBinding,	keyBinding, nBinding);
}

/**
 * Overide of IUNOXWrapper.
 *
 * @param    pXInterface    the pointer of UNO interface.
 */
STDMETHODIMP CAccAction::put_XInterface(long pXInterface)
{
    
    return CAccActionBase::put_XInterface(pXInterface);
}
/**
   * Put UNO interface.
   * @param pXSubInterface XAccessibleHyperlink interface.
   * @return Result.
*/
STDMETHODIMP CAccAction::put_XSubInterface(long pXSubInterface)
{
    

    pRXAct = (XAccessibleAction*)pXSubInterface;

    return S_OK;
}
