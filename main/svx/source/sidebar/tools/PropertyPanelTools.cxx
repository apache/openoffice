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

#include <svx/sidebar/PropertyPanelTools.hxx>
#include <sfx2/sidebar/Theme.hxx>
#include <vcl/gradient.hxx>

////////////////////////////////////////////////////////////////////////////////

namespace svx { namespace sidebar {

////////////////////////////////////////////////////////////////////////////////

PropertyPanelPopuplWindow::PropertyPanelPopuplWindow(Window* pParent)
:   FloatingWindow(pParent, WB_SYSTEMWINDOW | WB_3DLOOK) //  | WB_POPUP_FOCUSWIN)
{
}

PropertyPanelPopuplWindow::~PropertyPanelPopuplWindow()
{
}

long PropertyPanelPopuplWindow::Notify(NotifyEvent& rNEvt)
{
    if ( rNEvt.GetType() == EVENT_LOSEFOCUS )
    {
        if(!HasChildPathFocus(true))
        {
            EndPopupMode();
        }
    }

    return FloatingWindow::Notify(rNEvt);
}

//////////////////////////////////////////////////////////////////////////////

PropertyPanelPopupPage::PropertyPanelPopupPage(Window* pParent, const ResId& rResId)
:   Control( pParent,rResId)
{
	EnableChildTransparentMode(true);
	SetControlBackground();
}

PropertyPanelPopupPage::~PropertyPanelPopupPage()
{
}

void PropertyPanelPopupPage::Paint(const Rectangle& rect)
{
	Size aSize = GetOutputSizePixel();
	Color aOldLineColor = GetLineColor();
	Color aOldFillColor = GetFillColor();

    // todo: are this the correct colors ?!?
	Color aFillColor(sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBackground));
	Color aLineColor(sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderTopLeft));

    SetFillColor (aFillColor);
    SetLineColor (aLineColor);
	Rectangle aRect(Point(0,0),aSize);
	DrawRect(aRect);

    // todo: are this the correct colors ?!?
	aLineColor = sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderBottomRight);
	SetLineColor (aLineColor);
	DrawLine (Point(1,aSize.Height()-2),Point(aSize.Width()-2,aSize.Height()-2));	
	
	sal_uInt16 nGradientHeight = 9;		
		
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())//add  for High contract
	{
		aRect = Rectangle(Point(1,1),Size(aSize.Width()-2,nGradientHeight));
        // todo: are this the correct colors ?!?
        const Gradient aGradientA(
            GRADIENT_LINEAR, 
            sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderTopLeft),
            sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderBottomRight));
		DrawGradient(aRect, aGradientA);

		nGradientHeight = 12;
		aRect = Rectangle(Point(1,aSize.Height()-2-nGradientHeight),Size(aSize.Width()-2,nGradientHeight));
        // todo: are this the correct colors ?!?
        const Gradient aGradientB(
            GRADIENT_LINEAR, 
            sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderBottomRight),
            sfx2::sidebar::Theme::GetColor(sfx2::sidebar::Theme::Paint_ToolBoxBorderTopLeft));
		DrawGradient(aRect, aGradientB);
	}

	Window::Paint(rect);
}

//////////////////////////////////////////////////////////////////////////////

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////
// eof
