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



#ifndef _BASCTL_ACCESSIBLEDIALOGWINDOW_HXX_
#define _BASCTL_ACCESSIBLEDIALOGWINDOW_HXX_

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <comphelper/accessiblecomponenthelper.hxx>
#include <cppuhelper/implbase3.hxx>
#include <tools/link.hxx>
#include <svl/lstner.hxx>

#include <vector>

class DialogWindow;
class DlgEditor;
class DlgEdModel;
class DlgEdObj;
class VCLExternalSolarLock;
class VclSimpleEvent;
class VclWindowEvent;

namespace utl {
class AccessibleStateSetHelper;
}


//	----------------------------------------------------
//	class AccessibleDialogWindow
//	----------------------------------------------------

typedef ::comphelper::OAccessibleExtendedComponentHelper	AccessibleExtendedComponentHelper_BASE;

typedef ::cppu::ImplHelper3	<
	::com::sun::star::accessibility::XAccessible,
	::com::sun::star::accessibility::XAccessibleSelection,
	::com::sun::star::lang::XServiceInfo > AccessibleDialogWindow_BASE;

class AccessibleDialogWindow :	public AccessibleExtendedComponentHelper_BASE,
								public AccessibleDialogWindow_BASE,
								public SfxListener
{
private:

	class ChildDescriptor
	{
	public:
		DlgEdObj*																					pDlgEdObj;
		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >	rxAccessible;

		ChildDescriptor( DlgEdObj* _pDlgEdObj );
		~ChildDescriptor();

		ChildDescriptor( const ChildDescriptor& rDesc );
		ChildDescriptor& operator=( const ChildDescriptor& rDesc );

		bool operator==( const ChildDescriptor& rDesc );
		bool operator<( const ChildDescriptor& rDesc ) const;
	};

	typedef ::std::vector< ChildDescriptor > AccessibleChildren;

	AccessibleChildren		m_aAccessibleChildren;
	VCLExternalSolarLock*	m_pExternalLock;
	DialogWindow*			m_pDialogWindow;
	DlgEditor*				m_pDlgEditor;
	DlgEdModel*				m_pDlgEdModel;

protected:
	void					UpdateFocused();
	void					UpdateSelected();
	void					UpdateBounds();

	sal_Bool				IsChildVisible( const ChildDescriptor& rDesc );

	void					InsertChild( const ChildDescriptor& rDesc );
	void					RemoveChild( const ChildDescriptor& rDesc );
	void					UpdateChild( const ChildDescriptor& rDesc );
	void					UpdateChildren();
	void					SortChildren();

	DECL_LINK( WindowEventListener, VclSimpleEvent* );

	virtual void			ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

	// OCommonAccessibleComponent
	virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

	// XComponent
	virtual void SAL_CALL	disposing();

public:
	AccessibleDialogWindow( DialogWindow* pDialogWindow );
	~AccessibleDialogWindow();

	// SfxListener
	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

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


#endif // _BASCTL_ACCESSIBLEDIALOGWINDOW_HXX_
