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



#include "imestatuswindow.hxx"

#include "app.hxx"
#include "sfxsids.hrc"

#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/util/XChangesBatch.hpp"
#include "osl/diagnose.h"
#include "rtl/ustring.h"
#include "sal/types.h"
#include "vos/mutex.hxx"

namespace binfilter {

namespace css = ::com::sun::star;

using sfx2::appl::ImeStatusWindow;

ImeStatusWindow::ImeStatusWindow(
    SfxApplication & rApplication,
    css::uno::Reference< css::lang::XMultiServiceFactory > const &
        rServiceFactory):
    m_rApplication(rApplication),
    m_xServiceFactory(rServiceFactory),
    m_bDisposed(false)
{}

void ImeStatusWindow::init()
{
    if (Application::CanToggleImeStatusWindow())
        try
        {
            sal_Bool bShow;
            if (getConfig()->getPropertyValue(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "ShowStatusWindow")))
                >>= bShow)
                Application::ShowImeStatusWindow(bShow);
        }
        catch (css::uno::Exception &)
        {
            OSL_ENSURE(false, "com.sun.star.uno.Exception");
            // Degrade gracefully and use the VCL-supplied default if no
            // configuration is available.
        }
}

ImeStatusWindow::~ImeStatusWindow()
{
    if (m_xConfig.is())
        // We should never get here, but just in case...
        try
        {
            m_xConfig->removePropertyChangeListener(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ShowStatusWindow")),
                this);
        }
        catch (css::uno::Exception &)
        {
            OSL_ENSURE(false, "com.sun.star.uno.RuntimeException");
        }
}

void SAL_CALL ImeStatusWindow::disposing(css::lang::EventObject const & rSource)
    throw (css::uno::RuntimeException)
{
    osl::MutexGuard aGuard(m_aMutex);
    m_xConfig = 0;
    m_bDisposed = true;
}

void SAL_CALL
ImeStatusWindow::propertyChange(css::beans::PropertyChangeEvent const & rEvent)
    throw (css::uno::RuntimeException)
{
}

css::uno::Reference< css::beans::XPropertySet > ImeStatusWindow::getConfig()
{
    css::uno::Reference< css::beans::XPropertySet > xConfig;
    bool bAdd = false;
    {
        osl::MutexGuard aGuard(m_aMutex);
        if (!m_xConfig.is())
        {
            if (m_bDisposed)
                throw css::lang::DisposedException();
            if (!m_xServiceFactory.is())
                throw css::uno::RuntimeException(
                    ::rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "null ::legacy_binfilters::getLegacyProcessServiceFactory()")),
                    0);
            css::uno::Reference< css::lang::XMultiServiceFactory > xProvider(
                m_xServiceFactory->createInstance(
                    ::rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                          "com.sun.star.configuration.ConfigurationProvider"))),
                css::uno::UNO_QUERY);
            if (!xProvider.is())
                throw css::uno::RuntimeException(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "null com.sun.star.configuration."
                                      "ConfigurationProvider")),
                    0);
            css::beans::PropertyValue aArg(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath")), -1,
                css::uno::makeAny(
                    ::rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "/org.openoffice.Office.Common/I18N/InputMethod"))),
                css::beans::PropertyState_DIRECT_VALUE);
            css::uno::Sequence< css::uno::Any > aArgs(1);
            aArgs[0] <<= aArg;
            m_xConfig
                = css::uno::Reference< css::beans::XPropertySet >(
                    xProvider->createInstanceWithArguments(
                        ::rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                       "com.sun.star.configuration.ConfigurationUpdateAccess")),
                        aArgs),
                    css::uno::UNO_QUERY);
            if (!m_xConfig.is())
                throw css::uno::RuntimeException(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "null com.sun.star.configuration."
                                      "ConfigurationUpdateAccess")),
                    0);
            bAdd = true;
        }
        xConfig = m_xConfig;
    }
    if (bAdd)
        // Exceptions here could be handled individually, to support graceful
        // degradation (no update notification mechanism in this case---but also
        // no dispose notifications):
        xConfig->addPropertyChangeListener(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ShowStatusWindow")),
            this);
    return xConfig;
}

}
