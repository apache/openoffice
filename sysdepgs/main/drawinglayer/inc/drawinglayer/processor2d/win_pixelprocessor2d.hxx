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

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_WIN_PIXELPROCESSOR2D_HXX
#define INCLUDED_DRAWINGLAYER_PROCESSOR2D_WIN_PIXELPROCESSOR2D_HXX

#include <drawinglayer/drawinglayerdllapi.h>
#include <drawinglayer/processor2d/baseprocessor2d.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/color/bcolormodifier.hxx>
#include <svtools/optionsdrawinglayer.hxx>
#include <tools/svwin.h>

#ifndef min
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#include <GdiPlus.h>

#undef min
#undef max

//////////////////////////////////////////////////////////////////////////////
// predefines
class OutputDevice;
class VirtualDevice;
class Gdiplus::Graphics;
class Gdiplus::Bitmap;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        /** Win_PixelProcessor2D class

            This processor derived from BaseProcessor2D is the base class for rendering
            system-dependent on Win, using GDI+ for now
         */
        class Win_PixelProcessor2D : public BaseProcessor2D
        {
        private:
            // current pixel output size
            sal_uInt32                              mnWidth;
            sal_uInt32                              mnHeight;

            // buffer devices used for TransparencePrimitive2D
            Gdiplus::Bitmap*                        mpBitmapContent;
            Gdiplus::Graphics*                      mpGraphicsContent;
            Gdiplus::Bitmap*                        mpBitmapMask;
            Gdiplus::Graphics*                      mpGraphicsMask;
            Gdiplus::Bitmap*                        mpBitmapAlpha;
            Gdiplus::Graphics*                      mpGraphicsAlpha;

            // the current Gdiplus::Graphics and the stack
            Gdiplus::Graphics*                      mpGraphics;

            // the modifiedColorPrimitive stack
            basegfx::BColorModifierStack            maBColorModifierStack;

            // the current transformation. Since VCL pixel renderer transforms to pixels
            // and VCL MetaFile renderer to World (logic) coordinates, the local
            // ViewInformation2D cannot directly be used, but needs to be kept up to date
            basegfx::B2DHomMatrix                   maCurrentTransformation;

            // SvtOptionsDrawinglayer incarnation to react on diverse settings
            const SvtOptionsDrawinglayer            maDrawinglayerOpt;

        protected:
            //////////////////////////////////////////////////////////////////////////////
            // tooling
            Gdiplus::Graphics& getGraphics() const { OSL_ENSURE(0 != mpGraphics, "0 == mpGraphics (!)"); return *mpGraphics; }

            // access to transformation stack
            const basegfx::B2DHomMatrix& getCurrentTransformation() const { return maCurrentTransformation; }
            void setCurrentTransformation(const basegfx::B2DHomMatrix& rNew) { maCurrentTransformation = rNew; }

            // access to color modifyer stack
            basegfx::BColorModifierStack& getBColorModifierStack() { return maBColorModifierStack; }

            // access to Drawinglayer configuration options
            const SvtOptionsDrawinglayer& getOptionsDrawinglayer() const { return maDrawinglayerOpt; }

            //////////////////////////////////////////////////////////////////////////////
            // the local processor for BasePrinitive2D-Implementation based primitives,
            // called from the common process()-implementation
            virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate);

        public:
            /// constructor/destructor
            Win_PixelProcessor2D(
                const geometry::ViewInformation2D& rViewInformation, 
                OutputDevice& rOutDev);
            virtual ~Win_PixelProcessor2D();
        };
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif // INCLUDED_DRAWINGLAYER_PROCESSOR2D_WIN_PIXELPROCESSOR2D_HXX

// eof
