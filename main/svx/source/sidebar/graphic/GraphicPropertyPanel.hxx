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



#ifndef SVX_PROPERTYPANEL_GRAPHICPAGE_HXX
#define SVX_PROPERTYPANEL_GRAPHICPAGE_HXX

#include <vcl/ctrl.hxx>
#include <sfx2/sidebar/SidebarPanelBase.hxx>
#include <sfx2/sidebar/ControllerItem.hxx>
#include <sfx2/sidebar/GridLayouter.hxx>
#include <vcl/fixed.hxx>
#include <memory>

class FixedText;
class MetricField;
class ListBox;
class FloatingWindow;


namespace svx { namespace sidebar {

class GraphicPropertyPanel
:   public Control,
    public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
{
public:
    static GraphicPropertyPanel* Create(
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

    virtual void Resize (void);

private:
    //ui controls
    ::std::unique_ptr< FixedText >                    mpFtBrightness;
    ::std::unique_ptr< MetricField >                  mpMtrBrightness;
    ::std::unique_ptr< FixedText >                    mpFtContrast;
    ::std::unique_ptr< MetricField >                  mpMtrContrast;
    ::std::unique_ptr< FixedText >                    mpFtColorMode;
    ::std::unique_ptr< ListBox >                      mpLBColorMode;
    ::std::unique_ptr< FixedText >                    mpFtTrans;
    ::std::unique_ptr< MetricField >                  mpMtrTrans;
    ::std::unique_ptr< MetricField >                  mpMtrRed;
    ::std::unique_ptr< MetricField >                  mpMtrGreen;
    ::std::unique_ptr< MetricField >                  mpMtrBlue;
    ::std::unique_ptr< MetricField >                  mpMtrGamma;

    ::sfx2::sidebar::ControllerItem                     maBrightControl;
    ::sfx2::sidebar::ControllerItem                     maContrastControl;
    ::sfx2::sidebar::ControllerItem                     maTransparenceControl;
    ::sfx2::sidebar::ControllerItem                     maRedControl;
    ::sfx2::sidebar::ControllerItem                     maGreenControl;
    ::sfx2::sidebar::ControllerItem                     maBlueControl;
    ::sfx2::sidebar::ControllerItem                     maGammaControl;
    ::sfx2::sidebar::ControllerItem                     maModeControl;

    Image                                               maImgNormal;
    Image                                               maImgBW;
    Image                                               maImgGray;
    Image                                               maImgWater;

    FixedImage                                          maImgRed;
    FixedImage                                          maImgGreen;
    FixedImage                                          maImgBlue;
    FixedImage                                          maImgGamma;

    String                                              msNormal;
    String                                              msBW;
    String                                              msGray;
    String                                              msWater;

    cssu::Reference<css::frame::XFrame>                 mxFrame;
    SfxBindings*                                        mpBindings;
    ::sfx2::sidebar::GridLayouter maLayouter;

    DECL_LINK( ModifyBrightnessHdl, void * );
    DECL_LINK( ModifyContrastHdl, void * );
    DECL_LINK( ModifyTransHdl, void * );
    DECL_LINK( ClickColorModeHdl, ToolBox * );
    DECL_LINK( ImplPopupModeEndHdl, FloatingWindow* );
    DECL_LINK( RedHdl, void*);
    DECL_LINK( GreenHdl, void*);
    DECL_LINK( BlueHdl, void*);
    DECL_LINK( GammaHdl, void*);

    // constructor/destructor
    GraphicPropertyPanel(
        Window* pParent,
        const cssu::Reference<css::frame::XFrame>& rxFrame,
        SfxBindings* pBindings);
    virtual ~GraphicPropertyPanel();

    void SetupIcons(void);
    void Initialize();
};


} } // end of namespace ::svx::sidebar

#endif

// eof
