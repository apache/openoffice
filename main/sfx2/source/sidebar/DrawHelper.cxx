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

#include "precompiled_sfx2.hxx"

#include "DrawHelper.hxx"
#include "Paint.hxx"

#include <vcl/lineinfo.hxx>


namespace sfx2 { namespace sidebar {

void DrawHelper::DrawBorder (
    OutputDevice& rDevice,
    const Rectangle rBox,
    const SvBorder aBorderSize,
    const Paint& rHorizontalPaint,
    const Paint& rVerticalPaint)
{
    // Draw top line.
    DrawHorizontalLine(
        rDevice,
        rBox.Left(),
        rBox.Right(),
        rBox.Top(),
        aBorderSize.Top(),
        rHorizontalPaint);
    // Draw bottom line.
    DrawHorizontalLine(
        rDevice,
        rBox.Left(),
        rBox.Right(),
        rBox.Bottom()-aBorderSize.Bottom()+1,
        aBorderSize.Bottom(),
        rHorizontalPaint);
    // Draw left line.
    DrawVerticalLine(
        rDevice,
        rBox.Top()+aBorderSize.Top(),
        rBox.Bottom()-aBorderSize.Bottom()+1,
        rBox.Left(),
        aBorderSize.Left(),
        rVerticalPaint);
    // Draw right line.
    DrawVerticalLine(
        rDevice,
        rBox.Top(),
        rBox.Bottom(),
        rBox.Right()-aBorderSize.Right()+1,
        aBorderSize.Right(),
        rVerticalPaint);
}




void DrawHelper::DrawHorizontalLine(
    OutputDevice& rDevice,
    const int nLeft,
    const int nRight,
    const int nY,
    const int nHeight,
    const Paint& rPaint)
{
    switch (rPaint.GetType())
    {
        case Paint::NoPaint:
        default:
            break;

        case Paint::ColorPaint:
            rDevice.SetLineColor(rPaint.GetColor());
            if (nHeight == 1)
                rDevice.DrawLine(
                    Point(nLeft,nY),
                    Point(nRight,nY));
            else
                rDevice.DrawLine(
                    Point(nLeft,nY),
                    Point(nRight,nY),
                    LineInfo(LINE_SOLID,nHeight));
            break;

        case Paint::GradientPaint:
            rDevice.DrawGradient(
                Rectangle(
                    nLeft,
                    nY,
                    nRight,
                    nY+nHeight-1),
                rPaint.GetGradient());
            break;
    }
}




void DrawHelper::DrawVerticalLine(
    OutputDevice& rDevice,
    const int nTop,
    const int nBottom,
    const int nX,
    const int nWidth,
    const Paint& rPaint)
{
    switch (rPaint.GetType())
    {
        case Paint::NoPaint:
        default:
            break;

        case Paint::ColorPaint:
            rDevice.SetLineColor(rPaint.GetColor());
            if (nWidth == 1)
                rDevice.DrawLine(
                    Point(nX, nTop),
                    Point(nX, nBottom));
            else
                rDevice.DrawLine(
                    Point(nX, nTop),
                    Point(nX, nBottom),
                    LineInfo(LINE_SOLID, nWidth));
            break;

        case Paint::GradientPaint:
            rDevice.DrawGradient(
                Rectangle(
                    nX,
                    nTop,
                    nX+nWidth-1,
                    nBottom),
                rPaint.GetGradient());
            break;
    }
}




void DrawHelper::DrawRoundedRectangle (
    OutputDevice& rDevice,
    const Rectangle& rBox,
    const int nCornerRadius,
    const Color& rBorderColor,
    const Paint& rFillPaint)
{
    rDevice.SetLineColor(rBorderColor);
    switch(rFillPaint.GetType())
    {
        case Paint::NoPaint:
        default:
            rDevice.SetFillColor();
            rDevice.DrawRect(rBox, nCornerRadius, nCornerRadius);
            break;

        case Paint::ColorPaint:
            rDevice.SetFillColor(rFillPaint.GetColor());
            rDevice.DrawRect(rBox, nCornerRadius, nCornerRadius);
            break;

        case Paint::GradientPaint:
            rDevice.DrawGradient(
                rBox,
                rFillPaint.GetGradient());
            rDevice.SetFillColor();
            rDevice.DrawRect(rBox, nCornerRadius, nCornerRadius);
            break;
    }    
}



} } // end of namespace sfx2::sidebar
