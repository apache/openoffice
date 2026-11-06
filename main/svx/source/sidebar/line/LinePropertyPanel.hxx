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

#ifndef SVX_PROPERTYPANEL_LINEPAGE_HXX
#define SVX_PROPERTYPANEL_LINEPAGE_HXX

#include <svx/xdash.hxx>
#include <vcl/ctrl.hxx>
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <sfx2/sidebar/ControllerItem.hxx>
#include <sfx2/sidebar/GridLayouter.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <memory>
#include <memory>
#include <svx/sidebar/ColorPopup.hxx>
#include <memory>
#include "LineWidthPopup.hxx"

namespace svx { class ToolboxButtonColorUpdater; }
class SvxLineColorPage;
class SvxLineStylePage;
class SvxLineWidthPage;
class XLineStyleItem;
class XLineDashItem;
class XLineStartItem;
class XLineEndItem;
class XLineEndList;
class XDashList;
class ListBox;
class ToolBox;
class FloatingWindow;

typedef ::std::shared_ptr< XLineEndList > XLineEndListSharedPtr;
typedef ::std::shared_ptr< XDashList > XDashListSharedPtr;

namespace {
    #define SIDEBAR_LINE_WIDTH_GLOBAL_VALUE String("PopupPanel_LineWidth", 20, RTL_TEXTENCODING_ASCII_US)
} //end of anonymous namespace

namespace svx { namespace sidebar {

class PopupContainer;
class ColorPopup;
class LineWidthControl;


class LinePropertyPanel
:   public Control,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
private:
    friend class ::SvxLineStylePage;
    friend class ::SvxLineWidthPage;

public:
    static LinePropertyPanel* Create(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);

    virtual void DataChanged(
        const DataChangedEvent& rEvent);

    virtual void NotifyItemUpdate(
        const sal_uInt16 nSId,
        const SfxItemState eState,
        const SfxPoolItem* pState,
        const bool bIsEnabled);

    SfxBindings* GetBindings();

    void SetWidth(long nWidth);
    void SetWidthIcon(int n);
    void SetWidthIcon();

    void EndLineWidthPopupMode (void);

    virtual void Resize (void);

private:
    //ui controls
    ::std::unique_ptr< FixedText []>                        mpFTWidth;
    ::std::unique_ptr< Window []>                           mpTBWidthBackground;
    ::std::unique_ptr< ToolBox []>                          mpTBWidth;
    ::std::unique_ptr< FixedText []>                        mpFTColor;
    ::std::unique_ptr< Window []>                           mpTBColorBackground;
    ::std::unique_ptr< ToolBox []>                          mpTBColor;
    ::std::unique_ptr< FixedText []>                        mpFTStyle;
    ::std::unique_ptr< ListBox []>                          mpLBStyle;
    ::std::unique_ptr< FixedText []>                        mpFTTrancparency;
    ::std::unique_ptr< MetricField []>                      mpMFTransparent;
    ::std::unique_ptr< FixedText []>                        mpFTArrow;
    ::std::unique_ptr< ListBox []>                          mpLBStart;
    ::std::unique_ptr< ListBox []>                          mpLBEnd;
    ::std::unique_ptr< FixedText []>                        mpFTEdgeStyle;
    ::std::unique_ptr< ListBox []>                          mpLBEdgeStyle;
    ::std::unique_ptr< FixedText []>                        mpFTCapStyle;
    ::std::unique_ptr< ListBox []>                          mpLBCapStyle;

    //ControllerItem
    ::sfx2::sidebar::ControllerItem                         maStyleControl;
    ::sfx2::sidebar::ControllerItem                         maDashControl;
    ::sfx2::sidebar::ControllerItem                         maWidthControl;
    ::sfx2::sidebar::ControllerItem                         maColorControl;
    ::sfx2::sidebar::ControllerItem                         maStartControl;
    ::sfx2::sidebar::ControllerItem                         maEndControl;
    ::sfx2::sidebar::ControllerItem                         maLineEndListControl;
    ::sfx2::sidebar::ControllerItem                         maLineStyleListControl;
    ::sfx2::sidebar::ControllerItem                         maTransControl;
    ::sfx2::sidebar::ControllerItem                         maEdgeStyle;
    ::sfx2::sidebar::ControllerItem                         maCapStyle;

    Color                                                   maColor;
    ::std::unique_ptr< ::svx::ToolboxButtonColorUpdater []> mpColorUpdater;
    ::std::unique_ptr< XLineStyleItem []>                   mpStyleItem;
    ::std::unique_ptr< XLineDashItem []>                    mpDashItem;
    sal_uInt16                                              mnTrans;
    SfxMapUnit                                              meMapUnit;
    sal_Int32                                               mnWidthCoreValue;
    XLineEndListSharedPtr                                   maLineEndList;
    XDashListSharedPtr                                      maLineStyleList;
    ::std::unique_ptr< XLineStartItem []>                   mpStartItem;
    ::std::unique_ptr< XLineEndItem []>                     mpEndItem;

    //popup windows
    ColorPopup maColorPopup;
    LineWidthPopup maLineWidthPopup;

    // images from resource
    Image                                                   maIMGColor;
    Image                                                   maIMGNone;

    // multi-images
    ::std::unique_ptr<Image[]>                            mpIMGWidthIcon;
    ::std::unique_ptr<Image[]>                            mpIMGWidthIconH;	//high contrast

    cssu::Reference< css::frame::XFrame >                   mxFrame;
    SfxBindings*                                            mpBindings;

    /// bitfield
	bool				mbColorAvailable : 1;
	bool				mbWidthValuable : 1;

    ::sfx2::sidebar::GridLayouter maLayouter;

    void SetupIcons(void);
	void Initialize();
	void FillLineEndList();
	void FillLineStyleList();
	void SelectEndStyle(bool bStart);
	void SelectLineStyle();

	DECL_LINK(ImplPopupModeEndHdl, FloatingWindow* );
	DECL_LINK(ImplWidthPopupModeEndHdl, FloatingWindow* );
	DECL_LINK(ToolboxColorSelectHdl, ToolBox*);
	DECL_LINK(ChangeLineStyleHdl, ToolBox*);
	DECL_LINK(ToolboxWidthSelectHdl, ToolBox*);
	DECL_LINK(ChangeTransparentHdl , void *);
	DECL_LINK(ChangeStartHdl, void *);
	DECL_LINK(ChangeEndHdl, void *);
	DECL_LINK(ChangeEdgeStyleHdl, void *);
	DECL_LINK(ChangeCapStyleHdl, void *);

    // constructor/destuctor
    LinePropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~LinePropertyPanel(void);

    void SetColor(
        const String& rsColorName,
        const Color aColor);

    PopupControl* CreateColorPopupControl (PopupContainer* pParent);
    PopupControl* CreateLineWidthPopupControl (PopupContainer* pParent);
};

} } // end of namespace svx::sidebar

#endif

// eof
