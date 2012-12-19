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

#include <tools/svborder.hxx>
#include <vcl/gradient.hxx>


namespace sfx2 {

TitleBar::TitleBar (
    const ::rtl::OUString& rsTitle,
    const bool bIsDeckTitle,
    Window* pParentWindow)
    : Window(pParentWindow),
      msTitle(rsTitle),
      mbIsDeckTitle(bIsDeckTitle)
{
    SetBackground(Wallpaper());
}




TitleBar::~TitleBar (void)
{
}




void TitleBar::Paint (const Rectangle& rUpdateArea)
{
    Push(PUSH_FONT | PUSH_FILLCOLOR | PUSH_LINECOLOR);

    // Set title bar colors.
    SetFillColor(GetSettings().GetStyleSettings().GetDialogColor());
    SetLineColor();
    
    // Paint title bar background.
    Size aWindowSize( GetOutputSizePixel() );
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
    const static Gradient aTitleBarGradient(
        GRADIENT_LINEAR,
        Color(0xe8e8f0),
        Color(0xf0f0ff));
    DrawGradient(aTitleBarBox, aTitleBarGradient);

    // Use a bold font for the deck title.
    Font aFont(GetFont());
    aFont.SetWeight(WEIGHT_BOLD);
    SetFont(aFont);

    // Paint title bar text.
    SetLineColor( GetSettings().GetStyleSettings().GetActiveTextColor() );
    aTitleBarBox.Left() += 3;
    DrawText(
        aTitleBarBox,
        msTitle,
        TEXT_DRAW_LEFT | TEXT_DRAW_VCENTER);

    Pop();
}


} // end of namespace sfx2
