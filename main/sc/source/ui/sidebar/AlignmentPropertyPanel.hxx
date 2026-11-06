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

#ifndef SC_PROPERTYPANEL_ALIGNMENT_HXX
#define SC_PROPERTYPANEL_ALIGNMENT_HXX

#include <sfx2/sidebar/ControllerItem.hxx>
#include <sfx2/sidebar/IContextChangeReceiver.hxx>
#include <vcl/fixed.hxx>
#include <memory>
#include <editeng/svxenum.hxx>

class ToolBox;
class MetricField;
class MetricBox;
class CheckBox;
namespace svx { namespace sidebar { class SidebarDialControl; }}

namespace sc { namespace sidebar {

class AlignmentPropertyPanel
:   public Control,
    public ::sfx2::sidebar::IContextChangeReceiver,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
public:
    static AlignmentPropertyPanel* Create(
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
        const SfxPoolItem* pState,
        const bool bIsEnabled);

    SfxBindings* GetBindings();

    virtual void Resize (void);

private:
    //ui controls
	::std::unique_ptr<Window> 			    mpTBHorizontalBackground;
    ::std::unique_ptr< ToolBox >              mpTBHorizontal;
	::std::unique_ptr<Window> 			    mpTBVerticalBackground;
    ::std::unique_ptr< ToolBox >              mpTBVertical;
    ::std::unique_ptr< FixedText >            mpFTLeftIndent;
    ::std::unique_ptr< MetricField >          mpMFLeftIndent;
    ::std::unique_ptr< CheckBox >             mpCBXWrapText;
    ::std::unique_ptr< CheckBox >             mpCBXMergeCell;
    ::std::unique_ptr< FixedText >            mpFtRotate;
    ::std::unique_ptr< svx::sidebar::SidebarDialControl > mpCtrlDial;
    ::std::unique_ptr< MetricBox >            mpMtrAngle;
    ::std::unique_ptr< CheckBox >             mpCbStacked;

    ::sfx2::sidebar::ControllerItem             maAlignHorControl;
    ::sfx2::sidebar::ControllerItem             maAlignVerControl;
    ::sfx2::sidebar::ControllerItem             maLeftIndentControl;
    ::sfx2::sidebar::ControllerItem             maMergeCellControl;
    ::sfx2::sidebar::ControllerItem             maWrapTextControl;
    ::sfx2::sidebar::ControllerItem             maAngleControl;
    ::sfx2::sidebar::ControllerItem             maStackControl;

    Image                                       maIMGAlignLeft;
    Image                                       maIMGAlignCenter;
    Image                                       maIMGAlignRight;
    Image                                       maIMGAlignJust;
    Image                                       maIMGAlignTop;
    Image                                       maIMGAlignCenterV;
    Image                                       maIMGAlignBottom;

    SvxCellHorJustify                           meHorAlignState;
    SvxCellVerJustify                           meVerAlignState;

    /// bitfield
    bool                                        mbMultiDisable : 1;

    cssu::Reference<css::frame::XFrame>         mxFrame;
    ::sfx2::sidebar::EnumContext                maContext;
    SfxBindings*                                mpBindings;

    DECL_LINK( TbxHorAlignSelectHdl, ToolBox* );
    DECL_LINK( TbxVerAlignSelectHdl, ToolBox* );
    DECL_LINK( MFLeftIndentMdyHdl, void * );
    DECL_LINK( CBOXMergnCellClkHdl, void * );
    DECL_LINK( CBOXWrapTextClkHdl, void * );
    DECL_LINK( AngleModifiedHdl, void * );
    DECL_LINK( RotationHdl, void * );
    DECL_LINK( ClickStackHdl, void * );

    // constructor/destuctor
    AlignmentPropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~AlignmentPropertyPanel();

    void Initialize();
    void UpdateHorAlign();
    void UpdateVerAlign();
    void FormatDegrees(double& dTmp);
};

} } // end of namespace ::sc::sidebar

#endif
// eof
