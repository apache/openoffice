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



#include <event.hxx>

namespace DOM { namespace events
{

    CEvent::CEvent()
        : m_canceled(sal_False)
        , m_phase(PhaseType_CAPTURING_PHASE)
        , m_bubbles(sal_False)
        , m_cancelable(sal_True)
    {
    }

    CEvent::~CEvent()
    {
    }

    OUString SAL_CALL CEvent::getType()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_eventType;
    }

    Reference< XEventTarget > SAL_CALL
    CEvent::getTarget()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_target;
    }

    Reference< XEventTarget > SAL_CALL
    CEvent::getCurrentTarget()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_currentTarget;
    }

    PhaseType SAL_CALL CEvent::getEventPhase()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_phase;
    }

    sal_Bool SAL_CALL CEvent::getBubbles()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_bubbles;
    }

    sal_Bool SAL_CALL CEvent::getCancelable()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_cancelable;
    }

    com::sun::star::util::Time SAL_CALL
    CEvent::getTimeStamp()
    {
        ::osl::MutexGuard const g(m_Mutex);
        return m_time;
    }

    void SAL_CALL CEvent::stopPropagation()
    {
        ::osl::MutexGuard const g(m_Mutex);
        if (m_cancelable) { m_canceled = sal_True; }
    }

    void SAL_CALL CEvent::preventDefault()
    {
    }

    void SAL_CALL
    CEvent::initEvent(OUString const& eventTypeArg, sal_Bool canBubbleArg,
        sal_Bool cancelableArg)
    {
        ::osl::MutexGuard const g(m_Mutex);

        m_eventType = eventTypeArg;
        m_bubbles = canBubbleArg;
        m_cancelable = cancelableArg;
    }

}}
