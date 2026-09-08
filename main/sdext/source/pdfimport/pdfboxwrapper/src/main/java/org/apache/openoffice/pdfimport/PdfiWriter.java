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

import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Locale;

/**
 * Writes the pdfi helper protocol -- see PROTOCOL.md, which is the normative
 * description; this class is one implementation of it.
 *
 * <p>v2 framing: commands and binary payloads share one stream, a payload
 * following immediately after the newline of the line that stated its length.
 * That is why everything here goes through a single {@link OutputStream} and
 * why nothing ever flushes mid-document -- there is no second pipe to stay in
 * step with.</p>
 *
 * <p><b>stdout is the protocol.</b> Nothing else in this process may write to
 * it; diagnostics belong on stderr, which v2 deliberately leaves free.</p>
 */
final class PdfiWriter
{
    /**
     * Device units per PDF point.
     *
     * <p>The consumer works in 1/7200 inch (PDFI_OUTDEV_RESOLUTION in
     * pdfihelper.hxx), a PDF point is 1/72 inch, so a point is 100 device
     * units.  Font sizes travel in the same unit -- drawtreevisiting.cxx
     * converts back with {@code size*72/7200}.</p>
     */
    static final double DEVICE_UNITS_PER_POINT = 100.0;

    /** Below this, a coordinate is noise from an inexact matrix product. */
    private static final double EPSILON = 0.0000001;

    private final OutputStream m_aOut;
    private final StringBuilder m_aLine = new StringBuilder( 256 );

    PdfiWriter()
    {
        m_aOut = new BufferedOutputStream(
            new FileOutputStream( FileDescriptor.out ), 1 << 16 );
    }

    /** Matches normalize() in pdfioutdev_gpl.cxx: clamp denormal noise away. */
    static double normalize( double fValue )
    {
        return Math.abs( fValue ) < EPSILON ? 0.0 : fValue;
    }

    // ---- line assembly ----------------------------------------------------

    PdfiWriter cmd( String aCommand )
    {
        if( m_aLine.length() != 0 )
            throw new IllegalStateException( "unterminated command: " + m_aLine );
        m_aLine.append( aCommand );
        return this;
    }

    /**
     * Appends a bare token to a line already in progress -- the {@code subpath}
     * marker, an image format name.  Unlike {@link #cmd} this does not insist
     * on being at the start of a line.
     */
    PdfiWriter tok( String aToken )
    {
        m_aLine.append( ' ' ).append( aToken );
        return this;
    }

    PdfiWriter i( int nValue )
    {
        m_aLine.append( ' ' ).append( nValue );
        return this;
    }

    PdfiWriter l( long nValue )
    {
        m_aLine.append( ' ' ).append( nValue );
        return this;
    }

    /**
     * Appends a number in the consumer's expected form: C {@code %f}, six
     * decimals, always a dot.  Locale.ROOT is not decoration -- under a
     * European default locale Java would emit {@code 1,5}, which the consumer
     * reads with rtl::OString::toDouble() as 1.0, silently displacing
     * everything on the page.
     */
    PdfiWriter d( double fValue )
    {
        m_aLine.append( ' ' )
               .append( String.format( Locale.ROOT, "%f", normalize( fValue ) ) );
        return this;
    }

    /** Appends the trailing free-text field, escaped, and ends the line. */
    void text( String aText ) throws IOException
    {
        m_aLine.append( ' ' );
        escapeInto( aText, m_aLine );
        eol();
    }

    void eol() throws IOException
    {
        m_aLine.append( '\n' );
        // The grammar is ASCII; only the free-text fields carry non-ASCII, and
        // those were already encoded to UTF-8 bytes by escapeInto().
        final int nLen = m_aLine.length();
        for( int i = 0; i < nLen; ++i )
            m_aOut.write( m_aLine.charAt( i ) & 0xFF );
        m_aLine.setLength( 0 );
    }

    /**
     * Writes a binary payload.  Must directly follow the {@link #eol()} of the
     * command that stated its length, with nothing in between.
     */
    void binary( byte[] aData ) throws IOException
    {
        if( m_aLine.length() != 0 )
            throw new IllegalStateException( "payload before end of command" );
        m_aOut.write( aData );
    }

    void finish() throws IOException
    {
        m_aOut.flush();
    }

    // ---- escaping ---------------------------------------------------------

    /**
     * Escapes the free-text fields of {@code drawChar}, {@code updateFont} and
     * {@code drawLink}, which run to end of line.
     *
     * <p>Text is UTF-8, and each byte is appended as one char so that
     * {@link #eol()} can write it out unchanged -- the consumer decodes the
     * field with RTL_TEXTENCODING_UTF8.</p>
     */
    private static void escapeInto( String aText, StringBuilder aBuf )
    {
        final byte[] aUtf8;
        try
        {
            aUtf8 = aText.getBytes( "UTF-8" );
        }
        catch( java.io.UnsupportedEncodingException e )
        {
            throw new AssertionError( "UTF-8 is required of every JVM" );
        }

        for( int i = 0; i < aUtf8.length; ++i )
        {
            final int c = aUtf8[i] & 0xFF;
            switch( c )
            {
                case '\n': aBuf.append( '\\' ).append( 'n' ); break;
                case '\r': aBuf.append( '\\' ).append( 'r' ); break;
                case '\\': aBuf.append( '\\' ).append( '\\' ); break;
                default:   aBuf.append( (char) c ); break;
            }
        }
    }
}
