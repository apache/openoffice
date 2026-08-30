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



#ifndef ACCESSIBILITY_STANDARD_ACCESSIBLEMENUCOMPONENT_HXX
#define ACCESSIBILITY_STANDARD_ACCESSIBLEMENUCOMPONENT_HXX

#include <accessibility/standard/accessiblemenubasecomponent.hxx>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif


//	----------------------------------------------------
//	class OAccessibleMenuComponent
//	----------------------------------------------------

typedef ::comphelper::OAccessibleExtendedComponentHelper	AccessibleExtendedComponentHelper_BASE;

typedef ::cppu::ImplHelper1<
	::com::sun::star::accessibility::XAccessibleSelection > OAccessibleMenuComponent_BASE;

class OAccessibleMenuComponent : public OAccessibleMenuBaseComponent,
								 public OAccessibleMenuComponent_BASE
{
protected:
	virtual sal_Bool		IsEnabled();
    virtual sal_Bool        IsVisible();

    virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

	// OCommonAccessibleComponent
	virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

public:
	OAccessibleMenuComponent( Menu* pMenu );
	virtual ~OAccessibleMenuComponent();

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XAccessibleContext
	virtual sal_Int32 SAL_CALL getAccessibleChildCount(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  );
	virtual sal_Int16 SAL_CALL getAccessibleRole(  );
	virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  );
	virtual ::rtl::OUString SAL_CALL getAccessibleName(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  );
	virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

	// XAccessibleComponent
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
	virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(	);
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

#endif // ACCESSIBILITY_STANDARD_ACCESSIBLEMENUCOMPONENT_HXX
