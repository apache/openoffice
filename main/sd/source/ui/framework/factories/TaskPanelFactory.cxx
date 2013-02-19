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
#include "TaskPanelFactory.hxx"
#include "SidebarViewShell.hxx"
#include "SidebarPanelId.hxx"
#include "DrawController.hxx"
#include "framework/FrameworkHelper.hxx"
#include "framework/TaskPanelResource.hxx"
#include "Window.hxx"
#include <cppuhelper/compbase1.hxx>
#include <tools/diagnose_ex.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::drawing::framework;

using ::rtl::OUString;
using ::sd::framework::FrameworkHelper;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sd { namespace framework {

Reference<XInterface> SAL_CALL TaskPanelFactory_createInstance (
    const Reference<XComponentContext>& rxContext)
{
    return Reference<XInterface>(static_cast<XWeak*>(new TaskPanelFactory(rxContext)));
}




::rtl::OUString TaskPanelFactory_getImplementationName (void) throw(RuntimeException)
{
    return ::rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Draw.framework.TaskPanelFactory"));
}




Sequence<rtl::OUString> SAL_CALL TaskPanelFactory_getSupportedServiceNames (void)
    throw (RuntimeException)
{
	static const OUString sServiceName(
        OUString::createFromAscii("com.sun.star.drawing.framework.TaskPanelFactory"));
	return Sequence<rtl::OUString>(&sServiceName, 1);
}




//===== TaskPanelFactory =======================================================

TaskPanelFactory::TaskPanelFactory (
    const ::com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext>& rxContext)
    : TaskPanelFactoryInterfaceBase(m_aMutex),
      mpViewShellBase(NULL)
{
    (void)rxContext;
}




TaskPanelFactory::~TaskPanelFactory (void)
{
}




void SAL_CALL TaskPanelFactory::disposing (void)
{
}



    
//===== XInitialization =======================================================
    
void SAL_CALL TaskPanelFactory::initialize(
    const ::com::sun::star::uno::Sequence<com::sun::star::uno::Any>& aArguments)
    throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    if (aArguments.getLength() > 0)
    {
        try
        {
            // Get the XController from the first argument.
            Reference<frame::XController> xController (aArguments[0], UNO_QUERY_THROW);

            // Tunnel through the controller to obtain access to the ViewShellBase.
            try
            {
                Reference<lang::XUnoTunnel> xTunnel (xController, UNO_QUERY_THROW);
                DrawController* pController
                    = reinterpret_cast<DrawController*>(
                        sal::static_int_cast<sal_uIntPtr>(
                            xTunnel->getSomething(DrawController::getUnoTunnelId())));
                if (pController != NULL)
                    mpViewShellBase = pController->GetViewShellBase();

            }
            catch(RuntimeException&)
            {}


            Reference<XControllerManager> xCM (xController, UNO_QUERY_THROW);
            Reference<XConfigurationController> xCC (
                xCM->getConfigurationController(), UNO_QUERY_THROW);
            xCC->addResourceFactory(FrameworkHelper::msAllMasterPagesTaskPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msRecentMasterPagesTaskPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msUsedMasterPagesTaskPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msLayoutTaskPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msTableDesignPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msCustomAnimationTaskPanelURL, this);
            xCC->addResourceFactory(FrameworkHelper::msSlideTransitionTaskPanelURL, this);
        }
        catch (RuntimeException&)
        {
        }
    }
}


//===== XResourceController ===================================================

namespace
{
    void lcl_collectResourceURLs( const Reference< XResourceId >& i_rResourceId, ::std::vector< ::rtl::OUString >& o_rResourceURLs )
    {
        ENSURE_OR_RETURN_VOID( i_rResourceId.is(), "illegal resource ID" );
        o_rResourceURLs.resize(0);

        Reference< XResourceId > xResourceId( i_rResourceId );
        ::rtl::OUString sResourceURL = xResourceId->getResourceURL();
        while ( sResourceURL.getLength() > 0 )
        {
            o_rResourceURLs.push_back( sResourceURL );
            xResourceId = xResourceId->getAnchor();
            sResourceURL = xResourceId->getResourceURL();
        }
    }
}

Reference<XResource> SAL_CALL TaskPanelFactory::createResource (
    const Reference<XResourceId>& rxResourceId)
    throw (RuntimeException, IllegalArgumentException, WrappedTargetException)
{
    Reference<XResource> xResource;
    
    if ( ! rxResourceId.is())
        return NULL;

    OUString sResourceURL (rxResourceId->getResourceURL());
    
    if (sResourceURL.match(FrameworkHelper::msTaskPanelURLPrefix))
    {
        sidebar::PanelId ePanelId (sidebar::PID_UNKNOWN);

        if (mpViewShellBase == NULL)
            return NULL;

        sidebar::SidebarViewShell* pSidebarViewShell
            = dynamic_cast<sidebar::SidebarViewShell*>(
                FrameworkHelper::Instance(*mpViewShellBase)
                    ->GetViewShell(FrameworkHelper::msSidebarPaneURL).get());
        if (pSidebarViewShell == NULL)
            return NULL;

        if (sResourceURL.equals(FrameworkHelper::msAllMasterPagesTaskPanelURL))
        {
            ePanelId = sidebar::PID_MASTER_PAGES_ALL;
        }
        else if (sResourceURL.equals(FrameworkHelper::msRecentMasterPagesTaskPanelURL))
        {
            ePanelId = sidebar::PID_MASTER_PAGES_RECENT;
        }
        else if (sResourceURL.equals(FrameworkHelper::msUsedMasterPagesTaskPanelURL))
        {
            ePanelId = sidebar::PID_MASTER_PAGES_USED;
        }
        else if (sResourceURL.equals(FrameworkHelper::msLayoutTaskPanelURL))
        {
            ePanelId = sidebar::PID_LAYOUT;
        }
        else if (sResourceURL.equals(FrameworkHelper::msCustomAnimationTaskPanelURL))
        {
            ePanelId = sidebar::PID_CUSTOM_ANIMATION;
        }
        else if (sResourceURL.equals(FrameworkHelper::msSlideTransitionTaskPanelURL))
        {
            ePanelId = sidebar::PID_SLIDE_TRANSITION;
        }
        else if (sResourceURL.equals(FrameworkHelper::msTableDesignPanelURL))
        {
            ePanelId = sidebar::PID_TABLE_DESIGN;
        }
        else
            return NULL;
        
        xResource = new TaskPanelResource(
            *pSidebarViewShell,
            ePanelId,
            rxResourceId);
    }

    return xResource;
}




void SAL_CALL TaskPanelFactory::releaseResource (
    const Reference<XResource>& rxResource)
    throw (RuntimeException)
{
    Reference<XComponent> xComponent (rxResource, UNO_QUERY);
    if (xComponent.is())
        xComponent->dispose();
}


} } // end of namespace sd::framework
