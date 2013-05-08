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

#ifndef _SV_GDIPLUSOBJECTBUFFER_HXX
#define _SV_GDIPLUSOBJECTBUFFER_HXX

#include <vcl/timer.hxx>
#include <basegfx/tools/cmanager.hxx>
#include <boost/shared_ptr.hpp>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace Gdiplus 
{ 
    class Bitmap; 
    class GraphicsPath;
}

namespace basegfx
{
    class B2DPolygon;
    class B2DPolyPolygon;
}

class WinSalBitmap;

//////////////////////////////////////////////////////////////////////////////
// Helper class to manage Gdiplus::Bitmap instances created from WinSalBitmap,
// and Gdiplus::GraphicsPath instances created from basegfx::B2DPolygon or
// basegfx::B2DPolyPolygon

class GdiPlusObjectBuffer : public basegfx::cache::manager, public Timer
{
private:
protected:
public:
    GdiPlusObjectBuffer();
    virtual ~GdiPlusObjectBuffer();

    // access to buffered Gdiplus::Bitmap instances created from WinSalBitmap(s)
    boost::shared_ptr< Gdiplus::Bitmap > getGdiPlusBitmapFromWinSalBitmap(
        const WinSalBitmap& rBitmapSource, 
        const WinSalBitmap* pAlphaSource);

    // access to Gdiplus::GraphicsPath instances created from basegfx::B2DPolygon/B2DPolyPolygon
    boost::shared_ptr< Gdiplus::GraphicsPath > getGdiPlusGraphicsPathFromB2DPolygon(
        const basegfx::B2DPolygon& rPolygonSource);
    boost::shared_ptr< Gdiplus::GraphicsPath > getGdiPlusGraphicsPathFromB2DPolyPolygon(
        const basegfx::B2DPolyPolygon& rPolyPolygonSource);

    virtual void onEmpty();
    virtual void onFilled();
    virtual void Timeout();
};

#endif  // _SV_GDIPLUSOBJECTBUFFER_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
