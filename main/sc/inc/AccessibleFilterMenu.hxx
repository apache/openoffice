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



#ifndef SC_ACCESSIBLEFILTERMENU_HXX
#define SC_ACCESSIBLEFILTERMENU_HXX

#include "AccessibleContextBase.hxx"
#include "cppuhelper/implbase1.hxx"

#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/accessibility/XAccessibleText.hpp>
#include <com/sun/star/accessibility/XAccessibleTextAttributes.hpp>
#include <com/sun/star/accessibility/TextSegment.hpp>

#include <vector>

namespace com { namespace sun { namespace star {
    namespace accessibility {
        struct AccessibleEventObject;
    }
}}}

class ScDocument;
class ScMenuFloatingWindow;

typedef ::cppu::ImplHelper1<
        ::com::sun::star::accessibility::XAccessibleSelection > ScAccessibleFilterMenu_BASE;

class ScAccessibleFilterMenu :
    public ScAccessibleContextBase,
    public ScAccessibleFilterMenu_BASE
{
public:
    explicit ScAccessibleFilterMenu(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>& rxParent,
            ScMenuFloatingWindow* pWin, const ::rtl::OUString& rName, size_t nMenuPos, ScDocument* pDoc);
    virtual ~ScAccessibleFilterMenu();

	// XAccessibleComponent

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
		SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& rPoint );

    virtual sal_Bool SAL_CALL isVisible();

    virtual void SAL_CALL grabFocus();

    virtual sal_Int32 SAL_CALL getForeground();

    virtual sal_Int32 SAL_CALL getBackground();

	// XAccessibleContext

	virtual ::rtl::OUString SAL_CALL getAccessibleName();

    virtual sal_Int32 SAL_CALL getAccessibleChildCount();

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleChild(sal_Int32 nIndex);

	virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
    	getAccessibleStateSet();

	virtual ::rtl::OUString SAL_CALL getImplementationName();

    // XAccessibleEventBroadcaster

    using ScAccessibleContextBase::addEventListener;
    using ScAccessibleContextBase::removeEventListener;

	virtual void SAL_CALL
    	addEventListener(
        	const ::com::sun::star::uno::Reference<
                ::com::sun::star::accessibility::XAccessibleEventListener>& xListener);

    //	Remove an existing event listener.
	virtual void SAL_CALL
    	removeEventListener(
			const ::com::sun::star::uno::Reference<
                ::com::sun::star::accessibility::XAccessibleEventListener>& xListener);

    // XAccessibleSelection

    virtual void SAL_CALL selectAccessibleChild(sal_Int32 nChildIndex);

    virtual sal_Bool SAL_CALL isAccessibleChildSelected(sal_Int32 nChildIndex);

    virtual void SAL_CALL clearAccessibleSelection();

    virtual void SAL_CALL selectAllAccessibleChildren();

    virtual ::sal_Int32 SAL_CALL getSelectedAccessibleChildCount();

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
        getSelectedAccessibleChild(sal_Int32 nChildIndex);

    virtual void SAL_CALL deselectAccessibleChild(sal_Int32 nChildIndex);

    // XInterface

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
		::com::sun::star::uno::Type const & rType );

	virtual void SAL_CALL acquire() throw ();
	virtual void SAL_CALL release() throw ();

    // XTypeProvider

    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL getImplementationId();

    // non-UNO methods

    void appendMenuItem(const ::rtl::OUString& rName, bool bEnabled, size_t nMenuPos);
    void setMenuPos(size_t nMenuPos);
    void setEnabled(bool bEnabled);

protected:

    sal_Int32 getMenuItemCount() const;

	virtual Rectangle GetBoundingBoxOnScreen() const;

	virtual Rectangle GetBoundingBox() const;

private:
    bool isSelected() const;
    bool isFocused() const;

    void updateStates();

private:
    ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > > maMenuItems;
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > mxStateSet;

    size_t mnMenuPos;
    ScMenuFloatingWindow* mpWindow;
    ScDocument* mpDoc;

    bool mbEnabled:1;
};

#endif
