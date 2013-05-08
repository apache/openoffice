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
#include "precompiled_vcl.hxx"

#include <stdio.h>
#include <string.h>
#include <tools/svwin.h>
#include <tools/debug.hxx>
#include <win/wincomp.hxx>
#include <win/saldata.hxx>
#include <win/salgdi.h>
#include <win/salbmp.h>

#ifndef min
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#if defined _MSC_VER
#pragma warning(push, 1)
#endif

#include <GdiPlus.h>
#include <GdiPlusEnums.h>
#include <GdiPlusColor.h>

#if defined _MSC_VER
#pragma warning(pop)
#endif

#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <vcl/salbtype.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/bitmapex.hxx>
#include <win/gdiplusobjectbuffer.hxx>
#include <impbmp.hxx>

//////////////////////////////////////////////////////////////////////////////
// cached entry for a Gdiplus::Bitmap which may be constructed from a single
// WinSalBitmap or two (when transparence is used). This class should only
// be used by GdiPlusObjectBuffer
class GdiPlusBitmapBufferNode : public basegfx::cache::node
{
private:
    boost::shared_ptr< Gdiplus::Bitmap >       maGdiPlusBitmapPtr;

    // conversion helpers
    Gdiplus::Bitmap* createGdiPlusBitmap(const WinSalBitmap& rBitmapSource);
    Gdiplus::Bitmap* createGdiPlusBitmap(const WinSalBitmap& rBitmapSource, const WinSalBitmap& rAlphaSource);

public:
    GdiPlusBitmapBufferNode(
        GdiPlusObjectBuffer& rBuffer, 
        const WinSalBitmap& rBitmapSource, 
        const WinSalBitmap* pAlphaSource);
    virtual ~GdiPlusBitmapBufferNode();

    boost::shared_ptr< Gdiplus::Bitmap > getGdiPlusBitmapPtr() const { return maGdiPlusBitmapPtr; }
};

//////////////////////////////////////////////////////////////////////////////
// cached entry for Gdiplus::GraphicsPath which may be constructed from a
// basegfx::B2DPolyPolygon or basegfx::B2DPolygon. This class should only
// be used by GdiPlusObjectBuffer
class GdiPlusGraphicsPathBufferNode : public basegfx::cache::node
{
private:
    boost::shared_ptr< Gdiplus::GraphicsPath >       maGdiPlusGraphicsPathPtr;

    // conversion helpers
    Gdiplus::GraphicsPath* createGdiPlusGraphicsPath(const basegfx::B2DPolygon& rSource);
    Gdiplus::GraphicsPath* createGdiPlusGraphicsPath(const basegfx::B2DPolyPolygon& rSource);

public:
    GdiPlusGraphicsPathBufferNode(
        GdiPlusObjectBuffer& rBuffer, 
        const basegfx::B2DPolygon& rSource);
    GdiPlusGraphicsPathBufferNode(
        GdiPlusObjectBuffer& rBuffer, 
        const basegfx::B2DPolyPolygon& rSource);
    virtual ~GdiPlusGraphicsPathBufferNode();

    boost::shared_ptr< Gdiplus::GraphicsPath > getGdiPlusGraphicsPathPtr() const { return maGdiPlusGraphicsPathPtr; }
};

//////////////////////////////////////////////////////////////////////////////

GdiPlusObjectBuffer::GdiPlusObjectBuffer()
:   basegfx::cache::manager(30), // keep for 30 lifetime rounds
    Timer()
{
    SetTimeout(1000); // one second
    Stop();
}

GdiPlusObjectBuffer::~GdiPlusObjectBuffer()
{
    Stop();
}

boost::shared_ptr< Gdiplus::Bitmap > GdiPlusObjectBuffer::getGdiPlusBitmapFromWinSalBitmap(
    const WinSalBitmap& rBitmapSource, 
    const WinSalBitmap* pAlphaSource)
{
    const Size aSize(rBitmapSource.GetSize());

    if(aSize.Width() && aSize.Height())
    {
        const GdiPlusBitmapBufferNode* pEntry = static_cast< const GdiPlusBitmapBufferNode* >(getEntry(rBitmapSource));

        if(!pEntry)
        {
            pEntry = new GdiPlusBitmapBufferNode(*this, rBitmapSource, pAlphaSource);
        }
        else
        {
            const_cast< GdiPlusBitmapBufferNode* >(pEntry)->touch();
        }

        return pEntry->getGdiPlusBitmapPtr();
    }

    return boost::shared_ptr< Gdiplus::Bitmap >();
}

boost::shared_ptr< Gdiplus::GraphicsPath > GdiPlusObjectBuffer::getGdiPlusGraphicsPathFromB2DPolygon(
    const basegfx::B2DPolygon& rPolygonSource)
{
    if(rPolygonSource.count())
    {
        const GdiPlusGraphicsPathBufferNode* pEntry = static_cast< const GdiPlusGraphicsPathBufferNode* >(getEntry(rPolygonSource.getCacheable()));

        if(!pEntry)
        {
            pEntry = new GdiPlusGraphicsPathBufferNode(*this, rPolygonSource);
        }
        else
        {
            const_cast< GdiPlusGraphicsPathBufferNode* >(pEntry)->touch();
        }

        return pEntry->getGdiPlusGraphicsPathPtr();
    }

    return boost::shared_ptr< Gdiplus::GraphicsPath >();
}

boost::shared_ptr< Gdiplus::GraphicsPath > GdiPlusObjectBuffer::getGdiPlusGraphicsPathFromB2DPolyPolygon(
    const basegfx::B2DPolyPolygon& rPolyPolygonSource)
{
    if(rPolyPolygonSource.count())
    {
        const GdiPlusGraphicsPathBufferNode* pEntry = static_cast< const GdiPlusGraphicsPathBufferNode* >(getEntry(rPolyPolygonSource.getCacheable()));

        if(!pEntry)
        {
            pEntry = new GdiPlusGraphicsPathBufferNode(*this, rPolyPolygonSource);
        }
        else
        {
            const_cast< GdiPlusGraphicsPathBufferNode* >(pEntry)->touch();
        }

        return pEntry->getGdiPlusGraphicsPathPtr();
    }

    return boost::shared_ptr< Gdiplus::GraphicsPath >();
}

void GdiPlusObjectBuffer::onEmpty()
{
    Stop();
}

void GdiPlusObjectBuffer::onFilled()
{
    Start();
}

void GdiPlusObjectBuffer::Timeout()
{
    if(!empty())
    {
        trigger();
    }

    if(!empty())
    {
        Start();
    }
}

//////////////////////////////////////////////////////////////////////////////

Gdiplus::Bitmap* GdiPlusBitmapBufferNode::createGdiPlusBitmap(
    const WinSalBitmap& rBitmapSource)
{
    Gdiplus::Bitmap* pRetval(0);
    WinSalBitmap* pSalRGB = const_cast< WinSalBitmap* >(&rBitmapSource);
    WinSalBitmap* pExtraWinSalRGB = 0;

    if(!pSalRGB->ImplGethDIB())
    {
        // we need DIB for success with AcquireBuffer, create a replacement WinSalBitmap
        pExtraWinSalRGB = new WinSalBitmap();
        pExtraWinSalRGB->Create(*pSalRGB, pSalRGB->GetBitCount());
        pSalRGB = pExtraWinSalRGB;
    }

    BitmapBuffer* pRGB = pSalRGB->AcquireBuffer(true);
    BitmapBuffer* pExtraRGB = 0;

    if(pRGB && BMP_FORMAT_24BIT_TC_BGR != (pRGB->mnFormat & ~BMP_FORMAT_TOP_DOWN))
    {
        // convert source bitmap to BMP_FORMAT_24BIT_TC_BGR format if not yet in that format
        SalTwoRect aSalTwoRect;

        aSalTwoRect.mnSrcX = aSalTwoRect.mnSrcY = aSalTwoRect.mnDestX = aSalTwoRect.mnDestY = 0;
        aSalTwoRect.mnSrcWidth = aSalTwoRect.mnDestWidth = pRGB->mnWidth;
        aSalTwoRect.mnSrcHeight = aSalTwoRect.mnDestHeight = pRGB->mnHeight;

        pExtraRGB = StretchAndConvert( 
            *pRGB,
            aSalTwoRect,
            BMP_FORMAT_24BIT_TC_BGR,
            0);

        pSalRGB->ReleaseBuffer(pRGB, true);
        pRGB = pExtraRGB;
    }

    if(pRGB 
        && pRGB->mnWidth > 0
        && pRGB->mnHeight > 0
        && BMP_FORMAT_24BIT_TC_BGR == (pRGB->mnFormat & ~BMP_FORMAT_TOP_DOWN))
    {
        const sal_uInt32 nW(pRGB->mnWidth);
        const sal_uInt32 nH(pRGB->mnHeight);

        pRetval = new Gdiplus::Bitmap(nW, nH, PixelFormat24bppRGB);

        if(pRetval)
        {
            sal_uInt8* pSrcRGB(pRGB->mpBits);
            const sal_uInt32 nExtraRGB(pRGB->mnScanlineSize - (nW * 3));
            const bool bTopDown(pRGB->mnFormat & BMP_FORMAT_TOP_DOWN);

            for(sal_uInt32 y(0); y < nH; y++)
            {
                const sal_uInt32 nYInsert(bTopDown ? y : nH - y - 1);

                for(sal_uInt32 x(0); x < nW; x++)
                {
                    const sal_uInt8 nB(*pSrcRGB++);
                    const sal_uInt8 nG(*pSrcRGB++);
                    const sal_uInt8 nR(*pSrcRGB++);

                    pRetval->SetPixel(x, nYInsert, Gdiplus::Color(nR, nG, nB));
                }

                pSrcRGB += nExtraRGB;
            }
        }
    }

    if(pExtraRGB)
    {
        delete pExtraRGB;
    }
    else
    {
        pSalRGB->ReleaseBuffer(pRGB, true);
    }

    if(pExtraWinSalRGB)
    {
        delete pExtraWinSalRGB;
    }

    return pRetval;
}

Gdiplus::Bitmap* GdiPlusBitmapBufferNode::createGdiPlusBitmap(
    const WinSalBitmap& rBitmapSource, 
    const WinSalBitmap& rAlphaSource)
{
    Gdiplus::Bitmap* pRetval(0);
    WinSalBitmap* pSalRGB = const_cast< WinSalBitmap* >(&rBitmapSource);
    WinSalBitmap* pExtraWinSalRGB = 0;

    if(!pSalRGB->ImplGethDIB())
    {
        // we need DIB for success with AcquireBuffer, create a replacement WinSalBitmap
        pExtraWinSalRGB = new WinSalBitmap();
        pExtraWinSalRGB->Create(*pSalRGB, pSalRGB->GetBitCount());
        pSalRGB = pExtraWinSalRGB;
    }

    BitmapBuffer* pRGB = pSalRGB->AcquireBuffer(true);
    BitmapBuffer* pExtraRGB = 0;

    if(pRGB && BMP_FORMAT_24BIT_TC_BGR != (pRGB->mnFormat & ~BMP_FORMAT_TOP_DOWN))
    {
        // convert source bitmap to BMP_FORMAT_24BIT_TC_BGR format if not yet in that format
        SalTwoRect aSalTwoRect;

        aSalTwoRect.mnSrcX = aSalTwoRect.mnSrcY = aSalTwoRect.mnDestX = aSalTwoRect.mnDestY = 0;
        aSalTwoRect.mnSrcWidth = aSalTwoRect.mnDestWidth = pRGB->mnWidth;
        aSalTwoRect.mnSrcHeight = aSalTwoRect.mnDestHeight = pRGB->mnHeight;

        pExtraRGB = StretchAndConvert( 
            *pRGB,
            aSalTwoRect,
            BMP_FORMAT_24BIT_TC_BGR,
            0);

        pSalRGB->ReleaseBuffer(pRGB, true);
        pRGB = pExtraRGB;
    }

    WinSalBitmap* pSalA = const_cast< WinSalBitmap* >(&rAlphaSource);
    WinSalBitmap* pExtraWinSalA = 0;

    if(!pSalA->ImplGethDIB())
    {
        // we need DIB for success with AcquireBuffer, create a replacement WinSalBitmap
        pExtraWinSalA = new WinSalBitmap();
        pExtraWinSalA->Create(*pSalA, pSalA->GetBitCount());
        pSalA = pExtraWinSalA;
    }

    BitmapBuffer* pA = pSalA->AcquireBuffer(true);
    BitmapBuffer* pExtraA = 0;

    if(pA && BMP_FORMAT_8BIT_PAL != (pA->mnFormat & ~BMP_FORMAT_TOP_DOWN))
    {
        // convert alpha bitmap to BMP_FORMAT_8BIT_PAL format if not yet in that format
        SalTwoRect aSalTwoRect;

        aSalTwoRect.mnSrcX = aSalTwoRect.mnSrcY = aSalTwoRect.mnDestX = aSalTwoRect.mnDestY = 0;
        aSalTwoRect.mnSrcWidth = aSalTwoRect.mnDestWidth = pA->mnWidth;
        aSalTwoRect.mnSrcHeight = aSalTwoRect.mnDestHeight = pA->mnHeight;
        const BitmapPalette& rTargetPalette = Bitmap::GetGreyPalette(256);

        pExtraA = StretchAndConvert( 
            *pA,
            aSalTwoRect,
            BMP_FORMAT_8BIT_PAL,
            &rTargetPalette);

        pSalA->ReleaseBuffer(pA, true);
        pA = pExtraA;
    }

    if(pRGB 
        && pA 
        && pRGB->mnWidth > 0
        && pRGB->mnHeight > 0
        && pRGB->mnWidth == pA->mnWidth 
        && pRGB->mnHeight == pA->mnHeight 
        && BMP_FORMAT_24BIT_TC_BGR == (pRGB->mnFormat & ~BMP_FORMAT_TOP_DOWN)
        && BMP_FORMAT_8BIT_PAL == (pA->mnFormat & ~BMP_FORMAT_TOP_DOWN))
    {
        // we have alpha and bitmap in known formats, create GdiPlus Bitmap as 32bit ARGB
        const sal_uInt32 nW(pRGB->mnWidth);
        const sal_uInt32 nH(pRGB->mnHeight);

        pRetval = new Gdiplus::Bitmap(nW, nH, PixelFormat32bppARGB);

        if(pRetval)
        {
            sal_uInt8* pSrcRGB(pRGB->mpBits);
            sal_uInt8* pSrcA(pA->mpBits);
            const sal_uInt32 nExtraRGB(pRGB->mnScanlineSize - (nW * 3));
            const sal_uInt32 nExtraA(pA->mnScanlineSize - nW);
            const bool bTopDown(pRGB->mnFormat & BMP_FORMAT_TOP_DOWN);

            for(sal_uInt32 y(0); y < nH; y++)
            {
                const sal_uInt32 nYInsert(bTopDown ? y : nH - y - 1);

                for(sal_uInt32 x(0); x < nW; x++)
                {
                    const sal_uInt8 nB(*pSrcRGB++);
                    const sal_uInt8 nG(*pSrcRGB++);
                    const sal_uInt8 nR(*pSrcRGB++);
                    const sal_uInt8 nA(0xff - *pSrcA++);

                    pRetval->SetPixel(x, nYInsert, Gdiplus::Color(nA, nR, nG, nB));
                }

                pSrcRGB += nExtraRGB;
                pSrcA += nExtraA;
            }
        }
    }

    if(pExtraA)
    {
        delete pExtraA;
    }
    else
    {
        pSalA->ReleaseBuffer(pA, true);
    }

    if(pExtraWinSalA)
    {
        delete pExtraWinSalA;
    }

    if(pExtraRGB)
    {
        delete pExtraRGB;
    }
    else
    {
        pSalRGB->ReleaseBuffer(pRGB, true);
    }

    if(pExtraWinSalRGB)
    {
        delete pExtraWinSalRGB;
    }

    return pRetval;
}

GdiPlusBitmapBufferNode::GdiPlusBitmapBufferNode(
    GdiPlusObjectBuffer& rBuffer, 
    const WinSalBitmap& rBitmapSource, 
    const WinSalBitmap* pAlphaSource)
:   basegfx::cache::node(rBuffer, rBitmapSource),
    maGdiPlusBitmapPtr()
{
    if(pAlphaSource)
    {
        maGdiPlusBitmapPtr.reset(createGdiPlusBitmap(rBitmapSource, *pAlphaSource));
    }
    else
    {
        maGdiPlusBitmapPtr.reset(createGdiPlusBitmap(rBitmapSource));
    }
}

GdiPlusBitmapBufferNode::~GdiPlusBitmapBufferNode()
{
}

//////////////////////////////////////////////////////////////////////////////

namespace
{
    void addGdiPlusGraphicsPath(
        Gdiplus::GraphicsPath* pRetval, const basegfx::B2DPolygon& rSource,
        bool bForceSingleEdges)
    {
        const sal_uInt32 nCount(rSource.count());

        if(nCount)
        {
            const sal_uInt32 nEdgeCount(rSource.isClosed() ? nCount : nCount - 1);
            const bool bControls(rSource.areControlPointsUsed());
            basegfx::B2DPoint aCurr(rSource.getB2DPoint(0));
            Gdiplus::PointF aFCurr(Gdiplus::REAL(aCurr.getX()), Gdiplus::REAL(aCurr.getY()));

            for(sal_uInt32 a(0); a < nEdgeCount; a++)
            {
                const sal_uInt32 nNextIndex((a + 1) % nCount);
                const basegfx::B2DPoint aNext(rSource.getB2DPoint(nNextIndex));
                const Gdiplus::PointF aFNext(Gdiplus::REAL(aNext.getX()), Gdiplus::REAL(aNext.getY()));

                if(bControls && (rSource.isNextControlPointUsed(a) || rSource.isPrevControlPointUsed(nNextIndex)))
                {
                    const basegfx::B2DPoint aCa(rSource.getNextControlPoint(a));
                    const basegfx::B2DPoint aCb(rSource.getPrevControlPoint(nNextIndex));

                    pRetval->AddBezier(
                        aFCurr, 
                        Gdiplus::PointF(Gdiplus::REAL(aCa.getX()), Gdiplus::REAL(aCa.getY())),
                        Gdiplus::PointF(Gdiplus::REAL(aCb.getX()), Gdiplus::REAL(aCb.getY())),
                        aFNext);
                }
                else
                {
                    pRetval->AddLine(aFCurr, aFNext);
                }

                if(a + 1 < nEdgeCount)
                {
                    aFCurr = aFNext;

                    if(bForceSingleEdges)
                    {
                        pRetval->StartFigure();
                    }
                }
            }
        }
    }
} // end of anonymous namespace

//////////////////////////////////////////////////////////////////////////////

Gdiplus::GraphicsPath* GdiPlusGraphicsPathBufferNode::createGdiPlusGraphicsPath(const basegfx::B2DPolygon& rSource)
{
    Gdiplus::GraphicsPath* pRetval = 0;
    const sal_uInt32 nCount(rSource.count());

    if(nCount)
    {
        pRetval = new Gdiplus::GraphicsPath;

        addGdiPlusGraphicsPath(pRetval, rSource, false);

        if(rSource.isClosed())
        {
            pRetval->CloseFigure();
        }
    }

    return pRetval;
}

Gdiplus::GraphicsPath* GdiPlusGraphicsPathBufferNode::createGdiPlusGraphicsPath(const basegfx::B2DPolyPolygon& rSource)
{
    Gdiplus::GraphicsPath* pRetval = 0;
    const sal_uInt32 nCount(rSource.count());

    if(nCount)
    {
        pRetval = new Gdiplus::GraphicsPath;
        bool bFirst(true);

        for(sal_uInt32 a(0); a < nCount; a++)
        {
            const basegfx::B2DPolygon aPolygon(rSource.getB2DPolygon(a));

            // try to get an already existing and buffered Gdiplus::GraphicsPath from the single B2DPolygon
            boost::shared_ptr< Gdiplus::GraphicsPath > aSource(GetSalData()->maGdiPlusObjectBuffer.getGdiPlusGraphicsPathFromB2DPolygon(aPolygon));

            if(aSource.get())
            {
                if(bFirst)
                {
                    pRetval->StartFigure();
                    bFirst = false;
                }

                // concatenate partial path
                pRetval->AddPath(aSource.get(), FALSE);

                // for PolyPolygon, always close
                pRetval->CloseFigure();
            }
        }
    }

    return pRetval;
}

GdiPlusGraphicsPathBufferNode::GdiPlusGraphicsPathBufferNode(
    GdiPlusObjectBuffer& rBuffer, 
    const basegfx::B2DPolygon& rSource)
:   basegfx::cache::node(rBuffer, rSource.getCacheable()),
    maGdiPlusGraphicsPathPtr()
{
    maGdiPlusGraphicsPathPtr.reset(createGdiPlusGraphicsPath(rSource));
}

GdiPlusGraphicsPathBufferNode::GdiPlusGraphicsPathBufferNode(
    GdiPlusObjectBuffer& rBuffer, 
    const basegfx::B2DPolyPolygon& rSource)
:   basegfx::cache::node(rBuffer, rSource.getCacheable()),
    maGdiPlusGraphicsPathPtr()
{
    maGdiPlusGraphicsPathPtr.reset(createGdiPlusGraphicsPath(rSource));
}

GdiPlusGraphicsPathBufferNode::~GdiPlusGraphicsPathBufferNode()
{
}

//////////////////////////////////////////////////////////////////////////////

VCL_DLLPUBLIC boost::shared_ptr< Gdiplus::Bitmap > getBufferedGdiPlusBitmapFromBitmapEx(
    const BitmapEx& rBitmapEx)
{
    const SalBitmap& rSalSrcBmp = *rBitmapEx.ImplGetBitmapImpBitmap()->ImplGetSalBitmap();
    const SalBitmap* pSalAlphaBmp = 0;

    if(rBitmapEx.IsTransparent())
    {
        pSalAlphaBmp = rBitmapEx.ImplGetMaskImpBitmap()->ImplGetSalBitmap();
    }

    return GetSalData()->maGdiPlusObjectBuffer.getGdiPlusBitmapFromWinSalBitmap(
        static_cast< const WinSalBitmap& >(rSalSrcBmp), 
        static_cast< const WinSalBitmap* >(pSalAlphaBmp));
}

VCL_DLLPUBLIC boost::shared_ptr< Gdiplus::GraphicsPath > getBufferedGdiPlusGraphicsPathFromB2DPolygon(
    const basegfx::B2DPolygon& rSource)
{
    return GetSalData()->maGdiPlusObjectBuffer.getGdiPlusGraphicsPathFromB2DPolygon(rSource);
}

VCL_DLLPUBLIC boost::shared_ptr< Gdiplus::GraphicsPath > getBufferedGdiPlusGraphicsPathFromB2DPolyPolygon(
    const basegfx::B2DPolyPolygon& rSource)
{
    return GetSalData()->maGdiPlusObjectBuffer.getGdiPlusGraphicsPathFromB2DPolyPolygon(rSource);
}

//////////////////////////////////////////////////////////////////////////////

bool WinSalGraphics::drawPolyPolygon( const ::basegfx::B2DPolyPolygon& rPolyPolygon, double fTransparency)
{
    const sal_uInt32 nCount(rPolyPolygon.count());

    if(mbBrush && nCount && (fTransparency >= 0.0 && fTransparency < 1.0))
    {
        const boost::shared_ptr< Gdiplus::GraphicsPath > aPath(getBufferedGdiPlusGraphicsPathFromB2DPolyPolygon(rPolyPolygon));

        if(aPath.get())
        {
            Gdiplus::Graphics aGraphics(getHDC());
            const sal_uInt8 aTrans((sal_uInt8)255 - (sal_uInt8)basegfx::fround(fTransparency * 255.0));
            Gdiplus::Color aTestColor(aTrans, SALCOLOR_RED(maFillColor), SALCOLOR_GREEN(maFillColor), SALCOLOR_BLUE(maFillColor));
            Gdiplus::SolidBrush aTestBrush(aTestColor);

            if(getAntiAliasB2DDraw())
            {
                aGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            }
            else
            {
                aGraphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
            }

            aGraphics.FillPath(&aTestBrush, aPath.get());
        }
    }

    return true;
}

bool WinSalGraphics::drawPolyLine( 
    const basegfx::B2DPolygon& rPolygon, 
    double fTransparency, 
    const basegfx::B2DVector& rLineWidths, 
    basegfx::B2DLineJoin eLineJoin,
    com::sun::star::drawing::LineCap eLineCap)
{
    const sal_uInt32 nCount(rPolygon.count());

    if(mbPen && nCount)
    {
        boost::shared_ptr< Gdiplus::GraphicsPath > aPath;
        const sal_uInt8 aTrans = (sal_uInt8)basegfx::fround( 255 * (1.0 - fTransparency) );
        Gdiplus::Color aTestColor(aTrans, SALCOLOR_RED(maLineColor), SALCOLOR_GREEN(maLineColor), SALCOLOR_BLUE(maLineColor));
        Gdiplus::Pen aTestPen(aTestColor, Gdiplus::REAL(rLineWidths.getX()));
        bool bNoLineJoin(false);

        switch(eLineJoin)
        {
            default : // basegfx::B2DLINEJOIN_NONE :
            {
                if(basegfx::fTools::more(rLineWidths.getX(), 0.0))
                {
                    bNoLineJoin = true;
                }
                break;
            }
            case basegfx::B2DLINEJOIN_BEVEL :
            {
                aTestPen.SetLineJoin(Gdiplus::LineJoinBevel);
                break;
            }
            case basegfx::B2DLINEJOIN_MIDDLE :
            case basegfx::B2DLINEJOIN_MITER :
            {
                const Gdiplus::REAL aMiterLimit(15.0);
                aTestPen.SetMiterLimit(aMiterLimit);
                aTestPen.SetLineJoin(Gdiplus::LineJoinMiter);
                break;
            }
            case basegfx::B2DLINEJOIN_ROUND :
            {
                aTestPen.SetLineJoin(Gdiplus::LineJoinRound);
                break;
            }
        }

        if(bNoLineJoin)
        {
            // need to create a special version to support the line join mode 'none'. This
            // is simply done by creating single edges, so no line joins will be visualized
            Gdiplus::GraphicsPath* pNew = new Gdiplus::GraphicsPath;

            addGdiPlusGraphicsPath(pNew, rPolygon, true);

            if(rPolygon.isClosed())
            {
                pNew->CloseFigure();
            }

            aPath.reset(pNew);
        }
        else
        {
            // use the buffered common geometry version
            aPath = getBufferedGdiPlusGraphicsPathFromB2DPolygon(rPolygon);
        }

        if(aPath.get())
        {
            Gdiplus::Graphics aGraphics(getHDC());

            switch(eLineCap)
            {
                default: /*com::sun::star::drawing::LineCap_BUTT*/
                {
                    // nothing to do
                    break;
                }
                case com::sun::star::drawing::LineCap_ROUND:
                {
                    aTestPen.SetStartCap(Gdiplus::LineCapRound);
                    aTestPen.SetEndCap(Gdiplus::LineCapRound);
                    break;
                }
                case com::sun::star::drawing::LineCap_SQUARE:
                {
                    aTestPen.SetStartCap(Gdiplus::LineCapSquare);
                    aTestPen.SetEndCap(Gdiplus::LineCapSquare);
                    break;
                }
            }

            if(getAntiAliasB2DDraw())
            {
                aGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            }
            else
            {
                aGraphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
            }

            aGraphics.DrawPath(&aTestPen, aPath.get());
        }
    }

    return true;
}

// -----------------------------------------------------------------------

void paintToGdiPlus(
    Gdiplus::Graphics& rGraphics,
    const SalTwoRect& rTR,
    Gdiplus::Bitmap& rBitmap)
{
    // only parts of source are used
    Gdiplus::PointF aDestPoints[3];
    Gdiplus::ImageAttributes aAttributes;

    // define target region as paralellogram
    aDestPoints[0].X = Gdiplus::REAL(rTR.mnDestX);
    aDestPoints[0].Y = Gdiplus::REAL(rTR.mnDestY);
    aDestPoints[1].X = Gdiplus::REAL(rTR.mnDestX + rTR.mnDestWidth);
    aDestPoints[1].Y = Gdiplus::REAL(rTR.mnDestY);
    aDestPoints[2].X = Gdiplus::REAL(rTR.mnDestX);
    aDestPoints[2].Y = Gdiplus::REAL(rTR.mnDestY + rTR.mnDestHeight);

    aAttributes.SetWrapMode(Gdiplus::WrapModeTileFlipXY);

    rGraphics.DrawImage(
        &rBitmap, 
        aDestPoints,
        3,
        Gdiplus::REAL(rTR.mnSrcX),
        Gdiplus::REAL(rTR.mnSrcY),
        Gdiplus::REAL(rTR.mnSrcWidth),
        Gdiplus::REAL(rTR.mnSrcHeight),
        Gdiplus::UnitPixel,
        &aAttributes,
        0,
        0);
}

// -----------------------------------------------------------------------

void setInterpolationMode(
    Gdiplus::Graphics& rGraphics,
    const long& rSrcWidth,
    const long& rDestWidth,
    const long& rSrcHeight,
    const long& rDestHeight)
{
    const bool bSameWidth(rSrcWidth == rDestWidth);
    const bool bSameHeight(rSrcHeight == rDestHeight);

    if(bSameWidth && bSameHeight)
    {
        rGraphics.SetInterpolationMode(Gdiplus::InterpolationModeInvalid);
    }
    else if(rDestWidth > rSrcWidth && rDestHeight > rSrcHeight)
    {
        rGraphics.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
    }
    else if(rDestWidth < rSrcWidth && rDestHeight < rSrcHeight)
    {
        rGraphics.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
    }
    else
    {
        rGraphics.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
    }
}


bool WinSalGraphics::tryDrawBitmapGdiPlus(const SalTwoRect& rTR, const SalBitmap& rSrcBitmap)
{
    if(rTR.mnSrcWidth && rTR.mnSrcHeight && rTR.mnDestWidth && rTR.mnDestHeight)
    {
        const WinSalBitmap& rSalBitmap = static_cast< const WinSalBitmap& >(rSrcBitmap);
        boost::shared_ptr< Gdiplus::Bitmap > aARGB(GetSalData()->maGdiPlusObjectBuffer.getGdiPlusBitmapFromWinSalBitmap(
            rSalBitmap,
            0));

        if(aARGB.get())
        {
            Gdiplus::Graphics aGraphics(getHDC());

            setInterpolationMode(
                aGraphics, 
                rTR.mnSrcWidth,
                rTR.mnDestWidth, 
                rTR.mnSrcHeight,
                rTR.mnDestHeight);

            paintToGdiPlus(
                aGraphics, 
                rTR, 
                *aARGB.get());

            return true;
        }
    }

    return false;
}

bool WinSalGraphics::drawAlphaBitmap( 
    const SalTwoRect& rTR,
    const SalBitmap& rSrcBitmap, 
    const SalBitmap& rAlphaBmp)
{
    if(rTR.mnSrcWidth && rTR.mnSrcHeight && rTR.mnDestWidth && rTR.mnDestHeight)
    {
        const WinSalBitmap& rSalBitmap = static_cast< const WinSalBitmap& >(rSrcBitmap);
        const WinSalBitmap& rSalAlpha = static_cast< const WinSalBitmap& >(rAlphaBmp);
        boost::shared_ptr< Gdiplus::Bitmap > aARGB(GetSalData()->maGdiPlusObjectBuffer.getGdiPlusBitmapFromWinSalBitmap(
            rSalBitmap, 
            &rSalAlpha));

        if(aARGB.get())
        {
            Gdiplus::Graphics aGraphics(getHDC());

            setInterpolationMode(
                aGraphics, 
                rTR.mnSrcWidth,
                rTR.mnDestWidth, 
                rTR.mnSrcHeight,
                rTR.mnDestHeight);

            paintToGdiPlus(
                aGraphics, 
                rTR, 
                *aARGB.get());

            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------

bool WinSalGraphics::drawTransformedBitmap(
    const basegfx::B2DPoint& rNull,
    const basegfx::B2DPoint& rX,
    const basegfx::B2DPoint& rY,
    const SalBitmap& rSourceBitmap,
    const SalBitmap* pAlphaBitmap)
{
    const WinSalBitmap& rSalBitmap = static_cast< const WinSalBitmap& >(rSourceBitmap);
    const WinSalBitmap* pSalAlpha = static_cast< const WinSalBitmap* >(pAlphaBitmap);
    boost::shared_ptr< Gdiplus::Bitmap > aARGB(GetSalData()->maGdiPlusObjectBuffer.getGdiPlusBitmapFromWinSalBitmap(
        rSalBitmap, 
        pSalAlpha));

    if(aARGB.get())
    {
        const long nSrcWidth(aARGB->GetWidth());
        const long nSrcHeight(aARGB->GetHeight());

        if(nSrcWidth && nSrcHeight)
        {
            const long nDestWidth(basegfx::fround(basegfx::B2DVector(rX - rNull).getLength()));
            const long nDestHeight(basegfx::fround(basegfx::B2DVector(rY - rNull).getLength()));

            if(nDestWidth && nDestHeight)
            {
                Gdiplus::Graphics aGraphics(getHDC());
                Gdiplus::PointF aDestPoints[3];
                Gdiplus::ImageAttributes aAttributes;

                setInterpolationMode(
                    aGraphics, 
                    nSrcWidth,
                    nDestWidth, 
                    nSrcHeight,
                    nDestHeight);

                // this mode is only capable of drawing the whole bitmap to a paralellogram
                aDestPoints[0].X = Gdiplus::REAL(rNull.getX());
                aDestPoints[0].Y = Gdiplus::REAL(rNull.getY());
                aDestPoints[1].X = Gdiplus::REAL(rX.getX());
                aDestPoints[1].Y = Gdiplus::REAL(rX.getY());
                aDestPoints[2].X = Gdiplus::REAL(rY.getX());
                aDestPoints[2].Y = Gdiplus::REAL(rY.getY());

                aAttributes.SetWrapMode(Gdiplus::WrapModeTileFlipXY);

                aGraphics.DrawImage(
                    aARGB.get(), 
                    aDestPoints, 
                    3,
                    Gdiplus::REAL(0.0),
                    Gdiplus::REAL(0.0),
                    Gdiplus::REAL(nSrcWidth),
                    Gdiplus::REAL(nSrcHeight),
                    Gdiplus::UnitPixel,
                    &aAttributes,
                    0,
                    0);
            }
        }

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------
// eof
