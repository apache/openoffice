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
#include "TitleBar.hxx"
#include "DeckDescriptor.hxx"


namespace sfx2 {

Deck::Deck (
    const DeckDescriptor& rDeckDescriptor,
    Window* pParentWindow)
    : Window(pParentWindow),
      msId(rDeckDescriptor.msId),
      mpTitleBar(new TitleBar(rDeckDescriptor.msTitle, true, this))
{
    SetBackground(Wallpaper());
}




Deck::~Deck (void)
{
}




const ::rtl::OUString& Deck::GetId (void) const
{
    return msId;
}




TitleBar* Deck::GetTitleBar (void) const
{
    return mpTitleBar;
}




void Deck::Paint (const Rectangle& rUpdateArea)
{
    Window::Paint(rUpdateArea);
}


} // end of namespace sfx2
