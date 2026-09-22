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

#ifndef INCLUDED_SVTOOLS_LINKPOLICY_HXX
#define INCLUDED_SVTOOLS_LINKPOLICY_HXX

#include "sal/config.h"

#include "rtl/string.h"
#include "rtl/uri.hxx"
#include "rtl/ustring.hxx"
#include "svtools/svtdllapi.h"

/** Policy for following a URL that came out of document content.

    Document content names resources by URL -- linked graphics, background
    images, instance data, media. These helpers are the one place that decides
    what may be followed, so the modules that hold the actual load calls
    (editeng, svgio, forms, sw, svtools itself) all apply the same rule instead
    of each carrying its own test.

    The classifying functions are inline and depend on nothing but rtl, so they
    can be exercised directly. mayFollowDocumentLink() needs the running
    document and therefore lives in the library.
 */
namespace svt { namespace linkpolicy {

/** Tells whether a URL uses one of the schemes that address process state or
    an internal object tree rather than an ordinary resource.

    vnd.sun.star.expand resolves bootstrap macros; script, tdoc and uno reach
    the scripting engine, the loaded document tree and the object model. None
    of them is a meaningful target for document content, so these are never
    followed regardless of any other permission.
 */
inline bool isVendorUrl( ::rtl::OUString const & rUrl )
{
    if ( !rUrl.matchIgnoreAsciiCaseAsciiL(
              RTL_CONSTASCII_STRINGPARAM( "vnd.sun.star." ) ) )
        return false;
    sal_Int32 const n = RTL_CONSTASCII_LENGTH( "vnd.sun.star." );
    return rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "expand" ), n )
        || rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "script" ), n )
        || rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "tdoc" ), n )
        || rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "uno" ), n );
}

/** Tells whether a reference carries a scheme, i.e. names a resource by
    itself rather than relative to the document that holds it.

    A scheme is a name followed by ':' before any path separator. Callers use
    this to leave a document-relative reference alone: relative references
    stay inside the document's own area and are the ordinary case, while a
    scheme is what lets a reference point somewhere else entirely.
 */
inline bool isAbsoluteUrl( ::rtl::OUString const & rUrl )
{
    sal_Int32 const nColon = rUrl.indexOf( ':' );
    if ( nColon <= 0 )
        return false;
    sal_Int32 const nSlash = rUrl.indexOf( '/' );
    if ( nSlash != -1 && nSlash < nColon )
        return false;
    // A scheme starts with a letter (RFC 3986); a Windows drive letter
    // followed by ':' is one character and cannot be confused with it.
    sal_Unicode const c = rUrl.getStr()[0];
    if ( !( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ) )
        return false;
    return nColon > 1;
}

/** Tells whether a URL is a file: reference whose authority names a host
    other than this one.

    On Windows such a reference resolves to a UNC path, so the access is a
    request to another machine rather than a read of a local file. An empty
    authority and localhost are this machine and are not affected.
 */
inline bool isOffHostFileUrl( ::rtl::OUString const & rUrl )
{
    if ( !rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "file:" ) ) )
        return false;
    ::rtl::OUString aRest( rUrl.copy( RTL_CONSTASCII_LENGTH( "file:" ) ) );

    // A backslash is not URL syntax; on Windows it would form a UNC
    // reference once the URL is converted to a system path.
    if ( aRest.indexOf( '\\' ) != -1 )
        return true;

    if ( !aRest.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "//" ) ) )
        return false;                       // no authority component at all

    sal_Int32 const nEnd = aRest.indexOf( '/', 2 );
    ::rtl::OUString const aAuthority(
        nEnd == -1 ? aRest.copy( 2 ) : aRest.copy( 2, nEnd - 2 ) );
    if ( aAuthority.getLength() != 0
         && !aAuthority.equalsIgnoreAsciiCaseAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "localhost" ) ) )
        return true;

    // An empty authority followed by a path that starts another one
    // (file:////host/share) names a different host just the same.
    ::rtl::OUString const aPath(
        nEnd == -1 ? ::rtl::OUString() : aRest.copy( nEnd ) );
    return aPath.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "//" ) );
}

/** Tells whether a URL names a file on this machine: the file: scheme, an
    absolute path, and no authority pointing elsewhere.
 */
inline bool isLocalFileUrl( ::rtl::OUString const & rUrl )
{
    if ( !rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "file:" ) ) )
        return false;
    if ( isOffHostFileUrl( rUrl ) )
        return false;
    ::rtl::OUString aRest( rUrl.copy( RTL_CONSTASCII_LENGTH( "file:" ) ) );
    if ( aRest.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "//" ) ) )
    {
        sal_Int32 const nEnd = aRest.indexOf( '/', 2 );
        if ( nEnd == -1 )
            return false;                   // authority only, no path
        aRest = aRest.copy( nEnd );
    }
    return aRest.getLength() > 1 && aRest.getStr()[0] == '/';
}

/** Tells whether rTarget stays inside the directory rBase sits in.

    Both are expected to be absolute URLs, rTarget already resolved against
    rBase. Comparing the whole string means the scheme and the authority have
    to agree as well, so a target on another host or in another scheme is not
    contained. A segment that would climb back out is refused whether it is
    spelled plainly or percent-encoded, because the escape only disappears
    when the URL is turned into a system path.

    This is the test for the case where a document may reference a file
    delivered alongside it, but nothing else.
 */
inline bool isContainedUnder(
    ::rtl::OUString const & rBase, ::rtl::OUString const & rTarget )
{
    if ( rBase.getLength() == 0 || rTarget.getLength() == 0 )
        return false;

    // The directory rBase lives in, trailing separator included.
    sal_Int32 const nSlash = rBase.lastIndexOf( '/' );
    if ( nSlash == -1 )
        return false;
    ::rtl::OUString const aDir( rBase.copy( 0, nSlash + 1 ) );

    if ( !rTarget.match( aDir ) )
        return false;
    ::rtl::OUString aRest( rTarget.copy( aDir.getLength() ) );
    if ( aRest.getLength() == 0 )
        return false;                       // the directory itself
    if ( aRest.getStr()[0] == '/' )
        return false;                       // would open an authority

    // Refuse a climbing segment in either spelling. Decoding can throw on
    // malformed escapes; that is a refusal too.
    ::rtl::OUString aDecoded;
    try
    {
        aDecoded = ::rtl::Uri::decode(
            aRest, rtl_UriDecodeWithCharset, RTL_TEXTENCODING_UTF8 );
    }
    catch ( ... )
    {
        return false;
    }
    if ( aDecoded.getLength() == 0 )
        return false;
    if ( aDecoded.indexOf( '\\' ) != -1 )
        return false;

    for ( sal_Int32 i = 0; i != -1; )
    {
        ::rtl::OUString const aSegment( aDecoded.getToken( 0, '/', i ) );
        if ( aSegment.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( ".." ) ) )
            return false;
    }
    return true;
}

/** Decides whether document content may follow rUrl.

    A vendor URL (see isVendorUrl) is never followed. Otherwise the running
    document decides, through com::sun::star::document::XLinkAuthorizer, which
    refuses the request, permits it, or asks the user -- the same decision the
    linked-graphic loader has been making, now shared. When no document can be
    reached the URL is followed, as that loader has always done; callers that
    can enforce something stricter on their own (a containment rule, a scheme
    restriction) should apply it in addition to this.
 */
SVT_DLLPUBLIC bool mayFollowDocumentLink( ::rtl::OUString const & rUrl );

/** Decides whether a reference out of document content may be loaded.

    This is the form a load site wants unless it has a reason for something
    else. A relative reference stays in the document's own area, and a file on
    this machine is what a document has always been able to name, so both are
    loaded as before -- no new question is put to the user for the ordinary
    case. Anything else either leaves this machine (a remote scheme, or a
    file: URL whose authority names another host) or addresses process state,
    and needs permission first.

    A load site that can be stricter should be: GraphicObject asks about every
    URL, including local ones, and keeps doing so.
 */
inline bool mayLoadDocumentReference( ::rtl::OUString const & rUrl )
{
    if ( rUrl.getLength() == 0 )
        return false;
    if ( !isAbsoluteUrl( rUrl ) )
    {
        // No scheme, so relative to the document -- except for a system path that
        // opens with a host name, which names another machine just the same. A
        // caller that does not convert to a URL first hands such a path on
        // unchanged.
        if ( rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "\\\\" ) )
             || rUrl.matchIgnoreAsciiCaseAsciiL( RTL_CONSTASCII_STRINGPARAM( "//" ) ) )
            return mayFollowDocumentLink( rUrl );
        return true;
    }
    if ( isLocalFileUrl( rUrl ) )
        return true;
    return mayFollowDocumentLink( rUrl );
}

} }

#endif
