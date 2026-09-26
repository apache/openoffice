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
#include "com/sun/star/uri/XVndSunStarExpandUrlReference.hpp"
#include "com/sun/star/util/XMacroExpander.hpp"
#include "osl/diagnose.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#if defined SOLAR_JAVA
#include "jni.h"
#endif

namespace {

namespace css = ::com::sun::star;

#if defined SOLAR_JAVA
int hexDigitValue(sal_Unicode c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

// Undoes one level of %HH escapes, as the JDK's file: handler does before it
// opens a path.  Only the ASCII separators matter to the caller, so each escape
// simply becomes the code unit of its byte value; a malformed escape is kept.
::rtl::OUString percentDecoded(::rtl::OUString const & s)
{
    sal_Int32 const n = s.getLength();
    ::rtl::OUStringBuffer buf(n);
    for (sal_Int32 i = 0; i != n; ++i) {
        sal_Unicode c = s[i];
        if (c == '%' && n - i > 2) {
            int const hi = hexDigitValue(s[i + 1]);
            int const lo = hexDigitValue(s[i + 2]);
            if (hi != -1 && lo != -1) {
                c = static_cast< sal_Unicode >(hi * 16 + lo);
                i += 2;
            }
        }
        buf.append(c);
    }
    return buf.makeStringAndClear();
}

// Whether the part of a file: URL after the scheme names a path on this
// machine: the authority, if any, must be empty or localhost, and the path must
// not name another machine by itself.  The path is judged decoded, because the
// JDK's file: handler percent-decodes it and, on Windows, turns slashes into
// backslashes before opening it -- a leading escaped slash or backslash would
// otherwise reach the file system as a reference to a share.
bool isLocalFileLocation(::rtl::OUString const & afterScheme)
{
    ::rtl::OUString rest(afterScheme);
    if (rest.indexOf('\\') != -1) {
        return false;
    }
    if (rest.matchAsciiL(RTL_CONSTASCII_STRINGPARAM("//"))) {
        sal_Int32 const end = rest.indexOf('/', 2);
        ::rtl::OUString const authority(
            end == -1 ? rest.copy(2) : rest.copy(2, end - 2));
        if (authority.getLength() != 0
            && !authority.equalsIgnoreAsciiCaseAsciiL(
                RTL_CONSTASCII_STRINGPARAM("localhost")))
        {
            return false;
        }
        rest = end == -1 ? ::rtl::OUString() : rest.copy(end);
    }
    rest = percentDecoded(rest);
    return rest.getLength() >= 2 && rest[0] == '/' && rest[1] != '/'
        && rest.indexOf('\\') == -1;
}

// URL schemes that resolve to the local file system or the running JVM image,
// optionally wrapped in a jar: URL; a file: URL must in addition name a path on
// this machine.
//
// com.sun.star.comp.sdbc.Tools enforces the same allow-list on the Java side;
// keep the two in sync.
bool isLocalClassPathUrl(::rtl::OUString const & url)
{
    ::rtl::OUString rest(url);
    if (rest.matchIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("jar:"))) {
        rest = rest.copy(RTL_CONSTASCII_LENGTH("jar:"));
    }
    if (rest.matchIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("file:"))) {
        return isLocalFileLocation(rest.copy(RTL_CONSTASCII_LENGTH("file:")));
    }
    return rest.matchIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("jrt:"))
        || rest.matchIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("jmod:"));
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
    ::std::vector< jobject > urls;
    for (::sal_Int32 i = 0; i != -1;) {
        ::rtl::OUString url(classPath.getToken(0, ' ', i));
        if (url.getLength() != 0) {
            css::uno::Reference< css::uri::XVndSunStarExpandUrlReference >
                expUrl(
                    css::uri::UriReferenceFactory::create(context)->parse(url),
                    css::uno::UNO_QUERY);
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
            if (!isLocalClassPathUrl(url))
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
