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

#ifndef SFX_SIDEBAR_CONTROLLER_HXX
#define SFX_SIDEBAR_CONTROLLER_HXX

#include "ContentPanelManager.hxx"

#include <com/sun/star/ui/XContextChangeEventListener.hpp>
#include <boost/noncopyable.hpp>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace
{
    typedef ::cppu::WeakComponentImplHelper1 <
        css::ui::XContextChangeEventListener
        > SidebarControllerInterfaceBase;
}


namespace sfx2 {

class TabBar;
class DeckDescriptor;
class ContentPanelDescriptor;

class SidebarController
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public SidebarControllerInterfaceBase
{
public:
    SidebarController(
        Window* pParentWindow,
        const cssu::Reference<css::frame::XFrame>& rxFrame);
    virtual ~SidebarController (void);

    virtual void SAL_CALL notifyContextChangeEvent (const css::ui::ContextChangeEventObject& rEvent)
        throw(cssu::RuntimeException);

    virtual void SAL_CALL disposing (const css::lang::EventObject& rEventObject)
        throw(cssu::RuntimeException);

    void NotifyResize (void);

private:
    class Configuration;
    ::boost::scoped_ptr<Configuration> mpCurrentConfiguration;
    Window* mpParentWindow;
    TabBar* mpTabBar;
    cssu::Reference<css::frame::XFrame> mxFrame;

    DECL_LINK(WindowEventHandler, VclWindowEvent*);
    void UpdateConfiguration (
        const Context& rContext);
    void ProcessMatchingDeck (
        const DeckDescriptor& rDeck,
        Configuration& rConfiguration);
    void ProcessMatchingPanel (
        const ContentPanelDescriptor& rPanel,
        Configuration& rConfiguration);
    void MakeConfigurationCurrent (Configuration& rConfiguration);

    virtual void SAL_CALL disposing (void);
};


} // end of namespace sfx2


#endif
