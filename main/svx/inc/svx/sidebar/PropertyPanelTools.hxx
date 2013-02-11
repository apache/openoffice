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

#ifndef _SVX_SIDEBAR_PANEL_TOOLS_HXX_
#define _SVX_SIDEBAR_PANEL_TOOLS_HXX_

#include "svx/svxdllapi.h"

#include <vcl/ctrl.hxx>
//#include "layoutablewin.hxx"
//#include "propertypaneltype.hxx"
//#include <sfx2/dllapi.h>
//#include <sfx2/ctrlitem.hxx>
#include <vcl/floatwin.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines
//class SfxPropertyPanel;
//class SfxSectionPage;
//typedef SfxSectionPage* (*FnSectionPageCtor)(Window* pParent, SfxPropertyPanel* pPanel, PropertySectionType nPageId);

//////////////////////////////////////////////////////////////////////////////

namespace svx { namespace sidebar {

//////////////////////////////////////////////////////////////////////////////

//class SVX_DLLPUBLIC SfxSectionPage: public Control, public ILayoutableWindow
//{
//public:
//    SfxSectionPage(Window* pParent, const ResId& rResId ,SfxPropertyPanel* pPanel, PropertySectionType nPageId);	
//    virtual ~SfxSectionPage();
//
//    virtual void Init(PropertyContextType nContextId){}
//    virtual sal_Int32 GetPreferredWidth (sal_Int32 nHeight);
//    virtual sal_Int32 GetPreferredHeight (sal_Int32 nWidth);
//    virtual sal_Int32 GetMinimumWidth (void);
//    virtual void StateChangedImpl(sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState);
//    SfxBindings* GetBindings();
//
//    SfxPropertyPanel* GetPanel();
//    long GetPanelWidth();
//    void PanelResize();
//
//private:
//    SfxPropertyPanel*           mpPanel;
//    PropertySectionType         mnPageId;
//    sal_Int32                   mnMinimumWidth;
//};

//////////////////////////////////////////////////////////////////////////////

//class SVX_DLLPUBLIC SfxPropertyPageController:public SfxControllerItem
//{
//private:
//    SfxSectionPage* mpPage;
//
//public:
//    SfxPropertyPageController(sal_uInt16 nId, SfxSectionPage *pPage, SfxBindings &rBindings);
//    virtual ~SfxPropertyPageController();
//
//    virtual void StateChanged(sal_uInt16 nId, SfxItemState eState, const SfxPoolItem* pState);
//};

//////////////////////////////////////////////////////////////////////////////
// SfxPopupPanelWin

class SVX_DLLPUBLIC PropertyPanelPopuplWindow : public FloatingWindow
{
public:
    PropertyPanelPopuplWindow(Window* pParent);
    virtual ~PropertyPanelPopuplWindow();

    virtual long Notify( NotifyEvent& rNEvt );
};

//////////////////////////////////////////////////////////////////////////////
// SfxPopupPage

class SVX_DLLPUBLIC PropertyPanelPopupPage : public Control
{
public :
    PropertyPanelPopupPage(Window* pParent, const ResId& rResId);
    virtual ~PropertyPanelPopupPage();

    virtual void Paint(const Rectangle& rect);
};

//////////////////////////////////////////////////////////////////////////////

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////

#endif // _SVX_SIDEBAR_PANEL_TOOLS_HXX_

//////////////////////////////////////////////////////////////////////////////
// eof
