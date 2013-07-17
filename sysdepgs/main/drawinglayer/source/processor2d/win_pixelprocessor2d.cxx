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
#include "precompiled_drawinglayer.hxx"

#include <drawinglayer/processor2d/win_pixelprocessor2d.hxx>
#include <drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx>
#include <vcl/sysdata.hxx>
#include <vcl/outdev.hxx>
#include <svl/ctloptions.hxx>
#include <vcl/svapp.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <drawinglayer/primitive2d/pointarrayprimitive2d.hxx>
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>
#include <drawinglayer/primitive2d/polypolygonprimitive2d.hxx>
#include <drawinglayer/primitive2d/transparenceprimitive2d.hxx>
#include <drawinglayer/primitive2d/invertprimitive2d.hxx>
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <vcl/virdev.hxx>
//#include <GdiPlusEnums.h>
//#include <GdiPlusColor.h>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace
{
    void setAntialiasing(Gdiplus::Graphics& rGraphics, bool bOn)
    {
        static Gdiplus::SmoothingMode aModeOn = Gdiplus::SmoothingModeHighQuality;
        rGraphics.SetSmoothingMode(bOn 
            ? aModeOn
            : Gdiplus::SmoothingModeNone);
    }

    void setMatrix(Gdiplus::Graphics& rGraphics, const basegfx::B2DHomMatrix& rSource)
    {
        const Gdiplus::Matrix aMatrix(
            rSource.get(0, 0),
            rSource.get(1, 0),
            rSource.get(0, 1),
            rSource.get(1, 1),
            rSource.get(0, 2),
            rSource.get(1, 2));

        rGraphics.SetTransform(&aMatrix);
    }

    bool isVisible(Gdiplus::Graphics& rGraphics, const basegfx::B2DRange& rRange)
    {
        if(rRange.isEmpty())
        {
            return false;
        }

        return rGraphics.IsVisible(
            Gdiplus::REAL(rRange.getMinX()),
            Gdiplus::REAL(rRange.getMinY()),
            Gdiplus::REAL(rRange.getWidth()),
            Gdiplus::REAL(rRange.getHeight()));
    }
} // end of anonymous namespace

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        Win_PixelProcessor2D::Win_PixelProcessor2D(
            const geometry::ViewInformation2D& rViewInformation, 
            OutputDevice& rOutDev)
        :   BaseProcessor2D(rViewInformation),
            mnWidth(rOutDev.GetOutputSizePixel().Width()),
            mnHeight(rOutDev.GetOutputSizePixel().Height()),
            mpGraphics(0),
            maBColorModifierStack(),
            maCurrentTransformation(),
            maDrawinglayerOpt()
        {
            // prepare getCurrentTransformation() matrix with viewTransformation to target directly to pixels
            setCurrentTransformation(rViewInformation.getObjectToViewTransformation());

            // create Gdiplus::Graphics entry for new OutputDevice
            SystemGraphicsData aSystemGraphicsData(rOutDev.GetSystemGfxData());
            mpGraphics = new Gdiplus::Graphics(aSystemGraphicsData.hDC);

            // init AntiAliasing and transformation
            setAntialiasing(getGraphics(), getOptionsDrawinglayer().IsAntiAliasing());
            setMatrix(getGraphics(), getCurrentTransformation());
        }

        Win_PixelProcessor2D::~Win_PixelProcessor2D()
        {
            // cleanup Gdiplus::Graphics
            delete mpGraphics;
        }

        void Win_PixelProcessor2D::processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate)
        {
            switch(rCandidate.getPrimitive2DID())
            {
                // basic, non-decomposable primitives
                case PRIMITIVE2D_ID_BITMAPPRIMITIVE2D:
                {
                    const primitive2d::BitmapPrimitive2D& rBitmapPrimitive2D = static_cast< const primitive2d::BitmapPrimitive2D& >(rCandidate);
                    const BitmapEx& rBitmapEx = rBitmapPrimitive2D.getBitmapEx();

                    if(!rBitmapEx.IsEmpty())
                    {
                        const basegfx::B2DRange aRange(rBitmapPrimitive2D.getB2DRange(getViewInformation2D()));

                        if(isVisible(getGraphics(), aRange))
                        {
                            const boost::shared_ptr< Gdiplus::Bitmap > aBitmap(getBufferedGdiPlusBitmapFromBitmapEx(rBitmapEx));

                            if(aBitmap.get())
                            {
                                const basegfx::B2DHomMatrix aCurrent(getCurrentTransformation() * rBitmapPrimitive2D.getTransform());
                                Gdiplus::PointF aDestPoints[3];
                                Gdiplus::ImageAttributes aAttributes;

                                setMatrix(getGraphics(), aCurrent);
                                aDestPoints[0].X = Gdiplus::REAL(0.0);
                                aDestPoints[0].Y = Gdiplus::REAL(0.0);
                                aDestPoints[1].X = Gdiplus::REAL(1.0);
                                aDestPoints[1].Y = Gdiplus::REAL(0.0);
                                aDestPoints[2].X = Gdiplus::REAL(0.0);
                                aDestPoints[2].Y = Gdiplus::REAL(1.0);
                                aAttributes.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
                                getGraphics().DrawImage(
                                    aBitmap.get(), 
                                    aDestPoints, 
                                    3,
                                    Gdiplus::REAL(0.0),
                                    Gdiplus::REAL(0.0),
                                    Gdiplus::REAL(aBitmap->GetWidth()),
                                    Gdiplus::REAL(aBitmap->GetHeight()),
                                    Gdiplus::UnitPixel,
                                    &aAttributes,
                                    0,
                                    0);
                                setMatrix(getGraphics(), getCurrentTransformation());
                            }
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_POINTARRAYPRIMITIVE2D:
                {
                    const primitive2d::PointArrayPrimitive2D& rPointArrayPrimitive2D = static_cast< const primitive2d::PointArrayPrimitive2D& >(rCandidate);
                    const basegfx::B2DRange aRange(rPointArrayPrimitive2D.getB2DRange(getViewInformation2D()));

                    if(isVisible(getGraphics(), aRange))
                    {
                    }

                    break;
                }
                case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D:
                {
                    const primitive2d::PolygonHairlinePrimitive2D& rPolygonHairlinePrimitive2D = static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate);
                    const basegfx::B2DPolygon& rPolygon = rPolygonHairlinePrimitive2D.getB2DPolygon();

                    if(rPolygon.count())
                    {
                        const basegfx::B2DRange aRange(rPolygonHairlinePrimitive2D.getB2DRange(getViewInformation2D()));

                        if(isVisible(getGraphics(), aRange))
                        {
                            const boost::shared_ptr< Gdiplus::GraphicsPath > aPath(getBufferedGdiPlusGraphicsPathFromB2DPolygon(rPolygon));

                            if(aPath.get())
                            {
                                const basegfx::BColor aBColor(getBColorModifierStack().getModifiedColor(rPolygonHairlinePrimitive2D.getBColor()));
                                const Gdiplus::Color aColor(BYTE(aBColor.getRed() * 255.0), BYTE(aBColor.getGreen() * 255.0), BYTE(aBColor.getBlue() * 255.0));
                                const Gdiplus::Pen aPen(aColor, 1.0);

                                getGraphics().DrawPath(&aPen, aPath.get());
                            }
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONCOLORPRIMITIVE2D:
                {
                    const primitive2d::PolyPolygonColorPrimitive2D& rPolyPolygonColorPrimitive2D = static_cast< const primitive2d::PolyPolygonColorPrimitive2D& >(rCandidate);
                    const basegfx::B2DPolyPolygon& rPolyPolygon = rPolyPolygonColorPrimitive2D.getB2DPolyPolygon();

                    if(rPolyPolygon.count())
                    {
                        const basegfx::B2DRange aRange(rPolyPolygonColorPrimitive2D.getB2DRange(getViewInformation2D()));

                        if(isVisible(getGraphics(), aRange))
                        {
                            const boost::shared_ptr< Gdiplus::GraphicsPath > aPath(getBufferedGdiPlusGraphicsPathFromB2DPolyPolygon(rPolyPolygon));

                            if(aPath.get())
                            {
                                const basegfx::BColor aBColor(getBColorModifierStack().getModifiedColor(rPolyPolygonColorPrimitive2D.getBColor()));
                                const Gdiplus::Color aColor(BYTE(aBColor.getRed() * 255.0), BYTE(aBColor.getGreen() * 255.0), BYTE(aBColor.getBlue() * 255.0));
                                const Gdiplus::SolidBrush aBrush(aColor);

                                getGraphics().FillPath(&aBrush, aPath.get());
                            }
                        }
                    }
                    break;
                }

                // basic grouping primitives
                case PRIMITIVE2D_ID_TRANSPARENCEPRIMITIVE2D:
                {
                    const primitive2d::TransparencePrimitive2D& rTransparencePrimitive2D = static_cast< const primitive2d::TransparencePrimitive2D& >(rCandidate);

                    if(rTransparencePrimitive2D.getTransparence().hasElements())
                    {
                        basegfx::B2DRange aRange(rTransparencePrimitive2D.getB2DRange(getViewInformation2D()));

                        if(isVisible(getGraphics(), aRange))
                        {
                            aRange.transform(getCurrentTransformation());
                            aRange.intersect(basegfx::B2DRange(0.0, 0.0, mnWidth, mnHeight));

                            if(!aRange.isEmpty())
                            {
                                // get involved pixel sizes
                                const sal_uInt32 nLeft(floor(aRange.getMinX()));
                                const sal_uInt32 nTop(floor(aRange.getMinY()));
                                const sal_uInt32 nRight(ceil(aRange.getMaxX()));
                                const sal_uInt32 nBottom(ceil(aRange.getMaxY()));
                                const sal_uInt32 nWidth(nRight - nLeft);
                                const sal_uInt32 nHeight(nBottom - nTop);

                                // basic mechanism is: Allocate Gdi+ Bitmaps for content, mask and alpha, create
                                // GDI+ Graphics for them (to paint to these). Use RGBA for content, RGB only for
                                // mask and alpha. Mix these together (unfortunately GDI+ has no mechanism to mix
                                // or copy/mix these when blitting) and paint the result as GDI+ Bitmap.
                                //
                                // It would also be possible to first fill the content bitmap with the target content,
                                // but GDI+ seems not to provide a mechanism to get this data (a Bitmap from a Graphics).
                                // When this would be possible, creating mask could be avoided, but alpha still would
                                // need to be copied handish to content.
                                // This could evtl. be done by starting the whole renderer with getting a GDI+ Bitmap
                                // from the given OutputDevice and always have a Bitmap as render target. This would
                                // lead in the direction to always have renderers which rander to Bitmaps, not to
                                // OutputDevices in general, a valid thought anyways.
                                Gdiplus::Bitmap aBitmapContent(nWidth, nHeight, PixelFormat32bppARGB);
                                Gdiplus::Graphics aGraphicsContent(&aBitmapContent);
                                Gdiplus::Bitmap aBitmapMask(nWidth, nHeight, PixelFormat24bppRGB);
                                Gdiplus::Graphics aGraphicsMask(&aBitmapMask);
                                Gdiplus::Bitmap aBitmapAlpha(nWidth, nHeight, PixelFormat24bppRGB);
                                Gdiplus::Graphics aGraphicsAlpha(&aBitmapAlpha);

                                // create new transformation and set
                                const basegfx::B2DHomMatrix aLastCurrentTransformation(getCurrentTransformation());
                                basegfx::B2DHomMatrix aNewTransformation(aLastCurrentTransformation);
                                aNewTransformation.translate(nLeft * -1.0, nTop * -1.0);
                                setCurrentTransformation(aNewTransformation);

                                // clear both
                                const Gdiplus::SolidBrush aClearingBrush(Gdiplus::Color(255, 255, 255, 255));

                                aGraphicsContent.FillRectangle(
                                    &aClearingBrush,
                                    (INT)0,
                                    (INT)0,
                                    (INT)nWidth,
                                    (INT)nHeight);
                                aGraphicsMask.FillRectangle(
                                    &aClearingBrush,
                                    (INT)0,
                                    (INT)0,
                                    (INT)nWidth,
                                    (INT)nHeight);
                                aGraphicsAlpha.FillRectangle(
                                    &aClearingBrush,
                                    (INT)0,
                                    (INT)0,
                                    (INT)nWidth,
                                    (INT)nHeight);

                                // set other values
                                const Gdiplus::GraphicsState aStateContent(aGraphicsContent.Save());
                                const Gdiplus::GraphicsState aStateMask(aGraphicsMask.Save());
                                const Gdiplus::GraphicsState aStateAlpha(aGraphicsAlpha.Save());

                                setAntialiasing(aGraphicsContent, getOptionsDrawinglayer().IsAntiAliasing());
                                setMatrix(aGraphicsContent, getCurrentTransformation());
                                aGraphicsContent.SetClip(&getGraphics(), Gdiplus::CombineModeReplace);

                                setAntialiasing(aGraphicsMask, getOptionsDrawinglayer().IsAntiAliasing());
                                setMatrix(aGraphicsMask, getCurrentTransformation());
                                aGraphicsMask.SetClip(&getGraphics(), Gdiplus::CombineModeReplace);

                                setAntialiasing(aGraphicsAlpha, getOptionsDrawinglayer().IsAntiAliasing());
                                setMatrix(aGraphicsAlpha, getCurrentTransformation());
                                aGraphicsAlpha.SetClip(&getGraphics(), Gdiplus::CombineModeReplace);

                                // make content the graphics target, for Gdiplus::Graphics and OutputDevice
                                Gdiplus::Graphics* pOriginal = mpGraphics;
                                const sal_uInt32 nOrigWidth(mnWidth);
                                const sal_uInt32 nOrigHeight(mnHeight);
                                mpGraphics = &aGraphicsContent;
                                mnWidth = nWidth;
                                mnHeight = nHeight;

                                // do process children
                                process(rTransparencePrimitive2D.getChildren());

                                // target to mask
                                mpGraphics = &aGraphicsMask;

                                // do process children as black to create mask
                                const basegfx::BColor aBlack(0.0, 0.0, 0.0);
                                const basegfx::BColorModifier aBColorModifier(aBlack);

                                getBColorModifierStack().push(aBColorModifier);
                                process(rTransparencePrimitive2D.getChildren());
                                getBColorModifierStack().pop();

                                // target to alpha
                                mpGraphics = &aGraphicsAlpha;

                                // do process transparency
                                process(rTransparencePrimitive2D.getTransparence());

                                // restore graphics target and trans, flush
                                mpGraphics = pOriginal;
                                mnWidth = nOrigWidth;
                                mnHeight = nOrigHeight;

                                setCurrentTransformation(aLastCurrentTransformation);
                                aGraphicsContent.Flush(Gdiplus::FlushIntentionSync);
                                aGraphicsContent.Restore(aStateContent);
                                aGraphicsMask.Flush(Gdiplus::FlushIntentionSync);
                                aGraphicsMask.Restore(aStateMask);
                                aGraphicsAlpha.Flush(Gdiplus::FlushIntentionSync);
                                aGraphicsAlpha.Restore(aStateAlpha);

                                // copy alpha to content
                                const Gdiplus::Rect aRect(0, 0, nWidth, nHeight);
                                Gdiplus::BitmapData aBitmapDataContent;
                                Gdiplus::BitmapData aBitmapDataMask;
                                Gdiplus::BitmapData aBitmapDataAlpha;

                                aBitmapContent.LockBits(&aRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &aBitmapDataContent);
                                aBitmapMask.LockBits(&aRect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &aBitmapDataMask);
                                aBitmapAlpha.LockBits(&aRect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &aBitmapDataAlpha);
                                sal_uInt8* pTargetLineContent = (sal_uInt8*)aBitmapDataContent.Scan0;
                                sal_uInt8* pSourceLineMask = (sal_uInt8*)aBitmapDataMask.Scan0;
                                sal_uInt8* pSourceLineAlpha = (sal_uInt8*)aBitmapDataAlpha.Scan0;

                                for(sal_uInt32 y(0); y < nHeight; y++)
                                {
                                    sal_uInt8* pTargetPixelContent = pTargetLineContent;
                                    sal_uInt8* pSourcePixelMask = pSourceLineMask;
                                    sal_uInt8* pSourcePixelAlpha = pSourceLineAlpha;

                                    for(sal_uInt32 x(0); x < nWidth; x++)
                                    {
                                        const sal_uInt16 aMask(*pSourcePixelMask++);
                                        pSourcePixelMask += 2;

                                        if(0xff == aMask)
                                        {
                                            pSourcePixelAlpha += 3;
                                            pTargetPixelContent += 3;
                                            *pTargetPixelContent++ = 0;
                                        }
                                        else
                                        {
                                            sal_uInt16 aAlpha = 0x00ff - (((sal_uInt16)*pSourcePixelAlpha++ * 28 +
                                                (sal_uInt16)*pSourcePixelAlpha++ * 151 +
                                                (sal_uInt16)*pSourcePixelAlpha++ * 77) >> 8);

                                            aAlpha = ((0x00ff - aMask) * aAlpha) >> 8;

                                            pTargetPixelContent += 3;
                                            *pTargetPixelContent++ = sal_uInt8(aAlpha);
                                        }
                                    }

                                    pTargetLineContent += aBitmapDataContent.Stride;
                                    pSourceLineMask += aBitmapDataMask.Stride;
                                    pSourceLineAlpha += aBitmapDataAlpha.Stride;
                                }

                                aBitmapAlpha.UnlockBits(&aBitmapDataAlpha);
                                aBitmapMask.UnlockBits(&aBitmapDataMask);
                                aBitmapContent.UnlockBits(&aBitmapDataContent);

                                setMatrix(getGraphics(), basegfx::B2DHomMatrix());
                                getGraphics().DrawImage(
                                    &aBitmapContent,
                                    (INT)nLeft,
                                    (INT)nTop,
                                    (INT)0,
                                    (INT)0,
                                    (INT)nWidth,
                                    (INT)nHeight,
                                    Gdiplus::UnitPixel);
                                setMatrix(getGraphics(), getCurrentTransformation());
                            }
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_INVERTPRIMITIVE2D:
                {
                    const primitive2d::InvertPrimitive2D& rInvertPrimitive2D = static_cast< const primitive2d::InvertPrimitive2D& >(rCandidate);
                    basegfx::B2DRange aRange(rInvertPrimitive2D.getB2DRange(getViewInformation2D()));

                    if(isVisible(getGraphics(), aRange))
                    {
                        aRange.transform(getCurrentTransformation());
                    }

                    break;
                }
                case PRIMITIVE2D_ID_MASKPRIMITIVE2D:
                {
                    const primitive2d::MaskPrimitive2D& rMaskPrimitive2D = static_cast< const primitive2d::MaskPrimitive2D& >(rCandidate);

                    if(rMaskPrimitive2D.getChildren().hasElements())
                    {
                        const basegfx::B2DPolyPolygon& rMask = rMaskPrimitive2D.getMask();

                        if(rMask.count())
                        {
                            const basegfx::B2DRange aRange(rMaskPrimitive2D.getB2DRange(getViewInformation2D()));

                            if(isVisible(getGraphics(), aRange))
                            {
                                const boost::shared_ptr< Gdiplus::GraphicsPath > aMaskPath(getBufferedGdiPlusGraphicsPathFromB2DPolyPolygon(rMask));

                                if(aMaskPath.get())
                                {
                                    // save current clip region
                                    const Gdiplus::GraphicsState aState(getGraphics().Save());

                                    // add clip to current
                                    getGraphics().SetClip(
                                        aMaskPath.get(),
                                        Gdiplus::CombineModeIntersect);

                                    // do process children
                                    process(rMaskPrimitive2D.getChildren());

                                    // restore clip region
                                    getGraphics().Restore(aState);
                                }
                            }
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_MODIFIEDCOLORPRIMITIVE2D:
                {
                    const primitive2d::ModifiedColorPrimitive2D& rModifiedColorPrimitive2D = static_cast< const primitive2d::ModifiedColorPrimitive2D& >(rCandidate);

                    if(rModifiedColorPrimitive2D.getChildren().hasElements())
                    {
                        getBColorModifierStack().push(rModifiedColorPrimitive2D.getColorModifier());
                        process(rModifiedColorPrimitive2D.getChildren());
                        getBColorModifierStack().pop();
                    }
                    break;
                }
                case PRIMITIVE2D_ID_TRANSFORMPRIMITIVE2D:
                {
                    const primitive2d::TransformPrimitive2D& rTransformPrimitive2D = static_cast< const primitive2d::TransformPrimitive2D& >(rCandidate);

                    // remember current transformation and ViewInformation
                    const basegfx::B2DHomMatrix aLastCurrentTransformation(getCurrentTransformation());
                    const geometry::ViewInformation2D aLastViewInformation2D(getViewInformation2D());

                    // create new transformations for CurrentTransformation
                    // and for local ViewInformation2D
                    setCurrentTransformation(getCurrentTransformation() * rTransformPrimitive2D.getTransformation());
                    const geometry::ViewInformation2D aViewInformation2D(
                        getViewInformation2D().getObjectTransformation() * rTransformPrimitive2D.getTransformation(),
                        getViewInformation2D().getViewTransformation(),
                        getViewInformation2D().getViewport(),
                        getViewInformation2D().getVisualizedPage(),
                        getViewInformation2D().getViewTime(),
                        getViewInformation2D().getExtendedInformationSequence());
                    updateViewInformation(aViewInformation2D);
                    setMatrix(getGraphics(), getCurrentTransformation());

                    // proccess content
                    process(rTransformPrimitive2D.getChildren());

                    // restore transformations
                    setCurrentTransformation(aLastCurrentTransformation);
                    updateViewInformation(aLastViewInformation2D);
                    setMatrix(getGraphics(), getCurrentTransformation());
                    break;
                }

                // default case: process recursively using decomposition
                default :
                {
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    break;
                }
            }
        }
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
