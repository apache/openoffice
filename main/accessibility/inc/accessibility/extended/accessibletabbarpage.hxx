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



#ifndef ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGE_HXX_
#define ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGE_HXX_

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase2.hxx>
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
	//	class AccessibleTabBarPage
	//	----------------------------------------------------

	typedef ::cppu::ImplHelper2<
		::com::sun::star::accessibility::XAccessible,
		::com::sun::star::lang::XServiceInfo > AccessibleTabBarPage_BASE;

    class AccessibleTabBarPage :    public AccessibleTabBarBase,
									public AccessibleTabBarPage_BASE
	{
		friend class AccessibleTabBarPageList;

	private:
		sal_uInt16				m_nPageId;
		sal_Bool				m_bEnabled;
		sal_Bool				m_bShowing;
		sal_Bool				m_bSelected;
		::rtl::OUString			m_sPageText;

		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >		m_xParent;

	protected:
		sal_Bool				IsEnabled();
		sal_Bool				IsShowing();
		sal_Bool				IsSelected();

		void					SetEnabled( sal_Bool bEnabled );
		void					SetShowing( sal_Bool bShowing );
		void					SetSelected( sal_Bool bSelected );
		void					SetPageText( const ::rtl::OUString& sPageText );

		sal_uInt16				GetPageId() const { return m_nPageId; }

		virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

		// OCommonAccessibleComponent
		virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

		// XComponent
		virtual void SAL_CALL	disposing();

	public:
		AccessibleTabBarPage( TabBar* pTabBar, sal_uInt16 nPageId,
							  const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& rxParent );
		virtual ~AccessibleTabBarPage();

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
	};

//.........................................................................
}	// namespace accessibility
//.........................................................................

#endif // ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGE_HXX_
