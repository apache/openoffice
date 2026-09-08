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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sdext.hxx"

#include "pdfbackend.hxx"

#include "osl/file.hxx"
#include "osl/diagnose.h"
#include "rtl/bootstrap.hxx"
#include "rtl/ustrbuf.hxx"

#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/deployment/XPackageInformationProvider.hpp"

#include "jvmfwk/framework.h"

#include <boost/preprocessor/stringize.hpp>

#ifndef PDFI_IMPL_IDENTIFIER
# error define implementation name for pdfi extension, please!
#endif

using namespace com::sun::star;

namespace pdfi
{

namespace
{

#ifdef WNT
# define EXE_SUFFIX ".exe"
#else
# define EXE_SUFFIX ""
#endif

rtl::OUString toFileURL( const rtl::OUString& rPathOrURL );

rtl::OUString stripTrailingSlashes( rtl::OUString aURL )
{
    while( aURL.getLength() && aURL[aURL.getLength()-1] == sal_Unicode('/') )
        aURL = aURL.copy( 0, aURL.getLength()-1 );
    return aURL;
}

/** Directory the helper programs live in, as a file URL without a trailing
    slash.

    Two layouts, both real.  Deployed as an .oxt the helpers sit inside the
    extension, and only the deployment service knows where that was unpacked.
    Built and staged directly into the install -- which is what the Bazel build
    produces, and what anyone testing a change actually runs -- there is no
    package at all and they sit in program/ beside soffice.  Ask the deployment
    service first and fall back to the brand's program directory; empty means
    neither answered, and the caller lets PATH decide.
 */
rtl::OUString getHelperLocation( const uno::Reference< uno::XComponentContext >& xContext )
{
    // An explicit override wins: a test fixture bootstraps a bare UNO context
    // with neither a deployment service nor a brand layout, so this is the only
    // way it can say where the helpers were built.
    rtl::OUString aOverride;
    rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PDFI_HELPER_DIR" ) ),
                         aOverride );
    if( aOverride.getLength() )
        return stripTrailingSlashes( toFileURL( aOverride ) );

    rtl::OUString aLocation;
    try
    {
        uno::Reference< deployment::XPackageInformationProvider > xProvider(
            xContext->getValueByName(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                    "/singletons/com.sun.star.deployment.PackageInformationProvider" ) ) ),
            uno::UNO_QUERY );
        if( xProvider.is() )
            aLocation = xProvider->getPackageLocation(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                    BOOST_PP_STRINGIZE(PDFI_IMPL_IDENTIFIER) ) ) );
    }
    catch( uno::Exception& )
    {
        // no deployment context -- fall back to PATH
    }

    if( aLocation.getLength() )
        return stripTrailingSlashes( aLocation );

    rtl::OUString aBrandDir;
    rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "BRAND_BASE_DIR" ) ),
                         aBrandDir );
    if( aBrandDir.getLength() )
    {
        rtl::OUStringBuffer aBuf( aBrandDir.getLength() + 8 );
        aBuf.append( stripTrailingSlashes( aBrandDir ) );
        aBuf.appendAscii( "/program" );
        return aBuf.makeStringAndClear();
    }

    // URE-only bootstrap: no brand layout, but the libraries' own directory is
    // still where a helper staged beside them would be.
    rtl::OUString aUreLibDir;
    rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "URE_LIB_DIR" ) ),
                         aUreLibDir );
    return stripTrailingSlashes( aUreLibDir );
}

bool exists( const rtl::OUString& rFileURL )
{
    if( !rFileURL.getLength() )
        return false;

    osl::DirectoryItem aItem;
    return osl::DirectoryItem::get( rFileURL, aItem ) == osl::FileBase::E_None;
}

/** Join a directory URL and a file name, returning the result only if the file
    is really there.  An empty directory means "not deployed", in which case
    there is nothing to probe and the caller falls back to PATH.
 */
rtl::OUString probe( const rtl::OUString& rDirURL, const sal_Char* pName )
{
    if( !rDirURL.getLength() )
        return rtl::OUString();

    rtl::OUStringBuffer aBuf( rDirURL.getLength() + 32 );
    aBuf.append( rDirURL );
    aBuf.appendAscii( "/" );
    aBuf.appendAscii( pName );

    const rtl::OUString aURL( aBuf.makeStringAndClear() );
    return exists( aURL ) ? aURL : rtl::OUString();
}

/** Convert whatever a user put in a bootstrap variable -- a file URL or a
    native path -- into a file URL.
 */
rtl::OUString toFileURL( const rtl::OUString& rPathOrURL )
{
    if( !rPathOrURL.getLength() )
        return rtl::OUString();

    if( rPathOrURL.compareToAscii( RTL_CONSTASCII_STRINGPARAM( "file://" ) ) == 0 )
        return rPathOrURL;

    rtl::OUString aURL;
    if( osl::FileBase::getFileURLFromSystemPath( rPathOrURL, aURL ) == osl::FileBase::E_None )
        return aURL;

    return rtl::OUString();
}

/** Locate a java launcher.

    Asked in order of how confident we are that the answer is the one the user
    wants: an explicit override, then the JRE the office itself is configured
    with, then the ambient JAVA_HOME, then a bare name for PATH to resolve.
    The office's own selection outranks JAVA_HOME deliberately -- a machine
    commonly has several JREs, and only one of them is the one the user
    pointed the office at in Tools - Options - Java.
 */
rtl::OUString findJavaExecutable()
{
    rtl::OUString aOverride;
    rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PDFI_JAVA" ) ),
                         aOverride );
    if( aOverride.getLength() )
    {
        const rtl::OUString aURL( toFileURL( aOverride ) );
        if( exists( aURL ) )
            return aURL;
        OSL_TRACE( "pdfi: PDFI_JAVA set but no such file, ignoring" );
    }

    // the JRE the office is configured to use
    JavaInfo* pInfo = NULL;
    if( jfw_getSelectedJRE( &pInfo ) == JFW_E_NONE && pInfo != NULL )
    {
        const rtl::OUString aLocation( stripTrailingSlashes( pInfo->sLocation ) );
        jfw_freeJavaInfo( pInfo );

        const rtl::OUString aJava( probe( aLocation, "bin/java" EXE_SUFFIX ) );
        if( aJava.getLength() )
            return aJava;
    }

    rtl::OUString aJavaHome;
    if( rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "JAVA_HOME" ) ),
                             aJavaHome ) && aJavaHome.getLength() )
    {
        const rtl::OUString aJava( probe( toFileURL( aJavaHome ), "bin/java" EXE_SUFFIX ) );
        if( aJava.getLength() )
            return aJava;
    }

    // let osl_executeProcess's SEARCHPATH find it
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "java" EXE_SUFFIX ) );
}

/** The PDFBox helper.  Apache-licensed, so it needs no licence firewall, but
    it keeps the separate process anyway for containment (see PROTOCOL.md).
 */
PdfBackend makePdfBoxBackend( const rtl::OUString& rPkgDir )
{
    // The jar is what has to be present; java itself may legitimately come
    // from PATH, so it cannot be the availability test.
    //
    // Two layouts again: at the root of an unpacked .oxt, or -- when staged
    // straight into the install -- in program/classes, which is where every
    // other jar in the product lives.
    rtl::OUString aJar( probe( rPkgDir, "pdfimport-pdfbox.jar" ) );
    if( !aJar.getLength() )
        aJar = probe( rPkgDir, "classes/pdfimport-pdfbox.jar" );
    if( !aJar.getLength() )
        return PdfBackend();

    rtl::OUString aJarPath;
    if( osl::FileBase::getSystemPathFromFileURL( aJar, aJarPath ) != osl::FileBase::E_None )
        return PdfBackend();

    PdfBackend aRes;
    aRes.aId = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "pdfbox" ) );
    aRes.eBinaryChannel = BINARY_INLINE;
    aRes.aProgram = findJavaExecutable();
    aRes.aPrefixArgs.push_back( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "-jar" ) ) );
    aRes.aPrefixArgs.push_back( aJarPath );
    return aRes;
}

/** Reserved for the PDFium helper.  Nothing else in the tree changes when it
    lands: build the executable, drop it beside the jar, and it is preferred
    automatically.
 */
PdfBackend makePdfiumBackend( const rtl::OUString& rPkgDir )
{
    PdfBackend aRes;
    aRes.aId = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "pdfium" ) );
    aRes.eBinaryChannel = BINARY_INLINE;
    aRes.aProgram = probe( rPkgDir, "pdfiumimport" EXE_SUFFIX );
    return aRes;
}

/** The original poppler helper.  Built only where poppler is available and
    its GPL licence is acceptable, so it is normally absent.
 */
PdfBackend makeXpdfBackend( const rtl::OUString& rPkgDir )
{
    PdfBackend aRes;
    aRes.aId = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "xpdf" ) );
    aRes.eBinaryChannel = BINARY_ON_STDERR;
    aRes.aProgram = probe( rPkgDir, "xpdfimport" EXE_SUFFIX );

    if( !aRes.aProgram.getLength() && !rPkgDir.getLength() )
    {
        // undeployed build tree: let PATH answer, as this helper always did
        aRes.aProgram = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "xpdfimport" EXE_SUFFIX ) );
    }
    return aRes;
}

const int PDFI_BACKEND_COUNT = 3;

} // namespace

PdfBackend selectPdfBackend( const uno::Reference< uno::XComponentContext >& xContext )
{
    const rtl::OUString aPkgDir( getHelperLocation( xContext ) );

    rtl::OUString aWanted;
    rtl::Bootstrap::get( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PDFI_BACKEND" ) ),
                         aWanted );

    // Preference order.  PDFBox first: it is the one that is licensed to ship,
    // hence the one a stock build actually has.
    PdfBackend aCandidates[PDFI_BACKEND_COUNT];
    aCandidates[0] = makePdfBoxBackend( aPkgDir );
    aCandidates[1] = makePdfiumBackend( aPkgDir );
    aCandidates[2] = makeXpdfBackend( aPkgDir );

    if( aWanted.getLength() )
    {
        // An explicit request must not silently fall through to a different
        // engine -- that would make a comparison run attribute one backend's
        // output to the other.
        for( int i = 0; i < PDFI_BACKEND_COUNT; ++i )
        {
            if( !aCandidates[i].aId.equalsIgnoreAsciiCase( aWanted ) )
                continue;

            if( !aCandidates[i].isAvailable() )
                OSL_TRACE( "pdfi: PDFI_BACKEND names an unavailable backend" );

            return aCandidates[i].isAvailable() ? aCandidates[i] : PdfBackend();
        }

        OSL_TRACE( "pdfi: PDFI_BACKEND names an unknown backend" );
        return PdfBackend();
    }

    for( int i = 0; i < PDFI_BACKEND_COUNT; ++i )
    {
        if( aCandidates[i].isAvailable() )
            return aCandidates[i];
    }

    return PdfBackend();
}

} // namespace pdfi
