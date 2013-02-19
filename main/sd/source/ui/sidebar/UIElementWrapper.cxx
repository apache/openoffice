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

#include "UIElementWrapper.hxx"

#include "framework/FrameworkHelper.hxx"
#include "framework/TaskPanelResource.hxx"
#include "panels/ILayoutableWindow.hxx"
#include "panels/LayoutMenu.hxx"
#include "DrawController.hxx"
#include <comphelper/make_shared_from_uno.hxx>
#include <vcl/wrkwin.hxx>

#include <boost/bind.hpp>

using namespace css;
using namespace cssu;
using namespace cssdf;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sd { namespace sidebar {

UIElementWrapper::UIElementWrapper (
    const ::rtl::OUString& rsUIElementResourceURL,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
     const ::rtl::OUString& rsTaskPanelURL,
    const cssu::Reference<css::ui::XSidebar>& rxSidebar,
    ::Window* pParentWindow)
    : SidebarPanelBase(rsUIElementResourceURL, rxFrame, NULL, ::boost::function<void(void)>()),
      mxPanelResourceId(),
      mxSidebar(rxSidebar),
      mpParentWindow(pParentWindow),
      mpTemporaryParent(NULL),
      mbResourceHasBeenDeactivated(false)
{
    OSL_TRACE("created UIElementWrapper at %x", this);
    if ( ! rxFrame.is())
        throw RuntimeException(A2S("UIElementWrapper has no XFrame"), NULL);
    ::boost::shared_ptr<framework::FrameworkHelper> pFrameworkHelper (
        framework::FrameworkHelper::Instance(mxFrame->getController()));
    if ( ! pFrameworkHelper)
        throw RuntimeException(A2S("UIElementWrapper has no FrameworkHelper"), NULL);
    
    // Request the drawing framework to make pane and view visible.
    mxPanelResourceId = pFrameworkHelper->RequestSidebarPanel(rsTaskPanelURL);

    // Set up a function to run when the requested panel has been created.
    ::boost::shared_ptr<UIElementWrapper> pThis (
        ::comphelper::make_shared_from_UNO<UIElementWrapper>(this));
    pFrameworkHelper->RunOnResourceActivation(
        mxPanelResourceId,
        ::boost::bind(
            &UIElementWrapper::HandleResourceActivation,
            pThis,
            _1));
}




UIElementWrapper::~UIElementWrapper (void)
{
    OSL_TRACE("~UIElementWrapper at %x", this);
    if (mpTemporaryParent != NULL)
    {
        // This should not happen. But it does happen when the office
        // is closed.

        DestroyWindows(mpTemporaryParent);
    }
}




void SAL_CALL UIElementWrapper::disposing (void)
    throw (cssu::RuntimeException)
{
    OSL_TRACE("UIElementWrapper::disposing at %x", this);
        
    // Deactivate panel, view and pane.
    ::boost::shared_ptr<framework::FrameworkHelper> pFrameworkHelper (
        framework::FrameworkHelper::Instance(mxFrame->getController()));
    if ( ! pFrameworkHelper)
        return;

    if (mxPanelResourceId.is() && pFrameworkHelper)
    {
        pFrameworkHelper->RequestResourceDeactivation(mxPanelResourceId);

        Reference<XResourceId> xViewId (mxPanelResourceId->getAnchor());
        if (xViewId.is())
        {
            pFrameworkHelper->RequestResourceDeactivation(xViewId);

            Reference<XResourceId> xPaneId (xViewId->getAnchor());
            if (xPaneId.is())
            {
                pFrameworkHelper->RequestResourceDeactivation(xPaneId);
            }
        }
    }

    Reference<lang::XComponent> xPanelComponent (pFrameworkHelper->GetResource(mxPanelResourceId), UNO_QUERY);
    if (xPanelComponent.is())
        xPanelComponent->dispose();

    /*
    ::boost::scoped_ptr< ::Window> pTemporaryParent (new WorkWindow(NULL,WB_STDWORK));

    // Reparent all children of mpParentWindow to our local window.
    // This is necessary because mpParentWindow will be destroyed
    // before the children, which are owned by drawing framework
    // resoures.  These resources have been requested to be
    // deactivated above but deactivation happens asynchronously.
    if (GetControl() != NULL)
        GetControl()->SetParent(pTemporaryParent.get());

    if (mbResourceHasBeenDeactivated)
    {
        // View has been deactivated in the meantime.  We can delete
        // the windows now.
        DestroyWindows(pTemporaryParent);
    }
    else
    {
        // View will be deactivated later.  Remember the temporary window.
        mpTemporaryParent.swap(pTemporaryParent);
    }
    */
    SidebarPanelBase::disposing();
}




ui::LayoutSize SAL_CALL UIElementWrapper::getHeightForWidth (const sal_Int32 nWidth)
    throw(cssu::RuntimeException)
{
    ILayoutableWindow* pLayoutableWindow = dynamic_cast<ILayoutableWindow*>(GetControl());
    if (pLayoutableWindow != NULL)
        return pLayoutableWindow->GetHeightForWidth(nWidth);
    else
        return ui::LayoutSize(0,0,0);
}




void SAL_CALL UIElementWrapper::disposing (
    const css::lang::EventObject& rEvent)
    throw (cssu::RuntimeException)
{
    Reference<XResource> xPanel (rEvent.Source, UNO_QUERY);
    if (xPanel.is()
        && xPanel->getResourceId()->compareTo(mxPanelResourceId) == 0)
    {
        OSL_TRACE("UIElementWrapper at %x got disposing call for panel %x", this, GetControl());
        mbResourceHasBeenDeactivated = true;
        SetControl(NULL);
    }
    else
    {
        SidebarPanelBase::disposing(rEvent);
    }
}




void UIElementWrapper::HandleResourceActivation (const bool bIsResourceActive)
{
    if ( ! mxFrame.is())
        return;
    ::boost::shared_ptr<framework::FrameworkHelper> pFrameworkHelper (
        framework::FrameworkHelper::Instance(mxFrame->getController()));
    if ( ! pFrameworkHelper)
        return;
    cssu::Reference<cssdf::XResource> xPanel (pFrameworkHelper->GetResource(mxPanelResourceId));
    if ( ! xPanel.is())
        return;
    Reference<lang::XComponent> xComponent (xPanel, UNO_QUERY);
    if (xComponent.is())
        xComponent->addEventListener(this);
    framework::TaskPanelResource* pResource = dynamic_cast<framework::TaskPanelResource*>(xPanel.get());
    if (pResource == NULL)
        return;
    ::Window* pControl = pResource->GetControl();
    SetControl(pControl);
    if (pControl == NULL)
        return;
    pControl->SetParent(mpParentWindow);
    pControl->SetSizePixel(mpParentWindow->GetSizePixel());
    ISidebarReceiver* pSidebarReceiver = dynamic_cast<ISidebarReceiver*>(pControl);
    if (pSidebarReceiver != NULL)
        pSidebarReceiver->SetSidebar(mxSidebar);
	OSL_TRACE("UIELementWrapper has panel at %x", pControl);
}




void UIElementWrapper::DestroyWindows (::boost::scoped_ptr< ::Window>& rpRoot)
{
    if ( ! rpRoot)
        return;
    else if (rpRoot->GetChildCount() == 0)
        rpRoot.reset();
    else
        ; // The window still has living windows.  Better to leak
          // rpRoot than to destroy the parent of the living children.
}



} } // end of namespace sd::sidebar

