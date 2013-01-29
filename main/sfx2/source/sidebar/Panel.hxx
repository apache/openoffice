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

#include <vcl/window.hxx>

#include <com/sun/star/ui/XUIElement.hpp>
#include <com/sun/star/ui/XVerticalStackLayoutElement.hpp>

#include <boost/function.hpp>

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace sfx2 { namespace sidebar {

class PanelDescriptor;
class TitleBar;


class Panel
    : public Window
{
public:
    Panel (
        const PanelDescriptor& rPanelDescriptor,
        Window* pParentWindow,
        const ::boost::function<void(void)>& rDeckLayoutTrigger);
    virtual ~Panel (void);

    void Dispose (void);
    
    const ::rtl::OUString& GetLayoutHint (void) const;
    TitleBar* GetTitleBar (void) const;
    bool IsTitleBarOptional (void) const;
    void SetUIElement (const cssu::Reference<css::ui::XUIElement>& rxElement);
    cssu::Reference<css::ui::XVerticalStackLayoutElement> GetVerticalStackElement (void) const;
    void SetExpanded (const bool bIsExpanded);
    bool IsExpanded (void) const;
    bool HasIdPredicate (const ::rtl::OUString& rsId) const;
    
    virtual void Paint (const Rectangle& rUpdateArea);
    virtual void SetPosSizePixel (
        long nX,
        long nY,
        long nWidth,
        long nHeight,
        sal_uInt16 nFlags = WINDOW_POSSIZE_ALL);
    virtual void DataChanged (const DataChangedEvent& rEvent);
    virtual void Activate (void);

private:
    const ::rtl::OUString msPanelId;
    const ::rtl::OUString msLayoutHint;
    TitleBar* mpTitleBar;
    const bool mbIsTitleBarOptional;
    cssu::Reference<css::ui::XUIElement> mxElement;
    cssu::Reference<css::awt::XWindow> mxElementWindow;
    cssu::Reference<css::ui::XVerticalStackLayoutElement> mxVerticalStackLayoutElement;
    bool mbIsExpanded;
    const ::boost::function<void(void)> maDeckLayoutTrigger;
    Rectangle maBoundingBox;

    DECL_LINK(WindowEventHandler, VclWindowEvent*);
};

    

} } // end of namespace sfx2::sidebar

#endif
