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

#include "DeckTitleBar.hxx"
#include "sfx2/sidebar/Theme.hxx"

namespace sfx2 { namespace sidebar {

static const sal_Int32 gaLeftGripPadding (3);
static const sal_Int32 gaRightGripPadding (3);


DeckTitleBar::DeckTitleBar (
    const ::rtl::OUString& rsTitle,
    Window* pParentWindow)
    : TitleBar(rsTitle, pParentWindow)
{
}




DeckTitleBar::~DeckTitleBar (void)
{
}




Rectangle DeckTitleBar::GetTitleArea (const Rectangle& rTitleBarBox)
{
    Image aGripImage (Theme::GetImage(Theme::Image_Grip));
    return Rectangle(
        aGripImage.GetSizePixel().Width() + gaLeftGripPadding + gaRightGripPadding,
        rTitleBarBox.Top(),
        rTitleBarBox.Right(),
        rTitleBarBox.Bottom());
}




void DeckTitleBar::PaintDecoration (const Rectangle& rTitleBarBox)
{
    (void)rTitleBarBox;
    
    Image aGripImage (Theme::GetImage(Theme::Image_Grip));

    const Point aTopLeft (
        gaLeftGripPadding,
        (GetSizePixel().Height()-aGripImage.GetSizePixel().Height())/2);
    DrawImage(aTopLeft, aGripImage);
}




sidebar::Paint DeckTitleBar::GetBackgroundPaint (void)
{
    return Theme::GetPaint(Theme::Paint_DeckTitleBarBackground);
}




Color DeckTitleBar::GetTextColor (void)
{
    return Theme::GetColor(Theme::Color_DeckTitleFont);
}


} } // end of namespace sfx2::sidebar
