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

import org.apache.pdfbox.contentstream.PDFGraphicsStreamEngine;
import org.apache.pdfbox.cos.COSName;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.common.PDRectangle;
import org.apache.pdfbox.pdmodel.font.PDFont;
import org.apache.pdfbox.pdmodel.font.PDFontDescriptor;
import org.apache.pdfbox.pdmodel.graphics.PDLineDashPattern;
import org.apache.pdfbox.pdmodel.graphics.color.PDColor;
import org.apache.pdfbox.pdmodel.graphics.color.PDColorSpace;
import org.apache.pdfbox.pdmodel.graphics.image.PDImage;
import org.apache.pdfbox.pdmodel.graphics.image.PDImageXObject;
import org.apache.pdfbox.pdmodel.graphics.state.PDGraphicsState;
import org.apache.pdfbox.pdmodel.graphics.state.PDTextState;
import org.apache.pdfbox.pdmodel.interactive.action.PDAction;
import org.apache.pdfbox.pdmodel.interactive.action.PDActionURI;
import org.apache.pdfbox.pdmodel.interactive.annotation.PDAnnotation;
import org.apache.pdfbox.pdmodel.interactive.annotation.PDAnnotationLink;
import org.apache.pdfbox.util.Matrix;
import org.apache.pdfbox.util.Vector;

import javax.imageio.IIOImage;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriteParam;
import javax.imageio.ImageWriter;
import javax.imageio.stream.ImageOutputStream;
import java.awt.geom.AffineTransform;
import java.awt.geom.Area;
import java.awt.geom.GeneralPath;
import java.awt.geom.Path2D;
import java.awt.geom.PathIterator;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 * Walks one page's content stream and emits the pdfi helper protocol.
 *
 * <h3>Coordinate spaces -- read this before changing any geometry</h3>
 *
 * The protocol is not in one space, and that is not an accident: it mirrors
 * what the consumer in tree/pdfiprocessor.cxx does with each kind of value.
 *
 * <ul>
 * <li><b>Paths</b> go out in PDF <em>user</em> space, untransformed, because
 *     pdfiprocessor.cxx:674 multiplies them by the declared CTM itself.
 *     PDFBox has already applied the content stream's own CTM to the points
 *     it hands us, so the CTM we declare is just the page's user-to-device
 *     transform -- which is why this class never has to track q/Q.</li>
 * <li><b>Glyph rectangles</b> go out in <em>device</em> space, already
 *     transformed, because the consumer uses them as-is.</li>
 * <li><b>Images</b> declare a CTM of user-to-device composed with the image's
 *     own matrix, since the consumer places an image by transforming the unit
 *     square.</li>
 * <li><b>The font matrix</b> is the raw text matrix (Tm), carrying no page
 *     transform at all -- it tells the consumer how the text is oriented and
 *     sheared, nothing more.</li>
 * </ul>
 *
 * Device space is 1/7200 inch, y down, origin at the top left of the cropped
 * and rotated page.
 *
 * <h3>State</h3>
 *
 * Rather than mirroring the PDF graphics state machine, every drawing
 * operation is wrapped in {@code saveState} / {@code restoreState} and states
 * its own absolute state in between.  More verbose on the wire, but it cannot
 * drift out of step with the document, and it makes clipping correct for free:
 * the clip is redeclared per operation and unwound by {@code restoreState}.
 */
final class PdfiEngine extends PDFGraphicsStreamEngine
{
    private final PdfiWriter m_aOut;

    /** PDF user space to consumer device space, including crop and /Rotate. */
    private final AffineTransform m_aDevXform;

    /** The declared CTM as a protocol argument list: user-to-device. */
    private final double[] m_aDevCtm;

    /** Page bounds in user space, for suppressing no-op clips. */
    private final PDRectangle m_aCropBox;

    private final Map<PDFont, Long> m_aFontIds;
    private final long[] m_pNextFontId;

    /** Subpaths of the path being built, in user space. */
    private final List<SubPath> m_aPath = new ArrayList<SubPath>();
    private SubPath m_aCurrent;
    private Point2D m_aCurrentPoint = new Point2D.Double( 0, 0 );

    /** Winding rule recorded by W/W*, applied when the path op arrives. */
    private int m_nPendingClip = -1;

    /** Whether a text run is open and still needs its endTextObject. */
    private boolean m_bInText = false;

    // Emitted-state cache.  The protocol is absolute, so a value only has to
    // be restated when it actually changes; without this a text-heavy page
    // costs a few hundred bytes per glyph, which on a long document is tens
    // of megabytes down a pipe.
    private double[] m_aLastCtm = null;
    private double[] m_aLastFill = null;
    private double[] m_aLastStroke = null;
    private double m_fLastLineWidth = Double.NaN;
    private int m_nLastLineCap = -1;
    private int m_nLastLineJoin = -1;
    private double m_fLastMiterLimit = Double.NaN;
    private String m_aLastDash = null;
    private long m_nLastFontId = -1;
    private double m_fLastFontSize = Double.NaN;
    private int m_nLastRenderMode = -1;

    /** The clip currently declared, and whether a bracket is open for it. */
    private Area m_aActiveClip = null;
    private boolean m_bClipBracketOpen = false;

    private static final class SubPath
    {
        final List<double[]> aPoints = new ArrayList<double[]>();
        boolean bClosed = false;
    }

    PdfiEngine( PDPage aPage, PdfiWriter aOut,
                Map<PDFont, Long> aFontIds, long[] pNextFontId )
    {
        super( aPage );
        m_aOut = aOut;
        m_aFontIds = aFontIds;
        m_pNextFontId = pNextFontId;
        m_aCropBox = aPage.getCropBox();
        m_aDevXform = deviceTransform( m_aCropBox, aPage.getRotation() );
        m_aDevCtm = new double[] {
            m_aDevXform.getScaleX(), m_aDevXform.getShearY(),
            m_aDevXform.getShearX(), m_aDevXform.getScaleY(),
            m_aDevXform.getTranslateX(), m_aDevXform.getTranslateY() };
    }

    // ---- page geometry ----------------------------------------------------

    /**
     * Builds the user-to-device transform for a page.
     *
     * <p>PDFBox does <em>not</em> apply /Rotate -- measured, not assumed: a
     * page processed with rotation 0, 90 and 270 yields byte-identical
     * coordinates.  So the rotation has to be baked in here, or every
     * landscape and scanned document imports sideways.</p>
     */
    static AffineTransform deviceTransform( PDRectangle aCrop, int nRotation )
    {
        final double S = PdfiWriter.DEVICE_UNITS_PER_POINT;
        final double x0 = aCrop.getLowerLeftX(), y0 = aCrop.getLowerLeftY();
        final double x1 = aCrop.getUpperRightX(), y1 = aCrop.getUpperRightY();

        // normalise to 0/90/180/270; /Rotate may be negative or over 360
        int r = nRotation % 360;
        if( r < 0 )
            r += 360;
        r = ( r / 90 ) * 90;

        // AffineTransform(m00, m10, m01, m11, m02, m12), i.e. a, b, c, d, e, f
        switch( r )
        {
            case 90:
                return new AffineTransform( 0, S, S, 0, -S * y0, -S * x0 );
            case 180:
                return new AffineTransform( -S, 0, 0, S, S * x1, -S * y0 );
            case 270:
                return new AffineTransform( 0, -S, -S, 0, S * y1, S * x1 );
            default:
                return new AffineTransform( S, 0, 0, -S, -S * x0, S * y1 );
        }
    }

    /** Page size in device units, after crop and rotation. */
    static double[] pageSize( PDRectangle aCrop, int nRotation )
    {
        final double S = PdfiWriter.DEVICE_UNITS_PER_POINT;
        final double w = aCrop.getWidth() * S, h = aCrop.getHeight() * S;

        int r = nRotation % 360;
        if( r < 0 )
            r += 360;
        return ( ( r / 90 ) % 2 != 0 ) ? new double[] { h, w }
                                       : new double[] { w, h };
    }

    // ---- page driver ------------------------------------------------------

    void run() throws IOException
    {
        final double[] aSize = pageSize( m_aCropBox, getPage().getRotation() );
        m_aOut.cmd( "startPage" ).d( aSize[0] ).d( aSize[1] ).eol();

        processPage( getPage() );

        endTextRun();
        closeClipBracket();
        emitLinks();

        m_aOut.cmd( "endPage" ).eol();
    }

    private void emitLinks() throws IOException
    {
        final List<PDAnnotation> aAnnots;
        try
        {
            aAnnots = getPage().getAnnotations();
        }
        catch( IOException e )
        {
            return; // a broken annotation array costs the links, not the page
        }

        for( PDAnnotation aAnnot : aAnnots )
        {
            if( !( aAnnot instanceof PDAnnotationLink ) )
                continue;

            final PDAction aAction = ( (PDAnnotationLink) aAnnot ).getAction();
            if( !( aAction instanceof PDActionURI ) )
                continue;

            final String aURI = ( (PDActionURI) aAction ).getURI();
            if( aURI == null || aURI.length() == 0 )
                continue;

            final PDRectangle aRect = aAnnot.getRectangle();
            if( aRect == null )
                continue;

            // Annotation rectangles are in user space; the consumer stores
            // hyperlink bounds as page coordinates, so they need the same
            // device transform as everything else.
            final Point2D a = toDevice( aRect.getLowerLeftX(), aRect.getLowerLeftY() );
            final Point2D b = toDevice( aRect.getUpperRightX(), aRect.getUpperRightY() );

            m_aOut.cmd( "drawLink" )
                  .d( Math.min( a.getX(), b.getX() ) )
                  .d( Math.min( a.getY(), b.getY() ) )
                  .d( Math.max( a.getX(), b.getX() ) )
                  .d( Math.max( a.getY(), b.getY() ) );
            m_aOut.text( aURI );
        }
    }

    private Point2D toDevice( double x, double y )
    {
        return m_aDevXform.transform( new Point2D.Double( x, y ), null );
    }

    // ---- state emission ---------------------------------------------------

    /**
     * Declares everything the coming operation depends on, restating only what
     * has actually changed since the last one.
     */
    private void beginOp( double[] aCtm, boolean bStroke, boolean bFill )
        throws IOException
    {
        syncClip();

        if( !sameArray( m_aLastCtm, aCtm ) )
        {
            m_aOut.cmd( "updateCtm" );
            for( int i = 0; i < 6; ++i )
                m_aOut.d( aCtm[i] );
            m_aOut.eol();
            m_aLastCtm = aCtm.clone();
        }

        final PDGraphicsState aGS = getGraphicsState();

        if( bFill )
        {
            final double[] aCol = rgba( aGS.getNonStrokingColorSpace(),
                                        aGS.getNonStrokingColor(),
                                        aGS.getNonStrokeAlphaConstant() );
            if( !sameArray( m_aLastFill, aCol ) )
            {
                emitColor( "updateFillColor", aCol );
                m_aLastFill = aCol;
            }
        }

        if( bStroke )
        {
            final double[] aCol = rgba( aGS.getStrokingColorSpace(),
                                        aGS.getStrokingColor(),
                                        aGS.getAlphaConstant() );
            if( !sameArray( m_aLastStroke, aCol ) )
            {
                emitColor( "updateStrokeColor", aCol );
                m_aLastStroke = aCol;
            }
        }

        if( bStroke )
        {
            // Line width is in user space; the consumer scales it by the CTM.
            if( aGS.getLineWidth() != m_fLastLineWidth )
            {
                m_aOut.cmd( "updateLineWidth" ).d( aGS.getLineWidth() ).eol();
                m_fLastLineWidth = aGS.getLineWidth();
            }
            if( aGS.getLineCap() != m_nLastLineCap )
            {
                m_aOut.cmd( "updateLineCap" ).i( aGS.getLineCap() ).eol();
                m_nLastLineCap = aGS.getLineCap();
            }
            if( aGS.getLineJoin() != m_nLastLineJoin )
            {
                m_aOut.cmd( "updateLineJoin" ).i( aGS.getLineJoin() ).eol();
                m_nLastLineJoin = aGS.getLineJoin();
            }
            if( aGS.getMiterLimit() != m_fLastMiterLimit )
            {
                m_aOut.cmd( "updateMiterLimit" ).d( aGS.getMiterLimit() ).eol();
                m_fLastMiterLimit = aGS.getMiterLimit();
            }
            emitDash( aGS.getLineDashPattern() );
        }
    }

    /**
     * Keeps the declared clip in step with the document's.
     *
     * <p>The protocol can only ever narrow a clip ({@code clipPath}
     * intersects), so widening one means unwinding it with
     * {@code restoreState}.  Rather than bracket every operation, one bracket
     * is held open for as long as the clip stays the same -- which for a body
     * of text under a single clip is the whole run.  Unwinding also discards
     * the emitted-state cache, since the consumer has just popped its own
     * copy of that state.</p>
     */
    private void syncClip() throws IOException
    {
        final Area aClip = effectiveClip();
        if( aClip == m_aActiveClip )
            return;

        if( m_bClipBracketOpen )
        {
            m_aOut.cmd( "restoreState" ).eol();
            m_bClipBracketOpen = false;
            invalidateStateCache();
        }

        m_aActiveClip = aClip;

        if( aClip != null )
        {
            final List<SubPath> aSubs = flatten( aClip );
            if( !aSubs.isEmpty() )
            {
                m_aOut.cmd( "saveState" ).eol();
                m_bClipBracketOpen = true;
                invalidateStateCache();

                m_aOut.cmd( "clipPath" );
                writePath( aSubs );
                m_aOut.eol();
            }
        }
    }

    /**
     * The current clip, or null when it does not actually restrict anything --
     * the consumer already clips to the page bounds on startPage.
     */
    private Area effectiveClip()
    {
        final Area aClip = getGraphicsState().getCurrentClippingPath();
        if( aClip == null )
            return null;

        return aClip.contains( m_aCropBox.getLowerLeftX(), m_aCropBox.getLowerLeftY(),
                               m_aCropBox.getWidth(), m_aCropBox.getHeight() )
               ? null : aClip;
    }

    /** Closes any open clip bracket; called once the page is finished. */
    private void closeClipBracket() throws IOException
    {
        if( m_bClipBracketOpen )
        {
            m_aOut.cmd( "restoreState" ).eol();
            m_bClipBracketOpen = false;
            invalidateStateCache();
        }
        m_aActiveClip = null;
    }

    private void invalidateStateCache()
    {
        m_aLastCtm = null;
        m_aLastFill = null;
        m_aLastStroke = null;
        m_fLastLineWidth = Double.NaN;
        m_nLastLineCap = -1;
        m_nLastLineJoin = -1;
        m_fLastMiterLimit = Double.NaN;
        m_aLastDash = null;
        m_nLastFontId = -1;
        m_fLastFontSize = Double.NaN;
        m_nLastRenderMode = -1;
    }

    private static boolean sameArray( double[] a, double[] b )
    {
        if( a == null || b == null )
            return false;
        for( int i = 0; i < a.length; ++i )
        {
            if( a[i] != b[i] )
                return false;
        }
        return true;
    }

    private void emitColor( String aCommand, double[] aRGBA ) throws IOException
    {
        m_aOut.cmd( aCommand ).d( aRGBA[0] ).d( aRGBA[1] ).d( aRGBA[2] )
              .d( aRGBA[3] ).eol();
    }

    /** Resolves a PDF colour to sRGB plus alpha, defaulting to opaque black. */
    private static double[] rgba( PDColorSpace aCS, PDColor aColor, double fAlpha )
    {
        float[] aRGB = { 0f, 0f, 0f };
        if( aCS != null && aColor != null )
        {
            try
            {
                aRGB = aCS.toRGB( aColor.getComponents() );
            }
            catch( Exception e )
            {
                // An unsupported or malformed colour space costs this colour,
                // not the page; black is the PDF default anyway.
            }
        }
        return new double[] { aRGB[0], aRGB[1], aRGB[2], fAlpha };
    }

    private void emitDash( PDLineDashPattern aDash ) throws IOException
    {
        final StringBuilder aBuf = new StringBuilder( 32 );
        if( aDash != null && aDash.getDashArray() != null
            && aDash.getDashArray().length != 0 )
        {
            final float[] aArray = aDash.getDashArray();
            aBuf.append( aDash.getPhase() ).append( ' ' ).append( aArray.length );
            for( int i = 0; i < aArray.length; ++i )
                aBuf.append( ' ' ).append( aArray[i] );
        }

        final String aKey = aBuf.toString();
        if( aKey.equals( m_aLastDash ) )
            return;
        m_aLastDash = aKey;

        if( aDash == null || aDash.getDashArray() == null
            || aDash.getDashArray().length == 0 )
        {
            m_aOut.cmd( "updateLineDash" ).eol(); // empty means solid
            return;
        }

        final float[] aArray = aDash.getDashArray();
        m_aOut.cmd( "updateLineDash" ).d( aDash.getPhase() ).i( aArray.length );
        for( int i = 0; i < aArray.length; ++i )
            m_aOut.d( aArray[i] );
        m_aOut.eol();
    }

    /**
     * Declares the current clip, unless it is the whole page -- the consumer
     * already clips to the page bounds on startPage, so re-stating it would
     * only add work and rounding error.
     */
    private void emitClip() throws IOException
    {
        final Area aClip = getGraphicsState().getCurrentClippingPath();
        if( aClip == null )
            return;

        if( aClip.contains( m_aCropBox.getLowerLeftX(), m_aCropBox.getLowerLeftY(),
                            m_aCropBox.getWidth(), m_aCropBox.getHeight() ) )
            return;

        final List<SubPath> aSubs = flatten( aClip );
        if( aSubs.isEmpty() )
            return;

        m_aOut.cmd( "clipPath" );
        writePath( aSubs );
        m_aOut.eol();
    }

    // ---- path collection --------------------------------------------------

    public void moveTo( float x, float y )
    {
        m_aCurrent = new SubPath();
        m_aPath.add( m_aCurrent );
        m_aCurrent.aPoints.add( new double[] { x, y, 0 } );
        m_aCurrentPoint = new Point2D.Double( x, y );
    }

    public void lineTo( float x, float y )
    {
        if( m_aCurrent == null )
            moveTo( x, y );
        else
        {
            m_aCurrent.aPoints.add( new double[] { x, y, 0 } );
            m_aCurrentPoint = new Point2D.Double( x, y );
        }
    }

    public void curveTo( float x1, float y1, float x2, float y2, float x3, float y3 )
    {
        if( m_aCurrent == null )
            moveTo( x1, y1 );

        // Two control points flagged 1, then the on-curve endpoint flagged 0 --
        // the consumer folds exactly that triple back into a bezier segment.
        m_aCurrent.aPoints.add( new double[] { x1, y1, 1 } );
        m_aCurrent.aPoints.add( new double[] { x2, y2, 1 } );
        m_aCurrent.aPoints.add( new double[] { x3, y3, 0 } );
        m_aCurrentPoint = new Point2D.Double( x3, y3 );
    }

    public void appendRectangle( Point2D p0, Point2D p1, Point2D p2, Point2D p3 )
    {
        m_aCurrent = new SubPath();
        m_aPath.add( m_aCurrent );
        m_aCurrent.aPoints.add( new double[] { p0.getX(), p0.getY(), 0 } );
        m_aCurrent.aPoints.add( new double[] { p1.getX(), p1.getY(), 0 } );
        m_aCurrent.aPoints.add( new double[] { p2.getX(), p2.getY(), 0 } );
        m_aCurrent.aPoints.add( new double[] { p3.getX(), p3.getY(), 0 } );
        m_aCurrent.bClosed = true;
        m_aCurrentPoint = new Point2D.Double( p0.getX(), p0.getY() );
    }

    public void closePath()
    {
        if( m_aCurrent != null )
            m_aCurrent.bClosed = true;
    }

    public Point2D getCurrentPoint()
    {
        return m_aCurrentPoint;
    }

    public void clip( int nWindingRule )
    {
        // W/W* only marks the intent; the clip is taken from the path when the
        // painting operator that terminates it arrives.
        m_nPendingClip = nWindingRule;
    }

    public void endPath()
    {
        applyPendingClip();
        m_aPath.clear();
        m_aCurrent = null;
    }

    private void applyPendingClip()
    {
        if( m_nPendingClip == -1 || m_aPath.isEmpty() )
        {
            m_nPendingClip = -1;
            return;
        }

        final GeneralPath aPath = new GeneralPath(
            m_nPendingClip == PathIterator.WIND_EVEN_ODD
                ? Path2D.WIND_EVEN_ODD : Path2D.WIND_NON_ZERO );

        for( SubPath aSub : m_aPath )
        {
            final List<double[]> aPts = aSub.aPoints;
            if( aPts.isEmpty() )
                continue;

            aPath.moveTo( aPts.get( 0 )[0], aPts.get( 0 )[1] );
            for( int i = 1; i < aPts.size(); ++i )
            {
                // Control points are approximated by their chord here: this
                // path is only ever used as a clip region, never drawn.
                aPath.lineTo( aPts.get( i )[0], aPts.get( i )[1] );
            }
            if( aSub.bClosed )
                aPath.closePath();
        }

        getGraphicsState().intersectClippingPath( aPath );
        m_nPendingClip = -1;
    }

    // ---- path painting ----------------------------------------------------

    public void strokePath() throws IOException
    {
        paint( true, false, false );
    }

    public void fillPath( int nWindingRule ) throws IOException
    {
        paint( false, true, nWindingRule == PathIterator.WIND_EVEN_ODD );
    }

    public void fillAndStrokePath( int nWindingRule ) throws IOException
    {
        paint( true, true, nWindingRule == PathIterator.WIND_EVEN_ODD );
    }

    private void paint( boolean bStroke, boolean bFill, boolean bEvenOdd )
        throws IOException
    {
        endTextRun();

        if( !m_aPath.isEmpty() )
        {
            beginOp( m_aDevCtm, bStroke, bFill );

            if( bFill )
            {
                m_aOut.cmd( bEvenOdd ? "eoFillPath" : "fillPath" );
                writePath( m_aPath );
                m_aOut.eol();
            }
            if( bStroke )
            {
                m_aOut.cmd( "strokePath" );
                writePath( m_aPath );
                m_aOut.eol();
            }
        }

        applyPendingClip();
        m_aPath.clear();
        m_aCurrent = null;
    }

    private void writePath( List<SubPath> aSubs ) throws IOException
    {
        for( SubPath aSub : aSubs )
        {
            if( aSub.aPoints.isEmpty() )
                continue;

            m_aOut.tok( "subpath" ).i( aSub.bClosed ? 1 : 0 );
            for( double[] aPt : aSub.aPoints )
                m_aOut.d( aPt[0] ).d( aPt[1] ).i( (int) aPt[2] );
        }
    }

    /** Flattens an AWT area into protocol subpaths (straight segments only). */
    private static List<SubPath> flatten( Area aArea )
    {
        final List<SubPath> aRes = new ArrayList<SubPath>();
        final PathIterator it = aArea.getPathIterator( null, 0.1 );
        final double[] aSeg = new double[6];
        SubPath aCur = null;

        while( !it.isDone() )
        {
            switch( it.currentSegment( aSeg ) )
            {
                case PathIterator.SEG_MOVETO:
                    aCur = new SubPath();
                    aRes.add( aCur );
                    aCur.aPoints.add( new double[] { aSeg[0], aSeg[1], 0 } );
                    break;
                case PathIterator.SEG_LINETO:
                    if( aCur != null )
                        aCur.aPoints.add( new double[] { aSeg[0], aSeg[1], 0 } );
                    break;
                case PathIterator.SEG_CLOSE:
                    if( aCur != null )
                        aCur.bClosed = true;
                    break;
                default:
                    break; // flattened iterator yields no curves
            }
            it.next();
        }
        return aRes;
    }

    // ---- text -------------------------------------------------------------

    @Override
    protected void showGlyph( Matrix aTrm, PDFont aFont, int nCode, Vector aDisplacement )
        throws IOException
    {
        final String aText = safeToUnicode( aFont, nCode );

        if( aText != null && aText.length() != 0 )
        {
            // The glyph rectangle goes out in PDF USER space, like paths and
            // for the same reason: pdfiprocessor.cxx transforms it by the
            // declared CTM (drawCharGlyphs -> calcTransformedRectBounds).
            // Emitting device coordinates here means the page transform is
            // applied twice, putting every glyph a hundred times too far out
            // and a hundred times too large -- text frames metres wide, far off
            // the canvas, and so invisible rather than obviously wrong.
            final AffineTransform aTextXform = aTrm.createAffineTransform();

            final double fXScale = Math.hypot( aTextXform.getScaleX(), aTextXform.getShearY() );
            final double fYScale = Math.hypot( aTextXform.getShearX(), aTextXform.getScaleY() );

            final double fOriginX = aTextXform.getTranslateX();
            final double fBaseY = aTextXform.getTranslateY();

            double fAscent = 0.75; // a reasonable em fraction when unstated
            final PDFontDescriptor aFD = aFont.getFontDescriptor();
            if( aFD != null && aFD.getAscent() > 0 )
                fAscent = aFD.getAscent() / 1000.0;

            // User space is y-UP, so the top of the glyph is above the
            // baseline and the rectangle runs downwards from it -- the same
            // (top, top - height) convention poppler emits.
            final double fTop = fBaseY + fAscent * fYScale;
            final double fWidth = aDisplacement.getX() * fXScale;

            // The font SIZE, unlike the rectangle, is a device-space quantity:
            // drawtreevisiting.cxx converts it back with size*72/7200.  This is
            // poppler's getTransformedFontSize().
            final AffineTransform aComposed = new AffineTransform( m_aDevXform );
            aComposed.concatenate( aTextXform );
            final double fSizeDevice =
                Math.hypot( aComposed.getShearX(), aComposed.getScaleY() );

            beginOp( m_aDevCtm, false, true );

            emitFont( aFont, fSizeDevice );

            final PDTextState aTS = getGraphicsState().getTextState();
            if( aTS.getRenderingMode().ordinal() != m_nLastRenderMode )
            {
                m_nLastRenderMode = aTS.getRenderingMode().ordinal();
                m_aOut.cmd( "setTextRenderMode" ).i( m_nLastRenderMode ).eol();
            }

            // The font matrix carries orientation, shear and aspect -- no page
            // transform, and deliberately NO overall scale.
            //
            // The consumer applies this matrix as a transform about the glyph's
            // own origin (pdfiprocessor.cxx getTransformGlyphContext) *and*
            // separately sizes the text from the font size above.  A document
            // that puts its scale in Tm rather than in Tf -- [6 0 0 6] with
            // Tf 1, which plenty of generators do -- would therefore be scaled
            // by 6 twice over, throwing the text off the page.  That is exactly
            // what poppler does here: it sends getTransformedFontSize(), which
            // already includes Tm's scale, alongside the raw Tm.
            //
            // Dividing by Tm's own vertical scale removes the double count
            // while keeping rotation, shear and the horizontal/vertical aspect
            // ratio, which the single font-size number cannot express.
            final Matrix aTm = getTextMatrix();
            double fTmScale = Math.hypot( aTm.getShearX(), aTm.getScaleY() );
            if( fTmScale < 1e-9 )
                fTmScale = 1.0;

            m_aOut.cmd( "drawChar" )
                  .d( fOriginX )
                  .d( fTop )
                  .d( fOriginX + ( fWidth != 0.0 ? fWidth : fYScale ) )
                  .d( fTop - fYScale )
                  .d( aTm.getScaleX() / fTmScale ).d( aTm.getShearX() / fTmScale )
                  .d( aTm.getShearY() / fTmScale ).d( aTm.getScaleY() / fTmScale );
            m_aOut.text( aText );
            m_bInText = true;
        }

        super.showGlyph( aTrm, aFont, nCode, aDisplacement );
    }

    /**
     * A font whose /ToUnicode is missing or broken makes toUnicode() throw or
     * return null.  That costs the glyph, never the document -- an unmappable
     * glyph is exactly the case where the rest of the page still matters.
     */
    private static String safeToUnicode( PDFont aFont, int nCode )
    {
        try
        {
            return aFont.toUnicode( nCode );
        }
        catch( Exception e )
        {
            return null;
        }
    }

    private void emitFont( PDFont aFont, double fSizeDevice ) throws IOException
    {
        Long aId = m_aFontIds.get( aFont );
        if( aId == null )
        {
            aId = Long.valueOf( m_pNextFontId[0]++ );
            m_aFontIds.put( aFont, aId );
        }

        String aName = aFont.getName();
        if( aName == null || aName.length() == 0 )
            aName = "Arial";

        boolean bBold = false, bItalic = false;
        final PDFontDescriptor aFD = aFont.getFontDescriptor();
        if( aFD != null )
        {
            bBold = aFD.isForceBold() || aFD.getFontWeight() >= 600;
            bItalic = aFD.isItalic() || aFD.getItalicAngle() != 0;
        }

        // fileLen 0: we never ship the embedded font file.  The consumer only
        // wants it to recover a family name via FontIdentificator, and PDFBox
        // has already given us one -- including the subset tag the consumer's
        // parseFontFamilyName() knows how to strip.
        if( aId.longValue() == m_nLastFontId && fSizeDevice == m_fLastFontSize )
            return;
        m_nLastFontId = aId.longValue();
        m_fLastFontSize = fSizeDevice;

        m_aOut.cmd( "updateFont" )
              .l( aId.longValue() )
              .i( aFont.isEmbedded() ? 1 : 0 )
              .i( bBold ? 1 : 0 )
              .i( bItalic ? 1 : 0 )
              .i( 0 )
              .d( fSizeDevice )
              .i( 0 );
        m_aOut.text( aName );
    }

    private void endTextRun() throws IOException
    {
        if( m_bInText )
        {
            m_aOut.cmd( "endTextObject" ).eol();
            m_bInText = false;
        }
    }

    // ---- images -----------------------------------------------------------

    public void drawImage( PDImage aImage ) throws IOException
    {
        endTextRun();

        // The image occupies the unit square under the current CTM, so the
        // consumer needs that composed with the page transform.
        final AffineTransform aComposed = new AffineTransform( m_aDevXform );
        aComposed.concatenate(
            getGraphicsState().getCurrentTransformationMatrix().createAffineTransform() );

        final double[] aCtm = {
            aComposed.getScaleX(), aComposed.getShearY(),
            aComposed.getShearX(), aComposed.getScaleY(),
            aComposed.getTranslateX(), aComposed.getTranslateY() };

        // Prefer the image's own JPEG bytes, untouched.  Decoding and
        // re-encoding to PNG is lossless but enormous -- a 19.7 MB book of
        // photographs produced 76 MB of PNG, four times the whole source file
        // -- and costs a decode plus a compress per image.  The consumer takes
        // JPEG directly (readImageImpl), so the common case converts nothing
        // and never builds a BufferedImage at all.
        byte[] aData = originalJpeg( aImage );
        String aFormat = "JPEG";
        int nWidth, nHeight;

        if( aData != null )
        {
            nWidth  = aImage.getWidth();
            nHeight = aImage.getHeight();
        }
        else
        {
            final BufferedImage aBitmap;
            try
            {
                aBitmap = aImage.getImage();
            }
            catch( Exception e )
            {
                // An undecodable image (a broken JPX or JBIG2 stream, say)
                // costs the image and nothing else.
                return;
            }
            if( aBitmap == null )
                return;

            // The source was already lossy, so it is photographic by
            // construction and re-encoding it losslessly is the wrong trade:
            // PNG made a 19.7 MB book into 76 MB.  Staying lossy costs one
            // recompression of content that was never crisp.  Anything stored
            // losslessly -- line art, screenshots, masks -- keeps PNG, where
            // JPEG would smear edges for no gain.
            if( wasLossy( aImage ) && !aBitmap.getColorModel().hasAlpha() )
            {
                aData   = encodeJpeg( aBitmap );
                aFormat = "JPEG";
            }
            if( aData == null )
            {
                aData   = encodePng( aBitmap );
                aFormat = "PNG";
            }
            nWidth  = aBitmap.getWidth();
            nHeight = aBitmap.getHeight();
        }
        if( aData == null )
            return;

        beginOp( aCtm, false, true );

        // A stencil mask paints the current fill colour through the image, so
        // the consumer wants it as drawMask rather than drawImage.
        m_aOut.cmd( aImage.isStencil() ? "drawMask" : "drawImage" )
              .i( nWidth ).i( nHeight ).i( 0 )
              .tok( aFormat ).i( aData.length ).eol();
        m_aOut.binary( aData );
    }

    /**
     * The image's own JPEG bytes, when passing them through unchanged is
     * faithful -- otherwise null, and the caller re-encodes.
     *
     * <p>Every condition here is a way the stored bytes alone would not
     * reproduce what the page shows: a stencil or an explicit mask needs the
     * mask applied, a soft mask needs alpha that JPEG cannot carry, a /Decode
     * array remaps samples, and a four-component (CMYK) JPEG is the case
     * decoders most often disagree about.  In all of those, decoding to a
     * bitmap and re-encoding is the honest answer.</p>
     */
    private static byte[] originalJpeg( PDImage aImage )
    {
        if( !( aImage instanceof PDImageXObject ) )
            return null;

        final PDImageXObject aXObj = (PDImageXObject) aImage;
        try
        {
            if( !"jpg".equals( aXObj.getSuffix() ) )
                return null;
            if( aXObj.isStencil() || aXObj.getDecode() != null )
                return null;
            if( aXObj.getSoftMask() != null || aXObj.getMask() != null )
                return null;

            final PDColorSpace aCS = aXObj.getColorSpace();
            if( aCS == null || aCS.getNumberOfComponents() > 3 )
                return null;

            // stop before DCTDecode: the stream as stored, still JPEG
            final InputStream aIn =
                aXObj.createInputStream( Collections.singletonList( "DCTDecode" ) );
            try
            {
                final ByteArrayOutputStream aBuf = new ByteArrayOutputStream( 1 << 16 );
                final byte[] aChunk = new byte[8192];
                int n;
                while( ( n = aIn.read( aChunk ) ) > 0 )
                    aBuf.write( aChunk, 0, n );
                return aBuf.size() > 0 ? aBuf.toByteArray() : null;
            }
            finally
            {
                aIn.close();
            }
        }
        catch( Exception e )
        {
            return null; // anything unexpected: fall back to re-encoding
        }
    }

    /** Whether the image was stored in a lossy format to begin with. */
    private static boolean wasLossy( PDImage aImage )
    {
        final String aSuffix = aImage.getSuffix();
        return "jpg".equals( aSuffix ) || "jpx".equals( aSuffix );
    }

    /**
     * Re-encodes as JPEG.  Used only for images that arrived lossy and whose
     * colour PDFBox has already resolved to sRGB -- notably the CMYK JPEGs that
     * {@link #originalJpeg} refuses to pass through untouched, since decoders
     * disagree about Adobe's inverted CMYK and a wrong guess inverts the page.
     */
    private static byte[] encodeJpeg( BufferedImage aImage )
    {
        ImageWriter aWriter = null;
        try
        {
            // JPEG cannot store alpha, and a writer handed ARGB either fails or
            // writes colours that are subtly wrong; go through opaque RGB.
            BufferedImage aRgb = aImage;
            if( aImage.getType() != BufferedImage.TYPE_INT_RGB )
            {
                aRgb = new BufferedImage( aImage.getWidth(), aImage.getHeight(),
                                          BufferedImage.TYPE_INT_RGB );
                final java.awt.Graphics2D g = aRgb.createGraphics();
                g.drawImage( aImage, 0, 0, null );
                g.dispose();
            }

            final java.util.Iterator<ImageWriter> it =
                ImageIO.getImageWritersByFormatName( "jpeg" );
            if( !it.hasNext() )
                return null;
            aWriter = it.next();

            final ImageWriteParam aParam = aWriter.getDefaultWriteParam();
            if( aParam.canWriteCompressed() )
            {
                aParam.setCompressionMode( ImageWriteParam.MODE_EXPLICIT );
                aParam.setCompressionQuality( 0.85f );
            }

            final ByteArrayOutputStream aBuf = new ByteArrayOutputStream( 1 << 16 );
            final ImageOutputStream aOut = ImageIO.createImageOutputStream( aBuf );
            try
            {
                aWriter.setOutput( aOut );
                aWriter.write( null, new IIOImage( aRgb, null, null ), aParam );
            }
            finally
            {
                aOut.close();
            }
            return aBuf.size() > 0 ? aBuf.toByteArray() : null;
        }
        catch( Exception e )
        {
            return null; // caller falls back to PNG
        }
        finally
        {
            if( aWriter != null )
                aWriter.dispose();
        }
    }

    /**
     * Re-encodes to PNG for the wire.
     *
     * <p>Deliberately not the hand-written encoder the poppler helper uses
     * (xpdfwrapper/pnghelper.cxx): that one computes scanline offsets in
     * 32-bit arithmetic from PDF-controlled dimensions and writes past its
     * buffer, which is AOO-2026-014.  ImageIO is bounds-safe and needs no
     * maintenance from us.</p>
     */
    private static byte[] encodePng( BufferedImage aImage )
    {
        try
        {
            final ByteArrayOutputStream aBuf = new ByteArrayOutputStream( 1 << 16 );
            if( !ImageIO.write( aImage, "png", aBuf ) )
                return null;
            return aBuf.toByteArray();
        }
        catch( IOException e )
        {
            return null;
        }
    }

    // ---- unsupported ------------------------------------------------------

    public void shadingFill( COSName aName )
    {
        // Shadings (types 1-7, including mesh and coons patches) have no ODF
        // equivalent beyond a plain gradient.  Dropping is honest; forcing
        // them into a two-stop gradient would be a worse lie.
    }
}
