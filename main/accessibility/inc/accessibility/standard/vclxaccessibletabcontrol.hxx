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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLETABCONTROL_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLETABCONTROL_HXX

#include <toolkit/awt/vclxaccessiblecomponent.hxx>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>

#include <vector>

class TabControl;


//	----------------------------------------------------
//	class VCLXAccessibleTabControl
//	----------------------------------------------------

typedef ::cppu::ImplHelper1	<
	::com::sun::star::accessibility::XAccessibleSelection >	VCLXAccessibleTabControl_BASE;

class VCLXAccessibleTabControl :	public VCLXAccessibleComponent,
									public VCLXAccessibleTabControl_BASE
{
private:
	typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >	> AccessibleChildren;

	AccessibleChildren		m_aAccessibleChildren;
	TabControl*				m_pTabControl;

protected:
	void					UpdateFocused();
	void					UpdateSelected( sal_Int32 i, bool bSelected );
    void                    UpdatePageText( sal_Int32 i );
    void                    UpdateTabPage( sal_Int32 i, bool bNew );

	void					InsertChild( sal_Int32 i );
	void					RemoveChild( sal_Int32 i );

	virtual void			ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
	virtual void			ProcessWindowChildEvent( const VclWindowEvent& rVclWindowEvent );
	virtual void			FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

	// XComponent
	virtual void SAL_CALL	disposing();

public:
	VCLXAccessibleTabControl( VCLXWindow* pVCLXWindow );
	~VCLXAccessibleTabControl();

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
	virtual ::rtl::OUString SAL_CALL getAccessibleName(  );

	// XAccessibleSelection
	virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex );
	virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex );
	virtual void SAL_CALL clearAccessibleSelection(  );
	virtual void SAL_CALL selectAllAccessibleChildren(  );
	virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );
	virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nChildIndex );
};


#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLETABCONTROL_HXX
