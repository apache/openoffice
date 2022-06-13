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



#ifndef INCLUDED_CURLSESSION_HXX
#define INCLUDED_CURLSESSION_HXX

#include <vector>
#include <boost/shared_ptr.hpp>
#include <osl/mutex.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/logging.hxx>
#include "DAVResource.hxx"
#include "DAVSession.hxx"
#include "CurlTypes.hxx"
#include "CurlRequest.hxx"
#include "CurlLockStore.hxx"
#include "CurlUri.hxx"
#include "CurlInputStream.hxx"
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <curl/curl.h>
#include <openssl/ssl.h>

namespace ucbhelper { class ProxyDecider; }

namespace http_dav_ucp
{

// -------------------------------------------------------------------
// CurlSession
// A DAVSession implementation using the Curl library
// -------------------------------------------------------------------

class CurlSession : public DAVSession
{
private:
    osl::Mutex                      m_aMutex;
    ::comphelper::ComponentContext  m_aContext;
    ::comphelper::EventLogger       m_aLogger;

    CurlUri                 m_aUri;

    rtl::OUString           m_aProxyName;
    sal_Int32               m_nProxyPort;
    // The server, according RFC7231
    // http://tools.ietf.org/html/rfc7231#section-7.4.2
    rtl::OString            m_aServerHeaderField;

    CURL*                   m_pCurl;
    bool                    m_bUseChunkedEncoding;
    bool                    m_bTransferEncodingSwitched;

    const ucbhelper::InternetProxyDecider & m_rProxyDecider;

    DAVRequestEnvironment m_aEnv;

    static CurlLockStore m_aCurlLockStore;

    bool isSSLNeeded();


    rtl::OUString           composeCurrentUri( const rtl::OUString & inPath );
    void                    addEnvironmentRequestHeaders( CurlRequest &curlRequest,
                                                          const DAVRequestEnvironment &env )
                                throw ( DAVException );
    void                    processResponse( CurlRequest &curlRequest,
                                             CURLcode curlCode )
                                throw ( DAVException );

    static CURLcode         Curl_SSLContextCallback( CURL *curl,
                                                     void *ssl_ctx,
                                                     void *userptr );
    static int              OPENSSL_ValidateServerCertificate( int preverify_ok,
                                                               X509_STORE_CTX *x509_ctx );
    int                     validateServerX509Certificate( X509_STORE_CTX *x509StoreContext,
                                                           int preverifyOk );
    int                     verifyCertificateChain (
                                std::vector< uno::Sequence< sal_Int8 > > &asn1DerCertificates );

    static int              Curl_DebugCallback( CURL *,
                                                curl_infotype type,
                                                unsigned char *data,
                                                size_t size,
                                                void* userdata );
    int                     curlDebugOutput( curl_infotype type, char *data, int size );

    static bool             Curl_ProvideCredentials( long statusCode,
                                                     void *userdata ) throw (DAVException);
    bool                    provideCredentials( const DAVRequestEnvironment &env,
                                                CurlRequest &request,
                                                long statusCode ) throw (DAVException);

protected:
    virtual ~CurlSession();

public:
    CurlSession( const rtl::Reference< DAVSessionFactory > & rSessionFactory,
                 const rtl::OUString& inUri,
                 const ucbhelper::InternetProxyDecider & rProxyDecider )
        throw ( DAVException );

    // DAVSession methods
    virtual sal_Bool CanUse( const ::rtl::OUString & inUri );

    virtual sal_Bool UsesProxy();

    const DAVRequestEnvironment & getRequestEnvironment() const
    { return m_aEnv; }

    // allprop & named
    virtual void
    PROPFIND( const ::rtl::OUString & inPath,
              const Depth inDepth,
              const std::vector< ::rtl::OUString > & inPropNames,
              std::vector< DAVResource > & ioResources,
              const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    // propnames
    virtual void
    PROPFIND( const ::rtl::OUString & inPath,
              const Depth inDepth,
              std::vector< DAVResourceInfo >& ioResInfo,
              const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    PROPPATCH( const ::rtl::OUString & inPath,
               const std::vector< ProppatchValue > & inValues,
               const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    HEAD( const ::rtl::OUString &  inPath,
          const std::vector< ::rtl::OUString > & inHeaderNames,
          DAVResource & ioResource,
          const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
    GET( const ::rtl::OUString & inPath,
         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    GET( const ::rtl::OUString & inPath,
         com::sun::star::uno::Reference<
             com::sun::star::io::XOutputStream > &  ioOutputStream,
         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
    GET( const ::rtl::OUString & inPath,
         const std::vector< ::rtl::OUString > & inHeaderNames,
         DAVResource & ioResource,
         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    GET( const ::rtl::OUString & inPath,
         com::sun::star::uno::Reference<
             com::sun::star::io::XOutputStream > & ioOutputStream,
         const std::vector< ::rtl::OUString > & inHeaderNames,
         DAVResource & ioResource,
         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    PUT( const ::rtl::OUString & inPath,
         const com::sun::star::uno::Reference<
             com::sun::star::io::XInputStream > & inInputStream,
         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
    POST( const rtl::OUString & inPath,
          const rtl::OUString & rContentType,
          const rtl::OUString & rReferer,
          const com::sun::star::uno::Reference<
              com::sun::star::io::XInputStream > & inInputStream,
          const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    POST( const rtl::OUString & inPath,
          const rtl::OUString & rContentType,
          const rtl::OUString & rReferer,
          const com::sun::star::uno::Reference<
              com::sun::star::io::XInputStream > & inInputStream,
          com::sun::star::uno::Reference<
              com::sun::star::io::XOutputStream > & oOutputStream,
          const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    MKCOL( const ::rtl::OUString & inPath,
           const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void
    COPY( const ::rtl::OUString & inSourceURL,
          const ::rtl::OUString & inDestinationURL,
          const DAVRequestEnvironment & rEnv,
          sal_Bool inOverWrite )
        throw ( DAVException );

    virtual void
    MOVE( const ::rtl::OUString & inSourceURL,
          const ::rtl::OUString & inDestinationURL,
          const DAVRequestEnvironment & rEnv,
          sal_Bool inOverWrite )
        throw ( DAVException );

    virtual void DESTROY( const ::rtl::OUString & inPath,
                          const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    // set new lock.
    virtual void LOCK( const ::rtl::OUString & inURL,
                       com::sun::star::ucb::Lock & inLock,
                       const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    // refresh existing lock.
    virtual sal_Int64 LOCK( const ::rtl::OUString & inURL,
                            sal_Int64 nTimeout,
                            const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    virtual void UNLOCK( const ::rtl::OUString & inURL,
                         const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    // helpers
    virtual void abort()
        throw ( DAVException );

    const rtl::OUString & getHostName() const { return m_aUri.GetHost(); }
    int getPort() const { return m_aUri.GetPort(); }

    const ::uno::Reference< ::lang::XMultiServiceFactory > getMSF()
    { return m_xFactory->getServiceFactory(); }

    sal_Bool isDomainMatch( rtl::OUString certHostName );

    const rtl::OString & getServerHeaderField() { return m_aServerHeaderField; };

private:
    friend class CurlLockStore;

    void Init( void )
        throw ( DAVException );

    void Init( const DAVRequestEnvironment & rEnv )
        throw ( DAVException );

    const ucbhelper::InternetProxyServer & getProxySettings() const;
    
    void propfind( CurlRequest &curlRequest,
                   const rtl::OUString &inPath,
                   const Depth inDepth,
                   const std::vector< ::rtl::OUString > * propNames,
                   const bool onlyPropertyNames,
                   const DAVRequestEnvironment & rEnv );

    bool removeExpiredLocktoken( const rtl::OUString & inURL,
                                 const DAVRequestEnvironment & rEnv );

    // refresh lock, called by CurlLockStore::refreshLocks
    bool LOCK( CurlLock * pLock,
               sal_Int32 & rlastChanceToSendRefreshRequest );

    // unlock, called by CurlLockStore::~CurlLockStore
    bool UNLOCK( CurlLock * pLock );

    /*
    // low level GET implementation, used by public GET implementations
    static int GET( CurlConnection * sess,
                    const char * uri,
                    //ne_block_reader reader,
                    bool getheaders,
                    void * userdata );

    // Buffer-based PUT implementation. Serf only has file descriptor-
    // based API.
    static int PUT( CurlConnection * sess,
                    const char * uri,
                    const char * buffer,
                    size_t size );

    // Buffer-based POST implementation. Serf only has file descriptor-
    // based API.
    int POST( CurlConnection * sess,
              const char * uri,
              const char * buffer,
              //ne_block_reader reader,
              void * userdata,
              const rtl::OUString & rContentType,
              const rtl::OUString & rReferer );
    */

    // Helper: XInputStream -> Sequence< sal_Int8 >
    static bool getDataFromInputStream(
        const com::sun::star::uno::Reference<
            com::sun::star::io::XInputStream > & xStream,
        com::sun::star::uno::Sequence< sal_Int8 > & rData,
        bool bAppendTrailingZeroByte );

    /*
    rtl::OUString makeAbsoluteURL( rtl::OUString const & rURL ) const;
    */
};

} // namespace http_dav_ucp

#endif // INCLUDED_CURLSESSION_HXX
