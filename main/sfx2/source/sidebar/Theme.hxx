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

#ifndef SFX_SIDEBAR_THEME_HXX
#define SFX_SIDEBAR_THEME_HXX

class SvBorder;

namespace sfx2 { namespace sidebar {

class Paint;

/** Simple collection of colors, gradients, fonts that define the
    look of the sidebar and its controls.
*/
class Theme
{
public:
    static Paint& GetDeckBackground (void);
    static Paint& GetDeckTitleBackground (void);
    static Color GetDeckTitleFontColor (void);
    static SvBorder& GetDeckPadding (void);
    static sal_Int32 GetDeckTitleBarHeight (void);
    
    static Paint& GetPanelBackground (void);
    static Paint& GetPanelTitleBackground (void);
    static Color GetPanelTitleFontColor (void);
    static sal_Int32 GetPanelTitleBarHeight (void);

    static Paint& GetTabBarBackground (void);
    static SvBorder& GetTabBarPadding (void);

    static sal_Int32 GetTabMenuPadding (void);
    static Color GetTabMenuSeparatorColor (void);
    static sal_Int32 GetTabMenuSeparatorPadding (void);

    static Size GetTabItemSize (void);
    static Color GetTabItemBorderColor (void);
    static Paint& GetTabItemBackgroundPaint (void);

    static Paint& GetHorizontalBorderPaint (void);
    static Paint& GetVerticalBorderPaint (void);
    static sal_Int32 GetBorderSize (void);

    static Image GetGripImage (void);
    static Image GetExpandImage (void);
    static Image GetCollapseImage (void);
    static Image GetMenuImage (void);

    static bool IsHighContrastMode (void);

    static void HandleDataChange (void);
    
    class Data;
private:
    static bool mbIsInitialized;
    static Data& GetCurrentTheme (void);
};


} } // end of namespace sfx2::sidebar

#endif
