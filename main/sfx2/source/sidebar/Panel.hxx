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

#ifndef SFX_SIDEBAR_PANEL_HXX
#define SFX_SIDEBAR_PANEL_HXX

#include "vcl/window.hxx"
#include <com/sun/star/ui/XUIElement.hpp>
#include <com/sun/star/ui/XVerticalStackLayoutElement.hpp>

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace sfx2 {

class ContentPanelDescriptor;
class TitleBar;


class Panel
    : public Window
{
public:
    Panel (
        const ContentPanelDescriptor& rPanelDescriptor,
        Window* pParentWindow);
    virtual ~Panel (void);

    const ::rtl::OUString& GetLayoutHint (void) const;
    TitleBar* GetTitleBar (void) const;
    bool IsTitleBarOptional (void) const;
    void SetUIElement (const cssu::Reference<css::ui::XUIElement>& rxElement);
    cssu::Reference<css::ui::XVerticalStackLayoutElement> Panel::GetVerticalStackElement (void) const;
    
    virtual void Paint (const Rectangle& rUpdateArea);

private:
    const ::rtl::OUString msLayoutHint;
    TitleBar* mpTitleBar;
    const bool mbIsTitleBarOptional;
    cssu::Reference<css::ui::XUIElement> mxElement;
    cssu::Reference<css::ui::XVerticalStackLayoutElement> mxVerticalStackLayoutElement;
};


} // end of namespace sfx2

#endif
