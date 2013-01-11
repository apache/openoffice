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

#ifndef SFX_SIDEBAR_DRAW_HELPER_HXX
#define SFX_SIDEBAR_DRAW_HELPER_HXX

#include "vcl/window.hxx"

#include <tools/SvBorder.hxx>


class Color;

namespace sfx2 { namespace sidebar {

class Paint;

/** Some convenience functions for painting backgrounds and borders.
*/
class DrawHelper
{
public:
    static void DrawBorder (
        OutputDevice& rDevice,
        const Rectangle rBox,
        const SvBorder aBorderSize,
        const Paint& rHorizontalPaint,
        const Paint& rVerticalPaint);
    static void DrawHorizontalLine(
        OutputDevice& rDevice,
        const int nLeft,
        const int nRight,
        const int nY,
        const int nHeight,
        const Paint& rPaint);
    static void DrawVerticalLine(
        OutputDevice& rDevice,
        const int nTop,
        const int nBottom,
        const int nX,
        const int nWidth,
        const Paint& rPaint);
    static void DrawRoundedRectangle (
        OutputDevice& rDevice,
        const Rectangle& rBox,
        const int nCornerRadius,
        const Color& rBorderColor,
        const Paint& rFillPaint);
};


} } // end of namespace sfx2::sidebar

#endif
