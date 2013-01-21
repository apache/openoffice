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

#ifndef SFX_SIDEBAR_THEME_HXX
#define SFX_SIDEBAR_THEME_HXX

#include "sfx2/dllapi.h"

#include <tools/color.hxx>
#include <vcl/image.hxx>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

#include <hash_map>
#include <map>

class SvBorder;

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;


namespace sfx2 { namespace sidebar {


namespace
{
    typedef ::cppu::WeakComponentImplHelper1 <
        css::beans::XPropertySet
        > ThemeInterfaceBase;
}

class Paint;

/** Simple collection of colors, gradients, fonts that define the
    look of the sidebar and its controls.
*/
class SFX2_DLLPUBLIC Theme
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public ThemeInterfaceBase
{
public:
    enum ThemeItem
    {
        __Pre_Image,
        __AnyItem = __Pre_Image,
        
        Image_Grip,
        Image_Expand,
        Image_Collapse,
        Image_Menu,

        __Image_Color,
        
        Color_DeckTitleFont,
        Color_PanelTitleFont,
        Color_TabMenuSeparator,
        Color_TabItemBorder,

        __Color_Paint,
        
        Paint_DeckBackground,
        Paint_DeckTitleBarBackground,
        Paint_PanelBackground,
        Paint_PanelTitleBarBackground,
        Paint_TabBarBackground,
        Paint_TabItemBackground,
        Paint_HorizontalBorder,
        Paint_VerticalBorder,

        __Paint_Int,
        
        Int_DeckTitleBarHeight,
        Int_DeckBorderSize,
        Int_DeckSeparatorHeight,
        Int_PanelTitleBarHeight,
        Int_TabMenuPadding,
        Int_TabMenuSeparatorPadding,
        Int_TabItemWidth,
        Int_TabItemHeight,
        Int_DeckLeftPadding,
        Int_DeckTopPadding,
        Int_DeckRightPadding,
        Int_DeckBottomPadding,
        Int_TabBarLeftPadding,
        Int_TabBarTopPadding,
        Int_TabBarRightPadding,
        Int_TabBarBottomPadding,

        __Int_Bool,

        Bool_UseSymphonyIcons,
        
        __Post_Bool
    };
    
    static Image GetImage (const ThemeItem eItem);
    static Color GetColor (const ThemeItem eItem);
    static const Paint& GetPaint (const ThemeItem eItem);
    static sal_Int32 GetInteger (const ThemeItem eItem);
    static bool GetBoolean (const ThemeItem eItem);

    static bool IsHighContrastMode (void);

    static void HandleDataChange (void);

    Theme (void);
    virtual ~Theme (void);

    virtual void SAL_CALL disposing (void);

    static cssu::Reference<css::beans::XPropertySet> GetPropertySet (void);
    
    // beans::XPropertySet
    cssu::Reference<css::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo (void)
        throw(cssu::RuntimeException);
    void SAL_CALL setPropertyValue (
        const ::rtl::OUString& rsPropertyName,
        const cssu::Any& rValue)
        throw(cssu::RuntimeException);
    cssu::Any SAL_CALL getPropertyValue (
        const ::rtl::OUString& rsPropertyName)
        throw(css::beans::UnknownPropertyException,
            css::lang::WrappedTargetException,
            cssu::RuntimeException);
    void SAL_CALL addPropertyChangeListener(
        const ::rtl::OUString& rsPropertyName,
        const cssu::Reference<css::beans::XPropertyChangeListener>& rxListener)
        throw(css::beans::UnknownPropertyException,
            css::lang::WrappedTargetException,
            cssu::RuntimeException);
    void SAL_CALL removePropertyChangeListener(
        const ::rtl::OUString& rsPropertyName,
        const cssu::Reference<css::beans::XPropertyChangeListener>& rxListener)
        throw(css::beans::UnknownPropertyException,
            css::lang::WrappedTargetException,
            cssu::RuntimeException);
    void SAL_CALL addVetoableChangeListener(
        const ::rtl::OUString& rsPropertyName,
        const cssu::Reference<css::beans::XVetoableChangeListener>& rxListener)
        throw(css::beans::UnknownPropertyException,
            css::lang::WrappedTargetException,
            cssu::RuntimeException);
    void SAL_CALL removeVetoableChangeListener(
        const ::rtl::OUString& rsPropertyName,
        const cssu::Reference<css::beans::XVetoableChangeListener>& rxListener)
        throw(css::beans::UnknownPropertyException,
            css::lang::WrappedTargetException,
            cssu::RuntimeException);

private:
    static ::rtl::Reference<Theme> mpInstance;
    static Theme& GetCurrentTheme (void);

    ::std::vector<Image> maImages;
    ::std::vector<Color> maColors;
    ::std::vector<Paint> maPaints;
    ::std::vector<sal_Int32> maIntegers;
    ::std::vector<bool> maBooleans;
    bool mbIsHighContrastMode;
    
    typedef ::std::hash_map<rtl::OUString,ThemeItem, rtl::OUStringHash> PropertyNameToIdMap;
    PropertyNameToIdMap maPropertyNameToIdMap;
    typedef ::std::vector<rtl::OUString> PropertyIdToNameMap;
    PropertyIdToNameMap maPropertyIdToNameMap;
    typedef ::std::vector<cssu::Any> RawValueContainer;
    RawValueContainer maRawValues;
        
    typedef ::std::vector<cssu::Reference<css::beans::XPropertyChangeListener> > ChangeListenerContainer;
    typedef ::std::map<ThemeItem,ChangeListenerContainer> ChangeListeners;
    ChangeListeners maChangeListeners;
    typedef ::std::vector<cssu::Reference<css::beans::XVetoableChangeListener> > VetoableListenerContainer;
    typedef ::std::map<ThemeItem,VetoableListenerContainer> VetoableListeners;
    VetoableListeners maVetoableListeners;

    enum PropertyType
    {
        PT_Image,
        PT_Color,
        PT_Paint,
        PT_Integer,
        PT_Boolean,
        PT_Invalid
    };
    
    void SetupPropertyMaps (void);
    void InitializeTheme (void);
    static PropertyType GetPropertyType (const ThemeItem eItem);
    static sal_Int32 GetIndex (
        const ThemeItem eItem,
        const PropertyType eType);

    VetoableListenerContainer* GetVetoableListeners (
        const ThemeItem eItem,
        const bool bCreate);
    ChangeListenerContainer* GetChangeListeners (
        const ThemeItem eItem,
        const bool bCreate);
    bool DoVetoableListenersVeto (
        const VetoableListenerContainer* pListeners,
        const css::beans::PropertyChangeEvent& rEvent) const;
    void BroadcastPropertyChange (
        const ChangeListenerContainer* pListeners,
        const css::beans::PropertyChangeEvent& rEvent) const;
    void ProcessNewValue (
        const cssu::Any& rValue,
        const ThemeItem eItem,
        const PropertyType eType);
};



} } // end of namespace sfx2::sidebar

#endif
