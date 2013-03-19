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
#include <sfx2/sidebar/IContextChangeReceiver.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include "sidebar/ColorPopup.hxx"
#include "LineStylePopup.hxx"
#include "LineWidthPopup.hxx"


namespace svx { class ToolboxButtonColorUpdater; }
class SvxLineColorPage;
class SvxLineStylePage;
class SvxLineWidthPage;
class XLineStyleItem;
class XLineDashItem;
class XLineStartItem;
class XLineEndItem;
class LineEndLB_LPP;
class XLineEndList;
class ListBox;
class ToolBox;
class FloatingWindow;

namespace {
    const String Dash_Name[] =
    {
        String("Ultrafine dashed", 16,  RTL_TEXTENCODING_ASCII_US),				//0
        String("Dashed (variable)", 17,RTL_TEXTENCODING_ASCII_US ),				//6
        String("Fine dashed (variable)",22,  RTL_TEXTENCODING_ASCII_US),		//5	
        String("Fine dashed", 11, RTL_TEXTENCODING_ASCII_US),					//1
        String("Fine dotted", 11, RTL_TEXTENCODING_ASCII_US),					//3	
        String("Ultrafine dotted (variable)",27, RTL_TEXTENCODING_ASCII_US),	//8
        String("3 dashes 3 dots (variable)", 26, RTL_TEXTENCODING_ASCII_US),	//7
        String("2 dots 1 dash",13,RTL_TEXTENCODING_ASCII_US  )	,				//9
        String("Ultrafine 2 dots 3 dashes",  25, RTL_TEXTENCODING_ASCII_US),	//2
        String("Line with fine dots", 19, RTL_TEXTENCODING_ASCII_US)			//4
    };


    const XDash Dash_Set[] =
    {
        //Dash, dots, dotlen, dash, dashlen, distance	
        XDash(XDASH_RECT, 1, 51, 1, 51, 51),									//0
        XDash(XDASH_RECTRELATIVE, 1, 197, 0,0, 127),							//6
        XDash(XDASH_RECTRELATIVE, 1, 197,0,0,197),								//5
        XDash(XDASH_RECT, 1,508, 1,508, 508),									//1
        XDash(XDASH_RECT, 1,  0, 0,  0, 457),									//3
        XDash(XDASH_RECTRELATIVE, 1,  0, 0, 0, 50),								//8
        XDash(XDASH_RECTRELATIVE, 3, 197, 3, 0, 100),							//7
        XDash(XDASH_RECT, 2, 0, 1, 203,203)	,									//9
        XDash(XDASH_RECT, 2, 51, 3,254, 127),									//2
        XDash(XDASH_RECT, 1,2007,10, 0, 152)									//4
    };

    #define SIDEBAR_LINE_WIDTH_GLOBAL_VALUE String("PopupPanel_LineWidth", 20, RTL_TEXTENCODING_ASCII_US)

} //end of anonymous namespace


namespace svx { namespace sidebar {

class PopupContainer;
class ColorPopup;
class LineStyleControl;
class LineWidthControl;


class LinePropertyPanel
:   public Control,
    public ::sfx2::sidebar::IContextChangeReceiver,
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

    virtual void HandleContextChange(
        const ::sfx2::sidebar::EnumContext aContext);

    virtual void NotifyItemUpdate(
        const sal_uInt16 nSId,
        const SfxItemState eState,
        const SfxPoolItem* pState);

    SfxBindings* GetBindings();
    void ShowMenu (void);

    void SetLineDashItem(XLineDashItem* pDash);
    void SetLineStyleItem(XLineStyleItem* pStyle);

    void SetWidth(long nWidth);
    void SetWidthIcon(int n);
    void SetWidthIcon();

    void EndLineStylePopupMode (void);
    void EndLineWidthPopupMode (void);

private:
    //ui controls
    ::boost::scoped_ptr< FixedText >                        mpFTWidth;
    ::boost::scoped_ptr< Window >                           mpTBWidthBackground;
    ::boost::scoped_ptr< ToolBox >                          mpTBWidth;
    ::boost::scoped_ptr< FixedText >                        mpFTColor;
    ::boost::scoped_ptr< Window >                           mpTBColorBackground;
    ::boost::scoped_ptr< ToolBox >                          mpTBColor;
    ::boost::scoped_ptr< FixedText >                        mpFTStyle;
    ::boost::scoped_ptr< Window >                           mpTBStyleBackground;
    ::boost::scoped_ptr< ToolBox >                          mpTBStyle;
    ::boost::scoped_ptr< FixedText >                        mpFTTrancparency;
    ::boost::scoped_ptr< MetricField >                      mpMFTransparent;
    ::boost::scoped_ptr< FixedText >                        mpFTArrow;
    ::boost::scoped_ptr< LineEndLB_LPP >                    mpLBStart;
    ::boost::scoped_ptr< LineEndLB_LPP >                    mpLBEnd;
    ::boost::scoped_ptr< FixedText >                        mpFTEdgeStyle;
    ::boost::scoped_ptr< ListBox >                          mpLBEdgeStyle;
    ::boost::scoped_ptr< FixedText >                        mpFTCapStyle;
    ::boost::scoped_ptr< ListBox >                          mpLBCapStyle;

    //ControllerItem
    ::sfx2::sidebar::ControllerItem                         maStyleControl;
    ::sfx2::sidebar::ControllerItem                         maDashControl;
    ::sfx2::sidebar::ControllerItem                         maWidthControl;
    ::sfx2::sidebar::ControllerItem                         maColorControl;
    ::sfx2::sidebar::ControllerItem                         maStartControl;
    ::sfx2::sidebar::ControllerItem                         maEndControl;
    ::sfx2::sidebar::ControllerItem                         maLineEndListControl;
    ::sfx2::sidebar::ControllerItem                         maTransControl;
    ::sfx2::sidebar::ControllerItem                         maEdgeStyle;
    ::sfx2::sidebar::ControllerItem                         maCapStyle;

    Color                                                   maColor;
    ::boost::scoped_ptr< ::svx::ToolboxButtonColorUpdater > mpColorUpdater;
    ::boost::scoped_ptr< XLineStyleItem >                   mpStyleItem;
    ::boost::scoped_ptr< XLineDashItem >                    mpDashItem;
    sal_uInt16                                              mnTrans;
    SfxMapUnit                                              meMapUnit;
    sal_Int32                                               mnWidthCoreValue;
    XLineEndList*                                           mpLineEndList;
    ::boost::scoped_ptr< XLineStartItem >                   mpStartItem;
    ::boost::scoped_ptr< XLineEndItem >                     mpEndItem;

    //popup windows
    ColorPopup maColorPopup;
    LineStylePopup maLineStylePopup;
    LineWidthPopup maLineWidthPopup;

    // images from ressource
    Image                                                   maIMGColor;
    Image                                                   maIMGNone;

    // multi-images
    ::boost::scoped_array<Image>                            mpIMGStyleIcon;
    ::boost::scoped_array<Image>                            mpIMGWidthIcon;
    ::boost::scoped_array<Image>                            mpIMGStyleIconH;	//high contrast
    ::boost::scoped_array<Image>                            mpIMGWidthIconH;	//high contrast

    cssu::Reference< css::frame::XFrame >                   mxFrame;
    ::sfx2::sidebar::EnumContext                            maContext;
    SfxBindings*                                            mpBindings;

    /// bitfield
	bool				mbColorAvailable : 1;
	bool				mbStyleAvailable : 1;
	bool				mbDashAvailable : 1;
	bool				mbTransAvailable : 1;
	bool				mbWidthValuable : 1;
	bool				mbStartAvailable : 1;
	bool				mbEndAvailable : 1;

    void SetupIcons(void);
	void Initialize();	
	void FillLineEndList();
	void SelectEndStyle(bool bStart);

	DECL_LINK(ImplPopupModeEndHdl, FloatingWindow* );
	DECL_LINK(ImplWidthPopupModeEndHdl, FloatingWindow* );
	DECL_LINK(ToolboxColorSelectHdl, ToolBox*);
	DECL_LINK(ToolboxStyleSelectHdl, ToolBox*);
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

    void SetStyleIcon();
    void SetColor(
        const String& rsColorName,
        const Color aColor);

    PopupControl* CreateColorPopupControl (PopupContainer* pParent);
    PopupControl* CreateLineStylePopupControl (PopupContainer* pParent);
    PopupControl* CreateLineWidthPopupControl (PopupContainer* pParent);
};

} } // end of namespace svx::sidebar

#endif
