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
#include "Paint.hxx"
#include "Panel.hxx"
#include "sfx2/sidebar/Theme.hxx"

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
      maPanels(),
      maSeparators(),
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
    const int nBorderSize (Theme::GetInteger(Theme::Int_DeckBorderSize));

    return Rectangle(
        Theme::GetInteger(Theme::Int_DeckLeftPadding) + nBorderSize,
        Theme::GetInteger(Theme::Int_DeckTopPadding) + nBorderSize,
        aWindowSize.Width() - 1 - Theme::GetInteger(Theme::Int_DeckRightPadding) - nBorderSize,
        aWindowSize.Height() - 1 - Theme::GetInteger(Theme::Int_DeckBottomPadding) - nBorderSize);
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
    const SvBorder aPadding (
            Theme::GetInteger(Theme::Int_DeckLeftPadding),
            Theme::GetInteger(Theme::Int_DeckTopPadding),
            Theme::GetInteger(Theme::Int_DeckRightPadding),
            Theme::GetInteger(Theme::Int_DeckBottomPadding));

    // Paint deck background outside the border.
    Rectangle aBox(
        0,
        0,
        aWindowSize.Width() - 1,
        aWindowSize.Height() - 1);
    DrawHelper::DrawBorder(
        *this,
        aBox,
        aPadding,
        Theme::GetPaint(Theme::Paint_DeckBackground),
        Theme::GetPaint(Theme::Paint_DeckBackground));

    // Paint the border.
    const int nBorderSize (Theme::GetInteger(Theme::Int_DeckBorderSize));
    aBox.Left() += aPadding.Left();
    aBox.Top() += aPadding.Top();
    aBox.Right() -= aPadding.Right();
    aBox.Bottom() -= aPadding.Bottom();
    const sfx2::sidebar::Paint& rHorizontalBorderPaint (Theme::GetPaint(Theme::Paint_HorizontalBorder));
    DrawHelper::DrawBorder(
        *this,
        aBox,
        SvBorder(nBorderSize, nBorderSize, nBorderSize, nBorderSize),
        rHorizontalBorderPaint,
        Theme::GetPaint(Theme::Paint_VerticalBorder));

    // Paint the separators.
    const int nSeparatorHeight (Theme::GetInteger(Theme::Int_DeckSeparatorHeight));
    const int nLeft = aBox.Left() + nBorderSize;
    const int nRight = aBox.Right() - nBorderSize;
    for (::std::vector<sal_Int32>::const_iterator iY(maSeparators.begin()), iEnd(maSeparators.end()); iY!=iEnd; ++iY)
    {
        DrawHelper::DrawHorizontalLine(
            *this,
            nLeft,
            nRight,
            *iY,
            nSeparatorHeight,
            rHorizontalBorderPaint);
    }
}




void Deck::DataChanged (const DataChangedEvent& rEvent)
{
    (void)rEvent;
    RequestLayout();
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
            if (mpFiller == NULL)
            {
                OSL_ASSERT(maPanels.size()>0);
            }
            ShowFiller(PlaceDeckTitle(GetTitleBar(), GetContentArea()));
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

    // Prepare the separators, horizontal lines above and below the
    // panel titels.
    const sal_Int32 nDeckSeparatorHeight (Theme::GetInteger(Theme::Int_DeckSeparatorHeight));
    maSeparators.clear();

    if (rPanel.IsExpanded())
    {
        rPanel.Show();
        rPanel.GetTitleBar()->Hide();

        // Add single separator between deck title (pane title is not
        // visible) and panel content.
        maSeparators.push_back(aBox.Top());
        aBox.Top() += nDeckSeparatorHeight;
        
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
    const sal_Int32 nPanelTitleBarHeight (Theme::GetInteger(Theme::Int_PanelTitleBarHeight));

    // Prepare the separators, horizontal lines above and below the
    // panel titels.
    const sal_Int32 nDeckSeparatorHeight (Theme::GetInteger(Theme::Int_DeckSeparatorHeight));
    maSeparators.clear();
    
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
        nHeight -= 2*nDeckSeparatorHeight;

        if (rPanel.IsExpanded())
        {
            if (rPanel.GetVerticalStackElement().is())
            {
                ++nVerticalStackCount;
                const sal_Int32 nRequestedHeight (rPanel.GetVerticalStackElement()->getHeightForWidth(nWidth));
                nHeight -= nRequestedHeight;
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

        // Separator above the panel title bar.
        maSeparators.push_back(nY);
        nY += nDeckSeparatorHeight;
        
        // Place the title bar.
        TitleBar* pTitleBar = rPanel.GetTitleBar();
        pTitleBar->SetPosSizePixel(nX, nY, nWidth, nPanelTitleBarHeight);
        pTitleBar->Show();
        nY += nPanelTitleBarHeight;

        // Separator below the panel title bar.
        maSeparators.push_back(nY);
        nY += nDeckSeparatorHeight;

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

    const sal_Int32 nDeckTitleBarHeight (Theme::GetInteger(Theme::Int_DeckTitleBarHeight));
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
        mpFiller->SetBackground(Theme::GetPaint(Theme::Paint_DeckBackground).GetWallpaper());
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
