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

#include "CurlRequest.hxx"

#include <string.h>

using namespace ::com::sun::star;
using namespace http_dav_ucp;

CurlRequest::CurlRequest( CURL *curl )
    : curl( curl )
    , curlUrl( NULL )
    , requestHeaders( NULL )
    , requestBody( NULL )
    , requestBodyOffset( 0 )
    , requestBodySize( 0 )
    , useChunkedEncoding( false )
    , provideCredentialsCallback( NULL )
    , provideCredentialsUserdata( NULL )
    , statusCode( 0 )
    , responseBodyInputStream( new CurlInputStream() )
{
    curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, Curl_HeaderReceived );
    curl_easy_setopt( curl, CURLOPT_HEADERDATA, this );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, Curl_MoreBodyReceived );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, this );
}

CurlRequest::~CurlRequest()
{
    if ( curlUrl != NULL )
        curl_url_cleanup( curlUrl );
    curl_easy_setopt( curl, CURLOPT_CURLU, NULL );
    
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, NULL );
    if ( requestHeaders != NULL )
        curl_slist_free_all( requestHeaders );
    curl_easy_setopt( curl, CURLOPT_READFUNCTION, NULL );
    curl_easy_setopt( curl, CURLOPT_READDATA, NULL );
    curl_easy_setopt( curl, CURLOPT_INFILESIZE, -1 );
    curl_easy_setopt( curl, CURLOPT_SEEKFUNCTION, NULL );
    curl_easy_setopt( curl, CURLOPT_SEEKDATA, NULL );
    curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, NULL );
    curl_easy_setopt( curl, CURLOPT_HEADERDATA, NULL );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, NULL );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, NULL );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, NULL );
}

void CurlRequest::addHeader( const rtl::OString &name, const rtl::OString &value) throw (DAVException)
{
    rtl::OString line = name + ": " + value;
    struct curl_slist *appended = curl_slist_append( requestHeaders, line.getStr() );
    if ( appended != NULL )
    {
        requestHeaders = appended;
        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, requestHeaders );
    }
    else
        throw DAVException( DAVException::DAV_SESSION_CREATE, rtl::OUString::createFromAscii( "Out of memory" ) );
}

void CurlRequest::setRequestBody( const char *body, size_t size )
{
    requestBody = body;
    requestBodyOffset = 0;
    requestBodySize = size;

    curl_easy_setopt( curl, CURLOPT_READFUNCTION, Curl_SendMoreBody );
    curl_easy_setopt( curl, CURLOPT_READDATA, this );
    curl_easy_setopt( curl, CURLOPT_SEEKFUNCTION, Curl_SeekCallback );
    curl_easy_setopt( curl, CURLOPT_SEEKDATA, this );
    if ( useChunkedEncoding )
        addHeader( "Transfer-Encoding", "chunked" );
    else
        curl_easy_setopt( curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size );
}

int CurlRequest::Curl_SeekCallback( void *userdata, curl_off_t offset, int origin )
{
    CurlRequest *request = static_cast< CurlRequest* >( userdata );
    if ( origin == SEEK_SET )
        request->requestBodyOffset = (size_t) offset;
    else if ( origin == SEEK_CUR )
        request->requestBodyOffset += (size_t) offset;
    else if ( origin == SEEK_END )
        request->requestBodyOffset += (size_t) ((curl_off_t)request->requestBodySize + offset);
    else
        return CURL_SEEKFUNC_CANTSEEK;
    if ( request->requestBodyOffset > request->requestBodySize )
        request->requestBodyOffset = request->requestBodySize;
    return CURL_SEEKFUNC_OK;
}

size_t CurlRequest::Curl_SendMoreBody( char *buffer, size_t size, size_t nitems, void *userdata )
{
    CurlRequest *request = static_cast< CurlRequest* >( userdata );
    OSL_TRACE("Curl_SendMoreBody");
    return request->curlSendMoreBody( buffer, nitems );
}

size_t CurlRequest::curlSendMoreBody( char *buffer, size_t len )
{
    size_t bytesToSend = requestBodySize - requestBodyOffset;
    if ( bytesToSend > len )
        bytesToSend = len;
    memcpy( buffer, requestBody, bytesToSend );
    requestBodyOffset += bytesToSend;
    return bytesToSend;
}

void CurlRequest::setProvideCredentialsCallback( bool (*callback)(long statusCode, void *userdata) throw (DAVException), void *userdata )
{
    provideCredentialsCallback = callback;
    provideCredentialsUserdata = userdata;
}

void CurlRequest::setURI( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    if ( curlUrl != NULL )
    {
        curl_url_cleanup( curlUrl );
        curlUrl = NULL;
    }

    curlUrl = curl_url();
    if ( curlUrl == NULL )
        throw DAVException( DAVException::DAV_SESSION_CREATE, rtl::OUString::createFromAscii( "Out of memory" ) );

    if ( CurlUri( path ).GetHost().isEmpty() )
    {
        // "path" is really a path, not a URI
        curl_url_set( curlUrl, CURLUPART_URL, rtl::OUStringToOString( uri.GetURI(), RTL_TEXTENCODING_UTF8 ).getStr(), 0 );
        curl_url_set( curlUrl, CURLUPART_PATH, rtl::OUStringToOString( path, RTL_TEXTENCODING_UTF8 ).getStr(), 0 );
    }
    else
    {
        // The "path" is a full URI
        curl_url_set( curlUrl, CURLUPART_URL, rtl::OUStringToOString( path, RTL_TEXTENCODING_UTF8 ).getStr(), 0 );
    }
    curl_easy_setopt( curl, CURLOPT_CURLU, curlUrl );
}

CURLcode CurlRequest::copy( CurlUri uri, rtl::OUString path ) throw(DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "COPY" );
    return perform();
}

CURLcode CurlRequest::delete_( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "DELETE" );
    return perform();    
}

CURLcode CurlRequest::get( CurlUri uri, rtl::OUString path ) throw(DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "GET" );
    return perform();
}

CURLcode CurlRequest::head( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_NOBODY, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "HEAD" );
    return perform();
}

CURLcode CurlRequest::lock( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "LOCK" );
    return perform();
}

CURLcode CurlRequest::mkcol( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "MKCOL" );
    return perform();
}

CURLcode CurlRequest::move( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "MOVE" );
    return perform();
}

CURLcode CurlRequest::post( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "POST" );
    return perform();
}

CURLcode CurlRequest::propfind( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "PROPFIND" );
    return perform();
}

CURLcode CurlRequest::proppatch( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "PROPPATCH" );
    return perform();
}

CURLcode CurlRequest::put( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "PUT" );
    return perform();
}

CURLcode CurlRequest::unlock( CurlUri uri, rtl::OUString path ) throw (DAVException)
{
    setURI( uri, path );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "UNLOCK" );
    return perform();
}

CURLcode CurlRequest::perform() throw (DAVException)
{
    CURLcode rc = curl_easy_perform( curl );
    long statusCode = 0;
    curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &statusCode );
    if ( ( statusCode == 401 || statusCode == 407 ) && provideCredentialsCallback != NULL )
    {
        bool haveCredentials = provideCredentialsCallback( statusCode, provideCredentialsUserdata );
        if ( haveCredentials )
        {
            // rewind body:
            requestBodyOffset = 0;
            // retry with credentials:
            rc = curl_easy_perform( curl );
            // If this was to authenticate with the proxy, we may need to authenticate
            // with the destination host too:
            if ( statusCode == 407 )
            {
                curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &statusCode );
                if ( statusCode == 401 && provideCredentialsCallback != NULL )
                {
                    haveCredentials = provideCredentialsCallback( statusCode, provideCredentialsUserdata );
                    if ( haveCredentials )
                    {
                        // rewind body:
                        requestBodyOffset = 0;
                        // retry with credentials:
                        rc = curl_easy_perform( curl );
                    }
                }
            }
        }
    }
    return rc;
}

size_t CurlRequest::Curl_HeaderReceived( char *buffer, size_t size, size_t nitems, void *userdata )
{
    CurlRequest *request = static_cast< CurlRequest* >( userdata );
    OSL_TRACE("Curl_HeaderReceived");
    request->curlHeaderReceived( buffer, nitems );
    return nitems;
}

void CurlRequest::curlHeaderReceived( const char *buffer, size_t size )
{
    rtl::OString lineCrLf( buffer, size );
    sal_Int32 cr = lineCrLf.indexOf( "\r" );
    if ( cr < 0 )
        return;

    rtl::OString line = lineCrLf.copy( 0, cr );
    if ( line.indexOf( "HTTP/" ) == 0 )
    {
        // Status line
        // Throw away any response headers from a prior response:
        responseHeaders.clear();
        sal_Int32 idxFirstSpace = line.indexOf( ' ' );
        if ( idxFirstSpace > 0 )
        {
            int idxSecondSpace = line.indexOf( ' ', idxFirstSpace + 1 );
            if ( idxSecondSpace > 0 )
            {
                reasonPhrase = line.copy( idxSecondSpace + 1 );
                statusCode = line.copy( idxFirstSpace + 1, idxSecondSpace - idxFirstSpace - 1 ).toInt32();
            }
            else
            {
                reasonPhrase = "";
                statusCode = line.copy( idxFirstSpace + 1 ).toInt32();
            }
        }
    }
    else
    {
        // Header line
        if ( line.getLength() == 0 )
        {
            // End of HTTP header
            // Discard any intermediate body from 100 Trying or 401 Authentication required:
            responseBodyInputStream = new CurlInputStream();
            return;
        }
        sal_Int32 colon = line.indexOf( ':' );
        if ( colon < 0 )
        {
            OSL_TRACE("Non-empty HTTP line without a ':'. Folded header deprecated by RFC 7230?");
            return;
        }
        Header header;
        header.name = line.copy( 0, colon ).toAsciiLowerCase();
        header.value = line.copy( colon + 1 ).trim();
        responseHeaders.push_back(header);
    }
}

const CurlRequest::Header *CurlRequest::findResponseHeader( const rtl::OString &name )
{
    std::vector< CurlRequest::Header >::const_iterator it( responseHeaders.begin() );
    const std::vector< CurlRequest::Header >::const_iterator end( responseHeaders.end() );
    for ( ; it != end; it++ )
    {
        if ( name.equalsIgnoreAsciiCase( it->name ) )
            return &(*it);
    }
    return NULL;
}

void CurlRequest::saveResponseBodyTo( const uno::Reference< io::XOutputStream > & xOutStream)
{
    xOutputStream = xOutStream;
}

size_t CurlRequest::Curl_MoreBodyReceived( char *buffer, size_t size, size_t nitems, void *userdata )
{
    CurlRequest *request = static_cast< CurlRequest* >( userdata );
    request->curlMoreBodyReceived( buffer, nitems );
    return nitems;
}

void CurlRequest::curlMoreBodyReceived( const char *buffer, size_t size )
{
    if ( xOutputStream.is() )
    {
        const uno::Sequence< sal_Int8 > aDataSeq( (sal_Int8 *)buffer, size );
        xOutputStream->writeBytes( aDataSeq );
    }
    else if ( responseBodyInputStream.is() )
        responseBodyInputStream->AddToStream( buffer, size );
}
