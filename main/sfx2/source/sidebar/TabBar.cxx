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

#include "TabBar.hxx"
#include <vcl/gradient.hxx>


namespace sfx2 {

TabBar::TabBar (Window* pParentWindow)
    : Window(pParentWindow)
{
    SetBackground(Wallpaper());
}




TabBar::~TabBar (void)
{
}




void TabBar::Paint (const Rectangle& rUpdateArea)
{
    const static Gradient aTitleBarGradient(
        GRADIENT_LINEAR,
        Color(0xe8e8f0),
        Color(0xf0f0ff));
    const Size aWindowSize (GetOutputSizePixel());
    DrawGradient(
        Rectangle(
            0,0,
            aWindowSize.Width(),
            aWindowSize.Height()),
        aTitleBarGradient);
}




sal_Int32 TabBar::GetDefaultWidth (void)
{
    return 32;
}


} // end of namespace sfx2
