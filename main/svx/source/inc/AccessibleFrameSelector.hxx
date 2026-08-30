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



#ifndef SVX_ACCESSIBLEFRAMESELECTOR_HXX
#define SVX_ACCESSIBLEFRAMESELECTOR_HXX

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/awt/XFocusListener.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <tools/resary.hxx>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <comphelper/accessibleeventnotifier.hxx>
#include <svx/framebordertype.hxx>

class VclSimpleEvent;
class VclWindowEvent;

namespace svx {

class FrameSelector;

namespace a11y {

// ============================================================================

class AccFrameSelector :
    public ::cppu::WeakImplHelper5<
				::com::sun::star::accessibility::XAccessible,
                ::com::sun::star::accessibility::XAccessibleContext,
                ::com::sun::star::accessibility::XAccessibleComponent,
                ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
                ::com::sun::star::lang::XServiceInfo
                >,
    public Resource
{
public:
    explicit            AccFrameSelector( FrameSelector& rFrameSel, FrameBorderType eBorder );

    virtual             ~AccFrameSelector();

    //XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  );

    //XAccessibleContext
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
    virtual void SAL_CALL addPropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );

    //XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  );
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual sal_Bool SAL_CALL isShowing(  );
    virtual sal_Bool SAL_CALL isVisible(  );
    virtual sal_Bool SAL_CALL isFocusTraversable(  );
    virtual void SAL_CALL addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );
    virtual void SAL_CALL removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );
    virtual void SAL_CALL grabFocus(  );
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  );
    virtual sal_Int32 SAL_CALL getForeground(  );
    virtual sal_Int32 SAL_CALL getBackground(  );

    // XAccessibleEventBroadcaster
    virtual void SAL_CALL addEventListener ( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener);
    virtual void SAL_CALL removeEventListener ( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener);

    //XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    void    Invalidate();
    void    NotifyFocusListeners(sal_Bool bGetFocus);
	void    NotifyAccessibleEvent( const sal_Int16 _nEventId, const ::com::sun::star::uno::Any& _rOldValue, const ::com::sun::star::uno::Any& _rNewValue );

protected:
    DECL_LINK( WindowEventListener, VclSimpleEvent* );

    virtual void    ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );

private:
    void                IsValid();

    FrameSelector*      mpFrameSel;
    ::osl::Mutex        maFocusMutex;
    ::osl::Mutex        maPropertyMutex;

    FrameBorderType     meBorder;

    ::cppu::OInterfaceContainerHelper maFocusListeners;
    ::cppu::OInterfaceContainerHelper maPropertyListeners;

    ResStringArray      maNames;
    ResStringArray      maDescriptions;

    ::comphelper::AccessibleEventNotifier::TClientId    mnClientId;
};

// ============================================================================

} // namespace a11y
} // namespace svx

#endif
