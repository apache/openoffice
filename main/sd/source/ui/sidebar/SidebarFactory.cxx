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

#include "SidebarFactory.hxx"
#include "framework/FrameworkHelper.hxx"
#include "framework/Pane.hxx"
#include "ViewShellBase.hxx"
#include "UIElementWrapper.hxx"

#include <sfx2/viewfrm.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include <vcl/window.hxx>
#include <toolkit/helper/vclunohelper.hxx>

#include <com/sun/star/drawing/framework/XControllerManager.hpp>

using namespace css;
using namespace cssu;
using namespace css::drawing::framework;
using namespace ::sd::framework;
using ::rtl::OUString;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sd { namespace sidebar {

namespace {
    /** Note that these names have to be identical to (the tail of)
        the entries in officecfg/registry/data/org/openoffice/Office/Impress.xcu
        for the TaskPanelFactory.
    */
    const static char* gsResourceNameCustomAnimations = "/CustomAnimations";
    const static char* gsResourceNameLayouts = "/Layouts";
    const static char* gsResourceNameAllMasterPages = "/AllMasterPages";
    const static char* gsResourceNameRecentMasterPages = "/RecentMasterPages";
    const static char* gsResourceNameUsedMasterPages = "/UsedMasterPages";
    const static char* gsResourceNameSlideTransitions = "/SlideTransitions";
    const static char* gsResourceNameTableDesign = "/TableDesign";
}

class SidebarFactory::Implementation
{
public:
    Reference<cssdf::XConfigurationController> mxConfigurationController;
    rtl::Reference<framework::Pane> mxPane;
    Reference<ui::XUIElement> mxUIElement;
    Reference<frame::XController> mxController;
    Reference<awt::XWindow> mxParentWindow;
    bool mbIsDisposed;

    Implementation (const Reference<frame::XController>& rxController);
    ~Implementation (void);
    void Dispose (void);
};
    

SidebarFactory::ControllerToImplementationMap SidebarFactory::maControllerToImplementationMap;
Reference<lang::XEventListener> mxControllerDisposeListener;



// ----- Service functions ----------------------------------------------------

Reference<XInterface> SAL_CALL SidebarFactory_createInstance (
    const Reference<XComponentContext>& rxContext)
{
    return Reference<XInterface>(static_cast<XWeak*>(new SidebarFactory(rxContext)));
}




::rtl::OUString SidebarFactory_getImplementationName (void) throw(RuntimeException)
{
    return ::rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM("org.openoffice.comp.Draw.framework.SidebarFactory"));
}




Sequence<rtl::OUString> SAL_CALL SidebarFactory_getSupportedServiceNames (void)
    throw (RuntimeException)
{
	static const ::rtl::OUString sServiceName(
        ::rtl::OUString::createFromAscii("com.sun.star.drawing.framework.SidebarFactory"));
	return Sequence<rtl::OUString>(&sServiceName, 1);
}




//----- SidebarFactory --------------------------------------------------------

SidebarFactory::SidebarFactory(
        const css::uno::Reference<css::uno::XComponentContext>& rxContext)
    : SidebarFactoryInterfaceBase(m_aMutex),
      mpImplementation()
{
}




SidebarFactory::~SidebarFactory (void)
{
}




void SAL_CALL SidebarFactory::disposing (void)
{
    SharedImplementation pImplementation;
    pImplementation.swap(mpImplementation);
    if (pImplementation)
        pImplementation->Dispose();
}




// XInitialization

void SAL_CALL SidebarFactory::initialize (const Sequence<Any>& aArguments)
    throw (Exception, RuntimeException)
{
    if (aArguments.getLength() > 0)
    {
        mpImplementation = GetImplementationForController(
            Reference<frame::XController>(aArguments[0], UNO_QUERY));
    }
}




// XUIElementFactory

Reference<ui::XUIElement> SAL_CALL SidebarFactory::createUIElement (
    const ::rtl::OUString& rsUIElementResourceURL,
    const ::cssu::Sequence<css::beans::PropertyValue>& rArguments)
    throw(
        css::container::NoSuchElementException,
        css::lang::IllegalArgumentException,
        cssu::RuntimeException)
{
    // Process arguments.
    const ::comphelper::NamedValueCollection aArguments (rArguments);
    Reference<frame::XFrame> xFrame (aArguments.getOrDefault("Frame", Reference<frame::XFrame>()));
    Reference<awt::XWindow> xParentWindow (aArguments.getOrDefault("ParentWindow", Reference<awt::XWindow>()));
    Reference<ui::XSidebar> xSidebar (aArguments.getOrDefault("Sidebar", Reference<ui::XSidebar>()));

    // Throw exceptions when the arguments are not as expected.
    ::Window* pParentWindow = VCLUnoHelper::GetWindow(xParentWindow);
    if ( ! xParentWindow.is() || pParentWindow==NULL)
        throw RuntimeException(
            A2S("SidebarFactory::createUIElement called without ParentWindow"),
            NULL);
    if ( ! xFrame.is())
        throw RuntimeException(
            A2S("SidebarFactory::createUIElement called without XFrame"),
            NULL);
    mpImplementation = GetImplementationForController(xFrame->getController());
    if ( ! mpImplementation)
        throw RuntimeException(
            A2S("SidebarFactory::createUIElement called without XController"),
            NULL);
    if (mpImplementation->mxConfigurationController.is())
        mpImplementation->mxConfigurationController->addResourceFactory(
            FrameworkHelper::msSidebarPaneURL+A2S("*"), this);

    //  Remember the parent window, so that following calls to
    //  createResource() can access it.
    mpImplementation->mxParentWindow = xParentWindow;
    if (mpImplementation->mxPane.is())
        mpImplementation->mxPane->SetWindow(VCLUnoHelper::GetWindow(xParentWindow));

    // Create a framework view.
    OUString sTaskPanelURL;
#define EndsWith(s,t) s.endsWithAsciiL(t,strlen(t))
    if (EndsWith(rsUIElementResourceURL, gsResourceNameCustomAnimations))
        sTaskPanelURL = FrameworkHelper::msCustomAnimationTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameLayouts))
        sTaskPanelURL = FrameworkHelper::msLayoutTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameAllMasterPages))
        sTaskPanelURL = FrameworkHelper::msAllMasterPagesTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameRecentMasterPages))
        sTaskPanelURL = FrameworkHelper::msRecentMasterPagesTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameUsedMasterPages))
        sTaskPanelURL = FrameworkHelper::msUsedMasterPagesTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameSlideTransitions))
        sTaskPanelURL = FrameworkHelper::msSlideTransitionTaskPanelURL;
    else if (EndsWith(rsUIElementResourceURL, gsResourceNameTableDesign))
        sTaskPanelURL = FrameworkHelper::msTableDesignPanelURL;
#undef EndsWith
    
    if (sTaskPanelURL.getLength() == 0)
        throw lang::IllegalArgumentException();

    // Create a wrapper around pane and view and return it as
    // XUIElement.
    Reference<ui::XUIElement> xUIElement;
    try
    {
        xUIElement.set(new UIElementWrapper(
                rsUIElementResourceURL,
                xFrame,
                sTaskPanelURL,
                xSidebar,
                VCLUnoHelper::GetWindow(xParentWindow)));
    }
    catch(Exception& rException)
    {
        // Creation of XUIElement failed. mxUIElement remains empty.
    }

    Reference<lang::XComponent> xComponent (xUIElement, UNO_QUERY);
    if (xComponent.is())
        xComponent->addEventListener(this);
    
    return xUIElement;
}




// XResourceFactory

Reference<drawing::framework::XResource>
    SAL_CALL SidebarFactory::createResource (
        const Reference<drawing::framework::XResourceId>& rxPaneId)
    throw (RuntimeException, lang::IllegalArgumentException, lang::WrappedTargetException)
{
    if ( ! rxPaneId.is())
        throw lang::IllegalArgumentException();

    const OUString sResourceURL (rxPaneId->getResourceURL());
    if ( ! sResourceURL.equals(FrameworkHelper::msSidebarPaneURL))
        throw lang::IllegalArgumentException();

    if ( ! mpImplementation)
        throw RuntimeException(A2S("SidebarFactory not initialized"), NULL);
    
    ::Window* pWindow = VCLUnoHelper::GetWindow(mpImplementation->mxParentWindow);
    if (pWindow == NULL)
        throw RuntimeException();

    mpImplementation->mxPane.set(new Pane(rxPaneId, pWindow));
    Reference<drawing::framework::XResource> xPane (static_cast<XPane*>(mpImplementation->mxPane.get()));
    return xPane;
}




void SAL_CALL SidebarFactory::releaseResource (
    const Reference<drawing::framework::XResource>& rxPane)
    throw (RuntimeException)
{
    (void)rxPane;
    
    // Any panes created by us are just wrappers around windows whose lifetime
    // is controlled somewhere else => nothing to do.
}




void SAL_CALL SidebarFactory::disposing (const ::css::lang::EventObject& rEvent)
    throw(cssu::RuntimeException)
{
    /*
    if (mpImplementation
        && rEvent.Source == mpImplementation->mxUIElement)
    {
        mpImplementation->mxUIElement.clear();
    }
    */
}




SidebarFactory::SharedImplementation SidebarFactory::GetImplementationForController (
    const cssu::Reference<css::frame::XController>& rxController)
{
    SharedImplementation pImplementation;
    if (rxController.is())
    {
        ControllerToImplementationMap::const_iterator iImplementation (
            maControllerToImplementationMap.find(rxController));
        if (iImplementation == maControllerToImplementationMap.end())
        {
            pImplementation.reset(new Implementation(rxController));
            maControllerToImplementationMap[rxController] = pImplementation;
            
            // Each Implementation object is referenced by two
            // SidebarFactory objects.  The Implementation object is
            // destroyed when the first of the SidebarFactory objects
            // is disposed.
        }
        else
            pImplementation = iImplementation->second;
    }
    return pImplementation;
}




//----- SidebarFactory::Implementation ----------------------------------------

SidebarFactory::Implementation::Implementation (const Reference<frame::XController>& rxController)
    : mxConfigurationController(),
      mxPane(),
      mxUIElement(),
      mxController(rxController),
      mxParentWindow(),
      mbIsDisposed(false)
{
    Reference<XControllerManager> xCM (rxController, UNO_QUERY);
    if (xCM.is())
    {
        mxConfigurationController = xCM->getConfigurationController();
    }
}




SidebarFactory::Implementation::~Implementation (void)
{
    Dispose();
}




void SidebarFactory::Implementation::Dispose (void)
{
    if (mbIsDisposed)
        return;

    mbIsDisposed = true;

    // Release the entry in the maControllerToImplementationMap even though there
    // may be another object that "references" to it.
    ControllerToImplementationMap::iterator iImplementation (
        maControllerToImplementationMap.find(mxController));
        if (iImplementation != maControllerToImplementationMap.end())
            maControllerToImplementationMap.erase(iImplementation);
    
    mxController.clear();
}


} } // end of namespace sd::sidebar
