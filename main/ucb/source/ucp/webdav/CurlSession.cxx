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

#include <hash_map>
#include <vector>
#include <string.h>
#include <rtl/string.h>
#include <rtl/strbuf.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/time.h>
#include "comphelper/sequence.hxx"
#include "ucbhelper/simplecertificatevalidationrequest.hxx"

#include "DAVAuthListener.hxx"
#include "CurlTypes.hxx"
#include "CurlSession.hxx"
#include "LockRequest.hxx"
#include "PropfindRequest.hxx"
#include "ProppatchRequest.hxx"
#include "CurlInputStream.hxx"
#include "UCBDeadPropertyValue.hxx"
#include "webdavuseragent.hxx"
#include "webdavresponseparser.hxx"
#include "webdavprovider.hxx"


#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/security/XCertificate.hpp>
#include <com/sun/star/security/CertificateValidity.hpp>
#include <com/sun/star/security/CertificateContainerStatus.hpp>
#include <com/sun/star/security/CertificateContainer.hpp>
#include <com/sun/star/security/XCertificateContainer.hpp>
#include <com/sun/star/security/CertAltNameEntry.hpp>
#include <com/sun/star/security/XSanExtension.hpp>
#include <com/sun/star/ucb/Lock.hpp>
#include <com/sun/star/xml/crypto/XSEInitializer.hpp>

using namespace com::sun::star;
using namespace com::sun::star::logging;
using namespace http_dav_ucp;

#define OID_SUBJECT_ALTERNATIVE_NAME "2.5.29.17"

struct CredentialsData
{
    CredentialsData( CurlSession *curlSession, CurlRequest &curlRequest, const DAVRequestEnvironment &requestEnvironment )
    : session( curlSession)
    , request( curlRequest )
    , env( requestEnvironment )
    {}

    CurlSession *session;
    CurlRequest &request;
    const DAVRequestEnvironment &env;
};

// -------------------------------------------------------------------
// static members!
CurlLockStore CurlSession::m_aCurlLockStore;


// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
CurlSession::CurlSession(
        const rtl::Reference< DAVSessionFactory > & rSessionFactory,
        const rtl::OUString& inUri,
        const ucbhelper::InternetProxyDecider & rProxyDecider )
    throw ( DAVException )
    : DAVSession( rSessionFactory )
    , m_aMutex()
    , m_aContext( m_xFactory->getServiceFactory() )
    , m_aLogger( m_aContext.getUNOContext(), WEBDAV_CONTENT_PROVIDER_SERVICE_NAME )
    , m_aUri( inUri )
    , m_aProxyName()
    , m_nProxyPort( 0 )
    , m_aServerHeaderField()
    , m_pCurl( 0 )
    , m_bUseChunkedEncoding( false )
    , m_bTransferEncodingSwitched( false )
    , m_rProxyDecider( rProxyDecider )
    , m_aEnv()
{
    m_pCurl = curl_easy_init();
    
    curl_easy_setopt( m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_ANY );
    curl_easy_setopt( m_pCurl, CURLOPT_PROXYAUTH, CURLAUTH_ANY );
    
    curl_easy_setopt( m_pCurl, CURLOPT_SSL_CTX_FUNCTION, Curl_SSLContextCallback );
    curl_easy_setopt( m_pCurl, CURLOPT_SSL_CTX_DATA, this );

    if ( m_aLogger.getLogLevel() == LogLevel::FINEST )
    {
        curl_easy_setopt( m_pCurl, CURLOPT_DEBUGFUNCTION, Curl_DebugCallback );
        curl_easy_setopt( m_pCurl, CURLOPT_DEBUGDATA, this );
        curl_easy_setopt( m_pCurl, CURLOPT_VERBOSE, 1L);
    }
    m_aLogger.log( LogLevel::INFO, "CurlSession::CurlSession with URL $1$",
        rtl::OUStringToOString( inUri, RTL_TEXTENCODING_UTF8 ).getStr() );
}

// -------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------
CurlSession::~CurlSession( )
{
    if ( m_pCurl )
    {
        curl_easy_cleanup( m_pCurl );
        m_pCurl = 0;
        m_aLogger.log( LogLevel::INFO, "CurlSession::~CurlSession: closed curl session");
    }
}

// -------------------------------------------------------------------
void CurlSession::Init( const DAVRequestEnvironment & rEnv )
  throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    m_aEnv = rEnv;
    Init();
}

// -------------------------------------------------------------------
void CurlSession::Init()
    throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    const sal_Char *url = rtl::OUStringToOString( m_aUri.GetURI(), RTL_TEXTENCODING_UTF8 ).getStr();
    CURLcode rc;
    rc = curl_easy_setopt( m_pCurl, CURLOPT_URL, url );
    if ( rc != CURLE_OK  )
        throw DAVException( DAVException::DAV_SESSION_CREATE,
                            CurlUri::makeConnectionEndPointString( m_aUri.GetHost(), m_aUri.GetPort() ) );

    const ucbhelper::InternetProxyServer & rProxyCfg = getProxySettings();
    if ( ( rProxyCfg.aName != m_aProxyName )
        || ( rProxyCfg.nPort != m_nProxyPort ) )
    {
        m_aProxyName = rProxyCfg.aName;
        m_nProxyPort = rProxyCfg.nPort;
        if ( !m_aProxyName.isEmpty() )
        {
            m_aLogger.log( LogLevel::INFO, "Using $1$ proxy server at $2$:$3$",
                m_aUri.GetScheme(), m_aProxyName, m_nProxyPort );
            curl_easy_setopt( m_pCurl, CURLOPT_PROXY, rtl::OUStringToOString( m_aProxyName, RTL_TEXTENCODING_UTF8 ).getStr() );
            curl_easy_setopt( m_pCurl, CURLOPT_PROXYPORT, (long)m_nProxyPort );
            if ( m_aUri.GetScheme().equalsAscii( "https" ) )
                curl_easy_setopt( m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS );
            else
                curl_easy_setopt( m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP );
            // no other proxy types are implemented by AOO
        }
        else
        {
            // Empty string as opposed to NULL, means don't use the default curl proxy.
            m_aLogger.log( LogLevel::INFO, "Not using a proxy server" );
            curl_easy_setopt( m_pCurl, CURLOPT_PROXY, "" );
        }
        // if we change the proxy settings, clear the credentials for the previous proxy too
        curl_easy_setopt( m_pCurl, CURLOPT_PROXYUSERNAME, "" );
        curl_easy_setopt( m_pCurl, CURLOPT_PROXYPASSWORD, "" );
    }
}

bool CurlSession::isSSLNeeded()
{
    return m_aUri.GetScheme().equalsIgnoreAsciiCase( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "https" ) ) );
}

// -------------------------------------------------------------------
// helper function
// it composes the uri for lockstore registration
rtl::OUString CurlSession::composeCurrentUri(const rtl::OUString & inPath)
{
    rtl::OUString aScheme( m_aUri.GetScheme() );
    rtl::OUStringBuffer aBuf( aScheme );
    aBuf.appendAscii( "://" );
    if ( m_aUri.GetUserName().getLength() > 0 )
    {
        aBuf.append( m_aUri.GetUserName() );
        if ( m_aUri.GetPassword().getLength() > 0 )
        {
            aBuf.appendAscii( ":" );
            aBuf.append( m_aUri.GetPassword() );
        }
        aBuf.appendAscii( "@" );
    }
    // Is host a numeric IPv6 address?
    if ( ( m_aUri.GetHost().indexOf( ':' ) != -1 ) &&
         ( m_aUri.GetHost()[ 0 ] != sal_Unicode( '[' ) ) )
    {
        aBuf.appendAscii( "[" );
        aBuf.append( m_aUri.GetHost() );
        aBuf.appendAscii( "]" );
    }
    else
    {
        aBuf.append( m_aUri.GetHost() );
    }

    // append port, but only, if not default port.
    bool bAppendPort = true;
    sal_Int32 aPort = m_aUri.GetPort();
    switch ( aPort )
    {
    case DEFAULT_HTTP_PORT:
        bAppendPort = aScheme.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "http" ) );
        break;

    case DEFAULT_HTTPS_PORT:
        bAppendPort = !aScheme.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "https" ) );
        break;
    }
    if ( bAppendPort )
    {
        aBuf.appendAscii( ":" );
        aBuf.append( rtl::OUString::valueOf( aPort ) );
    }
    aBuf.append( inPath );

    rtl::OUString   aUri(aBuf.makeStringAndClear() );
    return aUri;
}

// -------------------------------------------------------------------
// virtual
sal_Bool CurlSession::CanUse( const rtl::OUString & inUri )
{
    try
    {
        CurlUri theUri( inUri );
        if ( ( theUri.GetPort() == m_aUri.GetPort() ) &&
             ( theUri.GetHost() == m_aUri.GetHost() ) &&
             ( theUri.GetScheme() == m_aUri.GetScheme() ) )
        {
            return sal_True;
        }
    }
    catch ( DAVException const & )
    {
        return sal_False;
    }
    return sal_False;
}

// -------------------------------------------------------------------
// virtual
sal_Bool CurlSession::UsesProxy()
{
    Init();
    return ( m_aProxyName.getLength() > 0 );
}

int CurlSession::Curl_DebugCallback( CURL *, curl_infotype type, unsigned char *data, size_t size, void* userdata )
{
    CurlSession *session = static_cast< CurlSession* >( userdata );
    return session->curlDebugOutput( type, reinterpret_cast<char*>( data ), size );
}

int CurlSession::curlDebugOutput( curl_infotype type, char *data, int size )
{
    const char *prefix;
    switch ( type )
    {
        case CURLINFO_TEXT:
            prefix = "[CurlINFO  ]";
            break;
        case CURLINFO_HEADER_IN:
            prefix = "[CurlHDR <-]";
            break;
        case CURLINFO_HEADER_OUT:
            prefix = "[CurlHDR ->]";
            break;
        case CURLINFO_DATA_IN:
            prefix = "[CurlData<-]";
            break;
        case CURLINFO_DATA_OUT:
            prefix = "[CurlData->]";
            break;
        default:
            return 0;
    }

    // Trim the trailing \r\n
    if ( size >= 1 && ( data[size - 1] == '\r' || data[size - 1] == '\n' ) )
        --size;
    if ( size >= 1 && ( data[size - 1] == '\r' || data[size - 1] == '\n' ) )
        --size;
    rtl::OString message( data, size );
    m_aLogger.log( LogLevel::FINEST, "$1$ $2$", prefix, message );
    return 0;
}

CURLcode CurlSession::Curl_SSLContextCallback( CURL *, void *ssl_ctx, void *userptr )
{
    CurlSession *session = static_cast<CurlSession*>( userptr );
    SSL_CTX *context = static_cast<SSL_CTX*>( ssl_ctx );
    SSL_CTX_set_app_data( context, session );
    SSL_CTX_set_verify( context, SSL_VERIFY_PEER, OPENSSL_ValidateServerCertificate );
    return CURLE_OK;
}

int CurlSession::OPENSSL_ValidateServerCertificate( int preverify_ok, X509_STORE_CTX *x509_ctx )
{
    SSL *ssl = static_cast<SSL*> (
        X509_STORE_CTX_get_ex_data( x509_ctx, SSL_get_ex_data_X509_STORE_CTX_idx() ) );
    SSL_CTX *ssl_ctx = SSL_get_SSL_CTX( ssl );
    CurlSession *session = static_cast<CurlSession*>( SSL_CTX_get_app_data( ssl_ctx ) );
    int verifyOk = session->validateServerX509Certificate( x509_ctx, preverify_ok );
    // When a certificate's verification fails within OpenSSL, yet passes from the
    // SSL_CTX_set_verify() callback (ie. this function) (by returning 1),
    // OpenSSL allows the connection to proceed, yet stores that last verification
    // error and allows it to be retrieved using SSL_get_verify_result(3).
    //
    // Unfortunately, Curl calls SSL_get_verify_result(3) internally, and treats
    // errors as terminal, disconnecting with an error even when we return 1 here.
    // Therefore, to approve a certificate that OpenSSL would reject, we have to
    // both return 1, and overwrite the X509_STORE_CTX's last error with X509_V_OK:
    if ( verifyOk )
        X509_STORE_CTX_set_error( x509_ctx, X509_V_OK );
    return verifyOk;
}

static uno::Sequence< sal_Int8 > convertCertificateToAsn1Der( X509 *certificate )
{
    uno::Sequence< sal_Int8 > asn1DerCertificate;
    int len = i2d_X509( certificate, NULL );
    if ( len < 0 )
        return asn1DerCertificate;
    asn1DerCertificate.realloc( len );
    unsigned char *end = reinterpret_cast< unsigned char *>( asn1DerCertificate.getArray() );
    len = i2d_X509( certificate, &end );
    if ( len >= 0 )
        return asn1DerCertificate;
    else
        return uno::Sequence< sal_Int8 >();
}

int CurlSession::validateServerX509Certificate( X509_STORE_CTX *x509StoreContext, int verifyOk )
{
    X509 *serverCertificate = X509_STORE_CTX_get_current_cert( x509StoreContext );
    int depth = X509_STORE_CTX_get_error_depth( x509StoreContext );
    STACK_OF(X509) *chain =
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
         X509_STORE_CTX_get0_chain( x509StoreContext );
#else
         X509_STORE_CTX_get_chain( x509StoreContext );
#endif
    
    std::vector< uno::Sequence< sal_Int8 > > asn1DerCertificates;
    if ( chain != NULL ) {
        int nCertificates = sk_X509_num( chain );
        for ( int i = 0; i < nCertificates; i++ ) {
            X509 *certificate = sk_X509_value( chain, i );
            uno::Sequence< sal_Int8 > asn1DerCertificate = convertCertificateToAsn1Der( certificate );
            if ( asn1DerCertificate.getLength() == 0 )
                return 0;
            asn1DerCertificates.push_back( asn1DerCertificate );
        }
    } else {
        uno::Sequence< sal_Int8 > asn1DerCertificate = convertCertificateToAsn1Der( serverCertificate );
        if ( asn1DerCertificate.getLength() == 0 )
            return 0;
        asn1DerCertificates.push_back( asn1DerCertificate );
    }
    verifyOk = verifyCertificateChain( asn1DerCertificates );

    m_aLogger.log( LogLevel::FINE, "validateServerX509Certificate() returning $1$ at depth $2$",
        (sal_Int32)verifyOk, (sal_Int32)depth );
    return verifyOk;
}

int CurlSession::verifyCertificateChain (
    std::vector< uno::Sequence< sal_Int8 > > &asn1DerCertificates )
{
    // Check arguments.
    if (asn1DerCertificates.size()<=0)
    {
        OSL_ASSERT(asn1DerCertificates.size()>0);
        return 0;
    }

    // Create some crypto objects to decode and handle the base64
    // encoded certificate chain.
    uno::Reference< xml::crypto::XSEInitializer > xSEInitializer;
    uno::Reference< security::XCertificateContainer > xCertificateContainer;
    uno::Reference< xml::crypto::XXMLSecurityContext > xSecurityContext;
    uno::Reference< xml::crypto::XSecurityEnvironment > xSecurityEnv;
    try
    {
        // Create a certificate container.
        xCertificateContainer = uno::Reference< security::XCertificateContainer >(
            getMSF()->createInstance(
                rtl::OUString::createFromAscii(
                    "com.sun.star.security.CertificateContainer" ) ),
            uno::UNO_QUERY_THROW);

        xSEInitializer = uno::Reference< xml::crypto::XSEInitializer >(
            getMSF()->createInstance(
                rtl::OUString::createFromAscii( "com.sun.star.xml.crypto.SEInitializer" ) ),
            uno::UNO_QUERY_THROW);

        xSecurityContext = xSEInitializer->createSecurityContext( rtl::OUString() );
        if (xSecurityContext.is())
            xSecurityEnv = xSecurityContext->getSecurityEnvironment();

        if ( ! xSecurityContext.is() || ! xSecurityEnv.is())
        {
            // Do we have to dispose xSEInitializer or xCertificateContainer?
            m_aLogger.log( LogLevel::WARNING, "Failure creating security services for certificate verification" );
            return 0;
        }
    }
    catch ( uno::Exception const &e)
    {
        m_aLogger.log( LogLevel::WARNING, "Error creating security services: $1$", e.Message );
        return 0;
    }

    // Decode the server certificate.
    uno::Reference< security::XCertificate > xServerCertificate(
        xSecurityEnv->createCertificateFromRaw( asn1DerCertificates[0] ) );
    if ( ! xServerCertificate.is())
    {
        m_aLogger.log( LogLevel::WARNING, "Failed to create XCertificate" );
        return 0;
    }

    // Get the subject from the server certificate.
    ::rtl::OUString sServerCertificateSubject (xServerCertificate->getSubjectName());
    sal_Int32 nIndex = 0;
    while (nIndex >= 0)
    {
        const ::rtl::OUString sToken (sServerCertificateSubject.getToken(0, ',', nIndex));
        if (sToken.compareToAscii("CN=", 3) == 0)
        {
            sServerCertificateSubject = sToken.copy(3);
            break;
        }
        else if (sToken.compareToAscii(" CN=", 4) == 0)
        {
            sServerCertificateSubject = sToken.copy(4);
            break;
        }
    }

    // When the certificate container already contains a (trusted)
    // entry for the server then we do not have to authenticate any
    // certificate.
    const security::CertificateContainerStatus eStatus (
        xCertificateContainer->hasCertificate(
            getHostName(), sServerCertificateSubject ) );
    if (eStatus != security::CertificateContainerStatus_NOCERT)
    {
        m_aLogger.log( LogLevel::FINER, "Cached certificate found with status=$1$",
                eStatus == security::CertificateContainerStatus_TRUSTED ? "trusted" : "untrusted" );
        return eStatus == security::CertificateContainerStatus_TRUSTED
               ? 1
               : 0;
    }

    // The shortcut failed, so try to verify the whole chain. This is
    // done outside the isDomainMatch() block because the result is
    // used by the interaction handler.
    std::vector< uno::Reference< security::XCertificate > > aChain;
    for (nIndex=0; nIndex < asn1DerCertificates.size(); ++nIndex)
    {
        uno::Reference< security::XCertificate > xCertificate(
            xSecurityEnv->createCertificateFromRaw( asn1DerCertificates[ nIndex ] ) );
        if ( ! xCertificate.is())
        {
            m_aLogger.log( LogLevel::FINE, "Failed to create XCertificate $1$", nIndex );
            return 0;
        }
        aChain.push_back(xCertificate);
    }
    const sal_Int64 nVerificationResult (xSecurityEnv->verifyCertificate(
            xServerCertificate,
            ::comphelper::containerToSequence(aChain)));

    // When the certificate matches the host name then we can use the
    // result of the verification.
    bool bHostnameMatchesCertHostnames = false;
    {
        uno::Sequence< uno::Reference< security::XCertificateExtension > > extensions = xServerCertificate->getExtensions();
        uno::Sequence< security::CertAltNameEntry > altNames;
        for (sal_Int32 i = 0 ; i < extensions.getLength(); ++i)
        {
            uno::Reference< security::XCertificateExtension >element = extensions[i];

            const rtl::OString aId ( (const sal_Char *)element->getExtensionId().getArray(), element->getExtensionId().getLength());
            if ( aId.equals( OID_SUBJECT_ALTERNATIVE_NAME ) )
            {
                uno::Reference< security::XSanExtension > sanExtension ( element, uno::UNO_QUERY );
                altNames = sanExtension->getAlternativeNames();
                break;
            }
        }

        uno::Sequence< ::rtl::OUString > certHostNames(altNames.getLength() + 1);
        certHostNames[0] = sServerCertificateSubject;
        for( int n = 0; n < altNames.getLength(); ++n )
        {
            if (altNames[n].Type == security::ExtAltNameType_DNS_NAME)
            {
                altNames[n].Value >>= certHostNames[n+1];
            }
        }

        for ( int i = 0; i < certHostNames.getLength() && !bHostnameMatchesCertHostnames; ++i )
        {
            bHostnameMatchesCertHostnames = isDomainMatch( certHostNames[i] );
        }

    }
    m_aLogger.log( LogLevel::FINE, "URL hostname $1$ certificate hostname",
        bHostnameMatchesCertHostnames ? "matches" : "DOESN'T MATCH" );
    if ( bHostnameMatchesCertHostnames )
    {
        if (nVerificationResult == 0)
        {
            m_aLogger.log( LogLevel::FINE, "Certificate (chain) is valid" );
            xCertificateContainer->addCertificate(getHostName(), sServerCertificateSubject, sal_True);
            return 1;
        }
        else if ((nVerificationResult & security::CertificateValidity::CHAIN_INCOMPLETE) != 0)
        {
            // We do not have enough information for verification,
            // neither automatically (as we just discovered) nor
            // manually (so there is no point in showing any dialog.)
            m_aLogger.log( LogLevel::WARNING, "Certificate (chain) is incomplete" );
            return 0;
        }
        else if ((nVerificationResult & security::CertificateValidity::REVOKED) != 0)
        {
            // Certificate (chain) is invalid.
            m_aLogger.log( LogLevel::WARNING, "Certificate (chain) is revoked" );
            xCertificateContainer->addCertificate(getHostName(), sServerCertificateSubject,  sal_False);
            return 0;
        }
        else
        {
            // For all other we have to ask the user.
            m_aLogger.log( LogLevel::FINE, "Promping user to validate the certificate" );
        }
    }

    // We have not been able to automatically verify (or falsify) the
    // certificate chain. To resolve this we have to ask the user.
    const uno::Reference< ucb::XCommandEnvironment > xEnv( getRequestEnvironment().m_xEnv );
    if ( xEnv.is() )
    {
        uno::Reference< task::XInteractionHandler > xIH( xEnv->getInteractionHandler() );
        if ( xIH.is() )
        {
            rtl::Reference< ucbhelper::SimpleCertificateValidationRequest >
                xRequest( new ucbhelper::SimpleCertificateValidationRequest(
                        static_cast<sal_Int32>(nVerificationResult), xServerCertificate, getHostName() ) );
            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
            {
                uno::Reference< task::XInteractionApprove > xApprove( xSelection.get(), uno::UNO_QUERY );
                if ( xApprove.is() )
                {
                    m_aLogger.log( LogLevel::FINE, "The user approved the certificate" );
                    xCertificateContainer->addCertificate( getHostName(), sServerCertificateSubject, sal_True );
                    return 1;
                }
                else
                {
                    // Don't trust cert
                    m_aLogger.log( LogLevel::FINE, "The user REJECTED the certificate" );
                    xCertificateContainer->addCertificate( getHostName(), sServerCertificateSubject, sal_False );
                    return 0;
                }
            }
        }
        else
        {
            // Don't trust cert
            m_aLogger.log( LogLevel::WARNING, "Couldn't create the interaction handler for user feedback, rejecting the certificate" );
            xCertificateContainer->addCertificate( getHostName(), sServerCertificateSubject, sal_False );
            return 0;
        }
    }
    m_aLogger.log( LogLevel::WARNING, "No XCommandEnvironment, rejecting the certificate" );

    return 0;
}

bool CurlSession::Curl_ProvideCredentials( long statusCode, void *userdata ) throw (DAVException)
{
    CredentialsData *credentialsData = (CredentialsData*)userdata;
    return credentialsData->session->provideCredentials( credentialsData->env, credentialsData->request, statusCode );
}

bool CurlSession::provideCredentials( const DAVRequestEnvironment &env, CurlRequest &request, long statusCode ) throw (DAVException)
{
    DAVAuthListener * pListener = env.m_xAuthListener.get();
    if ( !pListener )
    {
        // abort
        m_aLogger.log( LogLevel::FINE, "No DAVAuthListener found, failing credentials entry" );
        return false;
    }

    rtl::OUString theUserName;
    rtl::OUString thePassWord;
    try
    {
        CurlUri uri( env.m_aRequestURI );
        theUserName = uri.GetUserName();
        thePassWord = uri.GetPassword();
    }
    catch ( DAVException const &e )
    {
        // abort
        m_aLogger.log(
            LogLevel::WARNING,
            "Error extracing userinfo from URI: exceptionCode=$1$, status=$2$, data=$3$, owner=$4$, extendedError=$5%",
            (sal_Int32)e.getError(), e.getStatus(), e.getData(), e.getOwner(), e.getExtendedError()
        );
        return false;
    }
    
    bool canUseSystemCreds = false;
    long authMethods = 0;
    CURLcode rc = CURLE_OK;
    if ( statusCode == 401 )
        rc = curl_easy_getinfo( m_pCurl, CURLINFO_HTTPAUTH_AVAIL, &authMethods );
    else if ( statusCode == 407 )
        rc = curl_easy_getinfo( m_pCurl, CURLINFO_PROXYAUTH_AVAIL, &authMethods );
    if ( rc == 0 )
        canUseSystemCreds = (authMethods & CURLAUTH_NEGOTIATE) || (authMethods & CURLAUTH_NTLM);
    m_aLogger.log( LogLevel::FINE, "authMethods=$1$, canUseSystemCreds=$2$",
        (sal_Int64)authMethods, (sal_Int32)canUseSystemCreds );
    
    const CurlRequest::Header *authHeader = NULL;
    if ( statusCode == 401 )
        authHeader = request.findResponseHeader( "WWW-Authenticate" );
    else if ( statusCode == 407 )
        authHeader = request.findResponseHeader( "Proxy-Authenticate" );
    rtl::OUString realm;
    if ( authHeader != NULL )
    {
        int realmStart = authHeader->value.indexOf( "realm=\"" );
        if ( realmStart >= 0 )
        {
            realmStart += 7;
            int realmEnd = authHeader->value.indexOf( "\"", realmStart );
            if ( realmEnd > 0 )
                realm = rtl::OStringToOUString( authHeader->value.copy( realmStart, realmEnd - realmStart ), RTL_TEXTENCODING_UTF8 );
        }
    }
    
    int theRetVal = pListener->authenticate( realm,
                                             getHostName(),
                                             theUserName,
                                             thePassWord,
                                             canUseSystemCreds,
                                             // Authenticating with both the proxy
                                             // and the destination server requires sal_True here,
                                             // and needs filling out 2 x password dialogs.
                                             sal_True );

    if ( theRetVal == 0 )
    {
        m_aLogger.log( LogLevel::FINEST, "got credentials for user=$1$ on realm=$2$", theUserName, realm );
        // "System credentials" means username and password are empty
        const char *curlUsername = NULL;
        const char *curlPassword = NULL;
        if ( !theUserName.isEmpty() )
            curlUsername = rtl::OUStringToOString( theUserName, RTL_TEXTENCODING_UTF8 ).getStr();
        if ( !thePassWord.isEmpty() )
            curlPassword = rtl::OUStringToOString( thePassWord, RTL_TEXTENCODING_UTF8 ).getStr();
        if ( statusCode == 401 )
        {
            curl_easy_setopt( m_pCurl, CURLOPT_USERNAME, curlUsername );
            curl_easy_setopt( m_pCurl, CURLOPT_PASSWORD, curlPassword );
        }
        else
        {
            curl_easy_setopt( m_pCurl, CURLOPT_PROXYUSERNAME, curlUsername );
            curl_easy_setopt( m_pCurl, CURLOPT_PROXYPASSWORD, curlPassword );
        }
        return true;
    }
    m_aLogger.log( LogLevel::WARNING, "credentials entry cancelled or failed" );

    return false;
}

void CurlSession::addEnvironmentRequestHeaders( CurlRequest &curlRequest, const DAVRequestEnvironment &env )
    throw ( DAVException )
{
    bool bHasUserAgent( false );
    DAVRequestHeaders::const_iterator aHeaderIter( env.m_aRequestHeaders.begin() );
    const DAVRequestHeaders::const_iterator aEnd( env.m_aRequestHeaders.end() );

    while ( aHeaderIter != aEnd )
    {
        const rtl::OString aHeader = rtl::OUStringToOString( aHeaderIter->first,
                                                             RTL_TEXTENCODING_UTF8 );
        const rtl::OString aValue = rtl::OUStringToOString( aHeaderIter->second,
                                                            RTL_TEXTENCODING_UTF8 );

        if ( !bHasUserAgent )
            bHasUserAgent = aHeaderIter->first.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "User-Agent" ) );

        curlRequest.addHeader( aHeader, aValue );
        
        ++aHeaderIter;
    }

    if ( !bHasUserAgent )
    {
        const rtl::OUString &rUserAgent = WebDAVUserAgent::get();
        curlRequest.addHeader( "User-Agent", rtl::OUStringToOString( rUserAgent, RTL_TEXTENCODING_UTF8 ) );
    }
}

void CurlSession::processResponse( CurlRequest &curlRequest, CURLcode curlCode )
    throw( DAVException )
{
    long statusCode = 0;
    CURLcode curlRes;
    curlRes = curl_easy_getinfo( m_pCurl, CURLINFO_RESPONSE_CODE, &statusCode );
    if ( curlRes != 0 || statusCode == 0 )
        statusCode = curlRequest.getStatusCode();

    // check header according:
    // http://tools.ietf.org/html/rfc7231#section-7.4.2
    // need to do this so we can adjust the protocol accordingly
    const CurlRequest::Header *server = curlRequest.findResponseHeader( "server" );
    if ( server != NULL )
        m_aServerHeaderField = server->value;

    if ( curlCode != 0 )
    {
        m_aLogger.log( LogLevel::WARNING, "Curl request failed with CURLcode $1$", (sal_Int64)curlCode );
        DAVException::ExceptionCode exCode = DAVException::DAV_HTTP_ERROR;
        rtl::OUString exData;
        switch (curlCode) {
        case CURLE_COULDNT_RESOLVE_HOST:
            exCode = DAVException::DAV_HTTP_LOOKUP;
            exData = CurlUri::makeConnectionEndPointString( getHostName(),
                                                            getPort() );
            break;
        case CURLE_COULDNT_CONNECT:
            exCode = DAVException::DAV_HTTP_CONNECT;
            exData = CurlUri::makeConnectionEndPointString( getHostName(),
                                                            getPort() );
            break;
        case CURLE_OPERATION_TIMEDOUT:
            exCode = DAVException::DAV_HTTP_TIMEOUT;
            exData = CurlUri::makeConnectionEndPointString( getHostName(),
                                                            getPort() );
            break;
        case CURLE_LOGIN_DENIED:
        case CURLE_AUTH_ERROR:
            exCode = DAVException::DAV_HTTP_AUTH;
            exData = CurlUri::makeConnectionEndPointString( getHostName(),
                                                            getPort() );
            break;
        default:
            {
                const char *s = curl_easy_strerror(curlCode);
                exCode = DAVException::DAV_HTTP_ERROR;
                exData = ::rtl::OUString(s, strlen(s),
                                         RTL_TEXTENCODING_UTF8);
                break;
            }
        }
        throw DAVException( exCode, exData );
    }
    
    rtl::OUString reasonPhrase = rtl::OStringToOUString( curlRequest.getReasonPhrase(), RTL_TEXTENCODING_UTF8 );
    if ( statusCode != 0 && statusCode / 100 != 2 )
    {
        switch (statusCode)
        {
            case SC_MOVED_PERMANENTLY:             // 301
            case SC_MOVED_TEMPORARILY:             // 302
            case SC_SEE_OTHER:                     // 303
            case SC_TEMPORARY_REDIRECT:            // 307
            {
                // new location for certain redirections
                
                const CurlRequest::Header *location = curlRequest.findResponseHeader( "location" );
                if ( location != NULL )
                {
                    m_aLogger.log( LogLevel::FINE, "HTTP $1$ response with new location = $2$",
                        statusCode, location->value );
                    throw DAVException( DAVException::DAV_HTTP_REDIRECT,
                                        rtl::OStringToOUString( location->value, RTL_TEXTENCODING_UTF8 ) );
                }
                break;
            }
            case SC_UNAUTHORIZED:                  // 401
            case SC_PROXY_AUTHENTICATION_REQUIRED: // 407
            {
                throw DAVException( DAVException::DAV_HTTP_ERROR,
                                    reasonPhrase,
                                    statusCode );
                break;
            }
            case SC_REQUEST_ENTITY_TOO_LARGE:      // 413
            {
                if ( m_bTransferEncodingSwitched )
                    throw DAVException( DAVException::DAV_HTTP_ERROR,
                                        reasonPhrase,
                                        statusCode );
                m_bTransferEncodingSwitched = true;
                curlRequest.setChunkedEncoding( !curlRequest.isChunkedEncoding() );
                break;
            }
            case SC_LOCKED:                        // 423
                throw DAVException( DAVException::DAV_LOCKED,
                                    reasonPhrase,
                                    statusCode );
            default:
                throw DAVException( DAVException::DAV_HTTP_ERROR,
                                    reasonPhrase,
                                    statusCode );
        }
    }
}

static void responseHeadersToDAVResource( const std::vector< CurlRequest::Header> &responseHeaders,
                                          const std::vector< ::rtl::OUString > &inHeaderNames,
                                          DAVResource &ioResource )
{
    std::vector< CurlRequest::Header >::const_iterator it( responseHeaders.begin() );
    const std::vector< CurlRequest::Header >::const_iterator end( responseHeaders.end() );
    while ( it != end )
    {
        bool storeHeader = false;
        if ( inHeaderNames.size() == 0 )
            storeHeader = true;
        else
        {
            std::vector< ::rtl::OUString >::const_iterator reqIt( inHeaderNames.begin() );
            const std::vector< ::rtl::OUString >::const_iterator reqEnd( inHeaderNames.end() );
            while ( reqIt != reqEnd )
            {
                // header names are case insensitive
                if ( (*reqIt).equalsIgnoreAsciiCase( rtl::OStringToOUString( (*it).name, RTL_TEXTENCODING_UTF8 ) ) )
                {
                    storeHeader = true;
                    break;
                }
                else
                {
                    ++reqIt;
                }
            }
        }
        
        if ( storeHeader )
        {
            DAVPropertyValue thePropertyValue;
            thePropertyValue.IsCaseSensitive = false;
            thePropertyValue.Name = rtl::OStringToOUString( (*it).name, RTL_TEXTENCODING_UTF8 );
            thePropertyValue.Value <<= rtl::OStringToOUString( (*it).value, RTL_TEXTENCODING_UTF8 );
            ioResource.properties.push_back( thePropertyValue );
        }
        
        it++;
    }
}

// -------------------------------------------------------------------
// PROPFIND - allprop & named
// -------------------------------------------------------------------

void CurlSession::propfind( CurlRequest &curlRequest,
                            const rtl::OUString &inPath,
                            const Depth inDepth,
                            const std::vector< ::rtl::OUString > * inPropNames,
                            const bool onlyPropertyNames,
                            const DAVRequestEnvironment & rEnv )
{
    addEnvironmentRequestHeaders( curlRequest, rEnv );

    if ( inDepth == DAVZERO )
        curlRequest.addHeader( "Depth", "0" );
    else if ( inDepth == DAVONE )
        curlRequest.addHeader( "Depth", "1" );
    else if ( inDepth == DAVINFINITY )
        curlRequest.addHeader( "Depth", "infinity" );

    rtl::OString xml = PropfindRequest::generatePROPFINDRequestBody( inPropNames, onlyPropertyNames );
    if ( xml.getLength() > 0 )
    {
        curlRequest.addHeader( "Content-Type", "application/xml" );
        curlRequest.setRequestBody( xml.getStr(), xml.getLength() );
    }

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );
    
    CURLcode rc = curlRequest.propfind( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

void CurlSession::PROPFIND( const rtl::OUString & inPath,
                            const Depth inDepth,
                            const std::vector< rtl::OUString > & inPropNames,
                            std::vector< DAVResource > & ioResources,
                            const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "PROPFIND line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    propfind( curlRequest, inPath, inDepth, &inPropNames, false, rEnv );

    const std::vector< DAVResource > rResources( parseWebDAVPropFindResponse( curlRequest.getResponseBody().get() ) );
    std::vector< DAVResource > *pIoResources = &ioResources;
    *pIoResources = rResources;
}

// -------------------------------------------------------------------
// PROPFIND - propnames
// -------------------------------------------------------------------
void CurlSession::PROPFIND( const rtl::OUString & inPath,
                            const Depth inDepth,
                            std::vector< DAVResourceInfo > & ioResInfo,
                            const DAVRequestEnvironment & rEnv )
    throw( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "PROPFIND line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    propfind( curlRequest, inPath, inDepth, NULL, true, rEnv );

    const std::vector< DAVResourceInfo > rResInfo( parseWebDAVPropNameResponse( curlRequest.getResponseBody().get() ) );
    std::vector< DAVResourceInfo > *pIoResInfo = &ioResInfo;
    *pIoResInfo = rResInfo;
}

// -------------------------------------------------------------------
// PROPPATCH
// -------------------------------------------------------------------
void CurlSession::PROPPATCH( const rtl::OUString & inPath,
                             const std::vector< ProppatchValue > & inValues,
                             const DAVRequestEnvironment & rEnv )
    throw( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "PROPPATCH line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }
    
    rtl::OString xml = ProppatchRequest::generatePROPPATCHRequestBody( inValues );
    if ( xml.getLength() > 0 )
    {
        curlRequest.addHeader( "Content-Type", "application/xml" );
        curlRequest.setRequestBody( xml.getStr(), xml.getLength() );
    }

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    CURLcode rc = curlRequest.proppatch( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// HEAD
// -------------------------------------------------------------------
void CurlSession::HEAD( const ::rtl::OUString & inPath,
                        const std::vector< ::rtl::OUString > & inHeaderNames,
                        DAVResource & ioResource,
                        const DAVRequestEnvironment & rEnv )
    throw( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "HEAD line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init(rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    ioResource.uri = inPath;
    ioResource.properties.clear();

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    CURLcode rc = curlRequest.head( m_aUri, inPath );
    processResponse( curlRequest, rc );
    responseHeadersToDAVResource( curlRequest.getResponseHeaders(), inHeaderNames, ioResource );
}

// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
uno::Reference< io::XInputStream >
CurlSession::GET( const rtl::OUString & inPath,
                  const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "GET line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    CURLcode rc = curlRequest.get( m_aUri, inPath );
    processResponse( curlRequest, rc );

    return uno::Reference< io::XInputStream >( curlRequest.getResponseBody().get() );
}

// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
void CurlSession::GET( const rtl::OUString & inPath,
                       uno::Reference< io::XOutputStream > & ioOutputStream,
                       const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "GET line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    curlRequest.saveResponseBodyTo( ioOutputStream );
    CURLcode rc = curlRequest.get( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
uno::Reference< io::XInputStream >
CurlSession::GET( const rtl::OUString & inPath,
                  const std::vector< ::rtl::OUString > & inHeaderNames,
                  DAVResource & ioResource,
                  const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "GET line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    CURLcode rc = curlRequest.get( m_aUri, inPath );
    processResponse( curlRequest, rc );
    responseHeadersToDAVResource( curlRequest.getResponseHeaders(), inHeaderNames, ioResource );

    return uno::Reference< io::XInputStream >( curlRequest.getResponseBody().get() );
}


// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
void CurlSession::GET( const rtl::OUString & inPath,
                       uno::Reference< io::XOutputStream > & ioOutputStream,
                       const std::vector< ::rtl::OUString > & inHeaderNames,
                       DAVResource & ioResource,
                       const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "GET line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    curlRequest.saveResponseBodyTo( ioOutputStream );
    CURLcode rc = curlRequest.get( m_aUri, inPath );
    processResponse( curlRequest, rc );
    responseHeadersToDAVResource( curlRequest.getResponseHeaders(), inHeaderNames, ioResource );
}

// -------------------------------------------------------------------
// PUT
// -------------------------------------------------------------------
void CurlSession::PUT( const rtl::OUString & inPath,
                       const uno::Reference< io::XInputStream > & inInputStream,
                       const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "PUT line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    uno::Sequence< sal_Int8 > aDataToSend;
    if ( !getDataFromInputStream( inInputStream, aDataToSend, false ) )
        throw DAVException( DAVException::DAV_INVALID_ARG );
    curlRequest.setRequestBody( reinterpret_cast< const char * >( aDataToSend.getConstArray() ),
                                aDataToSend.getLength() );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.put( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// POST
// -------------------------------------------------------------------
uno::Reference< io::XInputStream >
CurlSession::POST( const rtl::OUString & inPath,
                   const rtl::OUString & rContentType,
                   const rtl::OUString & rReferer,
                   const uno::Reference< io::XInputStream > & inInputStream,
                   const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "POST line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    uno::Sequence< sal_Int8 > aDataToSend;
    if ( !getDataFromInputStream( inInputStream, aDataToSend, false ) )
        throw DAVException( DAVException::DAV_INVALID_ARG );
    curlRequest.setRequestBody( reinterpret_cast< const char * >( aDataToSend.getConstArray() ),
                                aDataToSend.getLength() );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    if ( !rContentType.isEmpty() )
        curlRequest.addHeader( "Content-Type", rtl::OUStringToOString( rContentType, RTL_TEXTENCODING_UTF8 ).getStr() );
    if ( !rReferer.isEmpty() )
        curlRequest.addHeader( "Referer", rtl::OUStringToOString( rReferer, RTL_TEXTENCODING_UTF8 ).getStr() );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.post( m_aUri, inPath );
    processResponse( curlRequest, rc );
    return uno::Reference< io::XInputStream >( curlRequest.getResponseBody().get() );
}

// -------------------------------------------------------------------
// POST
// -------------------------------------------------------------------
void CurlSession::POST( const rtl::OUString & inPath,
                        const rtl::OUString & rContentType,
                        const rtl::OUString & rReferer,
                        const uno::Reference< io::XInputStream > & inInputStream,
                        uno::Reference< io::XOutputStream > & oOutputStream,
                        const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "POST line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    uno::Sequence< sal_Int8 > aDataToSend;
    if ( !getDataFromInputStream( inInputStream, aDataToSend, false ) )
        throw DAVException( DAVException::DAV_INVALID_ARG );
    curlRequest.setRequestBody( reinterpret_cast< const char * >( aDataToSend.getConstArray() ),
                                aDataToSend.getLength() );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    if ( !rContentType.isEmpty() )
        curlRequest.addHeader( "Content-Type", rtl::OUStringToOString( rContentType, RTL_TEXTENCODING_UTF8 ).getStr() );
    if ( !rReferer.isEmpty() )
        curlRequest.addHeader( "Referer", rtl::OUStringToOString( rReferer, RTL_TEXTENCODING_UTF8 ).getStr() );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    curlRequest.saveResponseBodyTo( oOutputStream );
    CURLcode rc = curlRequest.post( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// MKCOL
// -------------------------------------------------------------------
void CurlSession::MKCOL( const rtl::OUString & inPath,
                         const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "MKCOL line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.mkcol( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// COPY
// -------------------------------------------------------------------
void CurlSession::COPY( const rtl::OUString & inSourceURL,
                        const rtl::OUString & inDestinationURL,
                        const DAVRequestEnvironment & rEnv,
                        sal_Bool inOverWrite )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "COPY line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    curlRequest.addHeader( "Destination", rtl::OUStringToOString( inDestinationURL, RTL_TEXTENCODING_UTF8 ).getStr() );
    curlRequest.addHeader( "Overwrite", inOverWrite? "T" : "F" );
    
    // check whether a lock on the destination resource is already owned
    rtl::OUString aUri( composeCurrentUri( inDestinationURL ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.copy( m_aUri, CurlUri( inSourceURL ).GetPath() );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// MOVE
// -------------------------------------------------------------------
void CurlSession::MOVE( const rtl::OUString & inSourceURL,
                        const rtl::OUString & inDestinationURL,
                        const DAVRequestEnvironment & rEnv,
                        sal_Bool inOverWrite )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "MOVE line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    curlRequest.addHeader( "Destination", rtl::OUStringToOString( inDestinationURL, RTL_TEXTENCODING_UTF8 ).getStr() );
    curlRequest.addHeader( "Overwrite", inOverWrite? "T" : "F" );
    
    // check whether a lock on the destination resource is already owned
    rtl::OUString aUri( composeCurrentUri( inDestinationURL ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.copy( m_aUri, CurlUri( inSourceURL ).GetPath() );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------
// DESTROY
// -------------------------------------------------------------------
void CurlSession::DESTROY( const rtl::OUString & inPath,
                           const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "DESTROY line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    // check whether a lock on this resource is already owned
    rtl::OUString aUri( composeCurrentUri( inPath ) );
    ucb::Lock inLock;
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
        inLock = pLock->getLock();
    }
    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    CURLcode rc = curlRequest.delete_( m_aUri, inPath );
    processResponse( curlRequest, rc );
}

// -------------------------------------------------------------------

namespace
{
    sal_Int32 lastChanceToSendRefreshRequest( TimeValue const & rStart,
                                              sal_Int32 timeout )
    {
        TimeValue aEnd;
        osl_getSystemTime( &aEnd );

        // Try to estimate a safe absolute time for sending the
        // lock refresh request.
        sal_Int32 lastChanceToSendRefreshRequest = DAVINFINITY;
        if ( timeout != DAVINFINITY )
        {
            sal_Int32 calltime = aEnd.Seconds - rStart.Seconds;
            if ( calltime <= timeout )
            {
                lastChanceToSendRefreshRequest
                    = aEnd.Seconds + timeout - calltime;
            }
            else
            {
                OSL_TRACE( "No chance to refresh lock before timeout!" );
            }
        }
        return lastChanceToSendRefreshRequest;
    }

} // namespace

// -------------------------------------------------------------------
// LOCK (set new lock)
// -------------------------------------------------------------------
void CurlSession::LOCK( const ::rtl::OUString & inPath,
                        ucb::Lock & inLock,
                        const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "LOCK line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    // before locking, search in the lock store if we already own a lock for this resource
    // if present, return with exception DAV_LOCKED_SELF
    rtl::OUString   aUri( composeCurrentUri( inPath ) );
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( pLock )
    {
// already present, meaning already locked by the same AOO session and already in the lockstore
// just return, nothing to do
        return;
    }

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );
    
    addEnvironmentRequestHeaders( curlRequest, rEnv );
    
    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    if ( inLock.Timeout == -1 )
        curlRequest.addHeader( "Timeout", "Infinite" );
    else
        curlRequest.addHeader( "Timeout", "Second-" + rtl::OString::valueOf( inLock.Timeout ) );

    switch ( inLock.Depth )
    {
        //i126305 TODO investigate on this case...
    case ucb::LockDepth_MAKE_FIXED_SIZE:

    case ucb::LockDepth_ZERO:
        curlRequest.addHeader( "Depth", "0" );
        break;
    case ucb::LockDepth_ONE:
        curlRequest.addHeader( "Depth", "1" );
        break;
    case ucb::LockDepth_INFINITY:
        curlRequest.addHeader( "Depth", "infinity" );
        break;
    }
    
    rtl::OString xml = LockRequest::generateRequestBody( inLock );
    curlRequest.addHeader( "Content-Type", "application/xml" );
    curlRequest.setRequestBody( xml.getStr(), xml.getLength() );

    TimeValue startCall;
    osl_getSystemTime( &startCall );
    
    CURLcode rc = curlRequest.lock( m_aUri, inPath );
    processResponse( curlRequest, rc );

    // the returned property, a sequence of locks
    // only the first is used
    const DAVPropertyValue outLock( parseWebDAVLockResponse( curlRequest.getResponseBody().get() ) );
    if(outLock.Name.compareToAscii(RTL_CONSTASCII_STRINGPARAM( "DAV:lockdiscovery" )) == 0 )
    {
        // got a lock, use only the first returned
        uno::Sequence< ucb::Lock >      aLocks;
        outLock.Value >>= aLocks;
        ucb::Lock aLock = aLocks[0];

        CurlLock* aNewLock = new CurlLock( aLock, aUri, inPath );
        // add the store the new lock
        m_aCurlLockStore.addLock(aNewLock,this,
                                 lastChanceToSendRefreshRequest(
                                     startCall, static_cast< sal_Int32 >(aLock.Timeout) ) );
    }
}

// -------------------------------------------------------------------
// LOCK (refresh existing lock from DAVResourceAccess)
// -------------------------------------------------------------------
sal_Int64 CurlSession::LOCK( const ::rtl::OUString & /*inPath*/,
                             sal_Int64 nTimeout,
                             const DAVRequestEnvironment & /*rEnv*/ )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "LOCK line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    return nTimeout;
    /*
    // Try to get the neon lock from lock store
    CurlLock * theLock
        = m_aCurlLockStore.findByUri( makeAbsoluteURL( inPath ) );
    if ( !theLock )
         throw DAVException( DAVException::DAV_NOT_LOCKED );

    Init( rEnv );

    // refresh existing lock.
    theLock->timeout = static_cast< long >( nTimeout );

    TimeValue startCall;
    osl_getSystemTime( &startCall );

    int theRetVal = ne_lock_refresh( m_pHttpSession, theLock );

    if ( theRetVal == NE_OK )
    {
        m_aCurlLockStore.updateLock( theLock,
                                     lastChanceToSendRefreshRequest(
                                         startCall, theLock->timeout ) );
    }

    HandleError( theRetVal, inPath, rEnv );

    return theLock->timeout;
    */
}

// -------------------------------------------------------------------
// LOCK (refresh existing lock from CurlLockStore)
// -------------------------------------------------------------------
bool CurlSession::LOCK( CurlLock * pLock,
                        sal_Int32 & rlastChanceToSendRefreshRequest )
{
    m_aLogger.log( LogLevel::INFO, "LOCK line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init();
    CurlRequest curlRequest( m_pCurl );
    
    const ucb::Lock & inLock = pLock->getLock();
    rtl::OUString inPath = pLock->getResourcePath();
    
    if ( inLock.Timeout == -1 )
        curlRequest.addHeader( "Timeout", "Infinite" );
    else
        curlRequest.addHeader( "Timeout", "Second-" + rtl::OString::valueOf( inLock.Timeout ) );

    switch ( inLock.Depth )
    {
        //i126305 TODO investigate on this case...
    case ucb::LockDepth_MAKE_FIXED_SIZE:

    case ucb::LockDepth_ZERO:
        curlRequest.addHeader( "Depth", "0" );
        break;
    case ucb::LockDepth_ONE:
        curlRequest.addHeader( "Depth", "1" );
        break;
    case ucb::LockDepth_INFINITY:
        curlRequest.addHeader( "Depth", "infinity" );
        break;
    }

    if ( inLock.LockTokens.getLength() > 0 )
    {
        curlRequest.addHeader( "If",
            ( "(<" + rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">)" ).getStr() );
    }

    rtl::OString xml = LockRequest::generateRequestBody( inLock );
    curlRequest.addHeader( "Content-Type", "application/xml" );
    curlRequest.setRequestBody( xml.getStr(), xml.getLength() );
    
    TimeValue startCall;
    osl_getSystemTime( &startCall );

    CURLcode rc = curlRequest.lock( m_aUri, inPath );
    processResponse( curlRequest, rc );

    // the returned property, a sequence of locks
    // only the first is used
    const DAVPropertyValue outLock( parseWebDAVLockResponse( curlRequest.getResponseBody().get() ) );
    uno::Sequence< ucb::Lock >      aLocks;
    outLock.Value >>= aLocks;
    ucb::Lock aLock = aLocks[0];

    // if ok, update the lastchance refresh time in lock
    rlastChanceToSendRefreshRequest
        = lastChanceToSendRefreshRequest( startCall, static_cast< sal_Int32 >(aLock.Timeout) );

    return true;
}

// -------------------------------------------------------------------
// UNLOCK called from external (DAVResourceAccess)
// -------------------------------------------------------------------
void CurlSession::UNLOCK( const ::rtl::OUString & inPath,
                          const DAVRequestEnvironment & rEnv )
    throw ( DAVException )
{
    m_aLogger.log( LogLevel::INFO, "UNLOCK line $1$", (sal_Int32)__LINE__ );

    osl::Guard< osl::Mutex > theGuard( m_aMutex );

    rtl::OUString aUri( composeCurrentUri( inPath ) );
    CurlLock * pLock = m_aCurlLockStore.findByUri( aUri );
    if ( !pLock )
    {
        throw DAVException( DAVException::DAV_NOT_LOCKED );
    }

    Init( rEnv );
    CurlRequest curlRequest( m_pCurl );

    addEnvironmentRequestHeaders( curlRequest, rEnv );

    CredentialsData credsData( this, curlRequest, rEnv );
    curlRequest.setProvideCredentialsCallback( Curl_ProvideCredentials, &credsData );

    ucb::Lock inLock = pLock->getLock();
    curlRequest.addHeader( "Lock-Token",
            ( "<" + rtl::OUStringToOString( inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">" ).getStr() );

    // remove lock from lockstore
    // so, if something goes wrong, we don't refresh it anymore
    m_aCurlLockStore.removeLock( pLock );
    delete pLock;
    
    CURLcode rc = curlRequest.unlock( m_aUri, inPath );
    processResponse( curlRequest, rc );    
}

// -------------------------------------------------------------------
// UNLOCK (called from CurlLockStore)
// -------------------------------------------------------------------
bool CurlSession::UNLOCK( CurlLock * pLock )
{
    m_aLogger.log( LogLevel::INFO, "UNLOCK line $1$", (sal_Int32)__LINE__ );
    
    osl::Guard< osl::Mutex > theGuard( m_aMutex );
    
    Init();
    CurlRequest curlRequest( m_pCurl );

    rtl::OUString inPath = pLock->getResourcePath();
    ucb::Lock inLock = pLock->getLock();
    curlRequest.addHeader( "Lock-Token",
            ( "<" + rtl::OUStringToOString( inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ) + ">" ).getStr() );

    CURLcode rc = curlRequest.unlock( m_aUri, inPath );
    processResponse( curlRequest, rc );
    return true;
}

// -------------------------------------------------------------------
void CurlSession::abort()
    throw ( DAVException )
{
    // 11.11.09 (tkr): The following code lines causing crashes if
    // closing a ongoing connection. It turned out that this existing
    // solution doesn't work in multi-threading environments.
    // So I disabled them in 3.2. . Issue #73893# should fix it in OOo 3.3.
    //if ( m_pHttpSession )
    //    ne_close_connection( m_pHttpSession );
}

// -------------------------------------------------------------------
const ucbhelper::InternetProxyServer & CurlSession::getProxySettings() const
{
    if ( m_aUri.GetScheme().equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "http" ) ) ||
         m_aUri.GetScheme().equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "https" ) ) )
    {
        return m_rProxyDecider.getProxy( m_aUri.GetScheme(),
                                         m_aUri.GetHost(),
                                         m_aUri.GetPort() );
    }
    else
    {
        // TODO: figure out, if this case can occur
        return m_rProxyDecider.getProxy( m_aUri.GetScheme(),
                                         rtl::OUString() /* not used */,
                                         -1 /* not used */ );
    }
}

/*
// -------------------------------------------------------------------
namespace {

bool containsLocktoken( const uno::Sequence< ucb::Lock > & rLocks,
                        const char * token )
{
    for ( sal_Int32 n = 0; n < rLocks.getLength(); ++n )
    {
        const uno::Sequence< rtl::OUString > & rTokens
            = rLocks[ n ].LockTokens;
        for ( sal_Int32 m = 0; m < rTokens.getLength(); ++m )
        {
            if ( rTokens[ m ].equalsAscii( token ) )
                return true;
        }
    }
    return false;
}

} // namespace
*/

// -------------------------------------------------------------------
// This method doesn't seem to be used.
// In any case the default behavior is to ask a lock with a life of 3 minutes
// it will then be refreshed automatically (see CurlLockStore class)
// In case of AOO crash the lock will expire by itself
bool CurlSession::removeExpiredLocktoken( const rtl::OUString & /*inURL*/,
                                          const DAVRequestEnvironment & /*rEnv*/ )
{
    return true;
    /*
    CurlLock * theLock = m_aCurlLockStore.findByUri( inURL );
    if ( !theLock )
        return false;

    // do a lockdiscovery to check whether this lock is still valid.
    try
    {
        // @@@ Alternative: use ne_lock_discover() => less overhead

        std::vector< DAVResource > aResources;
        std::vector< rtl::OUString > aPropNames;
        aPropNames.push_back( DAVProperties::LOCKDISCOVERY );

        PROPFIND( rEnv.m_aRequestURI, DAVZERO, aPropNames, aResources, rEnv );

        if ( aResources.size() == 0 )
            return false;

        std::vector< DAVPropertyValue >::const_iterator it
            = aResources[ 0 ].properties.begin();
        std::vector< DAVPropertyValue >::const_iterator end
            = aResources[ 0 ].properties.end();

        while ( it != end )
        {
            if ( (*it).Name.equals( DAVProperties::LOCKDISCOVERY ) )
            {
                uno::Sequence< ucb::Lock > aLocks;
                if ( !( (*it).Value >>= aLocks ) )
                    return false;

                if ( !containsLocktoken( aLocks, theLock->token ) )
                {
                    // expired!
                    break;
                }

                // still valid.
                return false;
            }
            ++it;
        }

        // No lockdiscovery prop in propfind result / locktoken not found
        // in propfind result -> not locked
        OSL_TRACE( "CurlSession::removeExpiredLocktoken: Removing "
                   " expired lock token for %s. token: %s",
                   rtl::OUStringToOString( inURL,
                                           RTL_TEXTENCODING_UTF8 ).getStr(),
                   theLock->token );

        m_aCurlLockStore.removeLock( theLock );
        ne_lock_destroy( theLock );
        return true;
    }
    catch ( DAVException const & )
    {
    }
    return false;
    */
}

// -------------------------------------------------------------------
// static
bool
CurlSession::getDataFromInputStream(
    const uno::Reference< io::XInputStream > & xStream,
    uno::Sequence< sal_Int8 > & rData,
    bool bAppendTrailingZeroByte )
{
    if ( xStream.is() )
    {
        uno::Reference< io::XSeekable > xSeekable( xStream, uno::UNO_QUERY );
        if ( xSeekable.is() )
        {
            try
            {
                sal_Int32 nSize
                    = sal::static_int_cast<sal_Int32>(xSeekable->getLength());
                sal_Int32 nRead
                    = xStream->readBytes( rData, nSize );

                if ( nRead == nSize )
                {
                    if ( bAppendTrailingZeroByte )
                    {
                        rData.realloc( nSize + 1 );
                        rData[ nSize ] = sal_Int8( 0 );
                    }
                    return true;
                }
            }
            catch ( io::NotConnectedException const & )
            {
                // readBytes
            }
            catch ( io::BufferSizeExceededException const & )
            {
                // readBytes
            }
            catch ( io::IOException const & )
            {
                // getLength, readBytes
            }
        }
        else
        {
            try
            {
                uno::Sequence< sal_Int8 > aBuffer;
                sal_Int32 nPos = 0;

                sal_Int32 nRead = xStream->readSomeBytes( aBuffer, 65536 );
                while ( nRead > 0 )
                {
                    if ( rData.getLength() < ( nPos + nRead ) )
                        rData.realloc( nPos + nRead );

                    aBuffer.realloc( nRead );
                    rtl_copyMemory( (void*)( rData.getArray() + nPos ),
                                    (const void*)aBuffer.getConstArray(),
                                    nRead );
                    nPos += nRead;

                    aBuffer.realloc( 0 );
                    nRead = xStream->readSomeBytes( aBuffer, 65536 );
                }

                if ( bAppendTrailingZeroByte )
                {
                    rData.realloc( nPos + 1 );
                    rData[ nPos ] = sal_Int8( 0 );
                }
                return true;
            }
            catch ( io::NotConnectedException const & )
            {
                // readBytes
            }
            catch ( io::BufferSizeExceededException const & )
            {
                // readBytes
            }
            catch ( io::IOException const & )
            {
                // readBytes
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------
sal_Bool
CurlSession::isDomainMatch( rtl::OUString certHostName )
{
    rtl::OUString hostName = getHostName();

    if (hostName.equalsIgnoreAsciiCase( certHostName ) )
        return sal_True;

    if ( 0 == certHostName.indexOf( rtl::OUString::createFromAscii( "*" ) ) &&
         hostName.getLength() >= certHostName.getLength() )
    {
        rtl::OUString cmpStr = certHostName.copy( 1 );

        if ( hostName.matchIgnoreAsciiCase(
                cmpStr, hostName.getLength() - cmpStr.getLength() ) )
            return sal_True;
    }
    return sal_False;
}
