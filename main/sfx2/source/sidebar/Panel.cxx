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

#include "Panel.hxx"
#include "TitleBar.hxx"
#include "ContentPanelDescriptor.hxx"

#include <tools/svborder.hxx>


using namespace css;
using namespace cssu;

namespace {
    static const char* VerticalStackLayouterName("vertical-stack");
}


namespace sfx2 {

Panel::Panel (
    const ContentPanelDescriptor& rPanelDescriptor,
    Window* pParentWindow)
    : Window(pParentWindow),
      msLayoutHint(rPanelDescriptor.msLayout),
      mpTitleBar(new TitleBar(rPanelDescriptor.msTitle, false, pParentWindow)),
      mbIsTitleBarOptional(rPanelDescriptor.mbIsTitleBarOptional),
      mxElement(),
      mxVerticalStackLayoutElement()
{
    SetBackground(Wallpaper(Color(0xff4500)));

}




Panel::~Panel (void)
{
}




const ::rtl::OUString& Panel::GetLayoutHint (void) const
{
    return msLayoutHint;
}




TitleBar* Panel::GetTitleBar (void) const
{
    return mpTitleBar;
}




bool Panel::IsTitleBarOptional (void) const
{
    return mbIsTitleBarOptional;
}




void Panel::SetUIElement (const Reference<ui::XUIElement>& rxElement)
{
    mxElement = rxElement;
    if (mxElement.is())
    {
        SetBackground(Wallpaper());

        if (msLayoutHint.equalsAscii(VerticalStackLayouterName))
            mxVerticalStackLayoutElement.set(mxElement->getRealInterface(), UNO_QUERY);
    }
}




void Panel::Paint (const Rectangle& rUpdateArea)
{
    Window::Paint(rUpdateArea);
}




Reference<ui::XVerticalStackLayoutElement> Panel::GetVerticalStackElement (void) const
{
    return mxVerticalStackLayoutElement;
}

    
} // end of namespace sfx2
