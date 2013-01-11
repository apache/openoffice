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

#include "TitleBar.hxx"
#include "Theme.hxx"
#include "Paint.hxx"

#include <tools/svborder.hxx>
#include <vcl/gradient.hxx>

ToolbarValue::~ToolbarValue (void) {}


namespace sfx2 { namespace sidebar {

TitleBar::TitleBar (
    const ::rtl::OUString& rsTitle,
    Window* pParentWindow)
    : Window(pParentWindow),
      msTitle(rsTitle)
{
    SetBackground(Wallpaper());
}




TitleBar::~TitleBar (void)
{
}




void TitleBar::Paint (const Rectangle& rUpdateArea)
{
    // Paint title bar background.
    Size aWindowSize( GetOutputSizePixel() );
    /*
    int nOuterLeft = 0;
    const SvBorder aBorder( 3, 1, 3, 3 );
    const sal_Int32 m_nTitleBarHeight = GetSettings().GetStyleSettings().GetTitleHeight();
    int nInnerLeft = nOuterLeft + aBorder.Left() - 1;
    int nOuterRight = aWindowSize.Width() - 1;
    int nInnerRight = nOuterRight - aBorder.Right() + 1;
    int nInnerTop = m_nTitleBarHeight + aBorder.Top() - 1;
    int nOuterBottom = aWindowSize.Height() - 1;
    int nInnerBottom = nOuterBottom - aBorder.Bottom() + 1;
    Rectangle aTitleBarBox(
        nOuterLeft,
        0, 
        nOuterRight, 
        nInnerTop-1
        );
    */
    Rectangle aTitleBarBox(
        0,
        0, 
        aWindowSize.Width(), 
        aWindowSize.Height()
        );

    
    PaintBackground(aTitleBarBox);
    PaintDecoration(aTitleBarBox);
    PaintTitle(GetTitleArea(aTitleBarBox));
}



void TitleBar::PaintBackground (const Rectangle& rTitleBarBox)
{
    const sidebar::Paint aBackgroundPaint (GetBackgroundPaint());

    switch(aBackgroundPaint.GetType())
    {
        case Paint::NoPaint:
        default:
            break;

        case Paint::ColorPaint:
            // Set title bar colors.
            Push(PUSH_LINECOLOR | PUSH_FILLCOLOR);
            SetFillColor(aBackgroundPaint.GetColor());
            SetLineColor();
            DrawRect(rTitleBarBox);
            Pop();
            break;

        case Paint::GradientPaint:
            DrawGradient(rTitleBarBox, aBackgroundPaint.GetGradient());
            break;
    }
}




void TitleBar::PaintTitle (const Rectangle& rTitleBox)
{
    Push(PUSH_FONT | PUSH_TEXTCOLOR);

    // Use a bold font for the deck title.
    Font aFont(GetFont());
    aFont.SetWeight(WEIGHT_BOLD);
    SetFont(aFont);

    // Paint title bar text.
    SetTextColor(GetTextColor());
    DrawText(
        rTitleBox,
        msTitle,
        TEXT_DRAW_LEFT | TEXT_DRAW_VCENTER);
    
    Pop();
}


} } // end of namespace sfx2::sidebar
