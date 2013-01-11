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
#include "DeckConfiguration.hxx"
#include "Panel.hxx"
#include "SidebarResource.hxx"
#include "TitleBar.hxx"
#include "TabBar.hxx"
#include "Theme.hxx"

#include "sfxresid.hxx"
#include "sfx2/sfxsids.hrc"
#include "sfxlocal.hrc"
#include <vcl/floatwin.hxx>
#include <vcl/dockwin.hxx>
#include <comphelper/componentfactory.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/namedvaluecollection.hxx>

#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/ContextChangeEventObject.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>


using namespace css;
using namespace cssu;


#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sfx2 { namespace sidebar {

namespace {
    enum MenuId
    {
        MID_UNLOCK_TASK_PANEL = 1,
        MID_LOCK_TASK_PANEL,
        MID_CUSTOMIZATION,
        MID_RESTORE_DEFAULT,
        MID_FIRST_PANEL,
        MID_FIRST_HIDE = 1000
    };
}


SidebarController::SidebarController (
    DockingWindow* pParentWindow,
    const cssu::Reference<css::frame::XFrame>& rxFrame)
    : SidebarControllerInterfaceBase(m_aMutex),
      mpCurrentConfiguration(),
      mpParentWindow(pParentWindow),
      mpTabBar(new TabBar(
              mpParentWindow,
              rxFrame,
              ::boost::bind(&SidebarController::SwitchToDeck, this, _1),
              ::boost::bind(&SidebarController::ShowPopupMenu, this, _1))),
      mxFrame(rxFrame)
{
    if (pParentWindow == NULL)
    {
        OSL_ASSERT(pParentWindow!=NULL);
            return;
    }

    UpdateConfigurations(Context(A2S("default"), A2S("default")));

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
    UpdateConfigurations(Context(rEvent.ApplicationName, rEvent.ContextName));
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
        if (mpCurrentConfiguration->mpDeck==NULL || mpTabBar==NULL)
        {
            OSL_ASSERT(mpCurrentConfiguration->mpDeck!=NULL && mpTabBar!=NULL);
        }
        else
        {
            Window* pParentWindow = mpCurrentConfiguration->mpDeck->GetParent();
            if (pParentWindow==NULL || pParentWindow!=mpTabBar->GetParent())
            {
                OSL_ASSERT(mpCurrentConfiguration->mpDeck->GetParent() != NULL);
                OSL_ASSERT(mpCurrentConfiguration->mpDeck->GetParent() == mpTabBar->GetParent());
            }
    
            const sal_Int32 nWidth (pParentWindow->GetSizePixel().Width());
            const sal_Int32 nHeight (pParentWindow->GetSizePixel().Height());
            mpCurrentConfiguration->mpDeck->SetPosSizePixel(0,0, nWidth-TabBar::GetDefaultWidth(), nHeight);
            mpCurrentConfiguration->mpDeck->Show();
            mpTabBar->SetPosSizePixel(nWidth-TabBar::GetDefaultWidth(),0,TabBar::GetDefaultWidth(),nHeight);
            mpTabBar->Show();

            mpCurrentConfiguration->mpDeck->RequestLayout();
        }
    }
}




void SidebarController::UpdateConfigurations (const Context& rContext)
{
    maCurrentContext = rContext;

    ResourceManager::DeckContainer aDeckDescriptors;
    ResourceManager::Instance().GetMatchingDecks (
        aDeckDescriptors,
        rContext,
        mxFrame);
    mpTabBar->SetDecks(aDeckDescriptors);

    const DeckDescriptor* pDeckDescriptor (ResourceManager::Instance().GetBestMatchingDeck(rContext, mxFrame));
    if (pDeckDescriptor != NULL)
        SwitchToDeck(*pDeckDescriptor, rContext);
}




void SidebarController::SwitchToDeck (
    const DeckDescriptor& rDeckDescriptor)
{
    SwitchToDeck(rDeckDescriptor, maCurrentContext);
}




void SidebarController::SwitchToDeck (
    const DeckDescriptor& rDeckDescriptor,
    const Context& rContext)
{
    // Determine the panels to display in the deck.
    ResourceManager::PanelContainer aPanelDescriptors;
    ResourceManager::Instance().GetMatchingPanels(
        aPanelDescriptors,
        rContext,
        rDeckDescriptor.msId,
        mxFrame);

    // Setup a configuration for the requested deck
    // and create the deck.
    ::boost::shared_ptr<DeckConfiguration> pConfiguration (new DeckConfiguration);
    pConfiguration->mpDeck = new Deck(rDeckDescriptor, mpParentWindow);

    // Create the panels.
    for (ResourceManager::PanelContainer::const_iterator
             iPanel(aPanelDescriptors.begin()),
             iEnd(aPanelDescriptors.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        // Create the panel which is the parent window of the UIElement.
        Panel* pPanel = new Panel(
            *iPanel,
            pConfiguration->mpDeck,
            ::boost::bind(&Deck::RequestLayout,pConfiguration->mpDeck));

        // Create the XUIElement.
        Reference<ui::XUIElement> xUIElement (CreateUIElement(
                pPanel->GetComponentInterface(),
                iPanel->msImplementationURL));
        if (xUIElement.is())
        {
            // Initialize the panel and add it to the active deck.
            pPanel->SetUIElement(xUIElement);
            pConfiguration->maPanels.push_back(pPanel);
        }
        else
        {
            delete pPanel;
        }
    }

    // Activate the new configuration.
    MakeConfigurationCurrent(pConfiguration);

    // Tell the tab bar to highlight the button associated with the
    // deck.
    mpTabBar->HighlightDeck(rDeckDescriptor.msId);
}




Reference<ui::XUIElement> SidebarController::CreateUIElement (
    const Reference<awt::XWindowPeer>& rxWindow,
    const ::rtl::OUString& rsImplementationURL) const
{
    try
    {
        const ::comphelper::ComponentContext aComponentContext (::comphelper::getProcessServiceFactory());
        const Reference<ui::XUIElementFactory> xUIElementFactory (
            aComponentContext.createComponent("com.sun.star.ui.UIElementFactoryManager"),
            UNO_QUERY_THROW);


        // Create the XUIElement.
        ::comphelper::NamedValueCollection aCreationArguments;
        aCreationArguments.put("Frame", makeAny(mxFrame));
        aCreationArguments.put("ParentWindow", makeAny(rxWindow));
        return Reference<ui::XUIElement>(
            xUIElementFactory->createUIElement(
                rsImplementationURL,
                aCreationArguments.getPropertyValues()),
            UNO_QUERY_THROW);
    }
    catch(Exception& rException)
    {
        OSL_TRACE("caught exception: %s",
            OUStringToOString(rException.Message, RTL_TEXTENCODING_ASCII_US).getStr());
        // For some reason we can not create the actual panel.
        // Probably because its factory was not properly registered.
        // TODO: provide feedback to developer to better pinpoint the
        // source of the error.

        return NULL;
    }
}




void SidebarController::MakeConfigurationCurrent (const ::boost::shared_ptr<DeckConfiguration>& rpConfiguration)
{
    if ( ! rpConfiguration || rpConfiguration->mpDeck == NULL)
        return;

    // Deactivate the current deck and panels.
    if (mpCurrentConfiguration && mpCurrentConfiguration->mpDeck!=NULL)
        mpCurrentConfiguration->Disable();

    mpCurrentConfiguration = rpConfiguration;

    mpCurrentConfiguration->mpDeck->SetPosSizePixel(
        0,
        0,
        mpParentWindow->GetSizePixel().Width()-TabBar::GetDefaultWidth(),
        mpParentWindow->GetSizePixel().Height());
    mpCurrentConfiguration->mpDeck->Show();
    mpCurrentConfiguration->Activate();
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

            case VCLEVENT_WINDOW_DATACHANGED:
                // Force an update of deck and tab bar to reflect
                // changes in theme (high contrast mode).
                Theme::HandleDataChange();
                mpParentWindow->Invalidate();
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




void SidebarController::ShowPopupMenu (const Rectangle& rButtonBox) const
{
    ::boost::shared_ptr<PopupMenu> pMenu = CreatePopupMenu();
    pMenu->SetSelectHdl(LINK(this, SidebarController, OnMenuItemSelected));
        
    // pass toolbox button rect so the menu can stay open on button up
    Rectangle aBox (rButtonBox);
    aBox.Move(mpTabBar->GetPosPixel().X(), 0);
    pMenu->Execute(mpParentWindow, aBox, POPUPMENU_EXECUTE_DOWN);
}




::boost::shared_ptr<PopupMenu> SidebarController::CreatePopupMenu (void) const
{
    ::boost::shared_ptr<PopupMenu> pMenu (new PopupMenu());
    FloatingWindow* pMenuWindow = dynamic_cast<FloatingWindow*>(pMenu->GetWindow());
    if (pMenuWindow != NULL)
    {
        pMenuWindow->SetPopupModeFlags(pMenuWindow->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOMOUSEUPCLOSE);
    }

    SidebarResource aLocalResource;
    
    // Add one entry for every tool panel element to individually make
    // them visible or hide them.
    if (mpTabBar != NULL)
    {
        mpTabBar->AddPopupMenuEntries(*pMenu, MID_FIRST_PANEL);
        pMenu->InsertSeparator();
    }

    // Add entry for docking or un-docking the tool panel.
    if (mpParentWindow->IsFloatingMode())
        pMenu->InsertItem(MID_LOCK_TASK_PANEL, String(SfxResId(STR_SFX_DOCK)));
    else
        pMenu->InsertItem(MID_UNLOCK_TASK_PANEL, String(SfxResId(STR_SFX_UNDOCK)));

    // Add sub menu for customization (hiding of deck tabs.)
    PopupMenu* pCustomizationMenu = new PopupMenu();
    mpTabBar->AddCustomizationMenuEntries(*pCustomizationMenu, MID_FIRST_HIDE);
    pCustomizationMenu->InsertSeparator();
    pCustomizationMenu->InsertItem(MID_RESTORE_DEFAULT, String(SfxResId(STRING_RESTORE)));
    
    pMenu->InsertItem(MID_CUSTOMIZATION, String(SfxResId(STRING_CUSTOMIZATION)));
    pMenu->SetPopupMenu(MID_CUSTOMIZATION, pCustomizationMenu);

    pMenu->RemoveDisabledEntries(sal_False, sal_False);
    
    return pMenu;
}




IMPL_LINK(SidebarController, OnMenuItemSelected, Menu*, pMenu)
{
    if (pMenu == NULL)
    {
        OSL_ENSURE(pMenu!=NULL, "TaskPaneController_Impl::OnMenuItemSelected: illegal menu!");
        return 0;
    }

    pMenu->Deactivate();
    const sal_Int32 nIndex (pMenu->GetCurItemId());
    switch (nIndex)
    {
        case MID_UNLOCK_TASK_PANEL:
            mpParentWindow->SetFloatingMode(sal_True);
            break;

        case MID_LOCK_TASK_PANEL:
            mpParentWindow->SetFloatingMode(sal_False);
            break;

        case MID_RESTORE_DEFAULT:
            mpTabBar->RestoreHideFlags();
            break;
            
        default:
        {
            try
            {
                if (nIndex >= MID_FIRST_PANEL && nIndex<MID_FIRST_HIDE)
                    SwitchToDeck(mpTabBar->GetDeckDescriptorForIndex(nIndex - MID_FIRST_PANEL));
                else if (nIndex >=MID_FIRST_HIDE)
                    mpTabBar->ToggleHideFlag(nIndex-MID_FIRST_HIDE);
            }
            catch (RuntimeException&)
            {
            }
        }
        break;
    }

    return 1;
}





} } // end of namespace sfx2::sidebar
