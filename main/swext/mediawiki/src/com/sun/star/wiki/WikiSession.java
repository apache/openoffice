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



package com.sun.star.wiki;

import com.sun.star.uno.XComponentContext;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.CookieManager;
import java.net.CookiePolicy;
import java.net.HttpURLConnection;
import java.net.Proxy;
import java.net.URISyntaxException;
import java.net.URL;
import java.net.URLEncoder;
import java.security.KeyStore;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/** An HTTP session with one MediaWiki server, talking to its Action API (api.php). */
public class WikiSession
{
    public static class Response
    {
        public int m_nStatus;
        public String m_sBody;
        public URL m_aURL;
    }

    private static final int MAX_REDIRECTS = 10;
    private static final int CONNECT_TIMEOUT_MS = 30000;
    private static final String USER_AGENT = "Apache-OpenOffice-Wiki-Publisher";

    private static SSLSocketFactory m_aUnknownCertSocketFactory;

    private XComponentContext m_xContext;
    private URL m_aMainURL;
    // not CookieHandler.setDefault(): that would be shared by everything in the office JVM
    private CookieManager m_aCookies = new CookieManager( null, CookiePolicy.ACCEPT_ORIGINAL_SERVER );

    public WikiSession( XComponentContext xContext, URL aMainURL )
    {
        m_xContext = xContext;
        m_aMainURL = aMainURL;
    }

    public URL GetMainURL()
    {
        return m_aMainURL;
    }

    public Response Get( URL aURL )
        throws IOException, WikiCancelException
    {
        for ( int nRedirect = 0; ; nRedirect++ )
        {
            HttpURLConnection aConn = Open( aURL );
            Response aResponse = Execute( aConn, aURL, null );
            int nStatus = aResponse.m_nStatus;
            String sLocation = aConn.getHeaderField( "Location" );
            if ( nStatus < 300 || nStatus > 399 || sLocation == null || nRedirect >= MAX_REDIRECTS )
                return aResponse;

            aURL = new URL( aURL, sLocation );
        }
    }

    public Response Post( URL aURL, String[][] pParams )
        throws IOException, WikiCancelException
    {
        StringBuilder aBody = new StringBuilder();
        for ( int nInd = 0; nInd < pParams.length; nInd++ )
        {
            if ( pParams[nInd] == null || pParams[nInd][0] == null || pParams[nInd][1] == null )
                continue;
            if ( aBody.length() > 0 )
                aBody.append( '&' );
            aBody.append( URLEncoder.encode( pParams[nInd][0], "UTF-8" ) );
            aBody.append( '=' );
            aBody.append( URLEncoder.encode( pParams[nInd][1], "UTF-8" ) );
        }

        HttpURLConnection aConn = Open( aURL );
        aConn.setRequestMethod( "POST" );
        aConn.setDoOutput( true );
        aConn.setRequestProperty( "Content-Type", "application/x-www-form-urlencoded; charset=UTF-8" );
        return Execute( aConn, aURL, aBody.toString().getBytes( "UTF-8" ) );
    }

    /** Calls api.php; HTTP errors and non-XML answers throw, API-level errors are left in the returned document. */
    public Document CallAPI( String[][] pParams )
        throws IOException, WikiCancelException
    {
        String[][] pAllParams = new String[pParams.length + 1][];
        pAllParams[0] = new String[] { "format", "xml" };
        System.arraycopy( pParams, 0, pAllParams, 1, pParams.length );

        Response aResponse = Post( new URL( m_aMainURL, "api.php" ), pAllParams );
        if ( aResponse.m_nStatus != 200 )
            throw new IOException( "MediaWiki API request failed with HTTP status " + aResponse.m_nStatus );

        try
        {
            DocumentBuilderFactory aFactory = DocumentBuilderFactory.newInstance();
            aFactory.setFeature( "http://apache.org/xml/features/disallow-doctype-decl", true );
            aFactory.setExpandEntityReferences( false );
            DocumentBuilder aBuilder = aFactory.newDocumentBuilder();
            Document aDoc = aBuilder.parse( new ByteArrayInputStream( aResponse.m_sBody.getBytes( "UTF-8" ) ) );
            if ( !"api".equals( aDoc.getDocumentElement().getTagName() ) )
                throw new IOException( "Not a MediaWiki API response" );
            return aDoc;
        }
        catch ( IOException e )
        {
            throw e;
        }
        catch ( Exception e )
        {
            IOException aIOEx = new IOException( "Not a MediaWiki API response" );
            aIOEx.initCause( e );
            throw aIOEx;
        }
    }

    /** Returns the requested token ("login", "csrf"), or null if the wiki does not provide it. */
    public String GetToken( String sType )
        throws IOException, WikiCancelException
    {
        Document aDoc = CallAPI( new String[][] { { "action", "query" }, { "meta", "tokens" }, { "type", sType } } );
        Element aTokens = GetElement( aDoc, "tokens" );
        if ( aTokens == null || !aTokens.hasAttribute( sType + "token" ) )
            return null;
        return aTokens.getAttribute( sType + "token" );
    }

    public static Element GetElement( Document aDoc, String sTagName )
    {
        NodeList aList = aDoc.getElementsByTagName( sTagName );
        return aList.getLength() > 0 ? (Element) aList.item( 0 ) : null;
    }

    private HttpURLConnection Open( URL aURL )
        throws IOException, WikiCancelException
    {
        if ( !Helper.IsConnectionAllowed() )
            throw new WikiCancelException();

        String sProtocol = aURL.getProtocol();
        if ( !sProtocol.equals( "http" ) && !sProtocol.equals( "https" ) )
            throw new IOException( "Unsupported protocol: " + sProtocol );

        Proxy aProxy = Helper.GetProxy( m_xContext, aURL );
        HttpURLConnection aConn = (HttpURLConnection) ( aProxy == null ? aURL.openConnection() : aURL.openConnection( aProxy ) );
        aConn.setInstanceFollowRedirects( false );
        aConn.setConnectTimeout( CONNECT_TIMEOUT_MS );
        aConn.setUseCaches( false );
        aConn.setRequestProperty( "User-Agent", USER_AGENT );

        if ( aConn instanceof HttpsURLConnection && Helper.AllowUnknownCert( m_xContext, aURL.getHost() ) )
            ( (HttpsURLConnection) aConn ).setSSLSocketFactory( GetUnknownCertSocketFactory() );

        try
        {
            Map<String, List<String>> aCookieHeaders = m_aCookies.get( aURL.toURI(), new HashMap<String, List<String>>() );
            List<String> aCookies = aCookieHeaders.get( "Cookie" );
            if ( aCookies != null && !aCookies.isEmpty() )
            {
                StringBuilder aCookie = new StringBuilder();
                for ( String sCookie : aCookies )
                {
                    if ( aCookie.length() > 0 )
                        aCookie.append( "; " );
                    aCookie.append( sCookie );
                }
                aConn.setRequestProperty( "Cookie", aCookie.toString() );
            }
        }
        catch ( URISyntaxException e )
        {
            throw new IOException( e.getMessage() );
        }

        return aConn;
    }

    private Response Execute( HttpURLConnection aConn, URL aURL, byte[] pBody )
        throws IOException, WikiCancelException
    {
        Helper.RegisterConnection( aConn );
        try
        {
            // disconnect() is a no-op until the TCP connect has finished, so recheck a cancel that came during it
            aConn.connect();
            if ( !Helper.IsConnectionAllowed() )
                throw new WikiCancelException();

            if ( pBody != null )
            {
                OutputStream aOut = aConn.getOutputStream();
                try
                {
                    aOut.write( pBody );
                }
                finally
                {
                    aOut.close();
                }
            }

            Response aResponse = new Response();
            aResponse.m_nStatus = aConn.getResponseCode();
            aResponse.m_aURL = aURL;

            try
            {
                m_aCookies.put( aURL.toURI(), aConn.getHeaderFields() );
            }
            catch ( URISyntaxException e )
            {
                throw new IOException( e.getMessage() );
            }

            InputStream aIn = aResponse.m_nStatus >= 400 ? aConn.getErrorStream() : aConn.getInputStream();
            aResponse.m_sBody = ( aIn == null ) ? "" : ReadAll( aIn, GetCharset( aConn.getContentType() ) );
            return aResponse;
        }
        catch ( IOException e )
        {
            if ( !Helper.IsConnectionAllowed() )
                throw new WikiCancelException();
            throw e;
        }
        finally
        {
            Helper.UnregisterConnection( aConn );
        }
    }

    private static String GetCharset( String sContentType )
    {
        if ( sContentType != null )
        {
            String[] pParts = sContentType.split( ";" );
            for ( int nInd = 1; nInd < pParts.length; nInd++ )
            {
                String sPart = pParts[nInd].trim();
                if ( sPart.regionMatches( true, 0, "charset=", 0, 8 ) )
                    return sPart.substring( 8 ).replace( "\"", "" ).trim();
            }
        }
        return "UTF-8";
    }

    private static String ReadAll( InputStream aIn, String sCharset )
        throws IOException
    {
        try
        {
            ByteArrayOutputStream aOut = new ByteArrayOutputStream();
            byte[] pBuf = new byte[8192];
            int nRead;
            while ( ( nRead = aIn.read( pBuf ) ) != -1 )
                aOut.write( pBuf, 0, nRead );
            try
            {
                return aOut.toString( sCharset );
            }
            catch ( UnsupportedEncodingException e )
            {
                return aOut.toString( "UTF-8" );
            }
        }
        finally
        {
            aIn.close();
        }
    }

    private static synchronized SSLSocketFactory GetUnknownCertSocketFactory()
        throws IOException
    {
        if ( m_aUnknownCertSocketFactory == null )
        {
            try
            {
                TrustManagerFactory aFactory = TrustManagerFactory.getInstance( TrustManagerFactory.getDefaultAlgorithm() );
                aFactory.init( (KeyStore) null );
                X509TrustManager aDefault = null;
                TrustManager[] pManagers = aFactory.getTrustManagers();
                for ( int nInd = 0; nInd < pManagers.length && aDefault == null; nInd++ )
                    if ( pManagers[nInd] instanceof X509TrustManager )
                        aDefault = (X509TrustManager) pManagers[nInd];
                if ( aDefault == null )
                    throw new IOException( "No default trust manager" );

                SSLContext aContext = SSLContext.getInstance( "TLS" );
                aContext.init( null, new TrustManager[] { new UnknownCertTrustManager( aDefault ) }, null );
                m_aUnknownCertSocketFactory = aContext.getSocketFactory();
            }
            catch ( IOException e )
            {
                throw e;
            }
            catch ( Exception e )
            {
                IOException aIOEx = new IOException( "Cannot initialize TLS" );
                aIOEx.initCause( e );
                throw aIOEx;
            }
        }

        return m_aUnknownCertSocketFactory;
    }

    /** For servers the configuration marks with AllowUnknownCertificate: a server certificate
        that does not chain to a trusted root is accepted if it is within its validity period.
        The host name is still verified by HttpsURLConnection. */
    private static class UnknownCertTrustManager implements X509TrustManager
    {
        private X509TrustManager m_aDefault;

        UnknownCertTrustManager( X509TrustManager aDefault )
        {
            m_aDefault = aDefault;
        }

        public X509Certificate[] getAcceptedIssuers()
        {
            return m_aDefault.getAcceptedIssuers();
        }

        public void checkClientTrusted( X509Certificate[] pCerts, String sAuthType ) throws CertificateException
        {
            m_aDefault.checkClientTrusted( pCerts, sAuthType );
        }

        public void checkServerTrusted( X509Certificate[] pCerts, String sAuthType ) throws CertificateException
        {
            try
            {
                m_aDefault.checkServerTrusted( pCerts, sAuthType );
            }
            catch ( CertificateException e )
            {
                if ( pCerts == null || pCerts.length == 0 )
                    throw e;
                pCerts[0].checkValidity();
            }
        }
    }
}
