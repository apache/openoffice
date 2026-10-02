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

#include "jvmaccess/classpath.hxx"

#include <vector>

#include "com/sun/star/lang/IllegalArgumentException.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uri/UriReferenceFactory.hpp"
#include "com/sun/star/uri/XUriReference.hpp"
#include "com/sun/star/uri/XUriReferenceFactory.hpp"
#include "com/sun/star/uri/XVndSunStarExpandUrlReference.hpp"
#include "com/sun/star/util/XMacroExpander.hpp"
#include "osl/diagnose.h"
#include "rtl/uri.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#if defined SOLAR_JAVA
#include "jni.h"
#endif

namespace {

namespace css = ::com::sun::star;

#if defined SOLAR_JAVA
// Whether a class path entry is a file: URL naming a path on this machine.
//
// On Windows, Java opens a file: URL as a UNC path, i.e. a file on another
// machine, in two cases:
//
// - if its authority names a host, e.g. file://host/share/a.jar;
// - if its path (or opaque part), once percent-decoded and with backslashes
//   read as slashes, starts with two slashes, e.g. file:////host/share/a.jar,
//   file:/%5C%5Chost/share/a.jar or file:%5C%5Chost/share/a.jar.
//
// com.sun.star.comp.sdbc.Tools enforces the same check on the Java side; keep
// the two in sync.
bool isLocalFileUrl(
    css::uno::Reference< css::uri::XUriReferenceFactory > const & factory,
    ::rtl::OUString const & url)
{
    css::uno::Reference< css::uri::XUriReference > const uriRef(
        factory->parse(url));
    if (!uriRef.is()
        || !uriRef->getScheme().equalsIgnoreAsciiCaseAsciiL(
            RTL_CONSTASCII_STRINGPARAM("file")))
    {
        return false;
    }
    // The authority, if any, must be empty or localhost.
    if (uriRef->hasAuthority()) {
        ::rtl::OUString const authority(uriRef->getAuthority());
        if (authority.getLength() != 0
            && !authority.equalsIgnoreAsciiCaseAsciiL(
                RTL_CONSTASCII_STRINGPARAM("localhost")))
        {
            return false;
        }
    }
    // The decoded path must start with exactly one slash, which also rules out
    // the opaque form, and must contain no backslash at all.
    ::rtl::OUString const path(
        ::rtl::Uri::decode(
            uriRef->getPath(), rtl_UriDecodeWithCharset,
            RTL_TEXTENCODING_UTF8));
    return path.getLength() >= 2 && path[0] == '/' && path[1] != '/'
        && path.indexOf('\\') == -1;
}
#endif

}

void * ::jvmaccess::ClassPath::doTranslateToUrls(
    css::uno::Reference< css::uno::XComponentContext > const & context,
    void * environment, ::rtl::OUString const & classPath)
{
    OSL_ASSERT(context.is() && environment != 0);
#if defined SOLAR_JAVA
    ::JNIEnv * const env = static_cast< ::JNIEnv * >(environment);
    jclass classUrl(env->FindClass("java/net/URL"));
    if (classUrl == 0) {
        return 0;
    }
    jmethodID ctorUrl(
        env->GetMethodID(classUrl, "<init>", "(Ljava/lang/String;)V"));
    if (ctorUrl == 0) {
        return 0;
    }
    css::uno::Reference< css::uri::XUriReferenceFactory > const factory(
        css::uri::UriReferenceFactory::create(context));
    ::std::vector< jobject > urls;
    for (::sal_Int32 i = 0; i != -1;) {
        ::rtl::OUString url(classPath.getToken(0, ' ', i));
        if (url.getLength() != 0) {
            css::uno::Reference< css::uri::XVndSunStarExpandUrlReference >
                expUrl(factory->parse(url), css::uno::UNO_QUERY);
            if (expUrl.is()) {
                css::uno::Reference< css::util::XMacroExpander > expander(
                    context->getValueByName(
                        ::rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "/singletons/"
                                "com.sun.star.util.theMacroExpander"))),
                    css::uno::UNO_QUERY_THROW);
                try {
                    url = expUrl->expand(expander);
                } catch (css::lang::IllegalArgumentException & e) {
                    throw css::uno::RuntimeException(
                        (::rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "com.sun.star.lang.IllegalArgumentException: "))
                         + e.Message),
                        css::uno::Reference< css::uno::XInterface >());
                }
            }
            // Add only local entries; a non-local one is logged and skipped.
            if (!isLocalFileUrl(factory, url))
            {
                OSL_TRACE(
                    "jvmaccess::ClassPath: skipping non-local class path"
                    " entry: %s",
                    ::rtl::OUStringToOString(
                        url, RTL_TEXTENCODING_ASCII_US).getStr());
                continue;
            }
            jvalue arg;
            arg.l = env->NewString(
                static_cast< jchar const * >(url.getStr()),
                static_cast< jsize >(url.getLength()));
            if (arg.l == 0) {
                return 0;
            }
            jobject o(env->NewObjectA(classUrl, ctorUrl, &arg));
            if (o == 0) {
                return 0;
            }
            urls.push_back(o);
        }
    }
    jobjectArray result = env->NewObjectArray(
        static_cast< jsize >(urls.size()), classUrl, 0);
        // static_cast is ok, as each element of urls occupied at least one
        // character of the ::rtl::OUString classPath
    if (result == 0) {
        return 0;
    }
    jsize idx = 0;
    for (std::vector< jobject >::iterator i(urls.begin()); i != urls.end(); ++i)
    {
        env->SetObjectArrayElement(result, idx++, *i);
    }
    return result;
#else
    return 0;
#endif
}

void * ::jvmaccess::ClassPath::doLoadClass(
    css::uno::Reference< css::uno::XComponentContext > const & context,
    void * environment, ::rtl::OUString const & classPath,
    ::rtl::OUString const & name)
{
    OSL_ASSERT(context.is() && environment != 0);
#if defined SOLAR_JAVA
    ::JNIEnv * const env = static_cast< ::JNIEnv * >(environment);
    jclass classLoader(env->FindClass("java/net/URLClassLoader"));
    if (classLoader == 0) {
        return 0;
    }
    jmethodID ctorLoader(
        env->GetMethodID(classLoader, "<init>", "([Ljava/net/URL;)V"));
    if (ctorLoader == 0) {
        return 0;
    }
    jvalue arg;
    arg.l = translateToUrls(context, env, classPath);
    if (arg.l == 0) {
        return 0;
    }
    jobject cl = env->NewObjectA(classLoader, ctorLoader, &arg);
    if (cl == 0) {
        return 0;
    }
    jmethodID methLoadClass(
        env->GetMethodID(
            classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;"));
    if (methLoadClass == 0) {
        return 0;
    }
    arg.l = env->NewString(
        static_cast< jchar const * >(name.getStr()),
        static_cast< jsize >(name.getLength()));
    if (arg.l == 0) {
        return 0;
    }
    return env->CallObjectMethodA(cl, methLoadClass, &arg);
#else
    return 0;
#endif
}
