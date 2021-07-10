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
#include "precompiled_desktop.hxx"


#include "dp_misc.h"
#include "dp_version.hxx"
#include "dp_interact.h"
#include "rtl/uri.hxx"
#include "rtl/digest.h"
#include "rtl/random.h"
#include "rtl/bootstrap.hxx"
#include "unotools/bootstrap.hxx"
#include "osl/file.hxx"
#include "osl/pipe.hxx"
#include "osl/security.hxx"
#include "osl/thread.hxx"
#include "osl/mutex.hxx"
#include "com/sun/star/ucb/CommandAbortedException.hpp"
#include "com/sun/star/task/XInteractionHandler.hpp"
#include "com/sun/star/bridge/UnoUrlResolver.hpp"
#include "com/sun/star/bridge/XUnoUrlResolver.hpp"
#include "com/sun/star/deployment/ExtensionManager.hpp"
#include "com/sun/star/task/XRestartManager.hpp"
#include "boost/scoped_array.hpp"
#include "boost/shared_ptr.hpp"
#include <comphelper/processfactory.hxx>

#ifdef WNT
//#include "tools/prewin.h"
#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//#include "tools/postwin.h"
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;
using ::rtl::OString;


#define SOFFICE1 "soffice.exe"
#define SOFFICE2 "soffice.bin"
#define SBASE "sbase.exe"
#define SCALC "scalc.exe"
#define SDRAW "sdraw.exe"
#define SIMPRESS "simpress.exe"
#define SWRITER "swriter.exe"

namespace dp_misc {
namespace {

struct UnoRc : public rtl::StaticWithInit<
    const boost::shared_ptr<rtl::Bootstrap>, UnoRc> {
    const boost::shared_ptr<rtl::Bootstrap> operator () () {
        OUString unorc( RTL_CONSTASCII_USTRINGPARAM(
                            "$OOO_BASE_DIR/program/" SAL_CONFIGFILE("uno")) );
        ::rtl::Bootstrap::expandMacros( unorc );
        ::boost::shared_ptr< ::rtl::Bootstrap > ret(
            new ::rtl::Bootstrap( unorc ) );
        OSL_ASSERT( ret->getHandle() != 0 );
        return ret;
    }
};

struct OfficePipeId : public rtl::StaticWithInit<const OUString, OfficePipeId> {
    const OUString operator () ();
};

const OUString OfficePipeId::operator () ()
{
    OUString userPath;
	::utl::Bootstrap::PathStatus aLocateResult =
	::utl::Bootstrap::locateUserInstallation( userPath );
	if (!(aLocateResult == ::utl::Bootstrap::PATH_EXISTS ||
		aLocateResult == ::utl::Bootstrap::PATH_VALID))
	{
		throw Exception(OUSTR("Extension Manager: Could not obtain path for UserInstallation."), 0);
	}

    rtlDigest digest = rtl_digest_create( rtl_Digest_AlgorithmMD5 );
    if (digest == NULL) {
        throw RuntimeException(
            OUSTR("cannot get digest rtl_Digest_AlgorithmMD5!"), 0 );
    }

    sal_uInt8 const * data =
        reinterpret_cast<sal_uInt8 const *>(userPath.getStr());
    sal_Size size = (userPath.getLength() * sizeof (sal_Unicode));
    sal_uInt32 md5_key_len = rtl_digest_queryLength( digest );
    ::boost::scoped_array<sal_uInt8> md5_buf( new sal_uInt8 [ md5_key_len ] );

    rtl_digest_init( digest, data, static_cast<sal_uInt32>(size) );
    rtl_digest_update( digest, data, static_cast<sal_uInt32>(size) );
    rtl_digest_get( digest, md5_buf.get(), md5_key_len );
    rtl_digest_destroy( digest );

    // create hex-value string from the MD5 value to keep
    // the string size minimal
    ::rtl::OUStringBuffer buf;
    buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("SingleOfficeIPC_") );
    for ( sal_uInt32 i = 0; i < md5_key_len; ++i ) {
        buf.append( static_cast<sal_Int32>(md5_buf[ i ]), 0x10 );
    }
    return buf.makeStringAndClear();
}

bool existsOfficePipe()
{
    OUString const & pipeId = OfficePipeId::get();
    if (pipeId.getLength() == 0)
        return false;
    ::osl::Security sec;
    ::osl::Pipe pipe( pipeId, osl_Pipe_OPEN, sec );
    return pipe.is();
}


//Returns true if the Folder was more recently modified then
//the lastsynchronized file. That is the repository needs to
//be synchronized.
bool compareExtensionFolderWithLastSynchronizedFile(
    OUString const & folderURL, OUString const & fileURL)
{
    bool bNeedsSync = false;
    ::osl::DirectoryItem itemExtFolder;
    ::osl::File::RC err1 =
          ::osl::DirectoryItem::get(folderURL, itemExtFolder);
    //If it does not exist, then there is nothing to be done
    if (err1 == ::osl::File::E_NOENT)
    {
        return false;
    }
    else if (err1 != ::osl::File::E_None)
    {
        OSL_ENSURE(0, "Cannot access extension folder");
        return true; //sync just in case
    }

    //If last synchronized does not exist, then AOO is started for the first time
    ::osl::DirectoryItem itemFile;
    ::osl::File::RC err2 = ::osl::DirectoryItem::get(fileURL, itemFile);
    if (err2 == ::osl::File::E_NOENT)
    {
        return true;

    }
    else if (err2 != ::osl::File::E_None)
    {
        OSL_ENSURE(0, "Cannot access file lastsynchronized");
        return true; //sync just in case
    }

    //compare the modification time of the extension folder and the last
    //modified file
    ::osl::FileStatus statFolder(FileStatusMask_ModifyTime);
    ::osl::FileStatus statFile(FileStatusMask_ModifyTime);
    if (itemExtFolder.getFileStatus(statFolder) == ::osl::File::E_None)
    {
        if (itemFile.getFileStatus(statFile) == ::osl::File::E_None)
        {
            TimeValue timeFolder = statFolder.getModifyTime();
            TimeValue timeFile = statFile.getModifyTime();

            if (timeFile.Seconds < timeFolder.Seconds)
                bNeedsSync = true;
        }
        else
        {
            OSL_ASSERT(0);
            bNeedsSync = true;
        }
    }
    else
    {
        OSL_ASSERT(0);
        bNeedsSync = true;
    }
    return bNeedsSync;
}

bool needToSyncRepostitory(OUString const & name)
{
    OUString folder;
    OUString file;
    if (name.equals(OUString(RTL_CONSTASCII_USTRINGPARAM("bundled"))))
    {
        folder = OUString(
            RTL_CONSTASCII_USTRINGPARAM("$BUNDLED_EXTENSIONS"));
        file = OUString (
            RTL_CONSTASCII_USTRINGPARAM(
                "$BUNDLED_EXTENSIONS_USER/lastsynchronized"));
    }
    else if (name.equals(OUString(RTL_CONSTASCII_USTRINGPARAM("shared"))))
    {
        folder = OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "$UNO_SHARED_PACKAGES_CACHE/uno_packages"));
        file = OUString (
            RTL_CONSTASCII_USTRINGPARAM(
                "$SHARED_EXTENSIONS_USER/lastsynchronized"));
    }
    else
    {
        OSL_ASSERT(0);
        return true;
    }
    ::rtl::Bootstrap::expandMacros(folder);
    ::rtl::Bootstrap::expandMacros(file);
    return compareExtensionFolderWithLastSynchronizedFile(
        folder, file);
}


} // anon namespace

//==============================================================================

namespace {
inline OUString encodeForRcFile( OUString const & str )
{
    // escape $\{} (=> rtl bootstrap files)
    ::rtl::OUStringBuffer buf;
    sal_Int32 pos = 0;
    const sal_Int32 len = str.getLength();
    for ( ; pos < len; ++pos ) {
        sal_Unicode c = str[ pos ];
        switch (c) {
        case '$':
        case '\\':
        case '{':
        case '}':
            buf.append( static_cast<sal_Unicode>('\\') );
            break;
        }
        buf.append( c );
    }
    return buf.makeStringAndClear();
}
}

//==============================================================================
OUString makeURL( OUString const & baseURL, OUString const & relPath_ )
{
    ::rtl::OUStringBuffer buf;
    if (baseURL.getLength() > 1 && baseURL[ baseURL.getLength() - 1 ] == '/')
        buf.append( baseURL.copy( 0, baseURL.getLength() - 1 ) );
    else
        buf.append( baseURL );
    OUString relPath(relPath_);
    if (relPath.getLength() > 0 && relPath[ 0 ] == '/')
        relPath = relPath.copy( 1 );
    if (relPath.getLength() > 0)
    {
        buf.append( static_cast<sal_Unicode>('/') );
        if (baseURL.matchAsciiL(
                RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.expand:") )) {
            // encode for macro expansion: relPath is supposed to have no
            // macros, so encode $, {} \ (bootstrap mimic)
            relPath = encodeForRcFile(relPath);

            // encode once more for vnd.sun.star.expand schema:
            // vnd.sun.star.expand:$UNO_...
            // will expand to file-url
            relPath = ::rtl::Uri::encode( relPath, rtl_UriCharClassUric,
                                          rtl_UriEncodeIgnoreEscapes,
                                          RTL_TEXTENCODING_UTF8 );
        }
        buf.append( relPath );
    }
    return buf.makeStringAndClear();
}

OUString makeURLAppendSysPathSegment( OUString const & baseURL, OUString const & relPath_ )
{
    OUString segment = relPath_;
    OSL_ASSERT(segment.indexOf(static_cast<sal_Unicode>('/')) == -1);

    ::rtl::Uri::encode(
        segment, rtl_UriCharClassPchar, rtl_UriEncodeIgnoreEscapes,
        RTL_TEXTENCODING_UTF8);
    return makeURL(baseURL, segment);
}



//==============================================================================
OUString expandUnoRcTerm( OUString const & term_ )
{
    OUString term(term_);
    UnoRc::get()->expandMacrosFrom( term );
    return term;
}

OUString makeRcTerm( OUString const & url )
{
    OSL_ASSERT( url.matchAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                     "vnd.sun.star.expand:") ) );
    if (url.matchAsciiL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.expand:") )) {
        // cut protocol:
        OUString rcterm( url.copy( sizeof ("vnd.sun.star.expand:") - 1 ) );
        // decode uric class chars:
        rcterm = ::rtl::Uri::decode(
            rcterm, rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 );
        return rcterm;
    }
    else
        return url;
}

//==============================================================================
OUString expandUnoRcUrl( OUString const & url )
{
    if (url.matchAsciiL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.expand:") )) {
        // cut protocol:
        OUString rcurl( url.copy( sizeof ("vnd.sun.star.expand:") - 1 ) );
        // decode uric class chars:
        rcurl = ::rtl::Uri::decode(
            rcurl, rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 );
        // expand macro string:
        UnoRc::get()->expandMacrosFrom( rcurl );
        return rcurl;
    }
    else {
        return url;
    }
}

//==============================================================================
bool office_is_running()
{
    //We need to check if we run within the office process. Then we must not use the pipe, because
    //this could cause a deadlock. This is actually a workaround for i82778
    OUString sFile;
    oslProcessError err = osl_getExecutableFile(& sFile.pData);
    bool ret = false;
    if (osl_Process_E_None == err)
    {
        sFile = sFile.copy(sFile.lastIndexOf('/') + 1);
        if (
#if defined UNIX
            sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SOFFICE2)))
#elif defined WNT || defined OS2
            //osl_getExecutableFile should deliver "soffice.bin" on Windows
            //even if swriter.exe, scalc.exe etc. was started. This is a bug
            //in osl_getExecutableFile
            sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SOFFICE1)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SOFFICE2)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SBASE)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SCALC)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SDRAW)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SIMPRESS)))
            || sFile.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(SWRITER)))
#else
#error "Unsupported platform"
#endif

            )
            ret = true;
        else
            ret = existsOfficePipe();
    }
    else
    {
        OSL_ENSURE(0, "NOT osl_Process_E_None ");
        //if osl_getExecutable file than we take the risk of creating a pipe
        ret = existsOfficePipe();
    }
    return ret;
}

//==============================================================================
oslProcess raiseProcess(
    OUString const & appURL, Sequence<OUString> const & args )
{
    ::osl::Security sec;
    oslProcess hProcess = 0;
    oslProcessError rc = osl_executeProcess(
        appURL.pData,
        reinterpret_cast<rtl_uString **>(
            const_cast<OUString *>(args.getConstArray()) ),
        args.getLength(),
        osl_Process_DETACHED,
        sec.getHandle(),
        0, // => current working dir
        0, 0, // => no env vars
        &hProcess );

    switch (rc) {
    case osl_Process_E_None:
        break;
    case osl_Process_E_NotFound:
        throw RuntimeException( OUSTR("image not found!"), 0 );
    case osl_Process_E_TimedOut:
        throw RuntimeException( OUSTR("timeout occurred!"), 0 );
    case osl_Process_E_NoPermission:
        throw RuntimeException( OUSTR("permission denied!"), 0 );
    case osl_Process_E_Unknown:
        throw RuntimeException( OUSTR("unknown error!"), 0 );
    case osl_Process_E_InvalidError:
    default:
        throw RuntimeException( OUSTR("unmapped error!"), 0 );
    }

    return hProcess;
}

//==============================================================================
OUString generateRandomPipeId()
{
    // compute some good pipe id:
    static rtlRandomPool s_hPool = rtl_random_createPool();
    if (s_hPool == 0)
        throw RuntimeException( OUSTR("cannot create random pool!?"), 0 );
    sal_uInt8 bytes[ 32 ];
    if (rtl_random_getBytes(
            s_hPool, bytes, ARLEN(bytes) ) != rtl_Random_E_None) {
        throw RuntimeException( OUSTR("random pool error!?"), 0 );
    }
    ::rtl::OUStringBuffer buf;
    for ( sal_uInt32 i = 0; i < ARLEN(bytes); ++i ) {
        buf.append( static_cast<sal_Int32>(bytes[ i ]), 0x10 );
    }
    return buf.makeStringAndClear();
}

//==============================================================================
Reference<XInterface> resolveUnoURL(
    OUString const & connectString,
    Reference<XComponentContext> const & xLocalContext,
    AbortChannel * abortChannel )
{
    Reference<bridge::XUnoUrlResolver> xUnoUrlResolver(
        bridge::UnoUrlResolver::create( xLocalContext ) );

    for (;;)
    {
        if (abortChannel != 0 && abortChannel->isAborted()) {
            throw ucb::CommandAbortedException(
                OUSTR("abort!"), Reference<XInterface>() );
        }
        try {
            return xUnoUrlResolver->resolve( connectString );
        }
        catch (connection::NoConnectException &) {
            TimeValue tv = { 0 /* secs */, 500000000 /* nanosecs */ };
            ::osl::Thread::wait( tv );
        }
    }
}

#ifdef WNT
void writeConsoleWithStream(::rtl::OUString const & sText, HANDLE stream)
{
    DWORD nWrittenChars = 0;
    WriteFile(stream, sText.getStr(),
        sText.getLength() * 2, &nWrittenChars, NULL);
}
#else
void writeConsoleWithStream(::rtl::OUString const & sText, FILE * stream)
{
    OString s = OUStringToOString(sText, osl_getThreadTextEncoding());
    fprintf(stream, "%s", s.getStr());
    fflush(stream);
}
#endif

#ifdef WNT
void writeConsoleWithStream(::rtl::OString const & sText, HANDLE stream)
{
    writeConsoleWithStream(OStringToOUString(
        sText, RTL_TEXTENCODING_UTF8), stream);
}
#else
void writeConsoleWithStream(::rtl::OString const & sText, FILE * stream)
{
    fprintf(stream, "%s", sText.getStr());
    fflush(stream);
}
#endif

void writeConsole(::rtl::OUString const & sText)
{
#ifdef WNT
    writeConsoleWithStream(sText, GetStdHandle(STD_OUTPUT_HANDLE));
#else
    writeConsoleWithStream(sText, stdout);
#endif
}

void writeConsole(::rtl::OString const & sText)
{
#ifdef WNT
    writeConsoleWithStream(sText, GetStdHandle(STD_OUTPUT_HANDLE));
#else
    writeConsoleWithStream(sText, stdout);
#endif
}

void writeConsoleError(::rtl::OUString const & sText)
{
#ifdef WNT
    writeConsoleWithStream(sText, GetStdHandle(STD_ERROR_HANDLE));
#else
    writeConsoleWithStream(sText, stderr);
#endif
}


void writeConsoleError(::rtl::OString const & sText)
{
#ifdef WNT
    writeConsoleWithStream(sText, GetStdHandle(STD_ERROR_HANDLE));
#else
    writeConsoleWithStream(sText, stderr);
#endif
}



OUString readConsole()
{
#ifdef WNT
    sal_Unicode aBuffer[1024];
	DWORD	dwRead = 0;
    //unopkg.com feeds unopkg.exe with wchar_t|s
    if (ReadFile( GetStdHandle(STD_INPUT_HANDLE), &aBuffer, sizeof(aBuffer), &dwRead, NULL ) )
	{
        OSL_ASSERT((dwRead % 2) == 0);
        OUString value( aBuffer, dwRead / 2);
        return value.trim();
	}
#else
	char buf[1024];
	rtl_zeroMemory(buf, 1024);
	// read one char less so that the last char in buf is always zero
	if (fgets(buf, 1024, stdin) != NULL)
    {
        OUString value = ::rtl::OStringToOUString(::rtl::OString(buf), osl_getThreadTextEncoding());
        return value.trim();
    }
#endif
    return OUString();
}

void TRACE(::rtl::OUString const & sText)
{
    (void) sText;
#if OSL_DEBUG_LEVEL > 1
    writeConsole(sText);
#endif
}

void TRACE(::rtl::OString const & sText)
{
    (void) sText;
#if OSL_DEBUG_LEVEL > 1
    writeConsole(sText);
#endif
}

void syncRepositories(Reference<ucb::XCommandEnvironment> const & xCmdEnv)
{
    OUString sDisable;
    ::rtl::Bootstrap::get( OUSTR( "DISABLE_EXTENSION_SYNCHRONIZATION" ), sDisable, OUString() );
    if (sDisable.getLength() > 0)
        return;

    Reference<deployment::XExtensionManager> xExtensionManager;
    //synchronize shared before bundled otherwise there are
    //more revoke and registration calls.
    sal_Bool bModified = false;
    if (needToSyncRepostitory(OUString(RTL_CONSTASCII_USTRINGPARAM("shared")))
        || needToSyncRepostitory(OUString(RTL_CONSTASCII_USTRINGPARAM("bundled"))))
    {
        xExtensionManager =
            deployment::ExtensionManager::get(
                comphelper_getProcessComponentContext());

        if (xExtensionManager.is())
        {
            bModified = xExtensionManager->synchronize(
                Reference<task::XAbortChannel>(), xCmdEnv);
        }
    }

    if (bModified)
    {
        Reference<task::XRestartManager> restarter(
            comphelper_getProcessComponentContext()->getValueByName(
                OUSTR( "/singletons/com.sun.star.task.OfficeRestartManager") ), UNO_QUERY );
        if (restarter.is())
        {
            restarter->requestRestart(xCmdEnv.is() == sal_True ? xCmdEnv->getInteractionHandler() :
                                      Reference<task::XInteractionHandler>());
        }
     }
}



}
