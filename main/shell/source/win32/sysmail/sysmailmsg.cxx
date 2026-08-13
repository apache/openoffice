/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_shell.hxx"

#include "sysmailmsg.hxx"

using com::sun::star::lang::IllegalArgumentException;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Sequence;
using rtl::OUString;
using osl::MutexGuard;

namespace shell
{

WinSysMailMsg::WinSysMailMsg( )
    : WinSysMailMsg_Base()
{
}

void SAL_CALL WinSysMailMsg::setBody( const OUString& aBody )
{
    MutexGuard aGuard( m_aMutex );
    m_aBody = aBody;
}

OUString SAL_CALL WinSysMailMsg::getBody(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aBody;
}

void SAL_CALL WinSysMailMsg::setRecipient( const OUString& aRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_aRecipient = aRecipient;
}

OUString SAL_CALL WinSysMailMsg::getRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aRecipient;
}

void SAL_CALL WinSysMailMsg::setCcRecipient( const Sequence< OUString >& aCcRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_CcRecipients = aCcRecipient;
}

Sequence< OUString > SAL_CALL WinSysMailMsg::getCcRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_CcRecipients;
}

void SAL_CALL WinSysMailMsg::setBccRecipient( const Sequence< OUString >& aBccRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_BccRecipients = aBccRecipient;
}

Sequence< OUString > SAL_CALL WinSysMailMsg::getBccRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_BccRecipients;
}

void SAL_CALL WinSysMailMsg::setOriginator( const OUString& aOriginator )
{
    MutexGuard aGuard( m_aMutex );
    m_aOriginator = aOriginator;
}

OUString SAL_CALL WinSysMailMsg::getOriginator(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aOriginator;
}

void SAL_CALL WinSysMailMsg::setSubject( const OUString& aSubject )
{
    MutexGuard aGuard( m_aMutex );
    m_aSubject = aSubject;
}

OUString SAL_CALL WinSysMailMsg::getSubject(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aSubject;
}

void SAL_CALL WinSysMailMsg::setAttachement( const Sequence< OUString >& aAttachement )
{
    MutexGuard aGuard( m_aMutex );
    m_Attachements = aAttachement;
}

Sequence< OUString > SAL_CALL WinSysMailMsg::getAttachement(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_Attachements;
}

}
