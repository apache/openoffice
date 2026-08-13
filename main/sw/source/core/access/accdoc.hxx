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

#ifndef _ACCDOC_HXX
#define _ACCDOC_HXX
#ifndef _ACCCONTEXT_HXX
#include "acccontext.hxx"
#endif
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <accselectionhelper.hxx>

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XAccessibleExtendedAttributes_HPP_
#include <com/sun/star/accessibility/XAccessibleExtendedAttributes.hpp>
#endif

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEGETACCFLOWTO_HPP_
#include <com/sun/star/accessibility/XAccessibleGetAccFlowTo.hpp>
#endif

class VclSimpleEvent;

/**
 * base class for SwAccessibleDocument (in this same header file) and
 * SwAccessiblePreview
 */
class SwAccessibleDocumentBase : public	SwAccessibleContext
{
	::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible> mxParent;

    Window* mpChildWin; // protected by solar mutex

    using SwAccessibleFrame::SetVisArea;

protected:

	virtual ~SwAccessibleDocumentBase();

public:

    SwAccessibleDocumentBase( SwAccessibleMap* pInitMap );

	void SetVisArea();

	virtual void AddChild( Window *pWin, sal_Bool bFireEvent = sal_True );
	virtual void RemoveChild( Window *pWin );

	//=====  XAccessibleContext  ==============================================

    ///	Return the number of currently visible children.
    virtual sal_Int32 SAL_CALL getAccessibleChildCount (void);

    ///	Return the specified child or NULL if index is invalid.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleChild (sal_Int32 nIndex);

    ///	Return a reference to the parent.
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleParent (void);

    ///	Return this objects index among the parents children.
	virtual	sal_Int32 SAL_CALL
    	getAccessibleIndexInParent (void);

    ///	Return this object's description.
	virtual ::rtl::OUString SAL_CALL
    	getAccessibleDescription (void);
	virtual ::rtl::OUString SAL_CALL getAccessibleName (void);

	//=====  XAccessibleComponent  ==============================================
    virtual sal_Bool SAL_CALL containsPoint(
			const ::com::sun::star::awt::Point& aPoint );

    virtual ::com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint(
				const ::com::sun::star::awt::Point& aPoint );

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds();

    virtual ::com::sun::star::awt::Point SAL_CALL getLocation();

    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen();

    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
};


/**
 * access to an accessible Writer document
 */
class SwAccessibleDocument : public	SwAccessibleDocumentBase,
                             public com::sun::star::accessibility::XAccessibleSelection,
                             public com::sun::star::document::XEventListener,
							 public com::sun::star::accessibility::XAccessibleExtendedAttributes,
							 public com::sun::star::accessibility::XAccessibleGetAccFlowTo
{
    // Implementation for XAccessibleSelection interface
    SwAccessibleSelectionHelper maSelectionHelper;

protected:

	// Set states for getAccessibleStateSet.
	// This derived class additionally sets MULTISELECTABLE(1)
	virtual void GetStates( ::utl::AccessibleStateSetHelper& rStateSet );

	virtual ~SwAccessibleDocument();

public:

    SwAccessibleDocument( SwAccessibleMap* pInitMap );

	DECL_LINK( WindowChildEventListener, VclSimpleEvent* );
	//=====  XEventListener====================================================
	virtual void SAL_CALL notifyEvent( const ::com::sun::star::document::EventObject& Event );
	virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Event );

	//=====  XServiceInfo  ====================================================

    /**	Returns an identifier for the implementation of this object.
    */
	virtual ::rtl::OUString SAL_CALL
    	getImplementationName (void);

    /**	Return whether the specified service is supported by this class.
    */
    virtual sal_Bool SAL_CALL
    	supportsService (const ::rtl::OUString& sServiceName);

    /** Returns a list of all supported services.  In this case that is just
    	the AccessibleContext service.
    */
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
    	getSupportedServiceNames (void);

	//=====  XInterface  ======================================================

    // XInterface is inherited through SwAcessibleContext and
    // XAccessibleSelection. These methods are needed to avoid
    // ambiguities.

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
        const ::com::sun::star::uno::Type& aType );

    virtual void SAL_CALL acquire(  ) throw ()
        { SwAccessibleContext::acquire(); };

    virtual void SAL_CALL release(  ) throw ()
        { SwAccessibleContext::release(); };

	//====== XTypeProvider ====================================================
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//=====  XAccessibleSelection  ============================================

    virtual void SAL_CALL selectAccessibleChild(
        sal_Int32 nChildIndex );

    virtual sal_Bool SAL_CALL isAccessibleChildSelected(
        sal_Int32 nChildIndex );
    virtual void SAL_CALL clearAccessibleSelection(  );
    virtual void SAL_CALL selectAllAccessibleChildren(  );
    virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild(
        sal_Int32 nSelectedChildIndex );

    // --> OD 2004-11-16 #111714# - index has to be treated as global child index.
    virtual void SAL_CALL deselectAccessibleChild(
        sal_Int32 nChildIndex );

    virtual ::com::sun::star::uno::Any SAL_CALL getExtendedAttributes() ;
	//====== thread safe C++ interface ========================================

	// The object is not visible an longer and should be destroyed
	virtual void Dispose( sal_Bool bRecursive = sal_False );

	//=====  XAccessibleComponent  ============================================
	sal_Int32 SAL_CALL getBackground();

	//=====  XAccessibleGetAccFlowTo  ============================================
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >
		SAL_CALL get_AccFlowTo(const ::com::sun::star::uno::Any& rAny, sal_Int32 nType);
};

#endif

/* vim: set noet sw=4 ts=4: */
