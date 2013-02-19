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

#include "precompiled_sd.hxx"

#include "SidebarViewShell.hxx"

#include "panels/SidebarShellManager.hxx"
#include "SidebarFocusManager.hxx"
#include "taskpane/SlideSorterCacheDisplay.hxx"
#include "panels/LayoutMenu.hxx"
#include "panels/CurrentMasterPagesSelector.hxx"
#include "panels/RecentMasterPagesSelector.hxx"
#include "panels/AllMasterPagesSelector.hxx"
#include "panels/CustomAnimationPanel.hxx"
#include "panels/TableDesignPanel.hxx"
#include "panels/SlideTransitionPanel.hxx"

#include "glob.hrc"
#include "Window.hxx"
#include "FrameView.hxx"
#include "framework/FrameworkHelper.hxx"

#include <com/sun/star/drawing/framework/XResourceId.hpp>
#include <com/sun/star/drawing/framework/ResourceActivationMode.hpp>
#include <com/sun/star/drawing/XDrawSubController.hpp>

#include <svx/dlgctrl.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/msg.hxx>
#include <sfx2/objface.hxx>
#include <svx/colrctrl.hxx>
#include <svx/xtable.hxx>
#include <vcl/dockwin.hxx>
#include "sdtreelb.hxx"
#include "DrawViewShell.hxx"
#include "drawdoc.hxx"
#include "ViewShellBase.hxx"
#include <svx/ruler.hxx>
#include <vcl/svapp.hxx>

#include <vector>
#include <boost/shared_ptr.hpp>


#define SidebarViewShell
using namespace sd::sidebar;
#include "sdslots.hxx"


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using ::sd::framework::FrameworkHelper;


namespace sd { namespace sidebar {

SFX_IMPL_INTERFACE(SidebarViewShell, SfxShell, SdResId(STR_SIDEBARVIEWSHELL))
{
}

TYPEINIT1(SidebarViewShell, ViewShell);




SidebarViewShell::SidebarViewShell (
    SfxViewFrame* pFrame, 
    ViewShellBase& rViewShellBase,
    ::Window* pParentWindow,
    FrameView* pFrameViewArgument)
    : ViewShell (pFrame, pParentWindow, rViewShellBase),
      mpSubShellManager(),
      maContentWindowParent(NULL, WB_STDWORK)
{
    meShellType = ST_SIDEBAR;

    // Move the content window to a parent whose lifetime we have
    // under control.
    mpContentWindow->SetParent(&maContentWindowParent);
    mpContentWindow->SetViewShell(NULL);

#ifdef DEBUG
    mpContentWindow->SetText(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SidebarViewShell:ContentWindow")));
#endif

	mpContentWindow->SetCenterAllowed (false);

	SetPool (&GetDoc()->GetPool());

	if (pFrameViewArgument != NULL)
		mpFrameView = pFrameViewArgument;
	else
        mpFrameView = new FrameView(GetDoc());
	GetFrameView()->Connect();

    // Hide or delete unused controls that we have inherited from the
    // ViewShell base class.
	mpHorizontalScrollBar.reset();
	mpVerticalScrollBar.reset();
    mpScrollBarBox.reset();
    mpHorizontalRuler.reset();
    mpVerticalRuler.reset();
    mpContentWindow->Hide();
    SetActiveWindow(mpContentWindow.get());
    
	SetName (String (RTL_CONSTASCII_USTRINGPARAM("SidebarViewShell")));

    // Register the shell manager as factory at the ViewShellManager.
    mpSubShellManager.reset (new SidebarShellManager(
        GetViewShellBase().GetViewShellManager(),
        *this));
    GetViewShellBase().GetViewShellManager()->AddSubShellFactory(this, mpSubShellManager);
}




SidebarViewShell::~SidebarViewShell (void)
{
    GetViewShellBase().GetViewShellManager()->RemoveSubShellFactory(this, mpSubShellManager);
	GetFrameView()->Disconnect(); //, memory leak,

    // Delete the content window before its parent is destroyed.
    mpContentWindow.reset();
}



// static
void SidebarViewShell::RegisterControls (void)
{
	SfxModule* pModule = SD_MOD();
    MasterPagesSelector::RegisterInterface(pModule);
    LayoutMenu::RegisterInterface(pModule);
}




::Window* SidebarViewShell::CreatePanel (
    ::Window* pParentWindow,
    const PanelId ePanelId)
{
    if (pParentWindow == NULL)
        return NULL;
    switch(ePanelId)
    {
        case PID_CUSTOM_ANIMATION:
            return new CustomAnimationPanel(pParentWindow, this->GetViewShellBase());

        case PID_LAYOUT:
            return new LayoutMenu(
                pParentWindow,
                GetViewShellBase(),
                GetSubShellManager());

        case PID_MASTER_PAGES_ALL:
            return AllMasterPagesSelector::Create(
                pParentWindow,
                GetViewShellBase(),
                GetSubShellManager());
        case PID_MASTER_PAGES_RECENT:
            return RecentMasterPagesSelector::Create(
                pParentWindow,
                GetViewShellBase(),
                GetSubShellManager());
        case PID_MASTER_PAGES_USED:
            return CurrentMasterPagesSelector::Create(
                pParentWindow,
                GetViewShellBase(),
                GetSubShellManager());

        case PID_SLIDE_TRANSITION:
            return new SlideTransitionPanel(pParentWindow, this->GetViewShellBase());
            
        case PID_TABLE_DESIGN:
            return new TableDesignPanel(pParentWindow, this->GetViewShellBase());

        default:
            return NULL;
    }
}




void SidebarViewShell::ArrangeGUIElements (void)
{
    ViewShell::ArrangeGUIElements();
}




void SidebarViewShell::GetFocus (void)
{
    Invalidate ();
}




void SidebarViewShell::LoseFocus (void)
{
    Invalidate ();
}




void SidebarViewShell::KeyInput (const KeyEvent& rEvent)
{
	KeyCode nCode = rEvent.GetKeyCode();
    if (nCode == KEY_RETURN)
    {
//        mpTaskPane->GrabFocus();
    }
    else
        ViewShell::KeyInput (rEvent, NULL);
}







SdPage*	SidebarViewShell::GetActualPage (void)
{
    return NULL;
}

SdPage*	SidebarViewShell::getCurrentPage(void) const
{
    return NULL;
}



void SidebarViewShell::Execute (SfxRequest& )
{
}




void SidebarViewShell::GetState (SfxItemSet& rItemSet)
{
    (void)rItemSet;
}




SidebarShellManager& SidebarViewShell::GetSubShellManager (void) const
{
    return *mpSubShellManager.get();
}




DockingWindow* SidebarViewShell::GetDockingWindow (void)
{
    ::Window* pParentWindow = GetParentWindow();
    DockingWindow* pDockingWindow = NULL;
    while (pParentWindow!=NULL && pDockingWindow==NULL)
    {
        pDockingWindow = dynamic_cast<DockingWindow*>(pParentWindow);
        pParentWindow = pParentWindow->GetParent();
    }
    return pDockingWindow;
}




::com::sun::star::uno::Reference<
    ::com::sun::star::accessibility::XAccessible>
    SidebarViewShell::CreateAccessibleDocumentView (::sd::Window* pWindow)
{
    ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible> xAccessible;

    //if (mpTaskPane.get()!=NULL && pWindow!=NULL)
    //{
    //    // We have to call CreateAccessible directly so that we can specify
    //    // the correct accessible parent.
    //    ::Window* pParentWindow = pWindow->GetAccessibleParentWindow();
    //    if (pParentWindow != NULL)
    //        xAccessible = mpTaskPane->CreateAccessibleObject(
    //            pParentWindow->GetAccessible());
    //}

    return xAccessible;
}




Reference<drawing::XDrawSubController> SidebarViewShell::CreateSubController (void)
{
    // This view shell is not designed to be the main view shell and thus
    // does not support a UNO sub controller.
    return Reference<drawing::XDrawSubController>();
}




bool SidebarViewShell::RelocateToParentWindow (::Window* pParentWindow)
{
   /* ::Window* pOldParentWindow = GetParentWindow();
    FocusManager::Instance().RemoveLinks(pOldParentWindow, mpTaskPane.get());
    FocusManager::Instance().RemoveLinks(mpTaskPane.get(), pOldParentWindow);

    ViewShell::RelocateToParentWindow(pParentWindow);
    
    PaneDockingWindow* pDockingWindow = dynamic_cast<PaneDockingWindow*>(GetDockingWindow());
    if (pDockingWindow != NULL)
    {
        pDockingWindow->InitializeTitleToolBox();
        mnMenuId = pDockingWindow->AddMenu (
            String(SdResId(STR_TASKPANEL_MASTER_PAGE_MENU_TITLE)),
			HID_SD_TASK_PANE_VIEW_MENU,
            LINK(this, SidebarViewShell, ToolboxClickHandler));
    }
    FocusManager::Instance().RegisterDownLink(pParentWindow, mpTaskPane.get());

    Resize();*/

    return true;
}


} } // end of namespace ::sd::sidebar
