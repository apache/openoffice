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
 *   http:\\www.apache.org\licenses\LICENSE-2.0
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

#include <drawinglayer/tools/converters.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <drawinglayer/processor2d/vclpixelprocessor2d.hxx>
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <vcl/virdev.hxx>

#ifdef DBG_UTIL
#include <tools/stream.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace tools
	{
        BitmapEx DRAWINGLAYER_DLLPUBLIC convertToBitmapEx(
            const drawinglayer::primitive2d::Primitive2DSequence& rSeq, 
            const geometry::ViewInformation2D& rViewInformation2D,
            sal_uInt32 nDiscreteWidth,
            sal_uInt32 nDiscreteHeight,
            sal_uInt32 nMaxQuadratPixels)
        {
            BitmapEx aRetval;

            if(rSeq.hasElements() && nDiscreteWidth && nDiscreteHeight)
            {
                // get destination size in pixels
                const MapMode aMapModePixel(MAP_PIXEL);
				const sal_uInt32 nViewVisibleArea(nDiscreteWidth * nDiscreteHeight);
				double fReduceFactor(1.0);
                drawinglayer::primitive2d::Primitive2DSequence aSequence(rSeq);

				if(nViewVisibleArea > nMaxQuadratPixels)
				{
                    // reduce render size
					fReduceFactor = sqrt((double)nMaxQuadratPixels / (double)nViewVisibleArea);
					nDiscreteWidth = basegfx::fround((double)nDiscreteWidth * fReduceFactor);
					nDiscreteHeight = basegfx::fround((double)nDiscreteHeight * fReduceFactor);

                    const drawinglayer::primitive2d::Primitive2DReference aEmbed(
                        new drawinglayer::primitive2d::TransformPrimitive2D(
                            basegfx::tools::createScaleB2DHomMatrix(fReduceFactor, fReduceFactor),
                            rSeq));

                    aSequence = drawinglayer::primitive2d::Primitive2DSequence(&aEmbed, 1);
				}

        		const Point aEmptyPoint;
                const Size aSizePixel(nDiscreteWidth, nDiscreteHeight);
                geometry::ViewInformation2D aViewInformation2D(rViewInformation2D);
		        Bitmap aContent; 
                AlphaMask aAlphaMask;
                
                {
                    // prepare vdev
                    VirtualDevice maContent;
                    
                    maContent.SetOutputSizePixel(aSizePixel, false);
                    maContent.SetMapMode(aMapModePixel);
                    maContent.SetAntialiasing(true);

                    // render content
                    processor2d::VclPixelProcessor2D aContentProcessor(aViewInformation2D, maContent);
                    aContentProcessor.process(aSequence);

                    // get content pixels
                    maContent.EnableMapMode(false);
		            aContent = maContent.GetBitmap(aEmptyPoint, aSizePixel); 
                }

                {
                    // prepare vdev
                    VirtualDevice maAlpha;
		        
                    maAlpha.SetOutputSizePixel(aSizePixel, false);
                    maAlpha.SetMapMode(aMapModePixel);
                    maAlpha.SetAntialiasing(true);

                    // set alöpha to all white (fully transparent)
                    maAlpha.SetBackground(Wallpaper(Color(COL_WHITE)));
					maAlpha.Erase();

                    // embed primitives to paint them black
                    const primitive2d::Primitive2DReference xRef(
                        new primitive2d::ModifiedColorPrimitive2D(
                            aSequence,
                            basegfx::BColorModifier(
                                basegfx::BColor(0.0, 0.0, 0.0),
                                0.5,
                                basegfx::BCOLORMODIFYMODE_REPLACE)));
                    const primitive2d::Primitive2DSequence xSeq(&xRef, 1);

                    // render
                    processor2d::VclPixelProcessor2D aAlphaProcessor(aViewInformation2D, maAlpha);
                    aAlphaProcessor.process(xSeq);

                    // get alpha cahannel from vdev
                    maAlpha.EnableMapMode(false);
			        aAlphaMask = maAlpha.GetBitmap(aEmptyPoint, aSizePixel);
                }

                aRetval = BitmapEx(aContent, aAlphaMask);
            }

#ifdef DBG_UTIL
			static bool bDoSaveForVisualControl(false);
			if(bDoSaveForVisualControl)
			{
				SvFileStream aNew((const String&)String(ByteString( "c:\\test.png" ), RTL_TEXTENCODING_UTF8), STREAM_WRITE|STREAM_TRUNC);
				aNew << aRetval;
			}
#endif

            return aRetval;
        }

	} // end of namespace tools
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
