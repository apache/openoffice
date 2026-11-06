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

#ifndef SC_PROPERTYPANEL_NUMFORMAT_HXX
#define SC_PROPERTYPANEL_NUMFORMAT_HXX

#include <sfx2/sidebar/ControllerItem.hxx>
#include <sfx2/sidebar/IContextChangeReceiver.hxx>
#include <memory>

class FixedText;
class ListBox;
class NumericField;

namespace sc { namespace sidebar {

class NumberFormatPropertyPanel
:   public Control,
    public ::sfx2::sidebar::IContextChangeReceiver,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
public:
public:
    static NumberFormatPropertyPanel* Create(
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
    ::std::unique_ptr< FixedText >        mpFtCategory;
    ::std::unique_ptr< ListBox >          mpLbCategory;
    ::std::unique_ptr< Window >           mpTBCategoryBackground;
    ::std::unique_ptr< ToolBox >          mpTBCategory;
    ::std::unique_ptr< FixedText >        mpFtDecimals;
    ::std::unique_ptr< NumericField >     mpEdDecimals;
    ::std::unique_ptr< FixedText >        mpFtLeadZeroes;
    ::std::unique_ptr< NumericField >     mpEdLeadZeroes;
    ::std::unique_ptr< CheckBox >         mpBtnNegRed;
    ::std::unique_ptr< CheckBox >         mpBtnThousand;

    ::sfx2::sidebar::ControllerItem         maNumFormatControl;
    ::sfx2::sidebar::ControllerItem         maFormatControl;

    sal_uInt16                              mnCategorySelected;

    cssu::Reference<css::frame::XFrame>     mxFrame;
    ::sfx2::sidebar::EnumContext            maContext;
    SfxBindings*                            mpBindings;

    DECL_LINK(NumFormatHdl, ToolBox*);
    DECL_LINK(NumFormatSelectHdl, ListBox*);
    DECL_LINK(NumFormatValueHdl, void*);

    // constructor/destuctor
    NumberFormatPropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~NumberFormatPropertyPanel();

    void Initialize();
};

} } // end of namespace ::sc::sidebar

#endif

// eof
