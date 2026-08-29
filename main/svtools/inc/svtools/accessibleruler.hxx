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

#ifndef _SVTRULERACCESSIBLE_HXX
#define _SVTRULERACCESSIBLE_HXX

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECOMPONENT_HPP_
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECONTEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEEVENTBROADCASTER_HPP_
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_ILLEGALACCESSIBLECOMPONENTSTATEEXCEPTION_HPP_
#include <com/sun/star/accessibility/IllegalAccessibleComponentStateException.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#ifndef	_COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef	_CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICENAME_HPP_
#include <com/sun/star/lang/XServiceName.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_INDEXOUTOFBOUNDSEXCEPTION_HPP_
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_H_
#include <cppuhelper/interfacecontainer.h>
#endif
#ifndef _CPPUHELPER_COMPBASE5_HXX_
#include <cppuhelper/compbase5.hxx>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif
#ifndef _UTL_SERVICEHELPER_HXX_
#include <comphelper/servicehelper.hxx>
#endif

namespace com { namespace sun { namespace star { namespace awt {
	struct Point;
	struct Rectangle;
	struct Size;
	class XFocusListener;
} } } }

class Rectangle;
class Ruler;
class Window;


typedef ::cppu::WeakAggComponentImplHelper5<
			::com::sun::star::accessibility::XAccessible,
			::com::sun::star::accessibility::XAccessibleComponent,
			::com::sun::star::accessibility::XAccessibleContext,
			::com::sun::star::accessibility::XAccessibleEventBroadcaster,
			::com::sun::star::lang::XServiceInfo >
			SvtRulerAccessible_Base;

class SvtRulerAccessible : public ::comphelper::OBaseMutex, public SvtRulerAccessible_Base
{
public:
	//=====  internal  ========================================================
	SvtRulerAccessible(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxParent, Ruler&	rRepresentation, const ::rtl::OUString& rName );
protected:
	virtual ~SvtRulerAccessible();
public:
	//=====  XAccessible  =====================================================

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
		getAccessibleContext( void );

	//=====  XAccessibleComponent  ============================================

	virtual sal_Bool SAL_CALL
		containsPoint( const ::com::sun::star::awt::Point& rPoint );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
		getAccessibleAtPoint( const ::com::sun::star::awt::Point& rPoint );

	virtual ::com::sun::star::awt::Rectangle SAL_CALL
		getBounds();

	virtual ::com::sun::star::awt::Point SAL_CALL
		getLocation();

	virtual ::com::sun::star::awt::Point SAL_CALL
		getLocationOnScreen();

	virtual ::com::sun::star::awt::Size SAL_CALL
		getSize();

	virtual sal_Bool SAL_CALL
		isShowing();

	virtual sal_Bool SAL_CALL
		isVisible();

	virtual sal_Bool SAL_CALL
		isFocusTraversable();

	virtual void SAL_CALL
		addFocusListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );

	virtual void SAL_CALL
		removeFocusListener(
			const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );

	virtual void SAL_CALL
		grabFocus();

	virtual ::com::sun::star::uno::Any SAL_CALL
		getAccessibleKeyBinding();
    virtual sal_Int32 SAL_CALL
        getForeground(  );
    virtual sal_Int32 SAL_CALL
        getBackground(  );

	//=====  XAccessibleContext  ==============================================

	virtual sal_Int32 SAL_CALL
		getAccessibleChildCount( void );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
		getAccessibleChild( sal_Int32 nIndex );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
		getAccessibleParent( void );

	virtual	sal_Int32 SAL_CALL
		getAccessibleIndexInParent( void );

	virtual sal_Int16 SAL_CALL
		getAccessibleRole( void );

	virtual ::rtl::OUString SAL_CALL
		getAccessibleDescription( void );

	virtual ::rtl::OUString SAL_CALL
		getAccessibleName( void );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL
		getAccessibleRelationSet( void );

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL
		getAccessibleStateSet( void );

	virtual ::com::sun::star::lang::Locale SAL_CALL
		getLocale( void );
	//=====  XAccessibleEventBroadcaster  =====================================

	virtual void SAL_CALL
		addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

	virtual void SAL_CALL
		removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

	// Needed, because some compilers would complain about virtual functions above would hide cppu::WeakAggComponentImplHelperBase::add/removeEventListener(const Reference<lang::XEventListener>&)...
	virtual void SAL_CALL
		addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );

	virtual void SAL_CALL
		removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );


	//=====  XServiceInfo  ====================================================

	virtual ::rtl::OUString SAL_CALL
		getImplementationName( void );

	virtual sal_Bool SAL_CALL
		supportsService( const ::rtl::OUString& sServiceName );

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
		getSupportedServiceNames( void );

	//=====  XTypeProvider  ===================================================

	virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
		getImplementationId( void );




public:

	/// Sets the name
	void setName( const ::rtl::OUString& rName );

	/// Sets the description
	void setDescription( const ::rtl::OUString& rDescr );
private:
	static ::com::sun::star::uno::Sequence< sal_Int8 > getUniqueId( void );
protected:

    ///	@Return the object's current bounding box relative to the desktop.
	virtual Rectangle GetBoundingBoxOnScreen( void );

	///	@Return the object's current bounding box relative to the parent object.
	virtual Rectangle GetBoundingBox( void );


	virtual void SAL_CALL disposing();

	/// @returns true if it's disposed or in disposing
	inline sal_Bool IsAlive( void ) const;

	/// @returns true if it's not disposed and no in disposing
	inline sal_Bool IsNotAlive( void ) const;

	/// throws the exception DisposedException if it's not alive
	void ThrowExceptionIfNotAlive( void );

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
	Ruler*							mpRepr;

	    /// client id in the AccessibleEventNotifier queue
    sal_uInt32 mnClientId;


};

inline sal_Bool SvtRulerAccessible::IsAlive( void ) const
{
	return !rBHelper.bDisposed && !rBHelper.bInDispose;
}

inline sal_Bool SvtRulerAccessible::IsNotAlive( void ) const
{
	return rBHelper.bDisposed || rBHelper.bInDispose;
}



#endif
