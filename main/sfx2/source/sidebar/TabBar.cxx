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

#include "TabBar.hxx"
#include "TabItem.hxx"
#include "sfx2/sidebar/ControlFactory.hxx"
#include "DeckDescriptor.hxx"
#include "Paint.hxx"
#include "sfx2/sidebar/Theme.hxx"
#include "Tools.hxx"

#include <vcl/gradient.hxx>
#include <vcl/image.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/componentcontext.hxx>
#include <tools/SvBorder.hxx>

#include <com/sun/star/graphic/XGraphicProvider.hpp>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;




namespace sfx2 { namespace sidebar {

TabBar::TabBar (
    Window* pParentWindow,
    const Reference<frame::XFrame>& rxFrame,
    const ::boost::function<void(const DeckDescriptor&)>& rDeckActivationFunctor,
    const ::boost::function<void(const Rectangle&)>& rPopupMenuProvider)
    : Window(pParentWindow),
      mxFrame(rxFrame),
      mpMenuButton(ControlFactory::CreateMenuButton(this)),
      maItems(),
      maDeckActivationFunctor(rDeckActivationFunctor),
      maPopupMenuProvider(rPopupMenuProvider)
{
    SetBackground(Theme::GetPaint(Theme::Paint_TabBarBackground).GetWallpaper());
    
    mpMenuButton->SetHelpText(A2S("This is the menu button"));
    mpMenuButton->SetQuickHelpText(A2S("This is the menu button"));
    mpMenuButton->SetModeImage(
        Theme::GetImage(Theme::Image_Menu),
        Theme::IsHighContrastMode()
            ? BMP_COLOR_HIGHCONTRAST
            : BMP_COLOR_NORMAL);
    mpMenuButton->SetClickHdl(LINK(this, TabBar, OnToolboxClicked));

    EnableClipSiblings();
}




TabBar::~TabBar (void)
{
}




void TabBar::Paint (const Rectangle& rUpdateArea)
{
    Window::Paint(rUpdateArea);

    const sal_Int32 nVerticalPadding (Theme::GetTabMenuPadding());
    const sal_Int32 nHorizontalPadding (Theme::GetInteger(Theme::Int_TabMenuSeparatorPadding));
    SetLineColor(Theme::GetColor(Theme::Color_TabMenuSeparator));
    DrawLine(
        Point(nHorizontalPadding, mnMenuSeparatorY),
        Point(GetSizePixel().Width()-nHorizontalPadding, mnMenuSeparatorY));
}




sal_Int32 TabBar::GetDefaultWidth (void)
{
    return Theme::GetInteger(Theme::Int_TabItemWidth)
        + Theme::GetInteger(Theme::Int_TabBarLeftPadding)
        + Theme::GetInteger(Theme::Int_TabBarRightPadding);
}




void TabBar::SetDecks (
    const ResourceManager::DeckContainer& rDeckDescriptors)
{
    // Remove the current buttons.
    {
        Window aTemporaryParent (NULL,0);
        for(ItemContainer::const_iterator
                iItem(maItems.begin()), iEnd(maItems.end());
            iItem!=iEnd;
            ++iItem)
        {
            removeWindow(iItem->mpButton, &aTemporaryParent);
        }
        maItems.clear();
    }

    maItems.resize(rDeckDescriptors.size());
    sal_Int32 nIndex (0);
    for (ResourceManager::DeckContainer::const_iterator
             iDeck(rDeckDescriptors.begin()),
             iEnd(rDeckDescriptors.end());
         iDeck!=iEnd;
         ++iDeck)
    {
        Item& rItem (maItems[nIndex++]);
        rItem.maDeckDescriptor = *iDeck;
        rItem.mpButton = CreateTabItem(*iDeck);
        rItem.mpButton->SetClickHdl(LINK(&rItem, TabBar::Item, HandleClick));
        rItem.maDeckActivationFunctor = maDeckActivationFunctor;
        rItem.mbIsHiddenByDefault = false;
        rItem.mbIsHidden = rItem.mbIsHiddenByDefault;
    }

    UpdateButtonIcons();
    Layout();
}




void TabBar::UpdateButtonIcons (void)
{
    const BmpColorMode eColorMode (
        Theme::IsHighContrastMode()
            ? BMP_COLOR_HIGHCONTRAST
            : BMP_COLOR_NORMAL);
    
    mpMenuButton->SetModeImage(Theme::GetImage(Theme::Image_Menu), eColorMode);

    for(ItemContainer::const_iterator
            iItem(maItems.begin()), iEnd(maItems.end());
        iItem!=iEnd;
        ++iItem)
    {
        iItem->mpButton->SetModeImage(
            GetItemImage(iItem->maDeckDescriptor),
            eColorMode);
    }
}




void TabBar::Layout (void)
{
    const SvBorder aPadding (
        Theme::GetInteger(Theme::Int_TabBarLeftPadding),
        Theme::GetInteger(Theme::Int_TabBarTopPadding),
        Theme::GetInteger(Theme::Int_TabBarRightPadding),
        Theme::GetInteger(Theme::Int_TabBarBottomPadding));
    sal_Int32 nX (aPadding.Top());
    sal_Int32 nY (aPadding.Left());
    const Size aTabItemSize (
        Theme::GetInteger(Theme::Int_TabItemWidth),
        Theme::GetInteger(Theme::Int_TabItemHeight));

    if (mpMenuButton != NULL)
    {
        mpMenuButton->SetPosSizePixel(
            Point(nX,nY),
            aTabItemSize);
        mpMenuButton->Show();
        nY += mpMenuButton->GetSizePixel().Height() + 1 + Theme::GetInteger(Theme::Int_TabMenuPadding);
        mnMenuSeparatorY = nY - Theme::GetInteger(Theme::Int_TabMenuPadding)/2 - 1;
    }
    
    for(ItemContainer::const_iterator
            iItem(maItems.begin()), iEnd(maItems.end());
        iItem!=iEnd;
        ++iItem)
    {
        if (iItem->mbIsHidden)
            continue;

        Button& rButton (*iItem->mpButton);
        
        // Place and size the icon.
        rButton.SetPosSizePixel(
            Point(nX,nY),
            aTabItemSize);
        rButton.Show();
        
        nY += rButton.GetSizePixel().Height() + 1 + aPadding.Bottom();
    }
    Invalidate();
}




void TabBar::HighlightDeck (const ::rtl::OUString& rsDeckId)
{
    for (ItemContainer::const_iterator iItem(maItems.begin()),iEnd(maItems.end());
         iItem!=iEnd;
         ++iItem)
    {
        if (iItem->maDeckDescriptor.msId.equals(rsDeckId))
        {
            iItem->mpButton->Check();
            break;
        }
    }
}




void TabBar::DataChanged (const DataChangedEvent& rDataChangedEvent)
{
    if  (rDataChangedEvent.GetType() == DATACHANGED_SETTINGS
        &&  (rDataChangedEvent.GetFlags() & SETTINGS_STYLE)!= 0)
    {
        UpdateButtonIcons();
        Invalidate();
    }
    else
        Window::DataChanged(rDataChangedEvent);
}




RadioButton* TabBar::CreateTabItem (const DeckDescriptor& rDeckDescriptor)
{
    RadioButton* pItem = ControlFactory::CreateTabItem(this);
    pItem->SetHelpText(rDeckDescriptor.msHelpText);
    pItem->SetQuickHelpText(rDeckDescriptor.msHelpText);

    return pItem;
}




Image TabBar::GetItemImage (const DeckDescriptor& rDeckDescriptor) const
{
    return Tools::GetImage(
        rDeckDescriptor.msIconURL,
        rDeckDescriptor.msHighContrastIconURL,
        mxFrame);
}




IMPL_LINK(TabBar::Item, HandleClick, Button*, pButton)
{
    maDeckActivationFunctor(maDeckDescriptor);
    return 1;
}




void TabBar::AddPopupMenuEntries (
    PopupMenu& rMenu,
    const sal_Int32 nFirstIndex)
{
    sal_Int32 nIndex (nFirstIndex);
    for(ItemContainer::const_iterator iItem(maItems.begin()),iEnd(maItems.end());
        iItem!=iEnd;
        ++iItem)
    {
        rMenu.InsertItem(nIndex, iItem->maDeckDescriptor.msTitle, MIB_RADIOCHECK);
        rMenu.CheckItem(nIndex, iItem->mpButton->IsChecked());
        ++nIndex;
    }
}




void TabBar::AddCustomizationMenuEntries (
    PopupMenu& rMenu,
    const sal_Int32 nFirstIndex)
{
    sal_Int32 nIndex (nFirstIndex);
    for(ItemContainer::const_iterator iItem(maItems.begin()),iEnd(maItems.end());
        iItem!=iEnd;
        ++iItem)
    {
        rMenu.InsertItem(nIndex, iItem->maDeckDescriptor.msTitle, MIB_CHECKABLE);
        rMenu.CheckItem(nIndex, !iItem->mbIsHidden);
        ++nIndex;
    }
}




const DeckDescriptor& TabBar::GetDeckDescriptorForIndex (const sal_Int32 nIndex) const
{
    if (nIndex<0 || nIndex>=maItems.size())
        throw RuntimeException();
    else
        return maItems[nIndex].maDeckDescriptor;
}




void TabBar::ToggleHideFlag (const sal_Int32 nIndex)
{
    if (nIndex<0 || nIndex>=maItems.size())
        throw RuntimeException();
    else
    {
        maItems[nIndex].mbIsHidden = ! maItems[nIndex].mbIsHidden;
        Layout();
    }
}




void TabBar::RestoreHideFlags (void)
{
    bool bNeedsLayout (false);
    for(ItemContainer::iterator iItem(maItems.begin()),iEnd(maItems.end());
        iItem!=iEnd;
        ++iItem)
    {
        if (iItem->mbIsHidden != iItem->mbIsHiddenByDefault)
        {
            iItem->mbIsHidden = iItem->mbIsHiddenByDefault;
            bNeedsLayout = true;
        }
    }
    if (bNeedsLayout)
        Layout();
}




IMPL_LINK(TabBar, OnToolboxClicked, void*, pToolBox)
{
    //    mpMenuButton->EndSelection();
    maPopupMenuProvider(
        Rectangle(
            mpMenuButton->GetPosPixel(),
            mpMenuButton->GetSizePixel()));
    
    return 0;
}



} } // end of namespace sfx2::sidebar
