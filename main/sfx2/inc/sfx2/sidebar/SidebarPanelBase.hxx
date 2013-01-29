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

#ifndef SFX_SIDEBAR_PANEL_BASE_HXX
#define SFX_SIDEBAR_PANEL_BASE_HXX

#include "EnumContext.hxx"

#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/basemutex.hxx>

#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/ui/XContextChangeEventListener.hpp>
#include <com/sun/star/ui/XUIElement.hpp>
#include <com/sun/star/ui/XToolPanel.hpp>
#include <com/sun/star/ui/XVerticalStackLayoutElement.hpp>
#include <boost/noncopyable.hpp>


namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;


class Control;


namespace sfx2 { namespace sidebar {

namespace
{
    typedef ::cppu::WeakComponentImplHelper4 <
        css::ui::XContextChangeEventListener,
        css::ui::XUIElement,
        css::ui::XToolPanel,
        css::ui::XVerticalStackLayoutElement
        > SidebarPanelBaseInterfaceBase;
}

/** Base class for sidebar panels that provides some convenience
    functionality.
*/
class SFX2_DLLPUBLIC SidebarPanelBase
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public SidebarPanelBaseInterfaceBase
{
public:
    class ContextChangeReceiverInterface
    {
      public:
        virtual void HandleContextChange (
            const EnumContext aContext) = 0;
    };
    
    static cssu::Reference<css::ui::XUIElement> Create (
        const ::rtl::OUString& rsResourceURL,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        Control* pControl);

    // XContextChangeEventListener
    virtual void SAL_CALL notifyContextChangeEvent (
        const css::ui::ContextChangeEventObject& rEvent)
        throw (cssu::RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing (
        const css::lang::EventObject& rEvent)
        throw (cssu::RuntimeException);

    // XUIElement
    virtual cssu::Reference<css::frame::XFrame> SAL_CALL getFrame (void)
        throw(cssu::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getResourceURL (void)
        throw(cssu::RuntimeException);
    virtual sal_Int16 SAL_CALL getType (void)
        throw(cssu::RuntimeException);
    virtual cssu::Reference<cssu::XInterface> SAL_CALL getRealInterface (void)
        throw(cssu::RuntimeException);

    // XToolPanel
    virtual cssu::Reference<css::accessibility::XAccessible> SAL_CALL createAccessible (
        const cssu::Reference<css::accessibility::XAccessible>& rxParentAccessible)
        throw(cssu::RuntimeException);
    virtual cssu::Reference<css::awt::XWindow> SAL_CALL getWindow (void)
        throw(cssu::RuntimeException);

    // XVerticalStackLayoutElement
    virtual sal_Int32 SAL_CALL getHeightForWidth (const sal_Int32 nWidth)
        throw(cssu::RuntimeException);
    
protected:
    SidebarPanelBase (
        const ::rtl::OUString& rsResourceURL,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        Control* pControl);
    virtual ~SidebarPanelBase (void);

    virtual void SAL_CALL disposing (void)
        throw (cssu::RuntimeException);

private:
    const ::rtl::OUString msResourceURL;
    cssu::Reference<css::frame::XFrame> mxFrame;
    Control* mpControl;
};

} } // end of namespace sfx2::sidebar

#endif
