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



#include "sal/config.h"

#include "com/sun/star/bridge/UnoUrlResolver.hpp"
#include "com/sun/star/bridge/XUnoUrlResolver.hpp"
#include "com/sun/star/connection/NoConnectException.hpp"
#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "cppuhelper/bootstrap.hxx"
#include <preextstl.h>
#include "gtest/gtest.h"
#include <postextstl.h>
#include "osl/process.h"
#include "osl/time.h"
#include "sal/types.h"
#include "test/officeconnection.hxx"
#include "test/toabsolutefileurl.hxx"
#include "test/uniquepipename.hxx"

#include "getargument.hxx"

namespace {

namespace css = com::sun::star;

}

namespace test {

OfficeConnection::OfficeConnection(): process_(0) {}

OfficeConnection::~OfficeConnection() {}

void OfficeConnection::setUp() {
    rtl::OUString desc;
    rtl::OUString argSoffice;
    EXPECT_TRUE(
        detail::getArgument(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("soffice")),
            &argSoffice));
    if (argSoffice.matchAsciiL(RTL_CONSTASCII_STRINGPARAM("path:"))) {
        desc = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("pipe,name=")) +
            uniquePipeName(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("oootest")));
        rtl::OUString noquickArg(
            RTL_CONSTASCII_USTRINGPARAM("-quickstart=no"));
        rtl::OUString nofirstArg(
            RTL_CONSTASCII_USTRINGPARAM("-nofirststartwizard"));
        rtl::OUString norestoreArg(RTL_CONSTASCII_USTRINGPARAM("-norestore"));
        rtl::OUString acceptArg(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("-accept=")) + desc +
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(";urp")));
        rtl::OUString argUser;
        EXPECT_TRUE(
            detail::getArgument(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("user")), &argUser));
        rtl::OUString userArg(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM("-env:UserInstallation=")) +
            toAbsoluteFileUrl(argUser));
        rtl::OUString jreArg(
            RTL_CONSTASCII_USTRINGPARAM("-env:UNO_JAVA_JFW_ENV_JREHOME=true"));
        rtl_uString * args[] = {
            noquickArg.pData, nofirstArg.pData, norestoreArg.pData,
            acceptArg.pData, userArg.pData, jreArg.pData };
        rtl_uString ** envs = 0;
        rtl::OUString argEnv;
        if (detail::getArgument(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("env")), &argEnv))
        {
            envs = &argEnv.pData;
        }
        EXPECT_EQ(
            osl_Process_E_None,
            osl_executeProcess(
                toAbsoluteFileUrl(
                    argSoffice.copy(RTL_CONSTASCII_LENGTH("path:"))).pData,
                args, sizeof args / sizeof args[0], 0, 0, 0, envs,
                envs == 0 ? 0 : 1, &process_));
    } else if (argSoffice.matchAsciiL(RTL_CONSTASCII_STRINGPARAM("connect:"))) {
        desc = argSoffice.copy(RTL_CONSTASCII_LENGTH("connect:"));
    } else {
        EXPECT_TRUE(false) <<
            "\"soffice\" argument starts with neither \"path:\" nor"
            " \"connect:\"";
    }
    css::uno::Reference< css::bridge::XUnoUrlResolver > resolver(
        css::bridge::UnoUrlResolver::create(
            cppu::defaultBootstrap_InitialComponentContext()));
    for (;;) {
        try {
            context_ =
                css::uno::Reference< css::uno::XComponentContext >(
                    resolver->resolve(
                        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("uno:")) +
                        desc +
                        rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                ";urp;StarOffice.ComponentContext"))),
                    css::uno::UNO_QUERY_THROW);
            break;
        } catch (css::connection::NoConnectException &) {}
        if (process_ != 0) {
            TimeValue delay = { 1, 0 }; // 1 sec
            EXPECT_EQ(
                osl_Process_E_TimedOut,
                osl_joinProcessWithTimeout(process_, &delay));
        }
    }
}

void OfficeConnection::tearDown() {
    if (process_ != 0) {
        if (context_.is()) {
            css::uno::Reference< css::frame::XDesktop > desktop(
                context_->getServiceManager()->createInstanceWithContext(
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "com.sun.star.frame.Desktop")),
                    context_),
                css::uno::UNO_QUERY_THROW);
            context_.clear();
            try {
                EXPECT_TRUE(desktop->terminate());
                desktop.clear();
            } catch (css::lang::DisposedException &) {}
                // it appears that DisposedExceptions can already happen while
                // receiving the response of the terminate call
        }
        EXPECT_EQ(osl_Process_E_None, osl_joinProcess(process_));
        oslProcessInfo info;
        info.Size = sizeof info;
        EXPECT_EQ(
            osl_Process_E_None,
            osl_getProcessInfo(process_, osl_Process_EXITCODE, &info));
        EXPECT_EQ(oslProcessExitCode(0), info.Code);
        osl_freeProcessHandle(process_);
    }
}

css::uno::Reference< css::uno::XComponentContext >
OfficeConnection::getComponentContext() const {
    return context_;
}

}
