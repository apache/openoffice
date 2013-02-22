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

#ifndef SVX_PROPERTYPANEL_AREAPAGE_HXX
#define SVX_PROPERTYPANEL_AREAPAGE_HXX

#include <vcl/ctrl.hxx>
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <sfx2/sidebar/ControllerItem.hxx>
#include <svx/xgrad.hxx>
#include <svx/itemwin.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xflclit.hxx>
#include <svx/xflgrit.hxx>
#include <svx/xflhtit.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/drawitem.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <svl/intitem.hxx>
#include <svx/tbxcolorupdate.hxx>
#include <svx/sidebar/PropertyPanelTools.hxx>
#include <com/sun/star/ui/XUIElement.hpp>
#include <boost/scoped_ptr.hpp>

//////////////////////////////////////////////////////////////////////////////
// pedefines
class XFillFloatTransparenceItem;
namespace svx { class ToolboxButtonColorUpdater; }

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace svx { namespace sidebar {

class SvxColorPage;
class SvxAreaTrGrPage;

class AreaPropertyPanel
:   public Control,
    public ::sfx2::sidebar::SidebarPanelBase::ContextChangeReceiverInterface,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
private:
    friend class SvxColorPage;
    friend class SvxAreaTrGrPage;

public:
    static AreaPropertyPanel* Create(
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
    sal_uInt16                                          meLastXFS;
    Color                                               maLastColor;

    sal_uInt16                                          mnLastPosGradient;
    sal_uInt16                                          mnLastPosHatch;
    sal_uInt16                                          mnLastPosBitmap;
    sal_uInt16                                          mnLastTransSolid;

    XGradient                                           maGradientLinear;
    XGradient                                           maGradientAxial;
    XGradient                                           maGradientRadial;
    XGradient                                           maGradientElliptical;
    XGradient                                           maGradientSquare;
    XGradient                                           maGradientRect;

    //ui controls
    ::boost::scoped_ptr< FixedText >                    mpColorTextFT;
    ::boost::scoped_ptr< SvxFillTypeBox >               mpLbFillType;
    ::boost::scoped_ptr< SvxFillAttrBox >               mpLbFillAttr;
    ::boost::scoped_ptr< Window >                       mpToolBoxColorBackground;
    ::boost::scoped_ptr< ToolBox >                      mpToolBoxColor; // for new color picker
    ::boost::scoped_ptr< FixedText >                    mpTrspTextFT;
    ::boost::scoped_ptr< ListBox >                      mpLBTransType;
    ::boost::scoped_ptr< MetricField >                  mpMTRTransparent;
    ::boost::scoped_ptr< Window >                       mpBTNGradientBackground;
    ::boost::scoped_ptr< ToolBox >                      mpBTNGradient;

    ::boost::scoped_ptr< ::svx::ToolboxButtonColorUpdater > mpColorUpdater;

    ::boost::scoped_ptr< XFillStyleItem >               mpStyleItem;
    ::boost::scoped_ptr< XFillColorItem >               mpColorItem;
    ::boost::scoped_ptr< XFillGradientItem >            mpFillGradientItem;
    ::boost::scoped_ptr< XFillHatchItem >               mpHatchItem;
    ::boost::scoped_ptr< XFillBitmapItem >              mpBitmapItem;
    ::boost::scoped_ptr< SvxColorTableItem >            mpColorTableItem;
    ::boost::scoped_ptr< SvxGradientListItem >          mpGradientListItem;
    ::boost::scoped_ptr< SvxHatchListItem >             mpHatchListItem;
    ::boost::scoped_ptr< SvxBitmapListItem >            mpBitmapListItem;

    ::sfx2::sidebar::ControllerItem                     maStyleControl;
    ::sfx2::sidebar::ControllerItem                     maColorControl;
    ::sfx2::sidebar::ControllerItem                     maGradientControl;
    ::sfx2::sidebar::ControllerItem                     maHatchControl;
    ::sfx2::sidebar::ControllerItem                     maBitmapControl;
    ::sfx2::sidebar::ControllerItem                     maColorTableControl;
    ::sfx2::sidebar::ControllerItem                     maGradientListControl;
    ::sfx2::sidebar::ControllerItem                     maHatchListControl;
    ::sfx2::sidebar::ControllerItem                     maBitmapListControl;
    ::sfx2::sidebar::ControllerItem                     maTransTypeController;
    ::sfx2::sidebar::ControllerItem                     maTransController;
    ::sfx2::sidebar::ControllerItem                     maTransGradientController;

    Image                                               maImgAxial;
    Image                                               maImgElli;
    Image                                               maImgQuad;
    Image                                               maImgRadial;
    Image                                               maImgSquare;
    Image                                               maImgLinear;
    Image                                               maImgColor;

    //for high contract
    Image                                               maImgAxialH;
    Image                                               maImgElliH;
    Image                                               maImgQuadH;
    Image                                               maImgRadialH;
    Image                                               maImgSquareH;
    Image                                               maImgLinearH;
    Image                                               maImgColorH;

    String                                              msHelpFillType;
    String                                              msHelpFillAttr;

    ::boost::scoped_ptr< PropertyPanelPopuplWindow >    mpTrGrFloatWin;
    ::boost::scoped_ptr< SvxAreaTrGrPage >              mpTrGrPage;

    ::boost::scoped_ptr< PropertyPanelPopuplWindow >    mpFloatWinColor;
    ::boost::scoped_ptr< SvxColorPage >                 mpPageColor; 

    ::boost::scoped_ptr< XFillFloatTransparenceItem >   mpGradientItem;
    ::boost::scoped_ptr< SfxUInt16Item >                mpTransTypeItem;

    cssu::Reference<css::frame::XFrame>                 mxFrame;
    ::sfx2::sidebar::EnumContext                        maContext;
    SfxBindings*                                        mpBindings;

    /// bitfield
    bool                                                mbTBShow : 1;
    bool                                                mbColorAvail : 1;

    DECL_LINK(SelectFillTypeHdl, ListBox* );
    DECL_LINK(SelectFillAttrHdl, ListBox* );
    DECL_LINK(ToolBoxColorDropHdl, ToolBox *); //for new color picker 
    DECL_LINK(ChangeTrgrTypeHdl_Impl, void*);
    DECL_LINK(ModifyTransparentHdl_Impl, void*);
    DECL_LINK( ClickTrGrHdl_Impl, ToolBox* );
    DECL_LINK( ImplPopupModeEndHdl, FloatingWindow* );

    // for transparency gradient
    void ImpEnsureTrGrFloatWinAndTrGrPage();
    SvxAreaTrGrPage* GetTrGrPage();
    PropertyPanelPopuplWindow* GetTrGrFloatWin();

    // for color picker 
    void ImpEnsureFloatWinColorAndPageColor();
    SvxColorPage* GetColorPage();
    PropertyPanelPopuplWindow* GetColorFloatWin();

    // constructor/destuctor
    AreaPropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~AreaPropertyPanel(void);

    void SetupIcons(void);
    void Initialize();
    void Update();
};

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////

#endif // SVX_PROPERTYPANEL_AREAPAGE_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
