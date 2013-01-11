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

#include "Deck.hxx"
#include "DeckDescriptor.hxx"
#include "DrawHelper.hxx"
#include "DeckTitleBar.hxx"
#include "Panel.hxx"
#include "Theme.hxx"

#include <tools/SvBorder.hxx>


namespace sfx2 { namespace sidebar {


namespace {
    static const sal_Int32 MinimalPanelHeight (25);
}



Deck::Deck (
    const DeckDescriptor& rDeckDescriptor,
    Window* pParentWindow)
    : Window(pParentWindow),
      msId(rDeckDescriptor.msId),
      mpTitleBar(new DeckTitleBar(rDeckDescriptor.msTitle, this)),
      maIcon(),
      msIconURL(rDeckDescriptor.msIconURL),
      msHighContrastIconURL(rDeckDescriptor.msHighContrastIconURL),
      mpFiller(NULL)
{
    SetBackground(Wallpaper());
}




Deck::~Deck (void)
{
    Dispose();
}




void Deck::Dispose (void)
{
    ::std::vector<Panel*> aPanels;
    aPanels.swap(maPanels);
    for (::std::vector<Panel*>::const_iterator
             iPanel(aPanels.begin()),
             iEnd(aPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        (*iPanel)->Dispose();
    }
}




const ::rtl::OUString& Deck::GetId (void) const
{
    return msId;
}




TitleBar* Deck::GetTitleBar (void) const
{
    return mpTitleBar;
}




Rectangle Deck::GetContentArea (void) const
{
    const Size aWindowSize (GetSizePixel());
    const SvBorder aPadding (Theme::GetDeckPadding());
    const int nBorderSize (Theme::GetBorderSize());

    return Rectangle(
        aPadding.Left() + nBorderSize,
        aPadding.Top() + nBorderSize,
        aWindowSize.Width() - 1 - aPadding.Right() - nBorderSize,
        aWindowSize.Height() - 1 - aPadding.Bottom() - nBorderSize);
}




::rtl::OUString Deck::GetIconURL (const bool bIsHighContrastModeActive) const
{
    if (bIsHighContrastModeActive)
        return msHighContrastIconURL;
    else
        return msIconURL;
}




void Deck::Paint (const Rectangle& rUpdateArea)
{
    const Size aWindowSize (GetSizePixel());
    const SvBorder aPadding (Theme::GetDeckPadding());

    // Paint deck background outside the border.
    DrawHelper::DrawBorder(
        *this,
        Rectangle(
            0,
            0,
            aWindowSize.Width() - 1,
            aWindowSize.Height() - 1),
        aPadding,
        Theme::GetDeckBackground(),
        Theme::GetDeckBackground());

    // Paint the border.
    const int nBorderSize (Theme::GetBorderSize());
    DrawHelper::DrawBorder(
        *this,
        Rectangle(
            aPadding.Left(),
            aPadding.Top(),
            aWindowSize.Width() - 1 - aPadding.Right(),
            aWindowSize.Height() - 1 - aPadding.Bottom()),
        SvBorder(nBorderSize, nBorderSize, nBorderSize, nBorderSize),
        Theme::GetHorizontalBorderPaint(),
        Theme::GetVerticalBorderPaint());
}




void Deck::SetPanels (const ::std::vector<Panel*>& rPanels)
{
    maPanels.resize(rPanels.size());
    ::std::copy(rPanels.begin(), rPanels.end(), maPanels.begin());

    RequestLayout();
}




void Deck::RequestLayout (void)
{
    switch(maPanels.size())
    {
        case 0:
            // This basically is an error but there is not much that
            // we can do about it.
            OSL_ASSERT(maPanels.size()>0);
            break;

        case 1:
            if (maPanels[0]->IsTitleBarOptional())
            {
                // There is only one panel in the deck and its title
                // bar is optional => do not draw the title bar.
                LayoutSinglePanel();
            }
            else
                LayoutMultiplePanels();
            break;

        default:
            LayoutMultiplePanels();
            break;
    }

    Invalidate();
}




void Deck::LayoutSinglePanel (void)
{
    Rectangle aBox (GetContentArea());
    aBox = PlaceDeckTitle(GetTitleBar(), aBox);
    Panel& rPanel (*maPanels.front());

    if (rPanel.IsExpanded())
    {
        rPanel.Show();
        rPanel.GetTitleBar()->Hide();
        rPanel.SetPosSizePixel(aBox.Left(), aBox.Top(), aBox.GetWidth(), aBox.GetHeight());
        HideFiller();
    }
    else
    {
        rPanel.Hide();
        ShowFiller(aBox);
    }
}




void Deck::LayoutMultiplePanels (void)
{
    Rectangle aBox (GetContentArea());
    if (aBox.GetWidth()<=0 || aBox.GetHeight()<=0)
        return;
    aBox = PlaceDeckTitle(GetTitleBar(), aBox);
    const sal_Int32 nWidth (aBox.GetWidth());
    const sal_Int32 nPanelTitleBarHeight (Theme::GetPanelTitleBarHeight());

    // Determine the height that is available for panel content
    // and count the different layouts.
    const sal_Int32 nX = aBox.Left();
    sal_Int32 nHeight = aBox.GetHeight();
    sal_Int32 nFullCount (0);
    sal_Int32 nVerticalStackCount (0);
    for (::std::vector<Panel*>::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        const Panel& rPanel (**iPanel);
        
        nHeight -= nPanelTitleBarHeight;

        if (rPanel.IsExpanded())
        {
            if (rPanel.GetVerticalStackElement().is())
            {
                ++nVerticalStackCount;
                nHeight -= rPanel.GetVerticalStackElement()->getHeightForWidth(nWidth);
            }
            else
            {
                ++nFullCount;
            }
        }
    }

    sal_Int32 nHeightPerPanel (nFullCount<=0 ? 0 : nHeight / nFullCount);
    // The division might lead to rounding errors.  Enlarge the first
    // panel to compensate for that.
    sal_Int32 nHeightOfFirstPanel = nFullCount+nVerticalStackCount<=0
        ? 0
        : nHeightPerPanel + (nHeight - nHeightPerPanel*(nFullCount+nVerticalStackCount));
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
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        Panel& rPanel (**iPanel);
        
        // Place the title bar.
        TitleBar* pTitleBar = rPanel.GetTitleBar();
        pTitleBar->SetPosSizePixel(nX, nY, nWidth, nPanelTitleBarHeight);
        pTitleBar->Show();
        nY += nPanelTitleBarHeight;

        if (rPanel.IsExpanded())
        {
            rPanel.Show();
            
            // Place the panel.
            sal_Int32 nPanelHeight (0);
            if (rPanel.GetVerticalStackElement().is())
                nPanelHeight = rPanel.GetVerticalStackElement()->getHeightForWidth(nWidth);        
            else
                if (iPanel==maPanels.begin())
                    nPanelHeight = nHeightOfFirstPanel;
                else
                    nPanelHeight = nHeightPerPanel;
            
            rPanel.SetPosSizePixel(nX, nY, nWidth, nPanelHeight);
            nY += nPanelHeight;
        }
        else
            rPanel.Hide();
    }

    if (nY < aBox.Bottom())
        ShowFiller(Rectangle(aBox.Left(), nY, aBox.Right(), aBox.Bottom()));
    else
        HideFiller();
}




Rectangle Deck::PlaceDeckTitle (
    TitleBar* pDeckTitleBar,
    const Rectangle& rAvailableSpace)
{
    if (pDeckTitleBar == NULL)
    {
        OSL_ASSERT(pDeckTitleBar!=NULL);
        return rAvailableSpace;
    }

    const sal_Int32 nDeckTitleBarHeight (Theme::GetDeckTitleBarHeight());
    pDeckTitleBar->SetPosSizePixel(
        rAvailableSpace.Left(),
        rAvailableSpace.Top(),
        rAvailableSpace.GetWidth(),
        nDeckTitleBarHeight);
    pDeckTitleBar->Show();
    return Rectangle(
        rAvailableSpace.Left(),
        rAvailableSpace.Top() + nDeckTitleBarHeight,
        rAvailableSpace.Right(),
        rAvailableSpace.Bottom());
}




void Deck::ShowFiller (const Rectangle& rBox)
{
    if (mpFiller == NULL)
    {
        mpFiller = new Window(this);
        mpFiller->SetBackground(Theme::GetDeckBackground().GetWallpaper());
    }
    mpFiller->SetPosSizePixel(rBox.TopLeft(), rBox.GetSize());
    mpFiller->Show();
}




void Deck::HideFiller (void)
{
    if (mpFiller != NULL)
        mpFiller->Hide();
}



} } // end of namespace sfx2::sidebar
