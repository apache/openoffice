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
//#include "propertypanel.hxx"
//#include <vcl/svapp.hxx>
#include <vcl/gradient.hxx>

////////////////////////////////////////////////////////////////////////////////

namespace svx { namespace sidebar {

////////////////////////////////////////////////////////////////////////////////
//
//SfxSectionPage::SfxSectionPage(Window* pParent, const ResId& rResId ,SfxPropertyPanel* pPanel, PropertySectionType nPageId)
//:	Control(pParent, rResId ),
//	mpPanel(pPanel),
//	mnPageId(nPageId)
//{
//	mnMinimumWidth = GetOutputSizePixel().Width();
//	EnableChildTransparentMode( TRUE);	
//	//add  for high contract
//	if(!GetSettings().GetStyleSettings().GetHighContrastMode())
//		SetBackground(Wallpaper(GetSettings().GetStyleSettings().GetPropertyPanelBackgroundColor()));
//	else
//		SetBackground(Wallpaper(GetSettings().GetStyleSettings().GetMenuColor()));
//}
//
//SfxSectionPage::~SfxSectionPage()
//{
//}
//
//SfxPropertyPanel* SfxSectionPage::GetPanel()
//{
//	return mpPanel;
//}
//
//long SfxSectionPage::GetPanelWidth()
//{
//	return mpPanel->GetOutputSizePixel().Width();
//}
//
//void SfxSectionPage::PanelResize()
//{
//	mpPanel->RequestReSize();
//}
//
//sal_Int32 SfxSectionPage::GetPreferredWidth (sal_Int32 nHeight)
//{
//	//now we don't support auto layout, so return GetOutputSizePixel().Width();
//	return GetOutputSizePixel().Width();
//}
//
//sal_Int32 SfxSectionPage::GetPreferredHeight (sal_Int32 nWidth)
//{
//	//now we don't support auto layout, so return GetOutputSizePixel().Height();
//	return GetOutputSizePixel().Height();
//}
//
//sal_Int32 SfxSectionPage::GetMinimumWidth (void)
//{
//	return mnMinimumWidth;
//}
//
//void SfxSectionPage::StateChangedImpl(sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState) 
//{
//    // The SubClass can override this function to listent context switching
//}
//
//SfxBindings* SfxSectionPage::GetBindings()
//{
//	if(NULL == mpPanel) return NULL;
//	return mpPanel->GetBindings();
//}
//
////////////////////////////////////////////////////////////////////////////////
//
//SfxPropertyPageController::SfxPropertyPageController(sal_uInt16 nId, SfxSectionPage *p, SfxBindings &rBindings)
//:   SfxControllerItem(nId, rBindings),
//    mpPage(p)
//{
//}
//
//SfxPropertyPageController::~SfxPropertyPageController()
//{
//}
//
//void SfxPropertyPageController::StateChanged( sal_uInt16 nSID, SfxItemState eState,	const SfxPoolItem* pState )
//{
//    if(mpPage) 
//    {
//        mpPage->NotifyItemUpdate(nSID, eState, pState);
//    }
//}
//
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
