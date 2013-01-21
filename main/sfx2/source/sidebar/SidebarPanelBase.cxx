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
#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/UIElementType.hpp>

using namespace css;
using namespace cssu;


namespace sfx2 { namespace sidebar {

SidebarPanelBase::SidebarPanelBase (
    const ::rtl::OUString& rsResourceURL,
    Window* pParentWindow,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    const ResId& rResId)
    : SidebarPanelBaseInterfaceBase(m_aMutex),
      msResourceURL(rsResourceURL),
      Control(pParentWindow, rResId),
      mxFrame(rxFrame),
      meFontUnderline(UNDERLINE_SINGLE)
{
    // Let the Pane draw the background.
    SetBackground(Wallpaper());
    
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
{
    HandleContextChange (
        EnumContext(
            EnumContext::GetApplicationEnum(rEvent.ApplicationName),
            EnumContext::GetContextEnum(rEvent.ContextName)));
}




void SAL_CALL SidebarPanelBase::disposing (
    const css::lang::EventObject& rEvent)
    throw (cssu::RuntimeException)
{
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
    // Not yet implemented.
    return NULL;
}




Reference<awt::XWindow> SAL_CALL SidebarPanelBase::getWindow (void)
    throw(cssu::RuntimeException)
{
    return Reference<awt::XWindow>(
        Control::GetComponentInterface(),
        UNO_QUERY);    
}




FontUnderline SidebarPanelBase::GetDefaultUnderline (void) const
{
    return meFontUnderline;
}




Image SidebarPanelBase::GetIcon (const ::rtl::OUString& rsURL)
{
    return GetImage(mxFrame, rsURL, sal_False, Theme::IsHighContrastMode());
}




void SidebarPanelBase::SetDefaultUnderline (const FontUnderline eFontUnderline)
{
    meFontUnderline = eFontUnderline;
}



} } // end of namespace sfx2::sidebar
