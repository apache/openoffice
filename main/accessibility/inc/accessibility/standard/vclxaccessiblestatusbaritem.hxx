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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLESTATUSBARITEM_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLESTATUSBARITEM_HXX

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <comphelper/accessibletexthelper.hxx>
#ifndef _CPPUHELPER_IMPLBASE2_HXX
#include <cppuhelper/implbase2.hxx>
#endif


class StatusBar;
class VCLExternalSolarLock;

namespace utl {
class AccessibleStateSetHelper;
}


//	----------------------------------------------------
//	class VCLXAccessibleStatusBarItem
//	----------------------------------------------------

typedef ::comphelper::OAccessibleTextHelper	AccessibleTextHelper_BASE;

typedef ::cppu::ImplHelper2<
	::com::sun::star::accessibility::XAccessible,
	::com::sun::star::lang::XServiceInfo > VCLXAccessibleStatusBarItem_BASE;

class VCLXAccessibleStatusBarItem :	public AccessibleTextHelper_BASE,
									public VCLXAccessibleStatusBarItem_BASE
{
	friend class VCLXAccessibleStatusBar;

private:
	VCLExternalSolarLock*	m_pExternalLock;
	StatusBar*				m_pStatusBar;
	sal_uInt16				m_nItemId;
	::rtl::OUString			m_sItemName;
	::rtl::OUString			m_sItemText;
	sal_Bool				m_bShowing;

protected:
	sal_Bool				IsShowing();
	void					SetShowing( sal_Bool bShowing );
	void					SetItemName( const ::rtl::OUString& sItemName );
	::rtl::OUString			GetItemName();
	void					SetItemText( const ::rtl::OUString& sItemText );
	::rtl::OUString			GetItemText();
	sal_uInt16				GetItemId() const { return m_nItemId; }

    virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

	// OCommonAccessibleComponent
	virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

	// OCommonAccessibleText
	virtual ::rtl::OUString					implGetText();
	virtual ::com::sun::star::lang::Locale	implGetLocale();
	virtual void							implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex );

	// XComponent
	virtual void SAL_CALL	disposing();

public:
	VCLXAccessibleStatusBarItem( StatusBar* pStatusBar, sal_uInt16 nItemId );
	virtual ~VCLXAccessibleStatusBarItem();

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

	// XAccessibleText
	virtual sal_Int32 SAL_CALL getCaretPosition();
	virtual sal_Bool SAL_CALL setCaretPosition( sal_Int32 nIndex );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRequestedAttributes );
	virtual ::com::sun::star::awt::Rectangle SAL_CALL getCharacterBounds( sal_Int32 nIndex );
	virtual sal_Int32 SAL_CALL getIndexAtPoint( const ::com::sun::star::awt::Point& aPoint );
	virtual sal_Bool SAL_CALL setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
	virtual sal_Bool SAL_CALL copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
};

#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLESTATUSBARITEM_HXX
