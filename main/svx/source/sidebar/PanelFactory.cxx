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

#include <toolkit/helper/vclunohelper.hxx>
#include <vcl/window.hxx>
#include <sfx2/sfxbasecontroller.hxx>
#include <rtl/ref.hxx>


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
    throw(uno::Exception)
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
    Reference<awt::XWindow> xParentWindow;
    Reference<frame::XFrame> xFrame;
    SfxBindings* pBindings = NULL;

    for (sal_Int32 nIndex(0),nCount(rArguments.getLength()); nIndex<nCount; ++nIndex)
    {
        const beans::PropertyValue& rValue (rArguments[nIndex]);
        if (rValue.Name.equalsAscii("ParentWindow"))
        {
            xParentWindow = Reference<awt::XWindow>(rValue.Value, UNO_QUERY);
        }
        else if (rValue.Name.equalsAscii("Frame"))
        {
            xFrame = Reference<frame::XFrame>(rValue.Value, UNO_QUERY);
        }
        else if (rValue.Name.equalsAscii("SfxBindings"))
        {
            sal_uInt64 nValue;
            if (rValue.Value >>= nValue)
                pBindings = reinterpret_cast<SfxBindings*>(nValue);
        }
    }

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

    if (rsResourceURL.endsWithAsciiL("/TextPropertyPanel", strlen("/TextPropertyPanel")))
        xElement = TextPropertyPanel::Create(rsResourceURL, pParentWindow, xFrame, pBindings);

    return xElement;
}



} } // end of namespace svx::sidebar
