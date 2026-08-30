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

#ifndef __FRAMEWORK_UIELEMENT_STATUSBARITEM_HXX_
#define __FRAMEWORK_UIELEMENT_STATUSBARITEM_HXX_

#include <com/sun/star/ui/XStatusbarItem.hpp>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

class StatusBar;

namespace framework
{

struct AddonStatusbarItemData;

typedef cppu::WeakComponentImplHelper1< com::sun::star::ui::XStatusbarItem > StatusbarItem_Base;

class StatusbarItem : protected cppu::BaseMutex,
                      public StatusbarItem_Base
{
public:
    explicit StatusbarItem(
                            StatusBar              *pStatusBar,
                            AddonStatusbarItemData *pItemData,
                            sal_uInt16              nId,
                            const rtl::OUString&   aCommand );
    ~StatusbarItem();

    void SAL_CALL disposing();

    // com::sun::star::ui::XStatusbarItem Attributes
    virtual ::rtl::OUString SAL_CALL getCommand();
    virtual ::sal_uInt16 SAL_CALL getItemId();
    virtual ::sal_uInt32 SAL_CALL getWidth();
    virtual ::sal_uInt16 SAL_CALL getStyle();
    virtual ::sal_Int32 SAL_CALL getOffset();
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getItemRect();
    virtual ::rtl::OUString SAL_CALL getText();
    virtual void SAL_CALL setText( const rtl::OUString& rText );
    virtual ::rtl::OUString SAL_CALL getHelpText();
    virtual void SAL_CALL setHelpText( const rtl::OUString& rHelpText );
    virtual ::rtl::OUString SAL_CALL getQuickHelpText();
    virtual void SAL_CALL setQuickHelpText( const rtl::OUString& rQuickHelpText );
    virtual ::rtl::OUString SAL_CALL getAccessibleName();
    virtual void SAL_CALL setAccessibleName( const rtl::OUString& rAccessibleName );
    virtual ::sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( sal_Bool bVisible );

    // com::sun::star::ui::XStatusbarItem Methods
    virtual void SAL_CALL repaint(  );

private:
    StatusBar              *m_pStatusBar;
    AddonStatusbarItemData *m_pItemData;
    sal_uInt16              m_nId;
    sal_uInt16              m_nStyle;
    rtl::OUString           m_aCommand;

};

}

#endif
