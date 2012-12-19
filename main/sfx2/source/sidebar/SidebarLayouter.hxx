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

#ifndef SFX_SIDEBAR_SIDEBAR_LAYOUTER_HXX
#define SFX_SIDEBAR_SIDEBAR_LAYOUTER_HXX

#include <vector>


namespace sfx2 {

class Deck;
class TabBar;
class Panel;
class TitleBar;


/** Layout the sidebar.

    Deck and tabbar are placed horizontally side by side with the
    tabbar having a fixed width.

    Panels are placed stacked in vertical direction.
    Panel width is the same as the deck window.
    Therefore the layouter only has to determine height and vertical
    locatioin of the panels.
*/
class SidebarLayouter
{
public:
    static void LayoutSidebar (
        Deck* pDeck,
        TabBar* pTabBar);
    static void LayoutPanels (
        const Deck* pDeck,
        const ::std::vector<Panel*>& rPanels);

private:
    static void LayoutSinglePanel (
        const Deck* pDeck,
        Panel* pPanel);
    static void LayoutMultiplePanels (
        const Deck* pDeck,
        const ::std::vector<Panel*>& rPanels);
    static Rectangle PlaceDeckTitle (
        TitleBar* pDeckTitleBar,
        const Rectangle& rAvailableSpace);
};


} // end of namespace sfx2

#endif
