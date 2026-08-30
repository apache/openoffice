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



#ifndef SD_PRESENTER_PRESENTER_HELPER_HXX
#define SD_PRESENTER_PRESENTER_HELPER_HXX

#include <com/sun/star/drawing/XPresenterHelper.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase2.hxx>
#include <svtools/filter.hxx>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace css = ::com::sun::star;

namespace sd { namespace presenter {

namespace {
    typedef ::cppu::WeakComponentImplHelper2<
        css::lang::XInitialization,
        css::drawing::XPresenterHelper
    > PresenterHelperInterfaceBase;
}

/** Implementation of the XPresenterHelper interface: functionality that can
    not be implemented in an extension.
*/
class PresenterHelper
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public PresenterHelperInterfaceBase
{
public:
    PresenterHelper (const css::uno::Reference<css::uno::XComponentContext>& rxContext);
    virtual ~PresenterHelper (void);

    // XInitialize

    virtual void SAL_CALL initialize (const css::uno::Sequence<css::uno::Any>& rArguments);


    // XPresenterHelper

    virtual css::uno::Reference<css::awt::XWindow> SAL_CALL createWindow (
        const css::uno::Reference<css::awt::XWindow>& rxParentWindow,
        sal_Bool bCreateSystemChildWindow,
        sal_Bool bInitiallyVisible,
        sal_Bool bEnableChildTransparentMode,
        sal_Bool bEnableParentClip);

    virtual css::uno::Reference<css::rendering::XCanvas> SAL_CALL createSharedCanvas (
        const css::uno::Reference<css::rendering::XSpriteCanvas>& rxUpdateCanvas,
        const css::uno::Reference<css::awt::XWindow>& rxUpdateWindow,
        const css::uno::Reference<css::rendering::XCanvas>& rxSharedCanvas,
        const css::uno::Reference<css::awt::XWindow>& rxSharedWindow,
        const css::uno::Reference<css::awt::XWindow>& rxWindow);

    virtual css::uno::Reference<css::rendering::XCanvas> SAL_CALL createCanvas (
        const css::uno::Reference<css::awt::XWindow>& rxWindow,
        sal_Int16 nRequestedCanvasFeatures,
        const ::rtl::OUString& rsOptionalCanvasServiceName);

    virtual void SAL_CALL toTop (
        const css::uno::Reference<css::awt::XWindow>& rxWindow);

    virtual css::uno::Reference<css::rendering::XBitmap> SAL_CALL loadBitmap (
        const ::rtl::OUString& rsURL,
        const css::uno::Reference<css::rendering::XCanvas>& rxCanvas);

    virtual void SAL_CALL captureMouse (const css::uno::Reference<css::awt::XWindow>& rxWindow);

    virtual void SAL_CALL releaseMouse (const css::uno::Reference<css::awt::XWindow>& rxWindow);

    virtual css::awt::Rectangle SAL_CALL getWindowExtentsRelative (
        const css::uno::Reference<css::awt::XWindow>& rxChildWindow,
        const css::uno::Reference<css::awt::XWindow>& rxParentWindow);

private:
    css::uno::Reference<css::uno::XComponentContext> mxComponentContext;
    ::boost::scoped_ptr<GraphicFilter> mpGraphicFilter;
};

} } // end of namespace ::sd::presenter

#endif
