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

#ifndef INCLUDED_CURLREQUEST_HXX
#define INCLUDED_CURLREQUEST_HXX

#include <vector>
#include "DAVException.hxx"
#include <com/sun/star/io/XOutputStream.hpp>
#include "CurlInputStream.hxx"
#include "CurlTypes.hxx"
#include "CurlUri.hxx"
#include <curl/curl.h>

namespace http_dav_ucp
{

/* Since Curl persists all settings for the duration of
 * the CURL* structure, and we need some of them to only last for
 * the duration of a request, use this class to reset the short-lived
 * settings at the end of each request using RAII, and to capture
 * some results in a convenient format.
 *
 * Features are opt-in to their non-default values.
 *
 * External pointers (CURL*, request body, etc.) are only shared with us,
 * never copied, temporarily used by us, and never belong to us.
 */
class CurlRequest
{
public:
    class Header
    {
    public:
        rtl::OString name;
        rtl::OString value;
    };
    
    CurlRequest( CURL *curl );
    ~CurlRequest();

    // Request setup methods:
    void addHeader( const rtl::OString &name, const rtl::OString &value ) throw (DAVException);
    void setRequestBody( const char *body, size_t size );
    bool isChunkedEncoding() { return useChunkedEncoding; }
    void setChunkedEncoding( bool isChunkedEncoding ) { useChunkedEncoding = isChunkedEncoding; }
    void saveResponseBodyTo( const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream > & xOutputStream );
    void setProvideCredentialsCallback(
        bool (*provideCredsCallback)(long statusCode, void *userdata) throw (DAVException),
        void *credsUserdata );

    // Requests
    CURLcode copy( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode delete_( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode get( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode head( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode lock( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode mkcol( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode move( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode post( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode propfind( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode proppatch( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode put( CurlUri uri, rtl::OUString path ) throw(DAVException);
    CURLcode unlock( CurlUri uri, rtl::OUString path ) throw(DAVException);

    // Response methods:
    int getStatusCode() { return statusCode; }
    rtl::OString& getReasonPhrase() { return reasonPhrase; }
    // TODO: a multimap would be O(n * log(m)) instead of O(n * m)
    std::vector<Header>& getResponseHeaders() { return responseHeaders; }
    const Header *findResponseHeader( const rtl::OString &name );
    com::sun::star::uno::Reference < CurlInputStream > getResponseBody() { return responseBodyInputStream; }
    
private:
    CurlRequest( const CurlRequest &curlRequest ); // No copy constructor.
    void setURI( CurlUri uri, rtl::OUString path ) throw (DAVException);
    CURLcode perform() throw (DAVException);
    
    static int Curl_SeekCallback(void *userp, curl_off_t offset, int origin);

    static size_t Curl_SendMoreBody( char *buffer, size_t size, size_t nitems, void *userdata );
    size_t curlSendMoreBody( char *buffer, size_t len );

    static size_t Curl_HeaderReceived( char *buffer, size_t size, size_t nitems, void *userdata );
    void curlHeaderReceived( const char *buffer, size_t len );

    static size_t Curl_MoreBodyReceived( char *buffer, size_t size, size_t nitems, void *userdata );
    void curlMoreBodyReceived( const char *buffer, size_t len );

    CURL *curl;
    CURLU *curlUrl;
    
    // Request values:
    curl_slist *requestHeaders;
    const char *requestBody;
    size_t requestBodyOffset;
    size_t requestBodySize;
    bool useChunkedEncoding;
    bool (*provideCredentialsCallback)( long statusCode, void *userdata ) throw (DAVException);
    void *provideCredentialsUserdata;
    
    // Response values:
    rtl::OString reasonPhrase;
    int statusCode;
    std::vector<Header> responseHeaders;
    com::sun::star::uno::Reference < CurlInputStream > responseBodyInputStream;
    com::sun::star::uno::Reference< com::sun::star::io::XOutputStream > xOutputStream;
};

} // namespace http_dav_ucp

#endif // INCLUDED_CURLREQUEST_HXX