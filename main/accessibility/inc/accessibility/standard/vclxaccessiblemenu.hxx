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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENU_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENU_HXX

#include <accessibility/standard/vclxaccessiblemenuitem.hxx>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>

#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif


//	----------------------------------------------------
//	class VCLXAccessibleMenu
//	----------------------------------------------------

typedef ::cppu::ImplHelper1	<
	::com::sun::star::accessibility::XAccessibleSelection >	VCLXAccessibleMenu_BASE;

class VCLXAccessibleMenu :	public VCLXAccessibleMenuItem,
							public VCLXAccessibleMenu_BASE
{
protected:
    virtual sal_Bool        IsFocused();
    virtual sal_Bool        IsPopupMenuOpen();

public:
	VCLXAccessibleMenu( Menu* pParent, sal_uInt16 nItemPos, Menu* pMenu );
	virtual ~VCLXAccessibleMenu();

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XAccessibleContext
	virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
	virtual sal_Int16 SAL_CALL getAccessibleRole(  );

	// XAccessibleComponent
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );

	// XAccessibleSelection
	virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex );
	virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex );
	virtual void SAL_CALL clearAccessibleSelection(  );
	virtual void SAL_CALL selectAllAccessibleChildren(  );
	virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );
	virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nChildIndex );

	// XAccessibleAction
	virtual ::rtl::OUString SAL_CALL getAccessibleActionDescription ( sal_Int32 nIndex );
};

#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENU_HXX
