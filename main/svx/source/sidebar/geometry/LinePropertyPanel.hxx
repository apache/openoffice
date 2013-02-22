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

#include <vcl/ctrl.hxx>
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <sfx2/sidebar/ControllerItem.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <svx/sidebar/PropertyPanelTools.hxx>
#include <boost/scoped_ptr.hpp>

//////////////////////////////////////////////////////////////////////////////
// pedefines

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

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace svx { namespace sidebar {

class LinePropertyPanel
:   public Control,
    public ::sfx2::sidebar::SidebarPanelBase::ContextChangeReceiverInterface,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
private:
    friend class ::SvxLineColorPage;
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
    ::boost::scoped_ptr< LineEndLB_LPP >                        mpLBStart;
    ::boost::scoped_ptr< LineEndLB_LPP >                        mpLBEnd;

    //ControllerItem
    ::sfx2::sidebar::ControllerItem                         maColorControl;
    ::sfx2::sidebar::ControllerItem                         maStyleControl;
    ::sfx2::sidebar::ControllerItem                         maDashControl;
    ::sfx2::sidebar::ControllerItem                         maWidthControl;
    ::sfx2::sidebar::ControllerItem                         maTransControl;
    ::sfx2::sidebar::ControllerItem                         maStartControl;
    ::sfx2::sidebar::ControllerItem                         maEndControl;
    ::sfx2::sidebar::ControllerItem                         maLineEndListControl;

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
    ::boost::scoped_ptr< PropertyPanelPopuplWindow >        mpFloatWinColor;
    ::boost::scoped_ptr< SvxLineColorPage >                 mpPageColor;
    ::boost::scoped_ptr< PropertyPanelPopuplWindow >        mpFloatWinStyle;
    ::boost::scoped_ptr< SvxLineStylePage >                 mpPageStyle; 
    ::boost::scoped_ptr< PropertyPanelPopuplWindow >        mpFloatWinWidth;
    ::boost::scoped_ptr< SvxLineWidthPage >                 mpPageWidth;

    // images from ressource
    Image                                                   maIMGColor;
    Image                                                   maIMGNone;

    // multi-images
    ::boost::scoped_ptr< Image >                            mpIMGStyleIcon;
    ::boost::scoped_ptr< Image >                            mpIMGWidthIcon;
    ::boost::scoped_ptr< Image >                            mpIMGStyleIconH;	//high contrast
    ::boost::scoped_ptr< Image >                            mpIMGWidthIconH;	//high contrast

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

    // constructor/destuctor
    LinePropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~LinePropertyPanel(void);

    void SetStyleIcon();
    void SetColor(Color aColor);
    void SetLineStyleItem(XLineStyleItem* pStyle);
    void SetLineDashItem(XLineDashItem* pDash);
    void SetWidth(long nWidth);
    void SetWidthIcon(int n);
    void SetWidthIcon();

    void ImpEnsureFloatWinColorAndPageColor();
    SvxLineColorPage* GetColorPage();
    PropertyPanelPopuplWindow* GetColorFloatWin();

    void ImpEnsureFloatWinStyleAndPageStyle();
    SvxLineStylePage* GetStylePage();
    PropertyPanelPopuplWindow* GetStyleFloatWin();

    void ImpEnsureFloatWinWidthAndPageWidth();
    SvxLineWidthPage* GetWidthPage();
    PropertyPanelPopuplWindow* GetWidthFloatWin();
};

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////

#endif // SVX_PROPERTYPANEL_LINEPAGE_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
