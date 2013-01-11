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

#include "ControlFactory.hxx"

#include "MenuButton.hxx"
#include "TabItem.hxx"


namespace sfx2 { namespace sidebar {


CheckBox* ControlFactory::CreateMenuButton (Window* pParentWindow)
{
    return new MenuButton(pParentWindow);
}




ImageRadioButton* ControlFactory::CreateTabItem (Window* pParentWindow)
{
    return new TabItem(pParentWindow);
}


} } // end of namespace sfx2::sidebar
