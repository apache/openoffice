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



#ifndef SD_ACCESSIBILITY_ACCESSIBLE_SLIDE_VIEW_HXX
#define SD_ACCESSIBILITY_ACCESSIBLE_SLIDE_VIEW_HXX

#include <cppuhelper/implbase6.hxx>
#include <cppuhelper/implbase9.hxx>
//#include <cppuhelper/implbase7.hxx>
#include "SlideView.hxx"
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <com/sun/star/accessibility/XAccessibleExtendedAttributes.hpp>
#include <vector>

class SdDrawDocument;
class AccessibleSlideView;

namespace sd {
class SlideView;
class Window;
}

// -----------------------------
// - AccessibleSlideViewObject -
// -----------------------------

class AccessibleSlideViewObject : public ::cppu::WeakImplHelper6<
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::accessibility::XAccessible,
    ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
    ::com::sun::star::accessibility::XAccessibleContext,
    ::com::sun::star::accessibility::XAccessibleComponent,
    ::com::sun::star::lang::XServiceInfo >
{
private:

    ::osl::Mutex                                                                                                            maMutex;
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >                                mxParent;
    AccessibleSlideView*                                                                                                    mpManager;
    /// client id in the AccessibleEventNotifier queue
    sal_uInt32                                                                                                              mnClientId;
	sal_uInt16		                                                                                                        mnPage;
	sal_Bool		                                                                                                        mbVisible;
    sal_Bool                                                                                                                mbValid;

private:

	// Misc
    static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId();

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rId );

    // XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  );

    // XAccessibleEventBroadcaster
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

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
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

    // XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual void SAL_CALL grabFocus(  );

    virtual sal_Int32 SAL_CALL getForeground (void);

    virtual sal_Int32 SAL_CALL getBackground (void);

	//=====  XServiceInfo  ====================================================

    /**	Returns an identifier for the implementation of this object.
    */
	virtual ::rtl::OUString SAL_CALL
    	getImplementationName (void);

    /**	Return whether the specified service is supported by this class.
    */
    virtual sal_Bool SAL_CALL
    	supportsService (const ::rtl::OUString& sServiceName);

    /** Returns a list of all supported services.
    */
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
    	getSupportedServiceNames (void);

public:

    static AccessibleSlideViewObject* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxData ) throw();

public:

					            AccessibleSlideViewObject( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& rxParent, sal_uInt16 nPage, sal_Bool bVisible );
					            ~AccessibleSlideViewObject();

    void                        FireAccessibleEvent( short nEventId, const ::com::sun::star::uno::Any& rOldValue, const ::com::sun::star::uno::Any& rNewValue );

    /** This method acts like a dispose call.  It sends a disposing to all
        of its listeners.  It may be called twice.
    */
    void Destroyed (void);

    sal_uInt16                  GetPageNum() const { return mnPage; }

    void			            SetVisible( sal_Bool bVisible );
	sal_Bool		            IsVisible() const;
};

// -----------------------
// - AccessibleSlideView -
// -----------------------

class AccessibleSlideView : public ::cppu::WeakImplHelper9<
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::accessibility::XAccessible,
    ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
    ::com::sun::star::accessibility::XAccessibleContext,
    ::com::sun::star::accessibility::XAccessibleComponent,
    ::com::sun::star::accessibility::XAccessibleSelection,
    ::com::sun::star::lang::XServiceInfo
	,::com::sun::star::accessibility::XAccessibleExtendedAttributes
	,::com::sun::star::awt::XFocusListener >
{
public:

    static AccessibleSlideView* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxData ) throw();

    AccessibleSlideView(
        SdDrawDocument& rDoc,
        ::sd::SlideView& rView,
        ::sd::Window& rParentWindow);
    virtual ~AccessibleSlideView (void);

    void                        FireAccessibleEvent( short nEventId, const ::com::sun::star::uno::Any& rOldValue, const ::com::sun::star::uno::Any& rNewValue );

    /** This method acts like a dispose call.  It sends a disposing to all
        of its listeners.  It may be called twice.
    */
    void Destroyed (void);

    SdDrawDocument*             GetDrawDocument() const { return mpDoc; }
    ::sd::SlideView* GetSlideView() const { return mpView; }
    ::sd::Window* GetParentWindow() const { return mpParentWindow; }

	void			            SetPageVisible( sal_uInt16 nPage, sal_Bool bVisible );
	void			            Reset();
    void                        FocusHasChanged( sal_uInt16 nOldFocusPage, sal_uInt16 nNewFocusPage );
	void 			SelectionHasChanged (sal_uInt16 nPage, sal_Bool bSelect );
	//=====  XFocusListener  =================================================
	virtual void SAL_CALL focusGained (const ::com::sun::star::awt::FocusEvent& e);
	virtual void SAL_CALL focusLost (const ::com::sun::star::awt::FocusEvent& e);
	//=====  lang::XEventListener  ============================================
	virtual void SAL_CALL disposing (const struct com::sun::star::lang::EventObject &);
	// This method is called from the component helper base class while disposing.
	virtual void SAL_CALL disposing (void);
private:
    ::osl::Mutex maMutex;
	::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > >               maSlidePageObjects;
    SdDrawDocument* mpDoc;
	::sd::SlideView* mpView;
    ::sd::Window* mpParentWindow;
     /// client id in the AccessibleEventNotifier queue
    sal_uInt32 mnClientId;
	sal_uInt32	nFocusPageIndex;
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow>			mxWindow;

    // internal
	static const ::com::sun::star::uno::Sequence< sal_Int8 >&                                   getUnoTunnelId();
    sal_Int32                                                                                   ImplGetVisibleChildCount() const;
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >    ImplGetVisibleChild( sal_Int32 nVisibleChild ) const;

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rId );

    // XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  );

    // XAccessibleEventBroadcaster
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener );

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
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

    // XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual void SAL_CALL grabFocus(  );

    virtual sal_Int32 SAL_CALL getForeground (void);

    virtual sal_Int32 SAL_CALL getBackground (void);

    // XAccessibleSelection
    virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex );
    virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex );
    virtual void SAL_CALL clearAccessibleSelection(  );
    virtual void SAL_CALL selectAllAccessibleChildren(  );
    virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex );
    virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nSelectedChildIndex );

	//=====  XServiceInfo  ====================================================

	//----------------------------xAttribute----------------------------
	virtual com::sun::star::uno::Any SAL_CALL getExtendedAttributes() ;
    /**	Returns an identifier for the implementation of this object.
    */
	virtual ::rtl::OUString SAL_CALL
    	getImplementationName (void);

    /**	Return whether the specified service is supported by this class.
    */
    virtual sal_Bool SAL_CALL
    	supportsService (const ::rtl::OUString& sServiceName);

    /** Returns a list of all supported services.
    */
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
    	getSupportedServiceNames (void);

};

#endif
