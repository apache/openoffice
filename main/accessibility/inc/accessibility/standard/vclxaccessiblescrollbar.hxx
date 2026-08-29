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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLESCROLLBAR_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLESCROLLBAR_HXX

#include <toolkit/awt/vclxaccessiblecomponent.hxx>

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_ACTION_HPP_
#include <com/sun/star/accessibility/XAccessibleAction.hpp>
#endif
#include <com/sun/star/accessibility/XAccessibleValue.hpp>

#ifndef _CPPUHELPER_IMPLBASE2_HXX
#include <cppuhelper/implbase2.hxx>
#endif


//	----------------------------------------------------
//	class VCLXAccessibleScrollBar
//	----------------------------------------------------

typedef ::cppu::ImplHelper2<
	::com::sun::star::accessibility::XAccessibleAction,
	::com::sun::star::accessibility::XAccessibleValue > VCLXAccessibleScrollBar_BASE;

class VCLXAccessibleScrollBar : public VCLXAccessibleComponent,
								public VCLXAccessibleScrollBar_BASE
{
protected:
	virtual ~VCLXAccessibleScrollBar();

    virtual void ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
    virtual void FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

public:
	VCLXAccessibleScrollBar( VCLXWindow* pVCLXindow );

	// XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

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

	// XAccessibleContext
	::rtl::OUString SAL_CALL getAccessibleName(  );

};

#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLESCROLLBAR_HXX
