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



#ifndef SD_SLIDESORTER_SLIDE_SORTER_SERVICE_HXX
#define SD_SLIDESORTER_SLIDE_SORTER_SERVICE_HXX

#include "SlideSorter.hxx"

#include "tools/PropertySet.hxx"
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/drawing/SlideSorter.hpp>
#include <com/sun/star/drawing/XDrawView.hpp>
#include <com/sun/star/drawing/framework/XView.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase3.hxx>
#include <cppuhelper/propshlp.hxx>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace css = ::com::sun::star;

namespace sd { namespace slidesorter {

namespace {
    typedef ::cppu::WeakComponentImplHelper3 <
        css::drawing::XSlideSorterBase,
        css::lang::XInitialization,
        css::awt::XWindowListener
    > SlideSorterServiceInterfaceBase;
}


/** Implementation of the com.sun.star.drawing.SlideSorter service.
*/
class SlideSorterService
    : private ::boost::noncopyable,
      protected ::cppu::BaseMutex,
      public SlideSorterServiceInterfaceBase
{
public:
    explicit SlideSorterService (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext);
    virtual ~SlideSorterService (void);
    virtual void SAL_CALL disposing (void);


    // XInitialization

    virtual void SAL_CALL initialize (const css::uno::Sequence<css::uno::Any>& rArguments);


    // XResourceId

    css::uno::Reference<css::drawing::framework::XResourceId> SAL_CALL getResourceId (void);

    sal_Bool SAL_CALL isAnchorOnly (void);


    // XWindowListener

    virtual void SAL_CALL windowResized (const css::awt::WindowEvent& rEvent);

    virtual void SAL_CALL windowMoved (const css::awt::WindowEvent& rEvent);

    virtual void SAL_CALL windowShown (const css::lang::EventObject& rEvent);

    virtual void SAL_CALL windowHidden (const css::lang::EventObject& rEvent);


    // lang::XEventListener
    virtual void SAL_CALL disposing (const css::lang::EventObject& rEvent);


    // XDrawView

    virtual void SAL_CALL setCurrentPage(
        const css::uno::Reference<css::drawing::XDrawPage>& rxSlide);

    virtual css::uno::Reference<css::drawing::XDrawPage> SAL_CALL getCurrentPage (void);


    // Attributes

    virtual css::uno::Reference<css::container::XIndexAccess> SAL_CALL getDocumentSlides (void);

    virtual void SAL_CALL setDocumentSlides (
        const css::uno::Reference<css::container::XIndexAccess >& rxSlides);

    virtual sal_Bool SAL_CALL getIsHighlightCurrentSlide (void);

    virtual void SAL_CALL setIsHighlightCurrentSlide (::sal_Bool bIsHighlightCurrentSlide);

    virtual sal_Bool SAL_CALL getIsShowSelection (void);

    virtual void SAL_CALL setIsShowSelection (sal_Bool bIsShowSelection);

    virtual sal_Bool SAL_CALL getIsCenterSelection (void);

    virtual void SAL_CALL setIsCenterSelection (sal_Bool bIsCenterSelection);

    virtual sal_Bool SAL_CALL getIsSuspendPreviewUpdatesDuringFullScreenPresentation (void);

    virtual void SAL_CALL setIsSuspendPreviewUpdatesDuringFullScreenPresentation (
        sal_Bool bIsSuspendPreviewUpdatesDuringFullScreenPresentation);

    virtual sal_Bool SAL_CALL getIsOrientationVertical (void);

    virtual void SAL_CALL setIsOrientationVertical (sal_Bool bIsOrientationVertical);

    virtual sal_Bool SAL_CALL getIsSmoothScrolling (void);

    virtual void SAL_CALL setIsSmoothScrolling (sal_Bool bIsOrientationVertical);

    virtual css::util::Color SAL_CALL getBackgroundColor (void);

    virtual void SAL_CALL setBackgroundColor (css::util::Color aBackgroundColor);

    virtual css::util::Color SAL_CALL getTextColor (void);

    virtual void SAL_CALL setTextColor (css::util::Color aTextColor);

    virtual css::util::Color SAL_CALL getSelectionColor (void);

    virtual void SAL_CALL setSelectionColor (css::util::Color aSelectionColor);

    virtual css::util::Color SAL_CALL getHighlightColor (void);

    virtual void SAL_CALL setHighlightColor (css::util::Color aHighlightColor);

    virtual sal_Bool SAL_CALL getIsUIReadOnly (void);

    virtual void SAL_CALL setIsUIReadOnly (sal_Bool bIsUIReadOnly);

    virtual sal_Bool SAL_CALL getIsShowFocus (void);

    virtual void SAL_CALL setIsShowFocus (sal_Bool bIsShowFocus);

private:
    ::boost::shared_ptr<SlideSorter> mpSlideSorter;
    css::uno::Reference<css::drawing::framework::XResourceId> mxViewId;
    css::uno::Reference<css::awt::XWindow> mxParentWindow;
    ::boost::scoped_ptr<cppu::IPropertyArrayHelper> mpPropertyArrayHelper;

    void Resize (void);

    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void);
};

} } // end of namespace ::sd::slidesorter

#endif
