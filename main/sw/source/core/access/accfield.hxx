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

#ifndef _ACCFIELD_HXX
#define _ACCFIELD_HXX

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECONTEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECOMPONENT_HPP_
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEEVENTBROADCASTER_HPP_
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_ILLEGALACCESSIBLECOMPONENTSTATEEXCEPTION_HDL_
#include <com/sun/star/accessibility/IllegalAccessibleComponentStateException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_INDEXOUTOFBOUNDSEXCEPTION_HPP_
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEHYPERLINK_HPP_
#include <com/sun/star/accessibility/XAccessibleHyperlink.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEVALUE_HPP_
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif

class SwAccessibleParagraph;
class SwField;
class SwAccessibleField :
	public ::cppu::WeakImplHelper4<
				::com::sun::star::accessibility::XAccessible,
                ::com::sun::star::accessibility::XAccessibleContext,
                ::com::sun::star::accessibility::XAccessibleComponent,
				::com::sun::star::accessibility::XAccessibleEventBroadcaster
                >
{
	friend class SwAccessibleParagraph;
	friend class SwAccessibleHyperTextData;

protected:
//	sal_uInt16 nHintPos;
	::vos::ORef< SwAccessibleParagraph > m_xPara;
	SwField *m_pSwField;

    sal_uInt32 m_nClientId;   // client id in the AccessibleEventNotifier queue
    sal_Int16 m_nRole;		// immutable outside constructor
//	sal_Int32 nStartIdx;
//	sal_Int32 nEndIdx;
public:
	SwAccessibleField( SwField *pSwFld,SwAccessibleParagraph *p,sal_Int16);

    virtual void SAL_CALL addEventListener(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::accessibility::XAccessibleEventListener >& xListener );
    virtual void SAL_CALL removeEventListener(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

//	const SwTxtAttr *GetTxtAttr() const;
//	void Invalidate();
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
        const ::com::sun::star::uno::Type& aType );

    virtual void SAL_CALL acquire(  ) throw () ;
    virtual void SAL_CALL release(  ) throw () ;

public:
	//=====  XAccessible  =====================================================

    ///	Return the XAccessibleContext.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
    	getAccessibleContext (void);

	//=====  XAccessibleContext  ==============================================

    ///	Return the number of currently visible children.
    virtual sal_Int32 SAL_CALL getAccessibleChildCount (void);

    ///	Return the specified child or NULL if index is invalid.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleChild (long nIndex);

    ///	Return a reference to the parent.
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
    	getAccessibleParent (void);

    ///	Return this objects index among the parents children.
	virtual	sal_Int32 SAL_CALL
    	getAccessibleIndexInParent (void);

    ///	Return this object's role.
	virtual sal_Int16 SAL_CALL
    	getAccessibleRole (void);

    ///	Return this object's description.
	virtual ::rtl::OUString SAL_CALL
    	getAccessibleDescription (void);

    ///	Return the object's current name.
	virtual ::rtl::OUString SAL_CALL
    	getAccessibleName (void);

	///	Return NULL to indicate that an empty relation set.
	virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleRelationSet> SAL_CALL
    	getAccessibleRelationSet (void);

    ///	Return the set of current states.
	virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
    	getAccessibleStateSet (void);

	/**	Return the parents locale or throw exception if this object has no
    	parent yet/anymore.
    */
	virtual ::com::sun::star::lang::Locale SAL_CALL
    	getLocale (void);

	//=====  XAccessibleComponent  ============================================
    virtual sal_Bool SAL_CALL containsPoint(
			const ::com::sun::star::awt::Point& aPoint );

    virtual ::com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint(
				const ::com::sun::star::awt::Point& aPoint );

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds();

    virtual ::com::sun::star::awt::Point SAL_CALL getLocation();

    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen();

    virtual ::com::sun::star::awt::Size SAL_CALL getSize();

    virtual void SAL_CALL grabFocus();

    virtual sal_Int32 SAL_CALL getForeground();
    virtual sal_Int32 SAL_CALL getBackground();


    virtual ::com::sun::star::awt::Rectangle SAL_CALL
        getBoundsImpl(sal_Bool bRelative) ;


};

#endif
