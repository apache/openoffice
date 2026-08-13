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



#ifndef ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGELIST_HXX_
#define ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGELIST_HXX_

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase3.hxx>
#include "accessibility/extended/accessibletabbarbase.hxx"

#include <vector>

namespace utl {
class AccessibleStateSetHelper;
}

//.........................................................................
namespace accessibility
{
//.........................................................................

	//	----------------------------------------------------
	//	class AccessibleTabBarPageList
	//	----------------------------------------------------

	typedef ::cppu::ImplHelper3<
		::com::sun::star::accessibility::XAccessible,
		::com::sun::star::accessibility::XAccessibleSelection,
		::com::sun::star::lang::XServiceInfo > AccessibleTabBarPageList_BASE;

    class AccessibleTabBarPageList :    public AccessibleTabBarBase,
										public AccessibleTabBarPageList_BASE
	{
	private:
		typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >	> AccessibleChildren;

		AccessibleChildren		m_aAccessibleChildren;
		sal_Int32				m_nIndexInParent;

	protected:
		void					UpdateEnabled( sal_Int32 i, sal_Bool bEnabled );
		void					UpdateShowing( sal_Bool bShowing );
		void					UpdateSelected( sal_Int32 i, sal_Bool bSelected );
		void					UpdatePageText( sal_Int32 i );

		void					InsertChild( sal_Int32 i );
		void					RemoveChild( sal_Int32 i );
		void					MoveChild( sal_Int32 i, sal_Int32 j );

		virtual void			ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
		virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

		// OCommonAccessibleComponent
		virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

		// XComponent
		virtual void SAL_CALL	disposing();

	public:
		AccessibleTabBarPageList( TabBar* pTabBar, sal_Int32 nIndexInParent );
		virtual ~AccessibleTabBarPageList();

		// XInterface
		DECLARE_XINTERFACE()

		// XTypeProvider
		DECLARE_XTYPEPROVIDER()

		// XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName();
		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& rServiceName );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

		// XAccessible
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  );

		// XAccessibleContext
		virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
		virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  );
		virtual sal_Int16 SAL_CALL getAccessibleRole(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
		virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  );
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

		// XAccessibleSelection
		virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex );
		virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex );
		virtual void SAL_CALL clearAccessibleSelection(  );
		virtual void SAL_CALL selectAllAccessibleChildren(  );
		virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );
		virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nChildIndex );
	};

//.........................................................................
}	// namespace accessibility
//.........................................................................

#endif // ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGELIST_HXX_
