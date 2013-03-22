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

#include "sidebar/PanelFactory.hxx"

#include "text/TextPropertyPanel.hxx"
#include "area/AreaPropertyPanel.hxx"
#include "graphic/GraphicPropertyPanel.hxx"
#include "line/LinePropertyPanel.hxx"
#include "possize/PosSizePropertyPanel.hxx"
#include "GalleryControl.hxx"
#include "debug/ColorPanel.hxx"
#include "debug/ContextPanel.hxx"
#include "debug/NotYetImplementedPanel.hxx"
#include "EmptyPanel.hxx"
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <sfx2/sfxbasecontroller.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <vcl/window.hxx>
#include <rtl/ref.hxx>
#include <comphelper/namedvaluecollection.hxx>


#include <boost/bind.hpp>


using namespace css;
using namespace cssu;
using ::rtl::OUString;


namespace svx { namespace sidebar {

#define A2S(s) ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))
#define IMPLEMENTATION_NAME "org.apache.openoffice.comp.svx.sidebar.PanelFactory"
#define SERVICE_NAME "com.sun.star.ui.UIElementFactory"


::rtl::OUString SAL_CALL PanelFactory::getImplementationName (void)
{
    return A2S(IMPLEMENTATION_NAME);
}




cssu::Reference<cssu::XInterface> SAL_CALL PanelFactory::createInstance (
    const uno::Reference<lang::XMultiServiceFactory>& rxFactory)
{
    (void)rxFactory;
    
    ::rtl::Reference<PanelFactory> pPanelFactory (new PanelFactory());
    cssu::Reference<cssu::XInterface> xService (static_cast<XWeak*>(pPanelFactory.get()), cssu::UNO_QUERY);
    return xService;
}




cssu::Sequence<OUString> SAL_CALL PanelFactory::getSupportedServiceNames (void)
{
    cssu::Sequence<OUString> aServiceNames (1);
    aServiceNames[0] = A2S(SERVICE_NAME);
    return aServiceNames;

}




PanelFactory::PanelFactory (void)
    : PanelFactoryInterfaceBase(m_aMutex)
{
}




PanelFactory::~PanelFactory (void)
{
}




Reference<ui::XUIElement> SAL_CALL PanelFactory::createUIElement (
    const ::rtl::OUString& rsResourceURL,
    const ::cssu::Sequence<css::beans::PropertyValue>& rArguments)
    throw(
        container::NoSuchElementException,
        lang::IllegalArgumentException,
        RuntimeException)
{
    Reference<ui::XUIElement> xElement;

    const ::comphelper::NamedValueCollection aArguments (rArguments);
    Reference<frame::XFrame> xFrame (aArguments.getOrDefault("Frame", Reference<frame::XFrame>()));
    Reference<awt::XWindow> xParentWindow (aArguments.getOrDefault("ParentWindow", Reference<awt::XWindow>()));
    const sal_uInt64 nBindingsValue (aArguments.getOrDefault("SfxBindings", sal_uInt64(0)));
    SfxBindings* pBindings = reinterpret_cast<SfxBindings*>(nBindingsValue);

    ::Window* pParentWindow = VCLUnoHelper::GetWindow(xParentWindow);
    if ( ! xParentWindow.is() || pParentWindow==NULL)
        throw RuntimeException(
            A2S("PanelFactory::createUIElement called without ParentWindow"),
            NULL);
    if ( ! xFrame.is())
        throw RuntimeException(
            A2S("PanelFactory::createUIElement called without Frame"),
            NULL);
    if (pBindings == NULL)
        throw RuntimeException(
            A2S("PanelFactory::createUIElement called without SfxBindings"),
            NULL);

#define DoesResourceEndWith(s) rsResourceURL.endsWithAsciiL(s,strlen(s))
    if (DoesResourceEndWith("/TextPropertyPanel"))
    {
        TextPropertyPanel* pPanel = TextPropertyPanel::Create(pParentWindow, xFrame, pBindings);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(-1,-1,-1));
    }
    else if (DoesResourceEndWith("/AreaPropertyPanel"))
    {
        AreaPropertyPanel* pPanel = AreaPropertyPanel::Create(pParentWindow, xFrame, pBindings);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(-1,-1,-1));
    }
    else if (DoesResourceEndWith("/GraphicPropertyPanel"))
    {
        GraphicPropertyPanel* pPanel = GraphicPropertyPanel::Create(pParentWindow, xFrame, pBindings);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(-1,-1,-1));
    }
    else if (DoesResourceEndWith("/LinePropertyPanel"))
    {
        LinePropertyPanel* pPanel = LinePropertyPanel::Create(pParentWindow, xFrame, pBindings);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(-1,-1,-1));
    }
    else if (DoesResourceEndWith("/PosSizePropertyPanel"))
    {
        PosSizePropertyPanel* pPanel = PosSizePropertyPanel::Create(pParentWindow, xFrame, pBindings);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(-1,-1,-1));
    }
    else if (DoesResourceEndWith("/GalleryPanel"))
    {
        GalleryControl* pGalleryControl = new GalleryControl(
            pBindings,
            pParentWindow);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pGalleryControl,
            ui::LayoutSize(300,-1,400));
    }
    else if (DoesResourceEndWith("/Debug_ColorPanel"))
    {
        ColorPanel* pPanel = new ColorPanel(pParentWindow);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(300,-1,400));
    }
    else if (DoesResourceEndWith("/Debug_ContextPanel"))
    {
        ContextPanel* pPanel = new ContextPanel(pParentWindow);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(45,45,45));
    }
    else if (DoesResourceEndWith("/Debug_NotYetImplementedPanel"))
    {
        NotYetImplementedPanel* pPanel = new NotYetImplementedPanel(pParentWindow);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(20,25,25));
    }
    else if (DoesResourceEndWith("/EmptyPanel"))
    {
        EmptyPanel* pPanel = new EmptyPanel(pParentWindow);
        xElement = sfx2::sidebar::SidebarPanelBase::Create(
            rsResourceURL,
            xFrame,
            pPanel,
            ui::LayoutSize(20,-1, 50));
    }
#undef DoesResourceEndWith

    return xElement;
}

} } // end of namespace svx::sidebar

// eof
