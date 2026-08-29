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


#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLETOOLBOXITEM_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLETOOLBOXITEM_HXX

#include <com/sun/star/accessibility/XAccessible.hpp>
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_ACTION_HPP_
#include <com/sun/star/accessibility/XAccessibleAction.hpp>
#endif
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleStateSet.hpp>
#include <com/sun/star/accessibility/XAccessibleText.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
#include <comphelper/accessibletexthelper.hxx>
#include <tools/solar.h>

// class VCLXAccessibleToolBoxItem ---------------------------------------------

class ToolBox;

typedef ::comphelper::OAccessibleTextHelper AccessibleTextHelper_BASE;
typedef ::cppu::ImplHelper4 < ::com::sun::star::accessibility::XAccessible,
							  ::com::sun::star::accessibility::XAccessibleAction,
							  ::com::sun::star::accessibility::XAccessibleValue,
							  ::com::sun::star::lang::XServiceInfo > VCLXAccessibleToolBoxItem_BASE;

class VCLExternalSolarLock;

class VCLXAccessibleToolBoxItem : public AccessibleTextHelper_BASE,
								  public VCLXAccessibleToolBoxItem_BASE
{
private:
	::rtl::OUString			m_sOldName;
    ToolBox*				m_pToolBox;
	VCLExternalSolarLock*	m_pExternalLock;
	sal_Int32				m_nIndexInParent;
	sal_Int16 				m_nRole;
	sal_uInt16					m_nItemId;
	sal_Bool				m_bHasFocus;
	sal_Bool				m_bIsChecked;
    bool                    m_bIndeterminate;

	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >	m_xChild;

public:
	inline sal_Int32	getIndexInParent() const					{ return m_nIndexInParent; }
	inline void			setIndexInParent( sal_Int32 _nNewIndex )	{ m_nIndexInParent = _nNewIndex; }

protected:
	virtual ~VCLXAccessibleToolBoxItem();

	virtual void SAL_CALL					disposing();

	/// implements the calculation of the bounding rectangle
	virtual ::com::sun::star::awt::Rectangle SAL_CALL	implGetBounds(  );

	// OCommonAccessibleText
	virtual ::rtl::OUString								implGetText();
	virtual ::com::sun::star::lang::Locale				implGetLocale();
	virtual void										implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex );

	::rtl::OUString GetText( bool _bAsName );

public:
	VCLXAccessibleToolBoxItem( ToolBox*	_pToolBox, sal_Int32 _nPos );

	void				SetFocus( sal_Bool _bFocus );
	inline sal_Bool		HasFocus() const { return m_bHasFocus; }
	void				SetChecked( sal_Bool _bCheck );
	inline sal_Bool		IsChecked() const { return m_bIsChecked; }
    void                SetIndeterminate( bool _bIndeterminate );
    inline bool         IsIndeterminate() const { return m_bIndeterminate; }
	inline void			ReleaseToolBox() { m_pToolBox = NULL; }
	void				NameChanged();
    void                SetChild( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _xChild );
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
                        GetChild() const { return m_xChild; }
    void                NotifyChildEvent( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _xChild, bool _bShow );

    void                ToggleEnableState();

	// XInterface
	DECLARE_XINTERFACE( )
	DECLARE_XTYPEPROVIDER( )

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

	// XAccessibleText
	virtual sal_Int32 SAL_CALL getCaretPosition();
	virtual sal_Bool SAL_CALL setCaretPosition( sal_Int32 nIndex );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRequestedAttributes );
	virtual ::com::sun::star::awt::Rectangle SAL_CALL getCharacterBounds( sal_Int32 nIndex );
    virtual sal_Int32 SAL_CALL getIndexAtPoint( const ::com::sun::star::awt::Point& aPoint );
	virtual sal_Bool SAL_CALL setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex );
    virtual sal_Bool SAL_CALL copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex );

	// XAccessibleComponent
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
	virtual void SAL_CALL grabFocus(  );
	virtual sal_Int32 SAL_CALL getForeground(  );
	virtual sal_Int32 SAL_CALL getBackground(  );

	// XAccessibleExtendedComponent
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL getFont(	);
	virtual ::com::sun::star::awt::FontDescriptor SAL_CALL getFontMetrics( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont >& xFont );
	virtual ::rtl::OUString SAL_CALL getTitledBorderText(  );
	virtual ::rtl::OUString SAL_CALL getToolTipText(  );

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

#endif // ACCESSIBILITY_STANDARD_VCLXACCESSIBLETOOLBOXITEM_HXX
