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



#ifndef EVENT_UIEVENT_HXX
#define EVENT_UIEVENT_HXX

#include <sal/types.h>

#include <com/sun/star/xml/dom/events/PhaseType.hpp>
#include <com/sun/star/xml/dom/events/XUIEvent.hpp>
#include <com/sun/star/xml/dom/views/XAbstractView.hpp>

#include <cppuhelper/implbase1.hxx>

#include "event.hxx"


using ::rtl::OUString;
using namespace com::sun::star::xml::dom::views;

namespace DOM { namespace events {

typedef ::cppu::ImplInheritanceHelper1< CEvent, XUIEvent > CUIEvent_Base;

class CUIEvent
    : public CUIEvent_Base
{
protected:
    sal_Int32 m_detail;
    Reference< XAbstractView > m_view;

public:
    explicit CUIEvent();

    virtual Reference< XAbstractView > SAL_CALL getView();
    virtual sal_Int32 SAL_CALL getDetail();
    virtual void SAL_CALL initUIEvent(const OUString& typeArg,
                     sal_Bool canBubbleArg,
                     sal_Bool cancelableArg,
                     const Reference< XAbstractView >& viewArg,
                     sal_Int32 detailArg);

    // delegate to CEvent, since we are inheriting from CEvent and XEvent
    virtual OUString SAL_CALL getType();
    virtual Reference< XEventTarget > SAL_CALL getTarget();
    virtual Reference< XEventTarget > SAL_CALL getCurrentTarget();
    virtual PhaseType SAL_CALL getEventPhase();
    virtual sal_Bool SAL_CALL getBubbles();
    virtual sal_Bool SAL_CALL getCancelable();
    virtual com::sun::star::util::Time SAL_CALL getTimeStamp();
    virtual void SAL_CALL stopPropagation();
    virtual void SAL_CALL preventDefault();
    virtual void SAL_CALL initEvent(
        const OUString& eventTypeArg,
        sal_Bool canBubbleArg,
        sal_Bool cancelableArg);
};
}}
#endif
