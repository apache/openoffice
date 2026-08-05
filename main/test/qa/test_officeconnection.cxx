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

// MIGRATION-AUTHORED, not an upstream test.
//
// test::OfficeConnection (libtest) is the "running office" test fixture: it
// launches a real soffice with -accept=pipe,name=...;urp and resolves a remote
// component context over URP.  Upstream's ONLY C++ consumer of it is
// xmlsecurity/qa/certext, which cannot be built here at all — it includes
// <neon/ne_ssl.h> and calls ne_ssl_cert_read(), but AOO replaced neon with curl
// (main/ucb/source/ucp/webdav is curl-based, configure.in has no neon option and
// no neon source is in the tree; only a stale NEON3RDLIB lingers in
// solenv/inc/libs.mk).
//
// So without this file the fixture would be unexercised infrastructure — the
// kind that rots silently and is discovered to be broken by whoever first needs
// it.  This exercises the whole round trip end to end: launch, URP resolve,
// remote service creation, clean termination.  Same precedent as
// //build/testsupport:sal_process_init.cxx.

#include "sal/config.h"

#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "rtl/bootstrap.hxx"
#include "rtl/string.hxx"
#include "rtl/ustring.hxx"

#include <preextstl.h>
#include "gtest/gtest.h"
#include <postextstl.h>

#include "test/officeconnection.hxx"

namespace {

namespace css = com::sun::star;

// One office for the whole test case, not one per TEST_F: setUp() launches a
// process and tearDown() terminates it, so a per-test fixture would boot and
// shut down a full soffice for every assertion.
class OfficeConnectionTest: public ::testing::Test {
protected:
    static test::OfficeConnection * connection_;

    // OfficeConnection::setUp() can throw a UNO exception (toAbsoluteFileUrl
    // raises RuntimeException; the resolve loop only swallows
    // NoConnectException).  UNO exceptions do NOT derive from std::exception,
    // so gtest reports them as a bare "Unknown C++ exception" with no message —
    // useless for diagnosis.  Catch and surface the text, plus the two
    // arguments the fixture ran with, since a wrong arg-soffice/arg-user is by
    // far the most likely cause.
    static void SetUpTestCase() {
        connection_ = new test::OfficeConnection;
        try {
            connection_->setUp();
        } catch (const css::uno::Exception & e) {
            FAIL() << "OfficeConnection::setUp() threw "
                   << rtl::OUStringToOString(
                          e.Message, RTL_TEXTENCODING_UTF8).getStr()
                   << " [arg-soffice=" << reportArgument("soffice")
                   << "] [arg-user=" << reportArgument("user") << "]";
        }
    }

    static rtl::OString reportArgument(char const * name) {
        rtl::OUString value;
        if (!rtl::Bootstrap::get(
                rtl::OUString::createFromAscii("arg-") +
                    rtl::OUString::createFromAscii(name),
                value))
        {
            return rtl::OString(RTL_CONSTASCII_STRINGPARAM("<unset>"));
        }
        return rtl::OUStringToOString(value, RTL_TEXTENCODING_UTF8);
    }

    static void TearDownTestCase() {
        connection_->tearDown();
        delete connection_;
        connection_ = 0;
    }
};

test::OfficeConnection * OfficeConnectionTest::connection_ = 0;

// The URP resolve succeeded and handed back a usable remote context.
TEST_F(OfficeConnectionTest, resolvesComponentContext) {
    ASSERT_TRUE(connection_ != 0);
    ASSERT_TRUE(connection_->getComponentContext().is());
}

// The context is genuinely the OFFICE's, not a local one: ask it for its
// service manager and instantiate a service that only a running office
// provides.  This is what distinguishes fixture (b) from the in-process
// bootstrap of fixture (a).
TEST_F(OfficeConnectionTest, createsRemoteService) {
    css::uno::Reference< css::uno::XComponentContext > context(
        connection_->getComponentContext());
    ASSERT_TRUE(context.is());

    css::uno::Reference< css::lang::XMultiComponentFactory > manager(
        context->getServiceManager());
    ASSERT_TRUE(manager.is());

    css::uno::Reference< css::uno::XInterface > desktop(
        manager->createInstanceWithContext(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")),
            context));
    ASSERT_TRUE(desktop.is());
}

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
