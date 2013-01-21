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

#include "precompiled_sfx2.hxx"

#include "sfx2/sidebar/Theme.hxx"
#include "Paint.hxx"
#include "SidebarResource.hxx"
#include "Tools.hxx"

#include <tools/svborder.hxx>
#include <tools/rc.hxx>
#include <vcl/svapp.hxx>

using namespace css;
using namespace cssu;


namespace sfx2 { namespace sidebar {

::rtl::Reference<Theme> Theme::mpInstance;




Theme& Theme::GetCurrentTheme (void)
{
    if ( ! mpInstance.is())
    {
        mpInstance.set(new Theme());
        mpInstance->InitializeTheme();
    }
    return *mpInstance;
}




Theme::Theme (void)
    : ThemeInterfaceBase(m_aMutex),
      maImages(),
      maColors(),
      maPaints(),
      maIntegers(),
      maBooleans(),
      mbIsHighContrastMode(Application::GetSettings().GetStyleSettings().GetHighContrastMode()),
      maPropertyNameToIdMap(),
      maPropertyIdToNameMap(),
      maRawValues(),
      maChangeListeners(),
      maVetoableListeners()

{
    SetupPropertyMaps();
}




Theme::~Theme (void)
{
}




Image Theme::GetImage (const ThemeItem eItem)
{
    const PropertyType eType (GetPropertyType(eItem));
    OSL_ASSERT(eType==PT_Image);
    const sal_Int32 nIndex (GetIndex(eItem, eType));
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.maImages[nIndex];
}




Color Theme::GetColor (const ThemeItem eItem)
{
    const PropertyType eType (GetPropertyType(eItem));
    OSL_ASSERT(eType==PT_Color);
    const sal_Int32 nIndex (GetIndex(eItem, eType));
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.maColors[nIndex];
}




const Paint& Theme::GetPaint (const ThemeItem eItem)
{
    const PropertyType eType (GetPropertyType(eItem));
    OSL_ASSERT(eType==PT_Paint);
    const sal_Int32 nIndex (GetIndex(eItem, eType));
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.maPaints[nIndex];
}




sal_Int32 Theme::GetInteger (const ThemeItem eItem)
{
    const PropertyType eType (GetPropertyType(eItem));
    OSL_ASSERT(eType==PT_Integer);
    const sal_Int32 nIndex (GetIndex(eItem, eType));
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.maIntegers[nIndex];
}




bool Theme::GetBoolean (const ThemeItem eItem)
{
    const PropertyType eType (GetPropertyType(eItem));
    OSL_ASSERT(eType==PT_Boolean);
    const sal_Int32 nIndex (GetIndex(eItem, eType));
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.maBooleans[nIndex];
}




bool Theme::IsHighContrastMode (void)
{
    const Theme& rTheme (GetCurrentTheme());
    return rTheme.mbIsHighContrastMode;
}




void Theme::HandleDataChange (void)
{
    GetCurrentTheme().mbIsHighContrastMode = Application::GetSettings().GetStyleSettings().GetHighContrastMode();
    GetCurrentTheme().InitializeTheme();
}




void Theme::InitializeTheme (void)
{
    SidebarResource aLocalResource;

    try
    {
        setPropertyValue(
            maPropertyIdToNameMap[Paint_DeckBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 : 0xf0f0f0)));
        setPropertyValue(
            maPropertyIdToNameMap[Paint_DeckTitleBarBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 :0xf0f0f0)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckLeftPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckTopPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckRightPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckBottomPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckBorderSize],
            Any(sal_Int32(1)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckSeparatorHeight],
            Any(sal_Int32(1)));
        setPropertyValue(
            maPropertyIdToNameMap[Color_DeckTitleFont],
            Any(sal_Int32(mbIsHighContrastMode ? 0x00ff00 : 0x262626)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_DeckTitleBarHeight],
            Any(sal_Int32(26)));

        setPropertyValue(
            maPropertyIdToNameMap[Paint_PanelBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 : 0xffffff)));
        setPropertyValue(
            maPropertyIdToNameMap[Paint_PanelTitleBarBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 : 0xb2b2b2)));
        setPropertyValue(
            maPropertyIdToNameMap[Color_PanelTitleFont],
            Any(sal_Int32(mbIsHighContrastMode ? 0x00ff00 : 0x262626)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_PanelTitleBarHeight],
            Any(sal_Int32(26)));

        setPropertyValue(
            maPropertyIdToNameMap[Paint_TabBarBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 : 0xf0f0f0)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabBarLeftPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabBarTopPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabBarRightPadding],
            Any(sal_Int32(2)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabBarBottomPadding],
            Any(sal_Int32(2)));

        setPropertyValue(
            maPropertyIdToNameMap[Int_TabMenuPadding],
            Any(sal_Int32(6)));
        setPropertyValue(
            maPropertyIdToNameMap[Color_TabMenuSeparator],
            Any(sal_Int32(mbIsHighContrastMode ? 0x00ff00 : 0xbfbfbf)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabMenuSeparatorPadding],
            Any(sal_Int32(7)));
    
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabItemWidth],
            Any(sal_Int32(32)));
        setPropertyValue(
            maPropertyIdToNameMap[Int_TabItemHeight],
            Any(sal_Int32(32)));
        setPropertyValue(
            maPropertyIdToNameMap[Color_TabItemBorder],
            Any(sal_Int32(mbIsHighContrastMode ? 0x00ff00 : 0xbfbfbf)));
        setPropertyValue(
            maPropertyIdToNameMap[Paint_TabItemBackground],
            Any(sal_Int32(mbIsHighContrastMode ? 0x000000 : 0xffffff)));

        setPropertyValue(
            maPropertyIdToNameMap[Paint_HorizontalBorder],
            Any(sal_Int32(mbIsHighContrastMode ? 0xff00ff000 : 0xd9d9d9)));
        setPropertyValue(
            maPropertyIdToNameMap[Paint_VerticalBorder],
            Any(sal_Int32(mbIsHighContrastMode ? 0xff00ff000 : 0xd9d9d9)));

        setPropertyValue(
            maPropertyIdToNameMap[Image_Grip],
            Any(
                mbIsHighContrastMode
                    ? A2S("private:graphicrepository/sfx2/res/grip_hc.png")
                    : A2S("private:graphicrepository/sfx2/res/grip.png")));
        setPropertyValue(
            maPropertyIdToNameMap[Image_Expand],
            Any(
                mbIsHighContrastMode
                    ? A2S("private:graphicrepository/res/plus_sch.png")
                    : A2S("private:graphicrepository/res/plus.png")));
        setPropertyValue(
            maPropertyIdToNameMap[Image_Collapse],
            Any(
                mbIsHighContrastMode
                    ? A2S("private:graphicrepository/res/minus_sch.png")
                    : A2S("private:graphicrepository/res/minus.png")));
        setPropertyValue(
            maPropertyIdToNameMap[Image_Menu],
            Any(
                mbIsHighContrastMode
                    ? A2S("private:graphicrepository/sfx2/res/menu_ch.png")
                    : A2S("private:graphicrepository/sfx2/res/menu.png")));

        setPropertyValue(
            maPropertyIdToNameMap[Bool_UseSymphonyIcons],
            Any(true));
    }
    catch(beans::UnknownPropertyException&)
    {
        OSL_ASSERT(false);
    }
}




void SAL_CALL Theme::disposing (void)
{
    ChangeListeners aListeners;
    maChangeListeners.swap(aListeners);

    const lang::EventObject aEvent (static_cast<XWeak*>(this));
    
    for (ChangeListeners::const_iterator
             iContainer(maChangeListeners.begin()),
             iContainerEnd(maChangeListeners.end());
         iContainerEnd!=iContainerEnd;
         ++iContainerEnd)
    {
        for (ChangeListenerContainer::const_iterator
                 iListener(iContainer->second.begin()),
                 iEnd(iContainer->second.end());
             iListener!=iEnd;
             ++iListener)
        {
            try
            {
                (*iListener)->disposing(aEvent);
            }
            catch(const Exception&)
            {
            }
        }
    }
}




Reference<beans::XPropertySet> Theme::GetPropertySet (void)
{
    return Reference<beans::XPropertySet>(static_cast<XWeak*>(&GetCurrentTheme()), UNO_QUERY);
}




Reference<beans::XPropertySetInfo> SAL_CALL Theme::getPropertySetInfo (void)
    throw(cssu::RuntimeException)
{
    return Reference<beans::XPropertySetInfo>();
}




void SAL_CALL Theme::setPropertyValue (
    const ::rtl::OUString& rsPropertyName,
    const cssu::Any& rValue)
    throw(cssu::RuntimeException)
{
    PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
    if (iId == maPropertyNameToIdMap.end())
        throw beans::UnknownPropertyException();

    const PropertyType eType (GetPropertyType(iId->second));
    if (eType == PT_Invalid)
        throw beans::UnknownPropertyException();

    const ThemeItem eItem (iId->second);
        
    if (rValue == maRawValues[eItem])
    {
        // Value is not different from the one in the property
        // set => nothing to do.
        return;
    }

    const Any aOldValue (maRawValues[eItem]);

    const beans::PropertyChangeEvent aEvent(
        static_cast<XWeak*>(this),
        rsPropertyName,
        sal_False,
        eItem,
        aOldValue,
        rValue);
    
    if (DoVetoableListenersVeto(GetVetoableListeners(__AnyItem, false), aEvent))
        return;
    if (DoVetoableListenersVeto(GetVetoableListeners(eItem, false), aEvent))
        return;
    
    maRawValues[eItem] = rValue;
    ProcessNewValue(rValue, eItem, eType);

    BroadcastPropertyChange(GetChangeListeners(__AnyItem, false), aEvent);
    BroadcastPropertyChange(GetChangeListeners(eItem, false), aEvent);
}




Any SAL_CALL Theme::getPropertyValue (
    const ::rtl::OUString& rsPropertyName)
    throw(css::beans::UnknownPropertyException,
        css::lang::WrappedTargetException,
        cssu::RuntimeException)
{
    PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
    if (iId == maPropertyNameToIdMap.end())
        throw beans::UnknownPropertyException();

    const PropertyType eType (GetPropertyType(iId->second));
    if (eType == PT_Invalid)
        throw beans::UnknownPropertyException();

    const ThemeItem eItem (iId->second);

    return maRawValues[eItem];
}




void SAL_CALL Theme::addPropertyChangeListener(
    const ::rtl::OUString& rsPropertyName,
    const cssu::Reference<css::beans::XPropertyChangeListener>& rxListener)
    throw(css::beans::UnknownPropertyException,
        css::lang::WrappedTargetException,
        cssu::RuntimeException)
{
    ThemeItem eItem (__AnyItem);
    if (rsPropertyName.getLength() > 0)
    {
        PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
        if (iId == maPropertyNameToIdMap.end())
            throw beans::UnknownPropertyException();

        const PropertyType eType (GetPropertyType(iId->second));
        if (eType == PT_Invalid)
            throw beans::UnknownPropertyException();

        eItem = iId->second;
    }
    ChangeListenerContainer* pListeners = GetChangeListeners(eItem, true);
    if (pListeners != NULL)
        pListeners->push_back(rxListener);
}




void SAL_CALL Theme::removePropertyChangeListener(
    const ::rtl::OUString& rsPropertyName,
    const cssu::Reference<css::beans::XPropertyChangeListener>& rxListener)
    throw(css::beans::UnknownPropertyException,
        css::lang::WrappedTargetException,
        cssu::RuntimeException)
{
    ThemeItem eItem (__AnyItem);
    if (rsPropertyName.getLength() > 0)
    {
        PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
        if (iId == maPropertyNameToIdMap.end())
            throw beans::UnknownPropertyException();

        const PropertyType eType (GetPropertyType(iId->second));
        if (eType == PT_Invalid)
            throw beans::UnknownPropertyException();

        eItem = iId->second;
    }
    ChangeListenerContainer* pContainer = GetChangeListeners(eItem, false);
    if (pContainer != NULL)
    {
        ChangeListenerContainer::iterator iListener (::std::find(pContainer->begin(), pContainer->end(), rxListener));
        if (iListener != pContainer->end())
        {
            pContainer->erase(iListener);

            // Remove the listener container when empty.
            if (pContainer->empty())
                maChangeListeners.erase(eItem);
        }
    }
}




void SAL_CALL Theme::addVetoableChangeListener(
    const ::rtl::OUString& rsPropertyName,
    const cssu::Reference<css::beans::XVetoableChangeListener>& rxListener)
    throw(css::beans::UnknownPropertyException,
        css::lang::WrappedTargetException,
        cssu::RuntimeException)
{
    ThemeItem eItem (__AnyItem);
    if (rsPropertyName.getLength() > 0)
    {
        PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
        if (iId == maPropertyNameToIdMap.end())
            throw beans::UnknownPropertyException();

        const PropertyType eType (GetPropertyType(iId->second));
        if (eType == PT_Invalid)
            throw beans::UnknownPropertyException();

        eItem = iId->second;
    }
    VetoableListenerContainer* pListeners = GetVetoableListeners(eItem, true);
    if (pListeners != NULL)
        pListeners->push_back(rxListener);
}




void SAL_CALL Theme::removeVetoableChangeListener(
    const ::rtl::OUString& rsPropertyName,
    const cssu::Reference<css::beans::XVetoableChangeListener>& rxListener)
    throw(css::beans::UnknownPropertyException,
        css::lang::WrappedTargetException,
        cssu::RuntimeException)
{
    ThemeItem eItem (__AnyItem);
    if (rsPropertyName.getLength() > 0)
    {
        PropertyNameToIdMap::const_iterator iId (maPropertyNameToIdMap.find(rsPropertyName));
        if (iId == maPropertyNameToIdMap.end())
            throw beans::UnknownPropertyException();

        const PropertyType eType (GetPropertyType(iId->second));
        if (eType == PT_Invalid)
            throw beans::UnknownPropertyException();

        eItem = iId->second;
    }
    VetoableListenerContainer* pContainer = GetVetoableListeners(eItem, false);
    if (pContainer != NULL)
    {
        VetoableListenerContainer::iterator iListener (::std::find(pContainer->begin(), pContainer->end(), rxListener));
        if (iListener != pContainer->end())
        {
            pContainer->erase(iListener);
            // Remove container when empty.
            if (pContainer->empty())
                maVetoableListeners.erase(eItem);
        }
    }
}




void Theme::SetupPropertyMaps (void)
{
    maPropertyIdToNameMap.resize(__Post_Bool);
    maImages.resize(__Image_Color - __Pre_Image - 1);
    maColors.resize(__Color_Paint - __Image_Color - 1);
    maPaints.resize(__Paint_Int - __Color_Paint - 1);
    maIntegers.resize(__Int_Bool - __Paint_Int - 1);
    maBooleans.resize(__Post_Bool - __Int_Bool - 1);
    
    #define AddEntry(e) maPropertyNameToIdMap[A2S(#e)]=e; maPropertyIdToNameMap[e]=A2S(#e)
    AddEntry(Image_Grip);
    AddEntry(Image_Expand);
    AddEntry(Image_Collapse);
    AddEntry(Image_Menu);

    AddEntry(Color_DeckTitleFont);
    AddEntry(Color_PanelTitleFont);
    AddEntry(Color_TabMenuSeparator);
    AddEntry(Color_TabItemBorder);

    AddEntry(Paint_DeckBackground);
    AddEntry(Paint_DeckTitleBarBackground);
    AddEntry(Paint_PanelBackground);
    AddEntry(Paint_PanelTitleBarBackground);
    AddEntry(Paint_TabBarBackground);
    AddEntry(Paint_TabItemBackground);
    AddEntry(Paint_HorizontalBorder);
    AddEntry(Paint_VerticalBorder);

    AddEntry(Int_DeckTitleBarHeight);
    AddEntry(Int_DeckBorderSize);
    AddEntry(Int_DeckSeparatorHeight);
    AddEntry(Int_PanelTitleBarHeight);
    AddEntry(Int_TabMenuPadding);
    AddEntry(Int_TabMenuSeparatorPadding);
    AddEntry(Int_TabItemWidth);
    AddEntry(Int_TabItemHeight);
    AddEntry(Int_DeckLeftPadding);
    AddEntry(Int_DeckTopPadding);
    AddEntry(Int_DeckRightPadding);
    AddEntry(Int_DeckBottomPadding);
    AddEntry(Int_TabBarLeftPadding);
    AddEntry(Int_TabBarTopPadding);
    AddEntry(Int_TabBarRightPadding);
    AddEntry(Int_TabBarBottomPadding);

    AddEntry(Bool_UseSymphonyIcons);
    #undef AddEntry

    maRawValues.resize(maPropertyIdToNameMap.size());
}




Theme::PropertyType Theme::GetPropertyType (const ThemeItem eItem)
{
    switch(eItem)
    {
        case Image_Grip:
        case Image_Expand:
        case Image_Collapse:
        case Image_Menu:
            return PT_Image;

        case Color_DeckTitleFont:
        case Color_PanelTitleFont:
        case Color_TabMenuSeparator:
        case Color_TabItemBorder:
            return PT_Color;

        case Paint_DeckBackground:
        case Paint_DeckTitleBarBackground:
        case Paint_PanelBackground:
        case Paint_PanelTitleBarBackground:
        case Paint_TabBarBackground:
        case Paint_TabItemBackground:
        case Paint_HorizontalBorder:
        case Paint_VerticalBorder:
            return PT_Paint;

        case Int_DeckTitleBarHeight:
        case Int_DeckBorderSize:
        case Int_DeckSeparatorHeight:
        case Int_PanelTitleBarHeight:
        case Int_TabMenuPadding:
        case Int_TabMenuSeparatorPadding:
        case Int_TabItemWidth:
        case Int_TabItemHeight:
        case Int_DeckLeftPadding:
        case Int_DeckTopPadding:
        case Int_DeckRightPadding:
        case Int_DeckBottomPadding:
        case Int_TabBarLeftPadding:
        case Int_TabBarTopPadding:
        case Int_TabBarRightPadding:
        case Int_TabBarBottomPadding:
            return PT_Integer;

        case Bool_UseSymphonyIcons:
            return PT_Boolean;

        default:
            return PT_Invalid;
    }
}




sal_Int32 Theme::GetIndex (const ThemeItem eItem, const PropertyType eType)
{
    switch(eType)
    {
        case PT_Image:
            return eItem - __Pre_Image-1;
        case PT_Color:
            return eItem - __Image_Color-1;
        case PT_Paint:
            return eItem - __Color_Paint-1;
        case PT_Integer:
            return eItem - __Paint_Int-1;
        case PT_Boolean:
            return eItem - __Int_Bool-1;

        default:
            OSL_ASSERT(false);
            return 0;
    }
}




Theme::VetoableListenerContainer* Theme::GetVetoableListeners (
    const ThemeItem eItem,
    const bool bCreate)
{
    VetoableListeners::iterator iContainer (maVetoableListeners.find(eItem));
    if (iContainer != maVetoableListeners.end())
        return &iContainer->second;
    else if (bCreate)
    {
        maVetoableListeners[eItem] = VetoableListenerContainer();
        return &maVetoableListeners[eItem];
    }
    else
        return NULL;
}




Theme::ChangeListenerContainer* Theme::GetChangeListeners (
    const ThemeItem eItem,
    const bool bCreate)
{
    ChangeListeners::iterator iContainer (maChangeListeners.find(eItem));
    if (iContainer != maChangeListeners.end())
        return &iContainer->second;
    else if (bCreate)
    {
        maChangeListeners[eItem] = ChangeListenerContainer();
        return &maChangeListeners[eItem];
    }
    else
        return NULL;
}




bool Theme::DoVetoableListenersVeto (
    const VetoableListenerContainer* pListeners,
    const beans::PropertyChangeEvent& rEvent) const
{
    if (pListeners == NULL)
        return false;

    VetoableListenerContainer aListeners (*pListeners);
    try
    {
        for (VetoableListenerContainer::const_iterator
                 iListener(aListeners.begin()),
                 iEnd(aListeners.end());
             iListener!=iEnd;
             ++iListener)
        {
            (*iListener)->vetoableChange(rEvent);
        }
    }
    catch(const beans::PropertyVetoException&)
    {
        return true;
    }
    catch(const Exception&)
    {
        // Ignore any other errors (such as disposed listeners).
    }
    return false;
}




void Theme::BroadcastPropertyChange (
    const ChangeListenerContainer* pListeners,
    const beans::PropertyChangeEvent& rEvent) const
{
    if (pListeners == NULL)
        return;

    const ChangeListenerContainer aListeners (*pListeners);
    try
    {
        for (ChangeListenerContainer::const_iterator
                 iListener(aListeners.begin()),
                 iEnd(aListeners.end());
             iListener!=iEnd;
             ++iListener)
        {
            (*iListener)->propertyChange(rEvent);
        }
    }
    catch(const Exception&)
    {
        // Ignore any errors (such as disposed listeners).
    }
}




void Theme::ProcessNewValue (
    const Any& rValue,
    const ThemeItem eItem,
    const PropertyType eType)
{
    const sal_Int32 nIndex (GetIndex (eItem, eType));
    switch (eType)
    {
        case PT_Image:
        {
            ::rtl::OUString sURL;
            if (rValue >>= sURL)
            {
                maImages[nIndex] = Tools::GetImage(sURL, NULL);
            }
            break;
        }
        case PT_Color:
        {
            sal_Int32 nColorValue;
            if (rValue >>= nColorValue)
            {
                maColors[nIndex] = Color(nColorValue);
            }
            break;
        }
        case PT_Paint:
        {
            sal_Int32 nColorValue;
            if (rValue >>= nColorValue)
            {
                maPaints[nIndex] = Paint(Color(nColorValue));
            }
            break;
        }
        case PT_Integer:
        {
            sal_Int32 nValue;
            if (rValue >>= nValue)
            {
                maIntegers[nIndex] = nValue;
            }
            break;
        }
        case PT_Boolean:
        {
            sal_Bool nValue;
            if (rValue >>= nValue)
            {
                maBooleans[nIndex] = (nValue==sal_True);
            }
            break;
        }
        case PT_Invalid:
            OSL_ASSERT(eType != PT_Invalid);
            throw RuntimeException();
    }
}




} } // end of namespace sfx2::sidebar
