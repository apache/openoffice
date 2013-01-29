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
#include "SidebarPanel.hxx"
#include "SidebarResource.hxx"
#include "TitleBar.hxx"
#include "TabBar.hxx"
#include "sfx2/sidebar/Theme.hxx"

#include "sfxresid.hxx"
#include "sfx2/sfxsids.hrc"
#include "sfx2/dockwin.hxx"
#include "sfxlocal.hrc"
#include <vcl/floatwin.hxx>
#include <vcl/dockwin.hxx>
#include <svl/smplhint.hxx>
#include <tools/link.hxx>
#include <comphelper/componentfactory.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/namedvaluecollection.hxx>

#include <com/sun/star/ui/ContextChangeEventMultiplexer.hpp>
#include <com/sun/star/ui/ContextChangeEventObject.hpp>
#include <com/sun/star/ui/XUIElementFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>


using namespace css;
using namespace cssu;
using ::rtl::OUString;

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
              ::boost::bind(&SidebarController::ShowPopupMenu, this, _1,_2,_3))),
      mxFrame(rxFrame),
      maCurrentContext(
          EnumContext::Application_Unknown,
          EnumContext::Context_Unknown),
      msCurrentDeckId(A2S("PropertyDeck")),
      maPropertyChangeForwarder(::boost::bind(&SidebarController::BroadcastPropertyChange, this))
{
    if (pParentWindow == NULL)
    {
        OSL_ASSERT(pParentWindow!=NULL);
            return;
    }

    // Listen for context change events.
    cssu::Reference<css::ui::XContextChangeEventMultiplexer> xMultiplexer (
        css::ui::ContextChangeEventMultiplexer::get(
            ::comphelper::getProcessComponentContext()));
    if (xMultiplexer.is())
        xMultiplexer->addContextChangeEventListener(
            static_cast<css::ui::XContextChangeEventListener*>(this),
            mxFrame->getController());

    // Listen for window events.
    mpParentWindow->AddEventListener(LINK(this, SidebarController, WindowEventHandler));

    // Listen for theme property changes.
    Theme::GetPropertySet()->addPropertyChangeListener(
        A2S(""),
        static_cast<css::beans::XPropertyChangeListener*>(this));
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

    if (mpCurrentConfiguration)
    {
        mpCurrentConfiguration->Dispose();
        mpCurrentConfiguration.reset();
    }

    Theme::GetPropertySet()->removePropertyChangeListener(
        A2S(""),
        static_cast<css::beans::XPropertyChangeListener*>(this));
}




void SAL_CALL SidebarController::notifyContextChangeEvent (const css::ui::ContextChangeEventObject& rEvent)
    throw(cssu::RuntimeException)
{
    UpdateConfigurations(
        EnumContext(
            rEvent.ApplicationName,
            rEvent.ContextName));
}




void SAL_CALL SidebarController::disposing (const css::lang::EventObject& rEventObject)
    throw(cssu::RuntimeException)
{
    (void)rEventObject;
    
    if (mpCurrentConfiguration)
    {
        mpCurrentConfiguration->Dispose();
        mpCurrentConfiguration.reset();
    }
    if (mpTabBar != NULL)
    {
        mpTabBar->Hide();
        delete mpTabBar;
        mpTabBar = NULL;
    }
}




void SAL_CALL SidebarController::propertyChange (const css::beans::PropertyChangeEvent& rEvent)
    throw(cssu::RuntimeException)
{
    (void)rEvent;
    
    maPropertyChangeForwarder.RequestCall();
}




void SidebarController::BroadcastPropertyChange (void)
{
    DataChangedEvent aEvent (DATACHANGED_USER);
    mpParentWindow->NotifyAllChilds(aEvent);
    mpParentWindow->Invalidate(INVALIDATE_CHILDREN);
}




void SidebarController::NotifyResize (void)
{
    if (mpTabBar == NULL)
    {
        OSL_ASSERT(mpTabBar!=NULL);
        return;
    }
    
    Window* pParentWindow = mpTabBar->GetParent();
    
    const sal_Int32 nWidth (pParentWindow->GetSizePixel().Width());
    const sal_Int32 nHeight (pParentWindow->GetSizePixel().Height());

    if (mpCurrentConfiguration != NULL)
    {
        if (mpCurrentConfiguration->mpDeck==NULL)
        {
            OSL_ASSERT(mpCurrentConfiguration->mpDeck!=NULL);
        }
        else
        {
            mpCurrentConfiguration->mpDeck->SetPosSizePixel(0,0, nWidth-TabBar::GetDefaultWidth(), nHeight);
            mpCurrentConfiguration->mpDeck->Show();
            mpCurrentConfiguration->mpDeck->RequestLayout();
        }
    }

    mpTabBar->SetPosSizePixel(nWidth-TabBar::GetDefaultWidth(),0,TabBar::GetDefaultWidth(),nHeight);
    mpTabBar->Show();
}




void SidebarController::UpdateConfigurations (const EnumContext& rContext)
{
    if (maCurrentContext != rContext)
    {
        maCurrentContext = rContext;

        // Notify the tab bar about the updated set of decks.
        ResourceManager::IdContainer aDeckIds;
        ResourceManager::Instance().GetMatchingDecks (
            aDeckIds,
            rContext,
            mxFrame);
        mpTabBar->SetDecks(aDeckIds);

        // Check if the current deck is among the matching decks.
        bool bCurrentDeckMatches (false);
        for (ResourceManager::IdContainer::const_iterator
                 iDeck(aDeckIds.begin()),
                 iEnd(aDeckIds.end());
             iDeck!=iEnd;
             ++iDeck)
        {
            if (iDeck->equals(msCurrentDeckId))
            {
                bCurrentDeckMatches = true;
                break;
            }
        }

        DeckDescriptor const* pDeckDescriptor = NULL;
        if ( ! bCurrentDeckMatches)
        {
            pDeckDescriptor = ResourceManager::Instance().GetBestMatchingDeck(rContext, mxFrame);
            msCurrentDeckId = pDeckDescriptor->msId;
        }
        else
            pDeckDescriptor = ResourceManager::Instance().GetDeckDescriptor(msCurrentDeckId);
        if (pDeckDescriptor != NULL)
        {
            msCurrentDeckId = pDeckDescriptor->msId;
            SwitchToDeck(*pDeckDescriptor, rContext);
        }
    }
}




void SidebarController::SwitchToDeck (
    const ::rtl::OUString& rsDeckId)
{
    if ( ! msCurrentDeckId.equals(rsDeckId))
    {
        const DeckDescriptor* pDeckDescriptor = ResourceManager::Instance().GetDeckDescriptor(rsDeckId);
        if (pDeckDescriptor != NULL)
            SwitchToDeck(*pDeckDescriptor, maCurrentContext);
    }
}




void SidebarController::SwitchToDeck (
    const DeckDescriptor& rDeckDescriptor,
    const EnumContext& rContext)
{
    if ( ! msCurrentDeckId.equals(rDeckDescriptor.msId))
    {
        // When the deck changes then destroy the deck and all panels
        // and create everything new.
        if (mpCurrentConfiguration)
        {
            mpCurrentConfiguration->Dispose();
            mpCurrentConfiguration.reset();
        }

        msCurrentDeckId = rDeckDescriptor.msId;
    }
    
    // Determine the panels to display in the deck.
    ResourceManager::IdContainer aPanelIds;
    ResourceManager::Instance().GetMatchingPanels(
        aPanelIds,
        rContext,
        rDeckDescriptor.msId,
        mxFrame);

    // Provide a configuration and Deck object.
    if ( ! mpCurrentConfiguration)
    {
        mpCurrentConfiguration.reset(new DeckConfiguration);
        mpCurrentConfiguration->mpDeck = new Deck(rDeckDescriptor, mpParentWindow);
    }

    // Update the panel list.
    const sal_Int32 nNewPanelCount (aPanelIds.size());
    ::std::vector<Panel*> aNewPanels;
    ::std::vector<Panel*> aCurrentPanels;
    if (mpCurrentConfiguration)
        aCurrentPanels.swap(mpCurrentConfiguration->maPanels);
    aNewPanels.resize(nNewPanelCount);
    for (sal_Int32 nIndex=0; nIndex<nNewPanelCount; ++nIndex)
    {
        const OUString& rsPanelId (aPanelIds[nIndex]);

        // Find the corresponding panel among the currently active
        // panels.
        ::std::vector<Panel*>::iterator iPanel (::std::find_if(
                aCurrentPanels.begin(),
                aCurrentPanels.end(),
                ::boost::bind(&Panel::HasIdPredicate, _1, ::boost::cref(rsPanelId))));
        if (iPanel != aCurrentPanels.end())
        {
            // Panel already exists in current configuration.  Move it
            // to new configuration.
            aNewPanels[nIndex] = *iPanel;
            aCurrentPanels[::std::distance(aCurrentPanels.begin(), iPanel)] = NULL;
        }
        else
        {
            // Panel does not yet exist.  Create it.
            aNewPanels[nIndex] = CreatePanel(rsPanelId, mpCurrentConfiguration->mpDeck);
        }
    }

    // Destroy all panels that are not used in the new configuration.
    for (::std::vector<Panel*>::const_iterator iPanel(aCurrentPanels.begin()),iEnd(aCurrentPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        if (*iPanel != NULL)
            (*iPanel)->Dispose();
    }

    // Activate the deck and the new set of panels.
    mpCurrentConfiguration->maPanels.swap(aNewPanels);
    mpCurrentConfiguration->mpDeck->SetPosSizePixel(
        0,
        0,
        mpParentWindow->GetSizePixel().Width()-TabBar::GetDefaultWidth(),
        mpParentWindow->GetSizePixel().Height());
    mpCurrentConfiguration->mpDeck->SetPanels(mpCurrentConfiguration->maPanels);
    mpCurrentConfiguration->mpDeck->Show();

    // Tell the tab bar to highlight the button associated with the
    // deck.
    mpTabBar->HighlightDeck(rDeckDescriptor.msId);
}




Panel* SidebarController::CreatePanel (
    const OUString& rsPanelId,
    Deck* pDeck) const
{
    const PanelDescriptor* pPanelDescriptor = ResourceManager::Instance().GetPanelDescriptor(rsPanelId);
    if (pPanelDescriptor == NULL)
        return NULL;
            
    // Create the panel which is the parent window of the UIElement.
    Panel* pPanel = new Panel(
        *pPanelDescriptor,
        pDeck,
        ::boost::bind(&Deck::RequestLayout,pDeck));

    // Create the XUIElement.
    Reference<ui::XUIElement> xUIElement (CreateUIElement(
            pPanel->GetComponentInterface(),
            pPanelDescriptor->msImplementationURL,
            pPanel));
    if (xUIElement.is())
    {
        // Initialize the panel and add it to the active deck.
        pPanel->SetUIElement(xUIElement);
    }
    else
    {
        delete pPanel;
        pPanel = NULL;
    }

    return pPanel;
}




Reference<ui::XUIElement> SidebarController::CreateUIElement (
    const Reference<awt::XWindowPeer>& rxWindow,
    const ::rtl::OUString& rsImplementationURL,
    Panel* pPanel) const
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
        SfxDockingWindow* pSfxDockingWindow = dynamic_cast<SfxDockingWindow*>(mpParentWindow);
        if (pSfxDockingWindow != NULL)
            aCreationArguments.put("SfxBindings", makeAny(sal_uInt64(&pSfxDockingWindow->GetBindings())));
        const Sequence<beans::PropertyValue> aProperties (aCreationArguments.getPropertyValues());
        Reference<ui::XUIElement> xUIElement(
            xUIElementFactory->createUIElement(
                rsImplementationURL,
                aProperties),
            UNO_QUERY_THROW);

        // Provide the new ui element with the XSidebarPanel object
        // that gives access to a canvas, screen coordinates of the
        // panel or the theme properties.
        if (xUIElement.is())
        {
            Reference<lang::XInitialization> xInitialization(xUIElement->getRealInterface(), UNO_QUERY);
            if (xInitialization.is())
            {
                Sequence<Any> aArguments (1);
                Reference<ui::XSidebarPanel> xPanel (SidebarPanel::Create(pPanel));
                aArguments[0] = Any(xPanel);
                xInitialization->initialize(aArguments);
            }
        }
        
        return xUIElement;
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




IMPL_LINK(SidebarController, WindowEventHandler, VclWindowEvent*, pEvent)
{
    if (pEvent != NULL)
    {
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




void SidebarController::ShowPopupMenu (
    const Rectangle& rButtonBox,
    const ::std::vector<TabBar::DeckMenuData>& rDeckSelectionData,
    const ::std::vector<TabBar::DeckMenuData>& rDeckShowData) const
{
    ::boost::shared_ptr<PopupMenu> pMenu = CreatePopupMenu(rDeckSelectionData, rDeckShowData);
    pMenu->SetSelectHdl(LINK(this, SidebarController, OnMenuItemSelected));
        
    // pass toolbox button rect so the menu can stay open on button up
    Rectangle aBox (rButtonBox);
    aBox.Move(mpTabBar->GetPosPixel().X(), 0);
    pMenu->Execute(mpParentWindow, aBox, POPUPMENU_EXECUTE_DOWN);
}




::boost::shared_ptr<PopupMenu> SidebarController::CreatePopupMenu (
    const ::std::vector<TabBar::DeckMenuData>& rDeckSelectionData,
    const ::std::vector<TabBar::DeckMenuData>& rDeckShowData) const
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
    {
        sal_Int32 nIndex (MID_FIRST_PANEL);
        for(::std::vector<TabBar::DeckMenuData>::const_iterator
                iItem(rDeckSelectionData.begin()),
                iEnd(rDeckSelectionData.end());
            iItem!=iEnd;
            ++iItem)
        {
            pMenu->InsertItem(nIndex, iItem->get<0>(), MIB_RADIOCHECK);
            pMenu->CheckItem(nIndex, iItem->get<2>());
            ++nIndex;
        }
    }

    pMenu->InsertSeparator();

    // Add entry for docking or un-docking the tool panel.
    if (mpParentWindow->IsFloatingMode())
        pMenu->InsertItem(MID_LOCK_TASK_PANEL, String(SfxResId(STR_SFX_DOCK)));
    else
        pMenu->InsertItem(MID_UNLOCK_TASK_PANEL, String(SfxResId(STR_SFX_UNDOCK)));

    // Add sub menu for customization (hiding of deck tabs.)
    PopupMenu* pCustomizationMenu = new PopupMenu();
    {
        sal_Int32 nIndex (MID_FIRST_HIDE);
        for(::std::vector<TabBar::DeckMenuData>::const_iterator
                iItem(rDeckShowData.begin()),
                iEnd(rDeckShowData.end());
            iItem!=iEnd;
            ++iItem)
        {
            pCustomizationMenu->InsertItem(nIndex, iItem->get<0>(), MIB_CHECKABLE);
            pCustomizationMenu->CheckItem(nIndex, iItem->get<2>());
            ++nIndex;
        }
    }

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
                    SwitchToDeck(mpTabBar->GetDeckIdForIndex(nIndex - MID_FIRST_PANEL));
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
