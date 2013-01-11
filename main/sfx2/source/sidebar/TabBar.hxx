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

#ifndef SFX_SIDEBAR_TAB_BAR_HXX
#define SFX_SIDEBAR_TAB_BAR_HXX

#include "DeckDescriptor.hxx"
#include "ResourceManager.hxx"

#include "vcl/window.hxx"

class Button;

namespace sfx2 { namespace sidebar {

class TabBarConfiguration;
class TabItem;

/** The tab bar is the container for the individual tabs.
*/
class TabBar
    : public Window
{
public:
    TabBar (
        Window* pParentWindow,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        const ::boost::function<void(const DeckDescriptor&)>& rDeckActivationFunctor,
        const ::boost::function<void(const Rectangle&)>& rPopupMenuProvider);
    virtual ~TabBar (void);

    virtual void Paint (const Rectangle& rUpdateArea);
    virtual void DataChanged (const DataChangedEvent& rDataChangedEvent);

    static sal_Int32 GetDefaultWidth (void);

    void SetDecks (
        const ResourceManager::DeckContainer& rDeckDescriptors);
    void HighlightDeck (const ::rtl::OUString& rsDeckId);
    void AddPopupMenuEntries (
        PopupMenu& rMenu,
        const sal_Int32 nFirstIndex);
    void AddCustomizationMenuEntries (
        PopupMenu& rMenu,
        const sal_Int32 nFirstIndex);
    const DeckDescriptor& GetDeckDescriptorForIndex (const sal_Int32 nIndex) const;
    void ToggleHideFlag (const sal_Int32 nIndex);
    void RestoreHideFlags (void);
    
private:
    cssu::Reference<css::frame::XFrame> mxFrame;
    Button* mpMenuButton;
    class Item
    {
    public:
        DECL_LINK(HandleClick, Button*);
        RadioButton* mpButton;
        DeckDescriptor maDeckDescriptor;
        ::boost::function<void(const DeckDescriptor&)> maDeckActivationFunctor;
        bool mbIsHidden;
        bool mbIsHiddenByDefault;
    };
    typedef ::std::vector<Item> ItemContainer;
    ItemContainer maItems;
    const ::boost::function<void(const DeckDescriptor&)> maDeckActivationFunctor;
    sal_Int32 mnMenuSeparatorY;
    ::boost::function<void(const Rectangle&)> maPopupMenuProvider;
    
    RadioButton* CreateTabItem (const DeckDescriptor& rDeckDescriptor);
    Image GetItemImage (const DeckDescriptor& rDeskDescriptor) const;
    void Layout (void);
    void UpdateButtonIcons (void);

    ::boost::shared_ptr<PopupMenu> CreatePopupMenu (void) const;
    void ShowPopupMenu (void) const;
    DECL_LINK(OnToolboxClicked, void*);

};


} } // end of namespace sfx2::sidebar

#endif
