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



#ifndef ACC_ACCESSIBLETOOLPANELDECKTABBARITEM_HXX
#define ACC_ACCESSIBLETOOLPANELDECKTABBARITEM_HXX

/** === begin UNO includes === **/
/** === end UNO includes === **/

#include <comphelper/accessiblecomponenthelper.hxx>
#include <cppuhelper/implbase1.hxx>

#include <boost/scoped_ptr.hpp>

namespace svt
{
    class IToolPanelDeck;
    class PanelTabBar;
}

//......................................................................................................................
namespace accessibility
{
//......................................................................................................................

	//==================================================================================================================
	//= AccessibleToolPanelDeckTabBarItem
	//==================================================================================================================
    class AccessibleToolPanelDeckTabBarItem_Impl;
    typedef ::comphelper::OAccessibleExtendedComponentHelper AccessibleToolPanelDeckTabBarItem_Base;
    class AccessibleToolPanelDeckTabBarItem : public AccessibleToolPanelDeckTabBarItem_Base
	{
    public:
        AccessibleToolPanelDeckTabBarItem(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& i_rAccessibleParent,
            ::svt::IToolPanelDeck& i_rPanelDeck,
            ::svt::PanelTabBar& i_rTabBar,
            const size_t i_nItemPos
        );

        using AccessibleToolPanelDeckTabBarItem_Base::NotifyAccessibleEvent;
        using AccessibleToolPanelDeckTabBarItem_Base::lateInit;

    protected:
        virtual ~AccessibleToolPanelDeckTabBarItem();

    public:
        // XAccessibleContext
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );

        // XAccessibleComponent
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
        virtual void SAL_CALL grabFocus(  );
        virtual ::sal_Int32 SAL_CALL getForeground(  );
        virtual ::sal_Int32 SAL_CALL getBackground(  );

        // XAccessibleExtendedComponent
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL getFont(  );
        virtual ::rtl::OUString SAL_CALL getTitledBorderText(  );
        virtual ::rtl::OUString SAL_CALL getToolTipText(  );

    protected:
        // OCommonAccessibleComponent
		virtual ::com::sun::star::awt::Rectangle SAL_CALL implGetBounds(  );

        // OComponentHelper
        virtual void SAL_CALL disposing();

    protected:
        ::boost::scoped_ptr< AccessibleToolPanelDeckTabBarItem_Impl >   m_pImpl;
	};

//......................................................................................................................
} // namespace accessibility
//......................................................................................................................

#endif // ACC_ACCESSIBLETOOLPANELDECKTABBARITEM_HXX
