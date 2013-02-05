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
            mrOutDev(rOutDev),
            mpGraphics(0),
            maGraphics(),
            maBColorModifierStack(),
            maCurrentTransformation(),
            maDrawinglayerOpt()
        {
            // set digit language, derived from SvtCTLOptions to have the correct
            // number display for arabic/hindi numerals
            const SvtCTLOptions aSvtCTLOptions;
            LanguageType eLang(LANGUAGE_SYSTEM);

            if(SvtCTLOptions::NUMERALS_HINDI == aSvtCTLOptions.GetCTLTextNumerals())
            {
                eLang = LANGUAGE_ARABIC_SAUDI_ARABIA;
            }
            else if(SvtCTLOptions::NUMERALS_ARABIC == aSvtCTLOptions.GetCTLTextNumerals())
            {
                eLang = LANGUAGE_ENGLISH;
            }
            else
            {
                eLang = (LanguageType)Application::GetSettings().GetLanguage();
            }

            mrOutDev.SetDigitLanguage(eLang);

            // prepare getCurrentTransformation() matrix with viewTransformation to target directly to pixels
            setCurrentTransformation(rViewInformation.getObjectToViewTransformation());

            // prepare output directly to pixels
            mrOutDev.Push(PUSH_MAPMODE);
            mrOutDev.SetMapMode();

            // react on AntiAliasing settings
            if(getOptionsDrawinglayer().IsAntiAliasing())
            {
                mrOutDev.SetAntialiasing(mrOutDev.GetAntialiasing() | ANTIALIASING_ENABLE_B2DDRAW);
            }
            else
            {
                mrOutDev.SetAntialiasing(mrOutDev.GetAntialiasing() & ~ANTIALIASING_ENABLE_B2DDRAW);
            }

            // create Gdiplus::Graphics entry for new OutputDevice
            SystemGraphicsData aSystemGraphicsData(mrOutDev.GetSystemGfxData());
            Gdiplus::Graphics* pGraphics = new Gdiplus::Graphics(aSystemGraphicsData.hDC);

            pushGraphics(*pGraphics);
            setAntialiasing(getGraphics(), getOptionsDrawinglayer().IsAntiAliasing());
            setMatrix(getGraphics(), getCurrentTransformation());
        }

        Win_PixelProcessor2D::~Win_PixelProcessor2D()
        {
            popGraphics();
            OSL_ENSURE(maGraphics.empty(), "Mismatch in push/popGraphics (!)");

            // restore MapMode
            mrOutDev.Pop();

            // restore AntiAliasing
            mrOutDev.SetAntialiasing(mrOutDev.GetAntialiasing() & ~ANTIALIASING_ENABLE_B2DDRAW);
        }

        void Win_PixelProcessor2D::pushGraphics(Gdiplus::Graphics& rNew)
        {
            maGraphics.push_back(&rNew);
            mpGraphics = &rNew;
        }

        void Win_PixelProcessor2D::popGraphics()
        {
            if(maGraphics.empty())
            {
                OSL_ENSURE(false, "OOps, push/popGraphics mismatch (!)");
            }
            else
            {
                delete mpGraphics;
                mpGraphics = maGraphics.back();
                maGraphics.pop_back();
            }
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

                    break;
                }
                case PRIMITIVE2D_ID_POINTARRAYPRIMITIVE2D:
                {
                    const primitive2d::PointArrayPrimitive2D& rPointArrayPrimitive2D = static_cast< const primitive2d::PointArrayPrimitive2D& >(rCandidate);
                    break;
                }
                case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D:
                {
                    const primitive2d::PolygonHairlinePrimitive2D& rPolygonHairlinePrimitive2D = static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate);
                    const basegfx::B2DPolygon& rPolygon = rPolygonHairlinePrimitive2D.getB2DPolygon();

                    if(rPolygon.count())
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

                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONCOLORPRIMITIVE2D:
                {
                    const primitive2d::PolyPolygonColorPrimitive2D& rPolyPolygonColorPrimitive2D = static_cast< const primitive2d::PolyPolygonColorPrimitive2D& >(rCandidate);
                    const basegfx::B2DPolyPolygon& rPolyPolygon = rPolyPolygonColorPrimitive2D.getB2DPolyPolygon();

                    if(rPolyPolygon.count())
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
                    break;
                }

                // basic grouping primitives
                case PRIMITIVE2D_ID_TRANSPARENCEPRIMITIVE2D:
                {
                    const primitive2d::TransparencePrimitive2D& rTransparencePrimitive2D = static_cast< const primitive2d::TransparencePrimitive2D& >(rCandidate);
                    break;
                }
                case PRIMITIVE2D_ID_INVERTPRIMITIVE2D:
                {
                    const primitive2d::InvertPrimitive2D& rInvertPrimitive2D = static_cast< const primitive2d::InvertPrimitive2D& >(rCandidate);
                    break;
                }
                case PRIMITIVE2D_ID_MASKPRIMITIVE2D:
                {
                    const primitive2d::MaskPrimitive2D& rMaskPrimitive2D = static_cast< const primitive2d::MaskPrimitive2D& >(rCandidate);
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
