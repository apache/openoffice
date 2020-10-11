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




#ifndef _SVXRECTACCESSIBLECONTEXT_HXX
#define _SVXRECTACCESSIBLECONTEXT_HXX

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/accessibility/IllegalAccessibleComponentStateException.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <vos/mutex.hxx>
#include <cppuhelper/interfacecontainer.h>
#include <cppuhelper/compbase6.hxx>
#ifndef _CPPUHELPER_COMPBASE7_HXX_
#include <cppuhelper/compbase7.hxx>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_ACTION_HPP_
#include <com/sun/star/accessibility/XAccessibleAction.hpp>
#endif
#include <comphelper/broadcasthelper.hxx>
#include <cppuhelper/implbase6.hxx>
#include <comphelper/servicehelper.hxx>
#include <svx/rectenum.hxx>

namespace com { namespace sun { namespace star { namespace awt {
	struct Point;
	struct Rectangle;
	struct Size;
	class XFocusListener;
} } } }

class Rectangle;
class SvxRectCtl;
class SvxRectCtlChildAccessibleContext;
class Window;


typedef ::cppu::WeakAggComponentImplHelper6<
			::com::sun::star::accessibility::XAccessible,
			::com::sun::star::accessibility::XAccessibleComponent,
			::com::sun::star::accessibility::XAccessibleContext,
			::com::sun::star::accessibility::XAccessibleEventBroadcaster,
			::com::sun::star::accessibility::XAccessibleSelection,
			::com::sun::star::lang::XServiceInfo >
			SvxRectCtlAccessibleContext_Base;

class SvxRectCtlAccessibleContext : public ::comphelper::OBaseMutex, public SvxRectCtlAccessibleContext_Base
{
public:
	//=====  internal  ========================================================
	SvxRectCtlAccessibleContext(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxParent,
		SvxRectCtl&				rRepresentation,
		const ::rtl::OUString*	pName = NULL,
		const ::rtl::OUString*	pDescription = NULL );
protected:
	virtual ~SvxRectCtlAccessibleContext();
public:
	//=====  XAccessible  =====================================================

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
		getAccessibleContext( void ) throw( ::com::sun::star::uno::RuntimeException );

	//=====  XAccessibleComponent  ============================================

	virtual sal_Bool SAL_CALL
		containsPoint( const ::com::sun::star::awt::Point& rPoint ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
		getAccessibleAtPoint( const ::com::sun::star::awt::Point& rPoint ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::awt::Rectangle SAL_CALL
		getBounds() throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::awt::Point SAL_CALL
		getLocation() throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::awt::Point SAL_CALL
		getLocationOnScreen() throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::awt::Size SAL_CALL
		getSize() throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		isShowing() throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		isVisible() throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		isFocusTraversable() throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		addFocusListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener )
			throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		removeFocusListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener )
			throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		grabFocus() throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Any SAL_CALL
		getAccessibleKeyBinding() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL
        getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL
        getBackground(  ) throw (::com::sun::star::uno::RuntimeException);

	//=====  XAccessibleContext  ==============================================

	virtual sal_Int32 SAL_CALL
		getAccessibleChildCount( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
		getAccessibleChild( sal_Int32 nIndex )
			throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IndexOutOfBoundsException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
		getAccessibleParent( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual	sal_Int32 SAL_CALL
		getAccessibleIndexInParent( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Int16 SAL_CALL
		getAccessibleRole( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::rtl::OUString SAL_CALL
		getAccessibleDescription( void ) throw (::com::sun::star::uno::RuntimeException);

	virtual ::rtl::OUString SAL_CALL
		getAccessibleName( void ) throw (::com::sun::star::uno::RuntimeException);

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL
		getAccessibleRelationSet( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL
		getAccessibleStateSet( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::lang::Locale SAL_CALL
		getLocale( void )
			throw(	::com::sun::star::uno::RuntimeException,
					::com::sun::star::accessibility::IllegalAccessibleComponentStateException );

    //=====  XComponent  =====================================================

    using ::cppu::WeakAggComponentImplHelperBase::addEventListener;
    using ::cppu::WeakAggComponentImplHelperBase::removeEventListener;

    //=====  XAccessibleEventBroadcaster  =====================================

	virtual void SAL_CALL
		addEventListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
			throw( com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		removeEventListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
			throw( com::sun::star::uno::RuntimeException );

	//=====  XServiceInfo  ====================================================

	virtual ::rtl::OUString SAL_CALL
		getImplementationName( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		supportsService( const ::rtl::OUString& sServiceName ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
		getSupportedServiceNames( void ) throw( ::com::sun::star::uno::RuntimeException );

	//=====  XTypeProvider  ===================================================

	virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
		getImplementationId( void ) throw( ::com::sun::star::uno::RuntimeException );

	//=====  XAccessibleSelection =============================================

	virtual void SAL_CALL
		selectAccessibleChild( sal_Int32 nChildIndex )
			throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		isAccessibleChildSelected( sal_Int32 nChildIndex )
			throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		clearAccessibleSelection() throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		selectAllAccessibleChildren() throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Int32 SAL_CALL
		getSelectedAccessibleChildCount() throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
		getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex )
			throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		deselectAccessibleChild( sal_Int32 nSelectedChildIndex )
			throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException );


protected:
	//=====  internals ========================================================

	void checkChildIndex( long nIndexOfChild ) throw( ::com::sun::star::lang::IndexOutOfBoundsException );

	void checkChildIndexOnSelection( long nIndexOfChild ) throw( ::com::sun::star::lang::IndexOutOfBoundsException );

    /** Selects a new child by index.

        <p>If the child was not selected before, the state of the child will
        be updated. If the index is invalid, the index will internally set to NOCHILDSELECTED</p>

        @param nIndexOfChild
            Index of the new child which should be selected.
    */
	//void selectChild( long nIndexOfChild );
	void selectChild( long nIndexOfChild, sal_Bool bFireFocus = sal_True);

public:
    /** Selects a new child by point.

        <p>If the child was not selected before, the state of the child will
        be updated. If the point is not invalid, the index will internally set to NOCHILDSELECTED</p>

        @param eButton
            Button which belongs to the child which should be selected.
    */
	//void selectChild( RECT_POINT ePoint );
	void selectChild( RECT_POINT ePoint, sal_Bool bFireFocus = sal_True );
	void FireChildFocus( RECT_POINT eButton );
	//Solution: Add the event handling method
	void FireAccessibleEvent (short nEventId, const ::com::sun::star::uno::Any& rOld, const ::com::sun::star::uno::Any& rNew);
	/// Sets the name
	void setName( const ::rtl::OUString& rName );

	/// Sets the description
	void setDescription( const ::rtl::OUString& rDescr );
private:
	static ::com::sun::star::uno::Sequence< sal_Int8 > getUniqueId( void );
protected:

    ///	@Return the object's current bounding box relative to the desktop.
	virtual Rectangle GetBoundingBoxOnScreen( void ) throw( ::com::sun::star::uno::RuntimeException );

	///	@Return the object's current bounding box relative to the parent object.
	virtual Rectangle GetBoundingBox( void ) throw( ::com::sun::star::uno::RuntimeException );

	/// Calls all Listener to tell they the change.
	void CommitChange( const com::sun::star::accessibility::AccessibleEventObject& rEvent );

	virtual void SAL_CALL disposing();

	/// @returns true if it's disposed or in disposing
	inline sal_Bool IsAlive( void ) const;

	/// @returns true if it's not disposed and no in disposing
	inline sal_Bool IsNotAlive( void ) const;

	/// throws the exception DisposedException if it's not alive
	void ThrowExceptionIfNotAlive( void ) throw( ::com::sun::star::lang::DisposedException );

private:
    /**	Description of this object.  This is not a constant because it can
    	be set from the outside.
    */
    ::rtl::OUString						msDescription;

    /**	Name of this object.
    */
    ::rtl::OUString						msName;

    ///	Reference to the parent object.
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
										mxParent;

	/// pointer to internal representation
	SvxRectCtl*							mpRepr;

	/// array for all possible childs
	SvxRectCtlChildAccessibleContext**	mpChilds;

    /// client id in the AccessibleEventNotifier queue
    sal_uInt32 mnClientId;

    /// actual selected child
	long								mnSelectedChild;

	/// mode of control (true -> 8 points, false -> 9 points)
	sal_Bool							mbAngleMode;
};

inline sal_Bool SvxRectCtlAccessibleContext::IsAlive( void ) const
{
	return !rBHelper.bDisposed && !rBHelper.bInDispose;
}

inline sal_Bool SvxRectCtlAccessibleContext::IsNotAlive( void ) const
{
	return rBHelper.bDisposed || rBHelper.bInDispose;
}

typedef ::cppu::WeakAggComponentImplHelper7<
			::com::sun::star::accessibility::XAccessible,
			::com::sun::star::accessibility::XAccessibleComponent,
			::com::sun::star::accessibility::XAccessibleContext,
			::com::sun::star::accessibility::XAccessibleEventBroadcaster,
			::com::sun::star::accessibility::XAccessibleValue,
			::com::sun::star::accessibility::XAccessibleAction,
			::com::sun::star::lang::XServiceInfo >
			SvxRectCtlChildAccessibleContext_Base;


class SvxRectCtlChildAccessibleContext : public SvxRectCtlChildAccessibleContext_Base
{
public:
	SvxRectCtlChildAccessibleContext(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxParent,
		const Window& rParentWindow,
		const ::rtl::OUString& rName, const ::rtl::OUString& rDescription,
		const Rectangle& rBoundingBox,
		long nIndexInParent );
protected:
	virtual ~SvxRectCtlChildAccessibleContext();
public:
	//=====  XAccessible  =====================================================

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
    	getAccessibleContext( void ) throw( ::com::sun::star::uno::RuntimeException );

	//=====  XAccessibleComponent  ============================================

    virtual sal_Bool SAL_CALL
		containsPoint( const ::com::sun::star::awt::Point& rPoint ) throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
		getAccessibleAtPoint( const ::com::sun::star::awt::Point& rPoint ) throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::awt::Rectangle SAL_CALL
		getBounds() throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::awt::Point SAL_CALL
		getLocation() throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::awt::Point SAL_CALL
		getLocationOnScreen() throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::awt::Size SAL_CALL
		getSize() throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Bool SAL_CALL
		isShowing() throw( ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
		isVisible() throw( ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
		isFocusTraversable() throw( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
		addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener )
			throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
		removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener )
			throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL
		grabFocus() throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Any SAL_CALL
		getAccessibleKeyBinding() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL
        getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL
        getBackground(  ) throw (::com::sun::star::uno::RuntimeException);

	//=====  XAccessibleContext  ==============================================

    virtual sal_Int32 SAL_CALL
    	getAccessibleChildCount( void ) throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    	getAccessibleChild( sal_Int32 nIndex )
			throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IndexOutOfBoundsException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
    	getAccessibleParent( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual	sal_Int32 SAL_CALL
    	getAccessibleIndexInParent( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual sal_Int16 SAL_CALL
    	getAccessibleRole( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::rtl::OUString SAL_CALL
    	getAccessibleDescription( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::rtl::OUString SAL_CALL
    	getAccessibleName( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL
    	getAccessibleRelationSet( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL
    	getAccessibleStateSet( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::lang::Locale SAL_CALL
    	getLocale( void )
			throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::accessibility::IllegalAccessibleComponentStateException );

    //=====  XComponent  =====================================================

    using ::cppu::WeakAggComponentImplHelperBase::addEventListener;
    using ::cppu::WeakAggComponentImplHelperBase::removeEventListener;

	//=====  XAccessibleEventBroadcaster  =====================================

	virtual void SAL_CALL
    	addEventListener(
        	const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
			throw( ::com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
    	removeEventListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
	        throw( com::sun::star::uno::RuntimeException );

	//=====  XAccessibleValue  ================================================

    virtual ::com::sun::star::uno::Any SAL_CALL
		getCurrentValue() throw( ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
		setCurrentValue( const ::com::sun::star::uno::Any& aNumber ) throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Any SAL_CALL
		getMaximumValue() throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Any SAL_CALL
		getMinimumValue() throw( ::com::sun::star::uno::RuntimeException );

	// XAccessibleAction
	virtual sal_Int32 SAL_CALL getAccessibleActionCount( ) throw (::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL doAccessibleAction ( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getAccessibleActionDescription ( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleKeyBinding > SAL_CALL getAccessibleActionKeyBinding( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	//=====  XServiceInfo  ====================================================

	virtual ::rtl::OUString SAL_CALL
    	getImplementationName( void ) throw( ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
    	supportsService( const ::rtl::OUString& sServiceName ) throw( ::com::sun::star::uno::RuntimeException );

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
    	getSupportedServiceNames( void ) throw( ::com::sun::star::uno::RuntimeException );


	//=====  XTypeProvider  ===================================================

    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId( void ) throw( ::com::sun::star::uno::RuntimeException );

	//===== internal ==========================================================

	/// Sets the checked status
	//void setStateChecked( sal_Bool bChecked );
	void setStateChecked( sal_Bool bChecked, sal_Bool bFireFocus = sal_True);
	void FireFocusEvent();
protected:
	virtual Rectangle GetBoundingBoxOnScreen( void ) throw( ::com::sun::star::uno::RuntimeException );

	virtual Rectangle GetBoundingBox( void ) throw( ::com::sun::star::uno::RuntimeException );

	void CommitChange( const com::sun::star::accessibility::AccessibleEventObject& rEvent );

	virtual void SAL_CALL disposing();

	/// @returns true if it's disposed or in disposing
	inline sal_Bool IsAlive( void ) const;

	/// @returns true if it's not disposed and no in disposing
	inline sal_Bool IsNotAlive( void ) const;

	/// throws the exception DisposedException if it's not alive
	void ThrowExceptionIfNotAlive( void ) throw( ::com::sun::star::lang::DisposedException );

	///	Mutex guarding this object.
    ::osl::Mutex						maMutex;

private:

    /**	Description of this object.  This is not a constant because it can
    	be set from the outside.  Furthermore, it changes according the the
        draw page's display mode.
    */
    ::rtl::OUString						msDescription;

    /**	Name of this object.  It changes according the draw page's
        display mode.
    */
    ::rtl::OUString						msName;

    ///	Reference to the parent object.
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
										mxParent;

	/// Bounding box
	Rectangle*							mpBoundingBox;

	/// window of parent
	const Window&						mrParentWindow;

    /// client id in the AccessibleEventNotifier queue
    sal_uInt32                          mnClientId;

	/// index of child in parent
	long								mnIndexInParent;

	/// Indicates, if object is checked
	sal_Bool							mbIsChecked;
};

inline sal_Bool SvxRectCtlChildAccessibleContext::IsAlive( void ) const
{
	return !rBHelper.bDisposed && !rBHelper.bInDispose;
}

inline sal_Bool SvxRectCtlChildAccessibleContext::IsNotAlive( void ) const
{
	return rBHelper.bDisposed || rBHelper.bInDispose;
}


#endif
