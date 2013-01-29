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
    (void)rUpdateArea;

    // Paint title bar background.
    Size aWindowSize( GetOutputSizePixel() );
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
