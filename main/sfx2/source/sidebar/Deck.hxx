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


namespace sfx2 {

class DeckDescriptor;
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

    const ::rtl::OUString& GetId (void) const;
    TitleBar* GetTitleBar (void) const;
    
    virtual void Paint (const Rectangle& rUpdateArea);

private:
    const ::rtl::OUString msTitle;
    const ::rtl::OUString msId;

    TitleBar* mpTitleBar;
};


} // end of namespace sfx2

#endif
