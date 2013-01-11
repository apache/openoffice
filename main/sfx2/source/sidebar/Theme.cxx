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

#include "Theme.hxx"
#include "Paint.hxx"
#include "SidebarResource.hxx"

#include <tools/SvBorder.hxx>
#include <tools/rc.hxx>
#include <vcl/svapp.hxx>


namespace sfx2 { namespace sidebar {

void InitializeTheme1 (Theme::Data& rData);

class Theme::Data
{
public:
    Data (void) {}
    void Initialize (void) { if ( ! mbIsInitialized) {InitializeTheme1(*this);mbIsInitialized=true;} }

    Paint maDeckBackground;
    Paint maDeckTitleBackground;
    SvBorder maDeckPadding;
    sal_Int32 mnDeckBorderSize;
    Color maDeckTitleFontColor;
    sal_Int32 mnDeckTitleBarHeight;
    
    Paint maPanelBackground;
    Paint maPanelTitleBackground;
    Color maPanelTitleFontColor;
    sal_Int32 mnPanelTitleBarHeight;
    
    Paint maTabBarBackground;
    SvBorder maTabBarPadding;

    sal_Int32 mnTabMenuPadding;
    Color maTabMenuSeparatorColor;
    sal_Int32 mnTabMenuSeparatorPadding;

    Size maTabItemSize;
    Color maTabItemBorderColor;
    Paint maTabItemBackgroundPaint;

    Paint maHorizontalBorderPaint;
    Paint maVerticalBorderPaint;

    Image maGripImage;
    Image maExpandImage;
    Image maCollapseImage;
    Image maMenuImage;

    bool mbIsHighContrastMode;
};


bool Theme::mbIsInitialized (false);



Theme::Data& Theme::GetCurrentTheme (void)
{
    static Theme::Data maData;
    maData.Initialize();
    return maData;
}

Paint& Theme::GetDeckBackground (void)
{
    return GetCurrentTheme().maDeckBackground;
}




Paint& Theme::GetDeckTitleBackground (void)
{
    return GetCurrentTheme().maDeckTitleBackground;
}




SvBorder& Theme::GetDeckPadding (void)
{
    return GetCurrentTheme().maDeckPadding;
}




Color Theme::GetDeckTitleFontColor (void)
{
    return GetCurrentTheme().maDeckTitleFontColor;
}




sal_Int32 Theme::GetDeckTitleBarHeight (void)
{
    return GetCurrentTheme().mnDeckTitleBarHeight;
}




Paint& Theme::GetPanelBackground (void)
{
    return GetCurrentTheme().maPanelBackground;
}




Paint& Theme::GetPanelTitleBackground (void)
{
    return GetCurrentTheme().maPanelTitleBackground;
}




Color Theme::GetPanelTitleFontColor (void)
{
    return GetCurrentTheme().maPanelTitleFontColor;
}




sal_Int32 Theme::GetPanelTitleBarHeight (void)
{
    return GetCurrentTheme().mnPanelTitleBarHeight;
}




Paint& Theme::GetTabBarBackground (void)
{
    return GetCurrentTheme().maTabBarBackground;
}




SvBorder& Theme::GetTabBarPadding (void)
{
    return GetCurrentTheme().maTabBarPadding;
}




sal_Int32 Theme::GetTabMenuPadding (void)
{
    return GetCurrentTheme().mnTabMenuPadding;
}




Color Theme::GetTabMenuSeparatorColor (void)
{
    return GetCurrentTheme().maTabMenuSeparatorColor;
}




sal_Int32 Theme::GetTabMenuSeparatorPadding (void)
{
    return GetCurrentTheme().mnTabMenuSeparatorPadding;
}




Size Theme::GetTabItemSize (void)
{
    return GetCurrentTheme().maTabItemSize;
}




Color Theme::GetTabItemBorderColor (void)
{
    return GetCurrentTheme().maTabItemBorderColor;
}




Paint& Theme::GetTabItemBackgroundPaint (void)
{
    return GetCurrentTheme().maTabItemBackgroundPaint;
}




Paint& Theme::GetHorizontalBorderPaint (void)
{
    return GetCurrentTheme().maHorizontalBorderPaint;
}




Paint& Theme::GetVerticalBorderPaint (void)
{
    return GetCurrentTheme().maVerticalBorderPaint;
}




sal_Int32 Theme::GetBorderSize (void)
{
    return GetCurrentTheme().mnDeckBorderSize;
}




Image Theme::GetGripImage (void)
{
    return GetCurrentTheme().maGripImage;
}




Image Theme::GetExpandImage (void)
{
    return GetCurrentTheme().maExpandImage;
}




Image Theme::GetCollapseImage (void)
{
    return GetCurrentTheme().maCollapseImage;
}




Image Theme::GetMenuImage (void)
{
    return GetCurrentTheme().maMenuImage;
}




bool Theme::IsHighContrastMode (void)
{
    return GetCurrentTheme().mbIsHighContrastMode;
}




void Theme::HandleDataChange (void)
{
    mbIsInitialized = false;
}




void InitializeTheme1 (Theme::Data& rData)
{
    SidebarResource aLocalResource;
    
    rData.mbIsHighContrastMode = Application::GetSettings().GetStyleSettings().GetHighContrastMode();

    rData.maDeckBackground.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 : 0xf0f0f0)));
    rData.maDeckTitleBackground.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 :0xf0f0f0)));
    rData.maDeckPadding = SvBorder(2,2,2,2);
    rData.mnDeckBorderSize = 1;
    rData.maDeckTitleFontColor.SetColor(
        rData.mbIsHighContrastMode ? 0x00ff00 : 0x262626);
    rData.mnDeckTitleBarHeight = 26;
    
    rData.maPanelBackground.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 : 0xffffff)));
    rData.maPanelTitleBackground.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 : 0xb2b2b2)));
    rData.maPanelTitleFontColor.SetColor(
        rData.mbIsHighContrastMode ? 0x00ff00 : 0x262626);
    rData.mnPanelTitleBarHeight = 26;

    rData.maTabBarBackground.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 : 0xf0f0f0)));
    rData.maTabBarPadding = SvBorder(2,2,2,2);

    rData.mnTabMenuPadding = 6;
    rData.maTabMenuSeparatorColor.SetColor(
        rData.mbIsHighContrastMode ? 0x00ff00 : 0xbfbfbf);
    rData.mnTabMenuSeparatorPadding = 7;
    
    rData.maTabItemSize = Size(32,32);
    rData.maTabItemBorderColor.SetColor(
        rData.mbIsHighContrastMode ? 0x00ff00 : 0xbfbfbf);
    rData.maTabItemBackgroundPaint.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x000000 : 0xffffff)));

    rData.maHorizontalBorderPaint.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x00ff000 : 0xd9d9d9)));
    rData.maVerticalBorderPaint.Set(Paint(Color(
                rData.mbIsHighContrastMode ? 0x00ff000 : 0xd9d9d9)));
    
    rData.maGripImage = Image(SfxResId(
            rData.mbIsHighContrastMode
                ? IMAGE_SIDEBAR_GRIP_HC
                : IMAGE_SIDEBAR_GRIP));
    rData.maExpandImage = Image(SfxResId(
            rData.mbIsHighContrastMode
                ? IMAGE_SIDEBAR_PLUS_HC
                : IMAGE_SIDEBAR_PLUS));
    rData.maCollapseImage = Image(SfxResId(
            rData.mbIsHighContrastMode
                ? IMAGE_SIDEBAR_MINUS_HC
                : IMAGE_SIDEBAR_MINUS));
    rData.maMenuImage = Image(SfxResId(
            rData.mbIsHighContrastMode
                ? IMAGE_SIDEBAR_MENU_HC
                : IMAGE_SIDEBAR_MENU));
}

} } // end of namespace sfx2::sidebar
