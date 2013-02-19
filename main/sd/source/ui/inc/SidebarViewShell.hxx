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

#ifndef SD_SIDEBAR_VIEW_SHELL_HXX
#define SD_SIDEBAR_VIEW_SHELL_HXX

#include "ViewShell.hxx"
#include "SidebarPanelId.hxx"
#include "glob.hxx"
#include "framework/FrameworkHelper.hxx"
#include <vcl/button.hxx>
#include <vcl/wrkwin.hxx>
#include <sfx2/shell.hxx>
#include <sfx2/viewfac.hxx>
#include <sfx2/dockwin.hxx>

#include <memory>
#include <boost/shared_ptr.hpp>


namespace sd {  namespace sidebar {

class SidebarShellManager;

/** This is the bare minimum that is necessary to run the legacy task panels.
*/
class SidebarViewShell
    : public ViewShell
{
public:
    TYPEINFO();
    SFX_DECL_INTERFACE(SD_IF_SDSIDEBARVIEWSHELL)

    SidebarViewShell (
        SfxViewFrame* pFrame, 
        ViewShellBase& rViewShellBase,
        ::Window* pParentWindow,
        FrameView* pFrameView);
    virtual ~SidebarViewShell (void);

    /** Register the SFX interfaces so that (some of) the controls can be
        pushed as SFX shells on the shell stack and process slot calls and
        so on.
    */
    static void RegisterControls (void);

    /** This factory method is used by the TaskPanelFactory to create
        task panel objects.
    */
    ::Window* CreatePanel (
        ::Window* pParentWindow,
        const PanelId ePanelId);
    
    virtual void GetFocus (void);
    virtual void LoseFocus (void);
    virtual void KeyInput (const KeyEvent& rEvent);
	using sd::ViewShell::KeyInput;

	virtual SdPage*	GetActualPage (void);
	virtual SdPage*	getCurrentPage (void) const;

    void Execute (SfxRequest& rRequest);
    void GetState (SfxItemSet& rItemSet);

	virtual void ArrangeGUIElements (void);

    SidebarShellManager& GetSubShellManager (void) const;

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>
        CreateAccessibleDocumentView (::sd::Window* pWindow);

    virtual css::uno::Reference<css::drawing::XDrawSubController> CreateSubController (void);

    /** Relocate all toplevel controls to the given parent window.
    */
    virtual bool RelocateToParentWindow (::Window* pParentWindow);

private:
    ::boost::shared_ptr<SidebarShellManager> mpSubShellManager;
    WorkWindow maContentWindowParent;
        
    /** Return a pointer to the docking window that is the parent or a
        predecessor of the content window.
        @return
            When the view shell is not placed in a docking window, e.g. when
            shown in the center pane, then <NULL?> is returned.
    */
    DockingWindow* GetDockingWindow (void);
};



} } // end of namespace ::sd::sidebar

#endif
