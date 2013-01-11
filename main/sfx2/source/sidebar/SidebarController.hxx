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

#include "ResourceManager.hxx"

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


class DockingWindow;

namespace sfx2 { namespace sidebar {

class TabBar;
class TabBarConfiguration;
class DeckDescriptor;
class DeckConfiguration;
class ContentPanelDescriptor;

class SidebarController
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public SidebarControllerInterfaceBase
{
public:
    SidebarController(
        DockingWindow* pParentWindow,
        const cssu::Reference<css::frame::XFrame>& rxFrame);
    virtual ~SidebarController (void);

    virtual void SAL_CALL notifyContextChangeEvent (const css::ui::ContextChangeEventObject& rEvent)
        throw(cssu::RuntimeException);

    virtual void SAL_CALL disposing (const css::lang::EventObject& rEventObject)
        throw(cssu::RuntimeException);

    void NotifyResize (void);

    void SwitchToDeck (
        const DeckDescriptor& rDeckDescriptor);

private:
    ::boost::shared_ptr<DeckConfiguration> mpCurrentConfiguration;
    DockingWindow* mpParentWindow;
    TabBar* mpTabBar;
    cssu::Reference<css::frame::XFrame> mxFrame;
    Context maCurrentContext;

    DECL_LINK(WindowEventHandler, VclWindowEvent*);
    void UpdateConfigurations (const Context& rContext);
    cssu::Reference<css::ui::XUIElement> CreateUIElement (
        const cssu::Reference<css::awt::XWindowPeer>& rxWindow,
        const ::rtl::OUString& rsImplementationURL) const;
    void SwitchToDeck (
        const DeckDescriptor& rDeckDescriptor,
        const Context& rContext);
    void MakeConfigurationCurrent (const ::boost::shared_ptr<DeckConfiguration>& rpConfiguration);
    void ShowPopupMenu (const Rectangle& rButtonBox) const;
    ::boost::shared_ptr<PopupMenu> CreatePopupMenu (void) const;
    DECL_LINK(OnMenuItemSelected, Menu*);

    virtual void SAL_CALL disposing (void);
};


} } // end of namespace sfx2::sidebar

#endif
