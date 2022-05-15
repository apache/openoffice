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
#include "precompiled_webdav.hxx"

#include <string.h>
#include <rtl/uri.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include "CurlUri.hxx"
#include "DAVException.hxx"

#include <curl/curl.h>

#include "../inc/urihelper.hxx"

using namespace http_dav_ucp;

# if defined __SUNPRO_CC
# pragma enable_warn
#endif

// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------

namespace {

inline bool matchIgnoreAsciiCase(rtl::OString const & rStr1,
                                 sal_Char const * pStr2,
                                 sal_Int32 nStr2Len) SAL_THROW(())
{
    return
        rtl_str_shortenedCompareIgnoreAsciiCase_WithLength(
                rStr1.getStr(), rStr1.getLength(), pStr2, nStr2Len, nStr2Len)
            == 0;
}

inline rtl::OUString getURLStringPart( const CURLU *curlUrl, CURLUPart part, unsigned int flags )
{
    char *value = NULL;
    CURLUcode rc = curl_url_get( const_cast<CURLU*>( curlUrl ), part, &value, flags );
    if ( rc == CURLUE_OK )
    {
        rtl::OUString str = rtl::OStringToOUString( value, RTL_TEXTENCODING_UTF8 );
        curl_free( value );
        return str;
    }
    return rtl::OUString();
}

}

CurlUri::CurlUri( const CURLU * inUri )
    throw ( DAVException )
    : mURI()
    , mScheme()
    , mUserName()
    , mPassword()
    , mHostName()
    , mPort()
    , mPath()
{
    if ( inUri == 0 )
        throw DAVException( DAVException::DAV_INVALID_ARG );
    mCurlUri = curl_url_dup( const_cast<CURLU *>(inUri) );
    if ( mCurlUri == NULL )
         throw DAVException( DAVException::DAV_HTTP_ERROR,
                             rtl::OUString::createFromAscii( "Out of memory" ),
                             SC_INSUFFICIENT_STORAGE );

    char * uri;
    CURLUcode rc = curl_url_get( mCurlUri, CURLUPART_URL, &uri, 0 );
    if ( rc != CURLUE_OK )
    {
        curl_url_cleanup( mCurlUri );
        throw DAVException( DAVException::DAV_INVALID_ARG );
    }
    curl_free( uri );

    init( mCurlUri );

    calculateURI();
}

CurlUri::CurlUri( const rtl::OUString & inUri )
    throw ( DAVException )
    : mCurlUri( 0 )
    , mURI()
    , mScheme()
    , mUserName()
    , mPassword()
    , mHostName()
    , mPort( 0 )
    , mPath()
{
    if ( inUri.getLength() <= 0 )
        throw DAVException( DAVException::DAV_INVALID_ARG );
    mCurlUri = curl_url();
    if ( mCurlUri == NULL )
         throw DAVException( DAVException::DAV_HTTP_ERROR,
                             rtl::OUString::createFromAscii( "Out of memory" ),
                             SC_INSUFFICIENT_STORAGE );

    // #i77023#
    rtl::OUString aEscapedUri( ucb_impl::urihelper::encodeURI( inUri ) );

    rtl::OString theInputUri(
        aEscapedUri.getStr(), aEscapedUri.getLength(), RTL_TEXTENCODING_UTF8 );

    if ( curl_url_set( mCurlUri, CURLUPART_URL, theInputUri.getStr(), 0 ) != CURLUE_OK )
    {
        // I kid you not:
        // Sometimes, we are just given a URL's path part,
        // and CREATING THE URL ABSOLUTELY MUST SUCCEED, even though the resulting URL
        // of "/path/to/file.txt" will be the terrible "://:0/path/to/file.txt" !!!
        // (Such input usually comes from the callers of GetPathBaseName() and the like.)
        if ( !theInputUri.isEmpty() && theInputUri[0] == '/' &&
                curl_url_set( mCurlUri, CURLUPART_PATH, theInputUri.getStr(), 0 ) != CURLUE_OK )
        {
            throw DAVException( DAVException::DAV_INVALID_ARG );
        }
    }

    rtl::OUString portString = getURLStringPart( mCurlUri, CURLUPART_PORT, 0 );
    if ( portString.isEmpty() )
    {
        rtl::OUString defaultPortW = getURLStringPart( mCurlUri, CURLUPART_PORT, CURLU_DEFAULT_PORT );
        rtl::OString defaultPortA = OUStringToOString( defaultPortW, RTL_TEXTENCODING_UTF8 );
        if ( !defaultPortA.isEmpty() )
            curl_url_set( mCurlUri, CURLUPART_PORT, defaultPortA.getStr(), 0 );
    }
    rtl::OUString path = getURLStringPart( mCurlUri, CURLUPART_PATH, 0 );
    if ( path.isEmpty() )
        curl_url_set( mCurlUri, CURLUPART_PATH, "/", 0);

    init( mCurlUri );

    calculateURI();
}

CurlUri::CurlUri( const CurlUri &curlUri )
    throw ( DAVException )
    : mURI()
    , mScheme()
    , mUserName()
    , mPassword()
    , mHostName()
    , mPort()
    , mPath()
{
    mCurlUri = curl_url_dup( curlUri.mCurlUri );
    if ( mCurlUri == NULL )
         throw DAVException( DAVException::DAV_HTTP_ERROR,
                             rtl::OUString::createFromAscii( "Out of memory" ),
                             SC_INSUFFICIENT_STORAGE );

    char * uri;
    CURLUcode rc = curl_url_get( mCurlUri, CURLUPART_URL, &uri, 0 );
    if ( rc != CURLUE_OK )
    {
        curl_url_cleanup( mCurlUri );
        throw DAVException( DAVException::DAV_INVALID_ARG );
    }
    curl_free( uri );

    init( mCurlUri );

    calculateURI();
}

void CurlUri::init( const CURLU * pUri )
{
    mScheme   = getURLStringPart( pUri, CURLUPART_SCHEME, 0 );
    mUserName = getURLStringPart( pUri, CURLUPART_USER, 0 );
    mPassword = getURLStringPart( pUri, CURLUPART_PASSWORD, 0 );
    mHostName = getURLStringPart( pUri, CURLUPART_HOST, 0 );
    rtl::OUString portString = getURLStringPart( pUri, CURLUPART_PORT, 0);
    mPort     = 0;
    if ( !portString.isEmpty() )
        mPort = portString.toInt32();
    mPath     = getURLStringPart( pUri, CURLUPART_PATH, 0 );

    rtl::OUString query = getURLStringPart( pUri, CURLUPART_QUERY, 0 );
    if ( !query.isEmpty() )
    {
        mPath += rtl::OUString::createFromAscii( "?" );
        mPath += query;
    }

    rtl::OUString fragment = getURLStringPart( pUri, CURLUPART_FRAGMENT, 0 );
    if ( !fragment.isEmpty() )
    {
        mPath += rtl::OUString::createFromAscii( "#" );
        mPath += fragment;
    }
}

CurlUri::~CurlUri( )
{
    if ( mCurlUri )
        curl_url_cleanup( mCurlUri );
}

void CurlUri::calculateURI ()
{
    rtl::OUStringBuffer aBuf( mScheme );
    aBuf.appendAscii( "://" );
    if ( mUserName.getLength() > 0 )
    {
        aBuf.append( mUserName );
        if ( mPassword.getLength() > 0 )
        {
             aBuf.appendAscii( ":" );
             aBuf.append( mPassword );
        }
        aBuf.appendAscii( "@" );
    }
    // Is host a numeric IPv6 address?
    if ( ( mHostName.indexOf( ':' ) != -1 ) &&
         ( mHostName[ 0 ] != sal_Unicode( '[' ) ) )
    {
        aBuf.appendAscii( "[" );
        aBuf.append( mHostName );
        aBuf.appendAscii( "]" );
    }
    else
    {
        aBuf.append( mHostName );
    }

    // append port, but only, if not default port.
    bool bAppendPort = true;
    switch ( mPort )
    {
    case DEFAULT_HTTP_PORT:
        bAppendPort = !mScheme.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "http" ) );
        break;

    case DEFAULT_HTTPS_PORT:
        bAppendPort = !mScheme.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "https" ) );
        break;
    }
    if ( bAppendPort )
    {
        aBuf.appendAscii( ":" );
        aBuf.append( rtl::OUString::valueOf( mPort ) );
    }
    aBuf.append( mPath );

    mURI = aBuf.makeStringAndClear();
}

::rtl::OUString CurlUri::GetPathBaseName () const
{
    sal_Int32 nPos = mPath.lastIndexOf ('/');
    sal_Int32 nTrail = 0;
    if (nPos == mPath.getLength () - 1)
    {
        // Trailing slash found. Skip.
        nTrail = 1;
        nPos = mPath.lastIndexOf ('/', nPos);
    }
    if (nPos != -1)
    {
        rtl::OUString aTemp(
            mPath.copy (nPos + 1, mPath.getLength () - nPos - 1 - nTrail) );

        // query, fragment present?
        nPos = aTemp.indexOf( '?' );
        if ( nPos == -1 )
            nPos = aTemp.indexOf( '#' );

        if ( nPos != -1 )
            aTemp = aTemp.copy( 0, nPos );

        return aTemp;
    }
    else
        return rtl::OUString::createFromAscii ("/");
}

bool CurlUri::operator== ( const CurlUri & rOther ) const
{
    return ( mURI == rOther.mURI );
}

::rtl::OUString CurlUri::GetPathBaseNameUnescaped () const
{
    return unescape( GetPathBaseName() );
}

void CurlUri::AppendPath (const rtl::OUString& rPath)
{
    if (mPath.lastIndexOf ('/') != mPath.getLength () - 1)
        mPath += rtl::OUString::createFromAscii ("/");

    mPath += rPath;
    calculateURI ();
};

// static
rtl::OUString CurlUri::escapeSegment( const rtl::OUString& segment )
{
    return rtl::Uri::encode( segment,
                             rtl_UriCharClassPchar,
                             rtl_UriEncodeIgnoreEscapes,
                             RTL_TEXTENCODING_UTF8 );
}

// static
rtl::OUString CurlUri::unescape( const rtl::OUString& segment )
{
    return rtl::Uri::decode( segment,
                             rtl_UriDecodeWithCharset,
                             RTL_TEXTENCODING_UTF8 );
}

// static
rtl::OUString CurlUri::makeConnectionEndPointString(
                                const rtl::OUString & rHostName, int nPort )
{
    rtl::OUStringBuffer aBuf;

    // Is host a numeric IPv6 address?
    if ( ( rHostName.indexOf( ':' ) != -1 ) &&
         ( rHostName[ 0 ] != sal_Unicode( '[' ) ) )
    {
        aBuf.appendAscii( "[" );
        aBuf.append( rHostName );
        aBuf.appendAscii( "]" );
    }
    else
    {
        aBuf.append( rHostName );
    }

    if ( ( nPort != DEFAULT_HTTP_PORT ) && ( nPort != DEFAULT_HTTPS_PORT ) )
    {
        aBuf.appendAscii( ":" );
        aBuf.append( rtl::OUString::valueOf( sal_Int32( nPort ) ) );
    }
    return aBuf.makeStringAndClear();
}

