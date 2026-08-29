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



#ifndef ACCESSIBILITY_STANDARD_ACCESSIBLEMENUITEMCOMPONENT_HXX
#define ACCESSIBILITY_STANDARD_ACCESSIBLEMENUITEMCOMPONENT_HXX

#include <accessibility/standard/accessiblemenubasecomponent.hxx>


//	----------------------------------------------------
//	class OAccessibleMenuItemComponent
//	----------------------------------------------------

class OAccessibleMenuItemComponent : public OAccessibleMenuBaseComponent
{
    friend class OAccessibleMenuBaseComponent;

protected:
    Menu*					m_pParent;
    sal_uInt16				m_nItemPos;
    ::rtl::OUString			m_sAccessibleName;
    ::rtl::OUString			m_sItemText;

    virtual sal_Bool		IsEnabled();
    virtual sal_Bool        IsVisible();
    virtual void			Select();
    virtual void			DeSelect();
    virtual void            Click();

    void					SetItemPos( sal_uInt16 nItemPos );
    void					SetAccessibleName( const ::rtl::OUString& sAccessibleName );
    ::rtl::OUString			GetAccessibleName();
    void					SetItemText( const ::rtl::OUString& sItemText );
    ::rtl::OUString			GetItemText();

	virtual sal_Bool 		IsMenuHideDisabledEntries();

    virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

    // OCommonAccessibleComponent
    virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

    // XComponent
    virtual void SAL_CALL	disposing();

public:
    OAccessibleMenuItemComponent( Menu* pParent, sal_uInt16 nItemPos, Menu* pMenu );
    virtual ~OAccessibleMenuItemComponent();

    // XAccessibleContext
    virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  );
    virtual sal_Int16 SAL_CALL getAccessibleRole(  );
    virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
    virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

    // XAccessibleComponent
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual void SAL_CALL grabFocus(  );
    virtual sal_Int32 SAL_CALL getForeground(  );
    virtual sal_Int32 SAL_CALL getBackground(  );

    // XAccessibleExtendedComponent
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL getFont(	);
    virtual ::rtl::OUString SAL_CALL getTitledBorderText(  );
    virtual ::rtl::OUString SAL_CALL getToolTipText(  );
};

#endif // ACCESSIBILITY_STANDARD_ACCESSIBLEMENUITEMCOMPONENT_HXX
