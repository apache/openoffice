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

package org.apache.openoffice.pdfimport;

import org.apache.pdfbox.Loader;
import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.font.PDFont;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * The PDFBox pdfi backend: reads a PDF, writes the pdfi helper protocol on
 * stdout.  See PROTOCOL.md for the contract this implements and
 * wrapper/pdfbackend.cxx for how it gets selected.
 *
 * <p>Invoked as {@code java -jar pdfimport-pdfbox.jar [-opw pw] [-upw pw]
 * [-f file] <pdf-path>}, with the open password as the first line of stdin.
 * The {@code -f} option is accepted and ignored: it redirects the binary
 * channel of the v1 framing, and this backend speaks v2, where payloads travel
 * inline on stdout and there is no separate channel to redirect.</p>
 */
public final class Main
{
    private Main() {}

    public static void main( String[] args )
    {
        String aOwnerPassword = null;
        String aUserPassword = null;
        String aPath = null;

        for( int i = 0; i < args.length; ++i )
        {
            final String a = args[i];
            if( "-opw".equals( a ) && i + 1 < args.length )
                aOwnerPassword = args[++i];
            else if( "-upw".equals( a ) && i + 1 < args.length )
                aUserPassword = args[++i];
            else if( "-f".equals( a ) && i + 1 < args.length )
                ++i; // v1 binary redirect; not applicable to v2
            else
                aPath = a; // the PDF path is always last
        }

        // The caller always writes the password line, so it must always be
        // read: leaving it in the pipe risks blocking the caller on a full
        // buffer for a large document.
        final String aStdinPassword = readPasswordLine();

        if( aPath == null )
        {
            System.err.println( "pdfimport-pdfbox: no input file given" );
            System.exit( 1 );
        }

        String aPassword = aStdinPassword;
        if( aPassword == null || aPassword.length() == 0 )
            aPassword = aOwnerPassword != null ? aOwnerPassword : aUserPassword;
        if( aPassword == null )
            aPassword = "";

        PDDocument aDoc = null;
        try
        {
            aDoc = Loader.loadPDF( new File( aPath ), aPassword );
            convert( aDoc );
        }
        catch( Throwable e )
        {
            // stderr is free in v2 framing, so a failure can actually say why.
            System.err.println( "pdfimport-pdfbox: " + e );
            e.printStackTrace();
            System.exit( 1 );
        }
        finally
        {
            if( aDoc != null )
            {
                try
                {
                    aDoc.close();
                }
                catch( Exception e )
                {
                    // nothing useful left to do at this point
                }
            }
        }
    }

    private static void convert( PDDocument aDoc ) throws Exception
    {
        final PdfiWriter aOut = new PdfiWriter();

        final int nPages = aDoc.getNumberOfPages();

        // Sent before any page: the consumer sizes its progress bar with it.
        aOut.cmd( "setPageNum" ).i( nPages ).eol();

        // Font identity spans the document -- the consumer caches attributes by
        // id and expects an id to keep meaning the same font across pages.
        final Map<PDFont, Long> aFontIds = new HashMap<PDFont, Long>();
        final long[] pNextFontId = new long[] { 1 };

        for( int i = 0; i < nPages; ++i )
        {
            final PDPage aPage = aDoc.getPage( i );
            try
            {
                new PdfiEngine( aPage, aOut, aFontIds, pNextFontId ).run();
            }
            catch( Exception e )
            {
                // One unparsable page should not cost the other 200.
                System.err.println( "pdfimport-pdfbox: page " + ( i + 1 )
                                    + " failed: " + e );
            }
        }

        aOut.finish();
    }

    /**
     * Reads the open password: one line, ISO-8859-1, as written by
     * wrapper.cxx.  An immediate EOF means no password, which is the normal
     * case.
     */
    private static String readPasswordLine()
    {
        try
        {
            final InputStream aIn = System.in;
            final ByteArrayOutputStream aBuf = new ByteArrayOutputStream( 128 );
            int c;
            while( ( c = aIn.read() ) != -1 && c != '\n' )
            {
                if( c != '\r' )
                    aBuf.write( c );
            }
            return new String( aBuf.toByteArray(), "ISO-8859-1" );
        }
        catch( Exception e )
        {
            return "";
        }
    }
}
