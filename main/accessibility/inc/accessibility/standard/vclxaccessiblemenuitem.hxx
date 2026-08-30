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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENUITEM_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENUITEM_HXX

#include <accessibility/standard/accessiblemenuitemcomponent.hxx>

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_ACTION_HPP_
#include <com/sun/star/accessibility/XAccessibleAction.hpp>
#endif
#include <com/sun/star/accessibility/XAccessibleValue.hpp>

#ifndef _CPPUHELPER_IMPLBASE3_HXX
#include <cppuhelper/implbase3.hxx>
#endif
#include <comphelper/accessibletexthelper.hxx>


//	----------------------------------------------------
//	class VCLXAccessibleMenuItem
//	----------------------------------------------------

typedef ::cppu::ImplHelper3<
	::com::sun::star::accessibility::XAccessibleText,
	::com::sun::star::accessibility::XAccessibleAction,
	::com::sun::star::accessibility::XAccessibleValue > VCLXAccessibleMenuItem_BASE;

class VCLXAccessibleMenuItem :	public OAccessibleMenuItemComponent,
								public ::comphelper::OCommonAccessibleText,
								public VCLXAccessibleMenuItem_BASE
{
protected:
    virtual sal_Bool        IsFocused();
    virtual sal_Bool        IsSelected();
	virtual sal_Bool		IsChecked();

    virtual sal_Bool		IsHighlighted();

    virtual void            FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

	// OCommonAccessibleText
	virtual ::rtl::OUString						implGetText();
	virtual ::com::sun::star::lang::Locale		implGetLocale();
	virtual void								implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex );

public:
	VCLXAccessibleMenuItem( Menu* pParent, sal_uInt16 nItemPos, Menu* pMenu = 0 );
	virtual ~VCLXAccessibleMenuItem();

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XAccessibleContext
	virtual sal_Int16 SAL_CALL getAccessibleRole(  );

	// XAccessibleText
	virtual sal_Int32 SAL_CALL getCaretPosition();
	virtual sal_Bool SAL_CALL setCaretPosition( sal_Int32 nIndex );
	virtual sal_Unicode SAL_CALL getCharacter( sal_Int32 nIndex );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRequestedAttributes );
	virtual ::com::sun::star::awt::Rectangle SAL_CALL getCharacterBounds( sal_Int32 nIndex );
	virtual sal_Int32 SAL_CALL getCharacterCount();
	virtual sal_Int32 SAL_CALL getIndexAtPoint( const ::com::sun::star::awt::Point& aPoint );
	virtual ::rtl::OUString SAL_CALL getSelectedText();
	virtual sal_Int32 SAL_CALL getSelectionStart();
	virtual sal_Int32 SAL_CALL getSelectionEnd();
	virtual sal_Bool SAL_CALL setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
	virtual ::rtl::OUString SAL_CALL getText();
	virtual ::rtl::OUString SAL_CALL getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
    virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType );
    virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType );
    virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType );
	virtual sal_Bool SAL_CALL copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex );

	// XAccessibleAction
	virtual sal_Int32 SAL_CALL getAccessibleActionCount( );
	virtual sal_Bool SAL_CALL doAccessibleAction ( sal_Int32 nIndex );
	virtual ::rtl::OUString SAL_CALL getAccessibleActionDescription ( sal_Int32 nIndex );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleKeyBinding > SAL_CALL getAccessibleActionKeyBinding( sal_Int32 nIndex );

	// XAccessibleValue
	virtual ::com::sun::star::uno::Any SAL_CALL getCurrentValue(  );
	virtual sal_Bool SAL_CALL setCurrentValue( const ::com::sun::star::uno::Any& aNumber );
	virtual ::com::sun::star::uno::Any SAL_CALL getMaximumValue(  );
	virtual ::com::sun::star::uno::Any SAL_CALL getMinimumValue(  );
};

#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLEMENUITEM_HXX
