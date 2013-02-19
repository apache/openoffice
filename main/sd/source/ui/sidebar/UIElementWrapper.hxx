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

#ifndef SD_SIDEBAR_UIELEMENT_WRAPPER_HXX
#define SD_SIDEBAR_UIELEMENT_WRAPPER_HXX

#include <sfx2/sidebar/SidebarPanelBase.hxx>

#include <com/sun/star/drawing/framework/XResourceId.hpp>
#include <com/sun/star/drawing/framework/XConfigurationChangeListener.hpp>
#include <com/sun/star/ui/XSidebar.hpp>

#include <boost/scoped_ptr.hpp>


namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;
namespace cssdf = ::com::sun::star::drawing::framework;


namespace sd { namespace sidebar {

/** Make the legacy tool pane panels appear like XUIElement objects.

    In the constructor the drawing framework is requested to activate
    the associated resource.
    
    In the disposing() method the resource is requested to be
    deactivated.  Also in disposing() the windows (typically the
    content window of a view shell) below mpParentWindow
    are relocated into a temporary parent window and deleted in the
    destructor.
    
*/
class UIElementWrapper
    : public ::sfx2::sidebar::SidebarPanelBase
{
public:
    UIElementWrapper (
        const ::rtl::OUString& rsUIElementResourceURL,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        const ::rtl::OUString& rsTaskPanelPanelURL,
        const cssu::Reference<css::ui::XSidebar>& rxSidebar,
        ::Window* pParentWindow);
    virtual ~UIElementWrapper (void);

    // XSidebarPanel

    /** This is forwarded to the actual panel resource.
    */
    virtual css::ui::LayoutSize SAL_CALL getHeightForWidth (sal_Int32 nWidth)
        throw(cssu::RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing (
        const css::lang::EventObject& rEvent)
        throw (cssu::RuntimeException);

protected:
    virtual void SAL_CALL disposing (void)
        throw (cssu::RuntimeException);

private:
    cssu::Reference<cssdf::XResourceId> mxPanelResourceId;
    cssu::Reference<css::ui::XSidebar> mxSidebar;
    ::Window* mpParentWindow;
    // A temporary parent for children of mpParentWindow.  Typically
    // lives from calls to disposing(void) to ~UIElementWrapper().
    ::boost::scoped_ptr< ::Window> mpTemporaryParent;
    bool mbResourceHasBeenDeactivated;

    /** Called at the start of the lifetime of a new UIElementWrapper
        object.
        Transfers the XSidebar object to the panel control.
    */
    void HandleResourceActivation (const bool bIsResourceActive);
    void DestroyWindows (::boost::scoped_ptr< ::Window>& rpRoot);
};


} } // end of namespace sd::sidebar

#endif
