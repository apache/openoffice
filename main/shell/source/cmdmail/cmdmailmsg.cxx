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

#include "cmdmailmsg.hxx"

using com::sun::star::lang::IllegalArgumentException;
using rtl::OUString;
using osl::MutexGuard;

using namespace cppu;
using namespace com::sun::star::uno;

namespace shell
{

CmdMailMsg::CmdMailMsg()
{
}

void SAL_CALL CmdMailMsg::setBody( const OUString& aBody )
{
    MutexGuard aGuard( m_aMutex );
    m_aBody = aBody;
}

OUString SAL_CALL CmdMailMsg::getBody(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aBody;
}

void SAL_CALL CmdMailMsg::setRecipient( const OUString& aRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_aRecipient = aRecipient;
}

OUString SAL_CALL CmdMailMsg::getRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aRecipient;
}

void SAL_CALL CmdMailMsg::setCcRecipient( const Sequence< OUString >& aCcRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_CcRecipients = aCcRecipient;
}

Sequence< OUString > SAL_CALL CmdMailMsg::getCcRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_CcRecipients;
}

void SAL_CALL CmdMailMsg::setBccRecipient( const Sequence< OUString >& aBccRecipient )
{
    MutexGuard aGuard( m_aMutex );
    m_BccRecipients = aBccRecipient;
}

Sequence< OUString > SAL_CALL CmdMailMsg::getBccRecipient(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_BccRecipients;
}

void SAL_CALL CmdMailMsg::setOriginator( const OUString& aOriginator )
{
    MutexGuard aGuard( m_aMutex );
    m_aOriginator = aOriginator;
}

OUString SAL_CALL CmdMailMsg::getOriginator(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aOriginator;
}

void SAL_CALL CmdMailMsg::setSubject( const OUString& aSubject )
{
    MutexGuard aGuard( m_aMutex );
    m_aSubject = aSubject;
}

OUString SAL_CALL CmdMailMsg::getSubject(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_aSubject;
}

void SAL_CALL CmdMailMsg::setAttachement( const Sequence< OUString >& aAttachment )
{
    MutexGuard aGuard( m_aMutex );
    m_Attachments = aAttachment;
}

Sequence< OUString > SAL_CALL CmdMailMsg::getAttachement(  )
{
    MutexGuard aGuard( m_aMutex );
    return m_Attachments;
}

}
