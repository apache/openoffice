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

#include "SidebarController.hxx"
#include "Deck.hxx"
#include "Panel.hxx"
#include "TitleBar.hxx"
#include "SidebarLayouter.hxx"
#include "TabBar.hxx"
#include "sfxresid.hxx"
#include "sfx2/sfxsids.hrc"

#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/ContextChangeEventObject.hpp>

#include <boost/bind.hpp>
#include <comphelper/componentfactory.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/namedvaluecollection.hxx>


using namespace css;
using namespace cssu;


#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sfx2 {


class SidebarController::Configuration
{
public:
    Deck* mpDeck;
    ::std::vector<Panel*> maPanels;

    Configuration (void);
    void Disable (void);
    void Activate (void);
};




SidebarController::SidebarController (
    Window* pParentWindow,
    const cssu::Reference<css::frame::XFrame>& rxFrame)
    : SidebarControllerInterfaceBase(m_aMutex),
      mpCurrentConfiguration(),
      mpParentWindow(pParentWindow),
      mpTabBar(new TabBar(mpParentWindow)),
      mxFrame(rxFrame)
{
    if (pParentWindow == NULL)
    {
        OSL_ASSERT(pParentWindow!=NULL);
            return;
    }

    UpdateConfiguration(Context(A2S("default"), A2S("default")));

    // Listen for context change events.
    cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
        css::ui::ContextChangeEventMultiplexer::get(
            ::comphelper::getProcessComponentContext()));
    if (xMultiplexer.is())
        xMultiplexer->addContextChangeEventListener(
            static_cast<css::ui::XContextChangeEventListener*>(this),
            NULL);

    // Listen for window events.
    mpParentWindow->AddEventListener(LINK(this, SidebarController, WindowEventHandler));
}




SidebarController::~SidebarController (void)
{
}




void SAL_CALL SidebarController::disposing (void)
{
    cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
        css::ui::ContextChangeEventMultiplexer::get(
            ::comphelper::getProcessComponentContext()));
    if (xMultiplexer.is())
        xMultiplexer->removeAllContextChangeEventListeners(
            static_cast<css::ui::XContextChangeEventListener*>(this));

    if (mpParentWindow != NULL)
    {
        mpParentWindow->RemoveEventListener(LINK(this, SidebarController, WindowEventHandler));
        mpParentWindow = NULL;
    }
}




void SAL_CALL SidebarController::notifyContextChangeEvent (const css::ui::ContextChangeEventObject& rEvent)
    throw(cssu::RuntimeException)
{
    UpdateConfiguration(Context(rEvent.ApplicationName, rEvent.ContextName));
}




void SAL_CALL SidebarController::disposing (const css::lang::EventObject& rEventObject)
    throw(cssu::RuntimeException)
{
    if (mpCurrentConfiguration)
    {
        mpCurrentConfiguration->Disable();
        mpCurrentConfiguration.reset();
    }
    if (mpTabBar != NULL)
    {
        mpTabBar->Hide();
        delete mpTabBar;
        mpTabBar = NULL;
    }
}




void SidebarController::NotifyResize (void)
{
    if (mpCurrentConfiguration != NULL)
    {
        SidebarLayouter::LayoutSidebar(mpCurrentConfiguration->mpDeck, mpTabBar);
        SidebarLayouter::LayoutPanels(mpCurrentConfiguration->mpDeck, mpCurrentConfiguration->maPanels);
    }
}




void SidebarController::UpdateConfiguration (const Context& rContext)
{
    ::boost::scoped_ptr<Configuration> pConfiguration(new Configuration());
    
    SharedContentPanelManager pContentManager (ContentPanelManager::Instance());
    pContentManager->CallForBestMatchingDeck (
        ::boost::bind(&SidebarController::ProcessMatchingDeck, this, _1, ::boost::ref(*pConfiguration)),
        rContext);
    
    if (pConfiguration->mpDeck == NULL)
        return;
    
    pContentManager->CallForMatchingPanels (
        ::boost::bind(&SidebarController::ProcessMatchingPanel, this, _1, ::boost::ref(*pConfiguration)),
        rContext,
        pConfiguration->mpDeck->GetId());

    MakeConfigurationCurrent(*pConfiguration);
    mpCurrentConfiguration.swap(pConfiguration);
}




void SidebarController::ProcessMatchingDeck (
    const DeckDescriptor& rDeckDescriptor,
    Configuration& rConfiguration)
{
    if (rConfiguration.mpDeck != NULL)
    {
        // When more than one deck matches the context then choose the first.
        return;
    }

    rConfiguration.mpDeck = new Deck(rDeckDescriptor, mpParentWindow);
}




void SidebarController::ProcessMatchingPanel (
    const ContentPanelDescriptor& rPanelDescriptor,
    Configuration& rConfiguration)
{
    // Create the panel which is the parent window of the UIElement
    // that we are about to create.
    Panel* pPanel = new Panel(rPanelDescriptor, rConfiguration.mpDeck);

    Reference<ui::XUIElement> xElement;
    try
    {
        const ::comphelper::ComponentContext aComponentContext (::comphelper::getProcessServiceFactory());
        const Reference<ui::XUIElementFactory> xUIElementFactory (
            aComponentContext.createComponent("com.sun.star.ui.UIElementFactoryManager"),
            UNO_QUERY_THROW);
    
        ::comphelper::NamedValueCollection aCreationArguments;
        aCreationArguments.put("Frame", makeAny(mxFrame));
        aCreationArguments.put("ParentWindow", makeAny(pPanel->GetComponentInterface()));
        
        pPanel->SetUIElement(
            Reference<ui::XUIElement>(
                xUIElementFactory->createUIElement(
                    rPanelDescriptor.msImplementationURL,
                    aCreationArguments.getPropertyValues()),
                UNO_QUERY_THROW));
    }
    catch(Exception& rException)
    {
        OSL_TRACE("caught exception: %s",
            OUStringToOString(rException.Message, RTL_TEXTENCODING_ASCII_US).getStr());
        // For some reason we can not create the actual panel.
        // Probably because its factory was not properly registered.
        // TODO: provide feedback to developer to better pinpoint the
        // source of the error.
    }
    rConfiguration.maPanels.push_back(pPanel);
}




void SidebarController::MakeConfigurationCurrent (Configuration& rConfiguration)
{
    if (rConfiguration.mpDeck == NULL)
        return;

    // Deactivate the current deck and panels.
    if (mpCurrentConfiguration && mpCurrentConfiguration->mpDeck!=NULL)
        mpCurrentConfiguration->Disable();
    SidebarLayouter::LayoutSidebar(rConfiguration.mpDeck, mpTabBar);
    rConfiguration.Activate();
}




IMPL_LINK(SidebarController, WindowEventHandler, VclWindowEvent*, pEvent)
{
    if (pEvent != NULL)
    {
        ::Window* pWindow = pEvent->GetWindow();
        switch (pEvent->GetId())
        {
            case VCLEVENT_WINDOW_GETFOCUS:
            case VCLEVENT_WINDOW_LOSEFOCUS:
                break;
                
            case VCLEVENT_WINDOW_SHOW:
            case VCLEVENT_WINDOW_RESIZE:
                NotifyResize();
                break;

            case SFX_HINT_DYING:
                dispose();
                break;
                
            default:
                break;
        }
    }

    return sal_True;
}




//===== SidebarController::Configuration ======================================

SidebarController::Configuration::Configuration (void)
    : mpDeck(NULL),
      maPanels()
{
}




void SidebarController::Configuration::Disable (void)
{
    // Hide and destroy the panels.
    for (::std::vector<Panel*>::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        (*iPanel)->Hide();
        mpDeck->removeWindow(*iPanel, NULL);
        delete *iPanel;
    }
    maPanels.clear();

    // Hide and destroy the deck window.
    mpDeck->Hide();
    mpDeck->GetParent()->removeWindow(mpDeck, NULL);
    delete mpDeck;
    mpDeck = NULL;
}




void SidebarController::Configuration::Activate (void)
{
    SidebarLayouter::LayoutPanels(mpDeck, maPanels);

    // Show the deck window.
    mpDeck->GetTitleBar()->Show();
    mpDeck->Show();

    // Show the panels.
    for (::std::vector<Panel*>::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        (*iPanel)->GetTitleBar()->Show();
        (*iPanel)->Show();
    }
}


} // end of namespace sfx2
