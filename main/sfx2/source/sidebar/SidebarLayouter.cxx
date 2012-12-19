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

#include "SidebarLayouter.hxx"

#include "Deck.hxx"
#include "Panel.hxx"
#include "TabBar.hxx"
#include "TitleBar.hxx"



namespace sfx2 {

namespace {
    static const sal_Int32 MinimalPanelHeight (25);
}



void SidebarLayouter::LayoutSidebar (
    Deck* pDeck,
    TabBar* pTabBar)
{
    if (pDeck==NULL || pTabBar==NULL)
    {
        OSL_ASSERT(pDeck!=NULL && pTabBar!=NULL);
        return;
    }
    Window* pParentWindow = pDeck->GetParent();
    if (pParentWindow==NULL || pParentWindow!=pTabBar->GetParent())
    {
        OSL_ASSERT(pDeck->GetParent() != NULL);
        OSL_ASSERT(pDeck->GetParent() == pTabBar->GetParent());
    }
    
    const sal_Int32 nWidth (pParentWindow->GetSizePixel().Width());
    const sal_Int32 nHeight (pParentWindow->GetSizePixel().Height());
    pDeck->SetPosSizePixel(0,0, nWidth-TabBar::GetDefaultWidth(), nHeight);
    pDeck->Show();
    pTabBar->SetPosSizePixel(nWidth-TabBar::GetDefaultWidth(),0,TabBar::GetDefaultWidth(),nHeight);
    pTabBar->Show();
}




void SidebarLayouter::LayoutPanels (
    const Deck* pDeck,
    const ::std::vector<Panel*>& rPanels)
{
    switch(rPanels.size())
    {
        case 0:
            // This basically is an error but there is not much that
            // we can do about it.
            OSL_ASSERT(rPanels.size()>0);
            break;

        case 1:
            if (rPanels[0]->IsTitleBarOptional())
            {
                // There is only one panel in the deck and its title
                // bar is optional => do not draw the title bar.
                LayoutSinglePanel(pDeck, rPanels[0]);
            }
            else
                LayoutMultiplePanels(pDeck, rPanels);
            break;

        default:
            LayoutMultiplePanels(pDeck, rPanels);
            break;
    }
}




void SidebarLayouter::LayoutSinglePanel (
    const Deck* pDeck,
    Panel* pPanel)
{
    Rectangle aBox (Point(0,0), pDeck->GetSizePixel());
    aBox = PlaceDeckTitle(pDeck->GetTitleBar(), aBox);
    pPanel->GetTitleBar()->Hide();
    pPanel->SetPosSizePixel(aBox.Left(), aBox.Top(), aBox.GetWidth(), aBox.GetHeight());
}




void SidebarLayouter::LayoutMultiplePanels (
    const Deck* pDeck,
    const ::std::vector<Panel*>& rPanels)
{
    Rectangle aBox (Point(0,0), pDeck->GetSizePixel());
    aBox = PlaceDeckTitle(pDeck->GetTitleBar(), aBox);
    const sal_Int32 nWidth (aBox.GetWidth());
    const sal_Int32 nTitleBarHeight (pDeck->GetSettings().GetStyleSettings().GetTitleHeight());

    // Determine the height that is available for panel content
    // and count the different layouts.
    sal_Int32 nHeight = aBox.GetHeight();
    sal_Int32 nFullCount (0);
    sal_Int32 nVerticalStackCount (0);
    for (::std::vector<Panel*>::const_iterator
             iPanel(rPanels.begin()),
             iEnd(rPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        nHeight -= nTitleBarHeight;
        
        if ((*iPanel)->GetVerticalStackElement().is())
        {
            ++nVerticalStackCount;
            nHeight -= (*iPanel)->GetVerticalStackElement()->getHeightForWidth(nWidth);
        }
        else
        {
            ++nFullCount;
        }
    }

    sal_Int32 nHeightPerPanel (nHeight / nFullCount);
    // The division might lead to rounding errors.  Enlarge the first
    // panel to compensate for that.
    sal_Int32 nHeightOfFirstPanel = nHeightPerPanel + (nHeight - nHeightPerPanel*(nFullCount+nVerticalStackCount));
    if (nHeightPerPanel < MinimalPanelHeight)
    {
        // Display a vertical scroll bar.
        // Force height to the minimal panel height.
        nHeightPerPanel = MinimalPanelHeight;
        nHeightOfFirstPanel = nHeightPerPanel;
    }

    // Assign heights and places.
    sal_Int32 nY (aBox.Top());
    for (::std::vector<Panel*>::const_iterator
             iPanel(rPanels.begin()),
             iEnd(rPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        // Place the title bar.
        TitleBar* pTitleBar = (*iPanel)->GetTitleBar();
        pTitleBar->SetPosSizePixel(0, nY, nWidth, nTitleBarHeight);
        nY += nTitleBarHeight;
        
        // Place the panel.
        sal_Int32 nPanelHeight (0);
        if ((*iPanel)->GetVerticalStackElement().is())
            nPanelHeight = (*iPanel)->GetVerticalStackElement()->getHeightForWidth(nWidth);        
        else
            if (iPanel==rPanels.begin())
                nPanelHeight = nHeightOfFirstPanel;
            else
                nPanelHeight = nHeightPerPanel;
        
        (*iPanel)->SetPosSizePixel(0, nY, nWidth, nPanelHeight);
        nY += nPanelHeight;
    }
}




Rectangle SidebarLayouter::PlaceDeckTitle (
    TitleBar* pDeckTitleBar,
    const Rectangle& rAvailableSpace)
{
    if (pDeckTitleBar == NULL)
    {
        OSL_ASSERT(pDeckTitleBar!=NULL);
        return rAvailableSpace;
    }
    
    const sal_Int32 nTitleBarHeight (pDeckTitleBar->GetSettings().GetStyleSettings().GetTitleHeight());
    pDeckTitleBar->SetPosSizePixel(
        rAvailableSpace.Left(),
        rAvailableSpace.Top(),
        rAvailableSpace.GetWidth(),
        nTitleBarHeight);
    return Rectangle(
        rAvailableSpace.Left(),
        rAvailableSpace.Top() + nTitleBarHeight,
        rAvailableSpace.Right(),
        rAvailableSpace.Bottom());
}



} // end of namespace sfx2
