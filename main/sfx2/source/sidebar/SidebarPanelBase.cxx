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

#include "precompiled_sfx2.hxx"

#include "sfx2/sidebar/SidebarPanelBase.hxx"
#include "sfx2/sidebar/Theme.hxx"
#include "sfx2/imagemgr.hxx"
#include <vcl/ctrl.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/UIElementType.hpp>

using namespace css;
using namespace cssu;


namespace sfx2 { namespace sidebar {

Reference<ui::XUIElement> SidebarPanelBase::Create (
    const ::rtl::OUString& rsResourceURL,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    Control* pControl)
{
    Reference<ui::XUIElement> xUIElement (
        new SidebarPanelBase(
            rsResourceURL,
            rxFrame,
            pControl));
    return xUIElement;
}




SidebarPanelBase::SidebarPanelBase (
    const ::rtl::OUString& rsResourceURL,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    Control* pControl)
    : SidebarPanelBaseInterfaceBase(m_aMutex),
      msResourceURL(rsResourceURL),
      mxFrame(rxFrame),
      mpControl(pControl)
{
    cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
        css::ui::ContextChangeEventMultiplexer::get(
            ::comphelper::getProcessComponentContext()));
    if (xMultiplexer.is())
        xMultiplexer->addContextChangeEventListener(this, mxFrame->getController());
}




SidebarPanelBase::~SidebarPanelBase (void)
{
}




void SAL_CALL SidebarPanelBase::disposing (void)
    throw (cssu::RuntimeException)
{
    mpControl = NULL;
    
    if (mxFrame.is())
    {
        cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
            css::ui::ContextChangeEventMultiplexer::get(
                ::comphelper::getProcessComponentContext()));
        if (xMultiplexer.is())
            xMultiplexer->removeAllContextChangeEventListeners(this);
        mxFrame = NULL;
    }
}




// XContextChangeEventListener
void SAL_CALL SidebarPanelBase::notifyContextChangeEvent (
    const ui::ContextChangeEventObject& rEvent)
    throw (cssu::RuntimeException)
{
    if (mpControl != NULL)
    {
        const EnumContext aContext(
            EnumContext::GetApplicationEnum(rEvent.ApplicationName),
            EnumContext::GetContextEnum(rEvent.ContextName));

        ContextChangeReceiverInterface* pContextChangeReceiver
            = dynamic_cast<ContextChangeReceiverInterface*>(mpControl);
        if (pContextChangeReceiver != NULL)
            pContextChangeReceiver->HandleContextChange(aContext);
    }
}




void SAL_CALL SidebarPanelBase::disposing (
    const css::lang::EventObject& rEvent)
    throw (cssu::RuntimeException)
{
    (void)rEvent;
    
    mxFrame = NULL;
    mpControl = NULL;
}




cssu::Reference<css::frame::XFrame> SAL_CALL SidebarPanelBase::getFrame (void)
    throw(cssu::RuntimeException)
{
    return mxFrame;
}




::rtl::OUString SAL_CALL SidebarPanelBase::getResourceURL (void)
    throw(cssu::RuntimeException)
{
    return msResourceURL;
}




sal_Int16 SAL_CALL SidebarPanelBase::getType (void)
    throw(cssu::RuntimeException)
{
    return ui::UIElementType::TOOLPANEL;
}




Reference<XInterface> SAL_CALL SidebarPanelBase::getRealInterface (void)
    throw(cssu::RuntimeException)
{
    return Reference<XInterface>(static_cast<XWeak*>(this));
}




Reference<accessibility::XAccessible> SAL_CALL SidebarPanelBase::createAccessible (
    const Reference<accessibility::XAccessible>& rxParentAccessible)
    throw(cssu::RuntimeException)
{
    (void)rxParentAccessible;
    
    // Not yet implemented.
    return NULL;
}




Reference<awt::XWindow> SAL_CALL SidebarPanelBase::getWindow (void)
    throw(cssu::RuntimeException)
{
    if (mpControl != NULL)
        return Reference<awt::XWindow>(
            mpControl->GetComponentInterface(),
            UNO_QUERY);
    else
        return NULL;
}




sal_Int32 SAL_CALL SidebarPanelBase::getHeightForWidth (const sal_Int32 nWidth)
    throw(cssu::RuntimeException)
{
    if (mpControl != NULL)
        return mpControl->GetSizePixel().Height();
    else
        return -1;
}



} } // end of namespace sfx2::sidebar
