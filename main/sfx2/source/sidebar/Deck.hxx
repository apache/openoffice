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

#ifndef SFX_SIDEBAR_DECK_HXX
#define SFX_SIDEBAR_DECK_HXX

#include "vcl/window.hxx"
#include "vcl/image.hxx"


namespace sfx2 { namespace sidebar {

class DeckDescriptor;
class Panel;
class TitleBar;


/** This is the parent window of the panels.
    It displays the deck title.
*/
class Deck
    : public Window
{
public:
    Deck (
        const DeckDescriptor& rDeckDescriptor,
        Window* pParentWindow);
    virtual ~Deck (void);

    void Dispose (void);
    
    const ::rtl::OUString& GetId (void) const;
    TitleBar* GetTitleBar (void) const;
    Rectangle GetContentArea (void) const;
    ::rtl::OUString GetIconURL (const bool bIsHighContrastModeActive) const;
    void SetPanels (const ::std::vector<Panel*>& rPanels);
    void RequestLayout (void);
    
    virtual void Paint (const Rectangle& rUpdateArea);
    virtual void DataChanged (const DataChangedEvent& rEvent);

private:
    const ::rtl::OUString msTitle;
    const ::rtl::OUString msId;
    TitleBar* mpTitleBar;
    Image maIcon;
    const ::rtl::OUString msIconURL;
    const ::rtl::OUString msHighContrastIconURL;
    ::std::vector<Panel*> maPanels;
    ::std::vector<sal_Int32> maSeparators;
    Window* mpFiller;
    
    void LayoutSinglePanel (void);
    void LayoutMultiplePanels (void);
    Rectangle PlaceDeckTitle (
        TitleBar* pDeckTitleBar,
        const Rectangle& rAvailableSpace);
    void ShowFiller (const Rectangle& rBox);
    void HideFiller (void);
};


} } // end of namespace sfx2::sidebar

#endif
