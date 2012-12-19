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

#include "precompiled_framework.hxx"

#include "services/ContextChangeEventMultiplexer.hxx"
#include "services.h"

using ::rtl::OUString;

#define A2S(s) ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

namespace framework {

#define IMPLEMENTATION_NAME "org.apache.openoffice.comp.framework.ContextChangeEventMultiplexer"
#define SERVICE_NAME "com.sun.star.ui.ContextChangeEventMultiplexer"
#define SINGLETON_NAME "org.apache.openoffice.comp.framework.ContextChangeEventMultiplexerSigleton"


ContextChangeEventMultiplexer::ContextChangeEventMultiplexer (
    const cssu::Reference<css::uno::XComponentContext>& rxContext)
    : ContextChangeEventMultiplexerInterfaceBase(m_aMutex),
      maListeners()
{
    (void)rxContext;
}




ContextChangeEventMultiplexer::~ContextChangeEventMultiplexer (void)
{
    maListeners.clear();
}




// XContextChangeEventMultiplexer

void SAL_CALL ContextChangeEventMultiplexer::addContextChangeEventListener (
    const cssu::Reference<css::ui::XContextChangeEventListener>& rxListener,
    const cssu::Reference<cssu::XInterface>& rxEventFocus)
    throw(cssu::RuntimeException,cssl::IllegalArgumentException)
{
    if ( ! rxListener.is())
        throw css::lang::IllegalArgumentException(
            A2S("can not add an empty reference"),
            static_cast<XWeak*>(this),
            0);

    ListenerMap::iterator iListenerContainer (maListeners.find(rxEventFocus));
    if (iListenerContainer == maListeners.end())
    {
        // Create a new listener container for the event focus.
        iListenerContainer = maListeners.insert(
            ListenerMap::value_type(
                rxEventFocus,
                ListenerContainer())).first;
    }
    if (iListenerContainer != maListeners.end())
    {
        ListenerContainer& rContainer (iListenerContainer->second);
        if (::std::find(rContainer.begin(), rContainer.end(), rxListener) == rContainer.end())
            rContainer.push_back(rxListener);
        else
        {
            // The listener was added for the same event focus
            // previously.  That is an error.
            throw cssl::IllegalArgumentException(A2S("listener added twice"), static_cast<XWeak*>(this), 0);
        }
    }
}




void SAL_CALL ContextChangeEventMultiplexer::removeContextChangeEventListener (
    const cssu::Reference<css::ui::XContextChangeEventListener>& rxListener,
    const cssu::Reference<cssu::XInterface>& rxEventFocus)
    throw(cssu::RuntimeException,cssl::IllegalArgumentException)
{
    if ( ! rxListener.is())
        throw cssl::IllegalArgumentException(A2S("can not remove an empty reference"), static_cast<XWeak*>(this), 0);

    ListenerMap::iterator iListenerContainer (maListeners.find(rxEventFocus));
    if (iListenerContainer != maListeners.end())
    {
        ListenerContainer& rContainer (iListenerContainer->second);
        const ListenerContainer::iterator iListener (::std::find(rContainer.begin(), rContainer.end(), rxListener));
        if (iListener != rContainer.end())
            rContainer.erase(iListener);
    }
 
}




void SAL_CALL ContextChangeEventMultiplexer::removeAllContextChangeEventListeners (
    const cssu::Reference<css::ui::XContextChangeEventListener>& rxListener)
    throw(cssu::RuntimeException,cssl::IllegalArgumentException)
{
    if ( ! rxListener.is())
        throw cssl::IllegalArgumentException(A2S("can not remove an empty reference"), static_cast<XWeak*>(this), 0);

    for (ListenerMap::iterator
             iContainer(maListeners.begin()),
             iEnd(maListeners.end());
         iContainer!=iEnd;
         ++iContainer)
    {
        const ListenerContainer::iterator iListener (::std::find(iContainer->second.begin(), iContainer->second.end(), rxListener));
        if (iListener != iContainer->second.end())
            iContainer->second.erase(iListener);
    }
}





void SAL_CALL ContextChangeEventMultiplexer::broadcastContextChangeEvent (
    const css::ui::ContextChangeEventObject& rEventObject,
    const cssu::Reference<cssu::XInterface>& rxEventFocus)
    throw(cssu::RuntimeException)
{
    BroadcastEventToSingleContainer(rEventObject, rxEventFocus);
    if (rxEventFocus.is())
        BroadcastEventToSingleContainer(rEventObject, NULL);
}




void ContextChangeEventMultiplexer::BroadcastEventToSingleContainer (
    const css::ui::ContextChangeEventObject& rEventObject,
    const cssu::Reference<cssu::XInterface>& rxEventFocus)
{
    ListenerMap::iterator iListenerContainer (maListeners.find(rxEventFocus));
    if (iListenerContainer != maListeners.end())
    {
        // Create a copy of the listener container to avoid problems
        // when one of the called listeners calls add... or remove...
        ListenerContainer aContainer (iListenerContainer->second);
        for (ListenerContainer::const_iterator
                 iListener(aContainer.begin()),
                 iEnd(aContainer.end());
             iListener!=iEnd;
             ++iListener)
        {
            (*iListener)->notifyContextChangeEvent(rEventObject);
        }
    }
}




// XSingleComponentFactory

cssu::Reference<cssu::XInterface> SAL_CALL ContextChangeEventMultiplexer::createInstanceWithContext (
    const cssu::Reference<cssu::XComponentContext>& rxContext)
    throw (cssu::Exception, cssu::RuntimeException)
{
    (void)rxContext;
    return cssu::Reference<cssu::XInterface>();
}




cssu::Reference<cssu::XInterface > SAL_CALL ContextChangeEventMultiplexer::createInstanceWithArgumentsAndContext (
    const cssu::Sequence<cssu::Any>& rArguments,
    const cssu::Reference<cssu::XComponentContext>& rxContext)
    throw (cssu::Exception, cssu::RuntimeException)
{
    (void)rArguments;
    (void)rxContext;
    return cssu::Reference<cssu::XInterface>();
}




// XServiceInfo

::rtl::OUString SAL_CALL ContextChangeEventMultiplexer::getImplementationName (void)
    throw(cssu::RuntimeException)
{
    return impl_getStaticImplementationName();
}





sal_Bool SAL_CALL ContextChangeEventMultiplexer::supportsService (
    const ::rtl::OUString& rsServiceName)
    throw (cssu::RuntimeException)
{
    return ::comphelper::findValue(static_GetSupportedServiceNames(), rsServiceName, sal_True).getLength() != 0;
}




cssu::Sequence<OUString> SAL_CALL ContextChangeEventMultiplexer::getSupportedServiceNames (void)
    throw (cssu::RuntimeException)
{
    return static_GetSupportedServiceNames();
}




// Local and static methods.

OUString SAL_CALL ContextChangeEventMultiplexer::impl_getStaticImplementationName (void)
{
    return A2S(IMPLEMENTATION_NAME);
}




cssu::Sequence<OUString> SAL_CALL ContextChangeEventMultiplexer::static_GetSupportedServiceNames (void)
{
    cssu::Sequence<OUString> aServiceNames (2);
    aServiceNames[0] = A2S(SERVICE_NAME);
    aServiceNames[1] = A2S(SINGLETON_NAME);
    return aServiceNames;
}




cssu::Reference<cssu::XInterface> ContextChangeEventMultiplexer::impl_createFactory (
    const cssu::Reference<cssl::XMultiServiceFactory>& rxServiceManager)
{
    (void)rxServiceManager;
    return cppu::createSingleComponentFactory(
        ContextChangeEventMultiplexer::static_CreateInstance,
        ContextChangeEventMultiplexer::impl_getStaticImplementationName(),
        ContextChangeEventMultiplexer::static_GetSupportedServiceNames()
        );
}




cssu::Reference<cssu::XInterface> SAL_CALL ContextChangeEventMultiplexer::static_CreateInstance (
    const cssu::Reference<cssu::XComponentContext>& rxComponentContext)
    throw (cssu::Exception)
{
    ContextChangeEventMultiplexer* pObject = new ContextChangeEventMultiplexer(rxComponentContext);
    cssu::Reference<cssu::XInterface> xService (static_cast<XWeak*>(pObject), cssu::UNO_QUERY);
    return xService;
}

}  // end of namespace framework
