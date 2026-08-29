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



#ifndef TOOLKIT_STYLESETTINGS_HXX
#define TOOLKIT_STYLESETTINGS_HXX

/** === begin UNO includes === **/
#include <com/sun/star/awt/XStyleSettings.hpp>
/** === end UNO includes === **/

#include <cppuhelper/implbase1.hxx>

#include <boost/scoped_ptr.hpp>

namespace vos
{
    class IMutex;
}
namespace osl
{
    class Mutex;
}

class VCLXWindow;

//......................................................................................................................
namespace toolkit
{
//......................................................................................................................

	//==================================================================================================================
	//= WindowStyleSettings
	//==================================================================================================================
    struct WindowStyleSettings_Data;
    typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::awt::XStyleSettings
                                    >   WindowStyleSettings_Base;
    class WindowStyleSettings : public WindowStyleSettings_Base
	{
    public:
        WindowStyleSettings( ::vos::IMutex& i_rWindowMutex, ::osl::Mutex& i_rListenerMutex, VCLXWindow& i_rOwningWindow );
        ~WindowStyleSettings();

        void dispose();

        // XStyleSettings
        virtual ::sal_Int32 SAL_CALL getActiveBorderColor();
        virtual void SAL_CALL setActiveBorderColor( ::sal_Int32 _activebordercolor );
        virtual ::sal_Int32 SAL_CALL getActiveColor();
        virtual void SAL_CALL setActiveColor( ::sal_Int32 _activecolor );
        virtual ::sal_Int32 SAL_CALL getActiveTabColor();
        virtual void SAL_CALL setActiveTabColor( ::sal_Int32 _activetabcolor );
        virtual ::sal_Int32 SAL_CALL getActiveTextColor();
        virtual void SAL_CALL setActiveTextColor( ::sal_Int32 _activetextcolor );
        virtual ::sal_Int32 SAL_CALL getButtonRolloverTextColor();
        virtual void SAL_CALL setButtonRolloverTextColor( ::sal_Int32 _buttonrollovertextcolor );
        virtual ::sal_Int32 SAL_CALL getButtonTextColor();
        virtual void SAL_CALL setButtonTextColor( ::sal_Int32 _buttontextcolor );
        virtual ::sal_Int32 SAL_CALL getCheckedColor();
        virtual void SAL_CALL setCheckedColor( ::sal_Int32 _checkedcolor );
        virtual ::sal_Int32 SAL_CALL getDarkShadowColor();
        virtual void SAL_CALL setDarkShadowColor( ::sal_Int32 _darkshadowcolor );
        virtual ::sal_Int32 SAL_CALL getDeactiveBorderColor();
        virtual void SAL_CALL setDeactiveBorderColor( ::sal_Int32 _deactivebordercolor );
        virtual ::sal_Int32 SAL_CALL getDeactiveColor();
        virtual void SAL_CALL setDeactiveColor( ::sal_Int32 _deactivecolor );
        virtual ::sal_Int32 SAL_CALL getDeactiveTextColor();
        virtual void SAL_CALL setDeactiveTextColor( ::sal_Int32 _deactivetextcolor );
        virtual ::sal_Int32 SAL_CALL getDialogColor();
        virtual void SAL_CALL setDialogColor( ::sal_Int32 _dialogcolor );
        virtual ::sal_Int32 SAL_CALL getDialogTextColor();
        virtual void SAL_CALL setDialogTextColor( ::sal_Int32 _dialogtextcolor );
        virtual ::sal_Int32 SAL_CALL getDisableColor();
        virtual void SAL_CALL setDisableColor( ::sal_Int32 _disablecolor );
        virtual ::sal_Int32 SAL_CALL getFaceColor();
        virtual void SAL_CALL setFaceColor( ::sal_Int32 _facecolor );
        virtual ::sal_Int32 SAL_CALL getFaceGradientColor();
        virtual ::sal_Int32 SAL_CALL getFieldColor();
        virtual void SAL_CALL setFieldColor( ::sal_Int32 _fieldcolor );
        virtual ::sal_Int32 SAL_CALL getFieldRolloverTextColor();
        virtual void SAL_CALL setFieldRolloverTextColor( ::sal_Int32 _fieldrollovertextcolor );
        virtual ::sal_Int32 SAL_CALL getFieldTextColor();
        virtual void SAL_CALL setFieldTextColor( ::sal_Int32 _fieldtextcolor );
        virtual ::sal_Int32 SAL_CALL getGroupTextColor();
        virtual void SAL_CALL setGroupTextColor( ::sal_Int32 _grouptextcolor );
        virtual ::sal_Int32 SAL_CALL getHelpColor();
        virtual void SAL_CALL setHelpColor( ::sal_Int32 _helpcolor );
        virtual ::sal_Int32 SAL_CALL getHelpTextColor();
        virtual void SAL_CALL setHelpTextColor( ::sal_Int32 _helptextcolor );
        virtual ::sal_Int32 SAL_CALL getHighlightColor();
        virtual void SAL_CALL setHighlightColor( ::sal_Int32 _highlightcolor );
        virtual ::sal_Int32 SAL_CALL getHighlightTextColor();
        virtual void SAL_CALL setHighlightTextColor( ::sal_Int32 _highlighttextcolor );
        virtual ::sal_Int32 SAL_CALL getInactiveTabColor();
        virtual void SAL_CALL setInactiveTabColor( ::sal_Int32 _inactivetabcolor );
        virtual ::sal_Int32 SAL_CALL getInfoTextColor();
        virtual void SAL_CALL setInfoTextColor( ::sal_Int32 _infotextcolor );
        virtual ::sal_Int32 SAL_CALL getLabelTextColor();
        virtual void SAL_CALL setLabelTextColor( ::sal_Int32 _labeltextcolor );
        virtual ::sal_Int32 SAL_CALL getLightColor();
        virtual void SAL_CALL setLightColor( ::sal_Int32 _lightcolor );
        virtual ::sal_Int32 SAL_CALL getMenuBarColor();
        virtual void SAL_CALL setMenuBarColor( ::sal_Int32 _menubarcolor );
        virtual ::sal_Int32 SAL_CALL getMenuBarTextColor();
        virtual void SAL_CALL setMenuBarTextColor( ::sal_Int32 _menubartextcolor );
        virtual ::sal_Int32 SAL_CALL getMenuBorderColor();
        virtual void SAL_CALL setMenuBorderColor( ::sal_Int32 _menubordercolor );
        virtual ::sal_Int32 SAL_CALL getMenuColor();
        virtual void SAL_CALL setMenuColor( ::sal_Int32 _menucolor );
        virtual ::sal_Int32 SAL_CALL getMenuHighlightColor();
        virtual void SAL_CALL setMenuHighlightColor( ::sal_Int32 _menuhighlightcolor );
        virtual ::sal_Int32 SAL_CALL getMenuHighlightTextColor();
        virtual void SAL_CALL setMenuHighlightTextColor( ::sal_Int32 _menuhighlighttextcolor );
        virtual ::sal_Int32 SAL_CALL getMenuTextColor();
        virtual void SAL_CALL setMenuTextColor( ::sal_Int32 _menutextcolor );
        virtual ::sal_Int32 SAL_CALL getMonoColor();
        virtual void SAL_CALL setMonoColor( ::sal_Int32 _monocolor );
        virtual ::sal_Int32 SAL_CALL getRadioCheckTextColor();
        virtual void SAL_CALL setRadioCheckTextColor( ::sal_Int32 _radiochecktextcolor );
        virtual ::sal_Int32 SAL_CALL getSeparatorColor();
        virtual ::sal_Int32 SAL_CALL getShadowColor();
        virtual void SAL_CALL setShadowColor( ::sal_Int32 _shadowcolor );
        virtual ::sal_Int32 SAL_CALL getWindowColor();
        virtual void SAL_CALL setWindowColor( ::sal_Int32 _windowcolor );
        virtual ::sal_Int32 SAL_CALL getWindowTextColor();
        virtual void SAL_CALL setWindowTextColor( ::sal_Int32 _windowtextcolor );
        virtual ::sal_Int32 SAL_CALL getWorkspaceColor();
        virtual void SAL_CALL setWorkspaceColor( ::sal_Int32 _workspacecolor );
        virtual ::sal_Bool SAL_CALL getHighContrastMode();
        virtual void SAL_CALL setHighContrastMode( ::sal_Bool _highcontrastmode );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getApplicationFont();
        virtual void SAL_CALL setApplicationFont( const ::com::sun::star::awt::FontDescriptor& _applicationfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getHelpFont();
        virtual void SAL_CALL setHelpFont( const ::com::sun::star::awt::FontDescriptor& _helpfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getTitleFont();
        virtual void SAL_CALL setTitleFont( const ::com::sun::star::awt::FontDescriptor& _titlefont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFloatTitleFont();
        virtual void SAL_CALL setFloatTitleFont( const ::com::sun::star::awt::FontDescriptor& _floattitlefont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getMenuFont();
        virtual void SAL_CALL setMenuFont( const ::com::sun::star::awt::FontDescriptor& _menufont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getToolFont();
        virtual void SAL_CALL setToolFont( const ::com::sun::star::awt::FontDescriptor& _toolfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getGroupFont();
        virtual void SAL_CALL setGroupFont( const ::com::sun::star::awt::FontDescriptor& _groupfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getLabelFont();
        virtual void SAL_CALL setLabelFont( const ::com::sun::star::awt::FontDescriptor& _labelfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getInfoFont();
        virtual void SAL_CALL setInfoFont( const ::com::sun::star::awt::FontDescriptor& _infofont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getRadioCheckFont();
        virtual void SAL_CALL setRadioCheckFont( const ::com::sun::star::awt::FontDescriptor& _radiocheckfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getPushButtonFont();
        virtual void SAL_CALL setPushButtonFont( const ::com::sun::star::awt::FontDescriptor& _pushbuttonfont );
        virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFieldFont();
        virtual void SAL_CALL setFieldFont( const ::com::sun::star::awt::FontDescriptor& _fieldfont );
        virtual void SAL_CALL addStyleChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XStyleChangeListener >& Listener );
        virtual void SAL_CALL removeStyleChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XStyleChangeListener >& Listener );

    private:
        ::boost::scoped_ptr< WindowStyleSettings_Data > m_pData;
	};

//......................................................................................................................
} // namespace toolkit
//......................................................................................................................

#endif // TOOLKIT_STYLESETTINGS_HXX
