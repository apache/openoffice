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

#include "SidebarToolBox.hxx"

#include <vcl/gradient.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


namespace sfx2 { namespace sidebar {


SidebarToolBox::SidebarToolBox (
    Window* pParentWindow,
    const ResId& rResId)
    : ToolBox(pParentWindow, rResId)
{
	SetBackground(Wallpaper());
	SetPaintTransparent(true);
}




SidebarToolBox::~SidebarToolBox (void)
{
}




void SidebarToolBox::Paint (const Rectangle& rRect)
{
    /*
    OutputDevice* pDevice (GetParent());

    if (pDevice != NULL)
    {
        Rectangle aRect (GetPosPixel(), GetSizePixel());
        aRect.Left() -= 2;
        aRect.Top() -= 1;
        aRect.Right() += 2;
        aRect.Bottom() += 1;
        const Color aSavedLineColor = pDevice->GetLineColor();
        const Color aSavedFillColor = pDevice->GetFillColor();
        Color aLineColor(200,209,225);
        //	Color aLineColor = GetSettings().GetStyleSettings().GetPropertySectionTBxBorderColor();
        if(!GetSettings().GetStyleSettings().GetHighContrastMode())	
            pDevice->SetLineColor(aLineColor);
        else
            pDevice->SetLineColor(GetSettings().GetStyleSettings().GetShadowColor());
        pDevice->SetFillColor(COL_TRANSPARENT);
        pDevice->DrawRect(aRect);

        aRect.Left() += 1 ;
        aRect.Top() += 1;
        aRect.Right() -= 1;
        aRect.Bottom() -= 1;
        Color aStartColor(220,228,238);
        Color aEndColor(245,245,247);
        Gradient aBKGrad(GRADIENT_LINEAR, aStartColor, aEndColor);
        if(!GetSettings().GetStyleSettings().GetHighContrastMode())	
            //	DrawGradient(aRect, GetSettings().GetStyleSettings().GetPropertySectionTBxBKGGradient());
            pDevice->DrawGradient(aRect, aBKGrad);
	//else
	//{
	//	SetFillColor(GetSettings().GetStyleSettings().GetMenuColor());
	//	DrawRect(aRect);
	//}

        pDevice->SetLineColor(aSavedLineColor);
        pDevice->SetFillColor(aSavedFillColor);
    }
    */

    ToolBox::Paint(rRect);
}

} } // end of namespace sfx2::sidebar
