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



#ifndef __FRAMEWORK_SERVICES_BACKINGCOMP_HXX_
#define __FRAMEWORK_SERVICES_BACKINGCOMP_HXX_

//__________________________________________
// own includes

#include <threadhelp/threadhelpbase.hxx>
#include <general.h>
#include <stdtypes.h>

//__________________________________________
// interface includes
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XKeyListener.hpp>

#ifndef _COM_SUN_STAR_FAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGETELISTENER_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#endif
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XComponent.hpp>

//__________________________________________
// other includes
#include <cppuhelper/weak.hxx>

//__________________________________________
// definition

namespace framework
{

//__________________________________________
/**
    implements the backing component.

    This component is a special one, which doesn't provide a controller
    nor a model. It supports the following features:
        - Drag & Drop
        - Key Accelerators
        - Simple Menu
        - Progress Bar
        - Background
 */
class BackingComp : public  css::lang::XTypeProvider
                  , public  css::lang::XServiceInfo
                  , public  css::lang::XInitialization
                  , public  css::frame::XController  // => XComponent
                  , public  css::awt::XKeyListener // => XEventListener
                  // attention! Must be the first base class to guarantee right initialize lock ...
                  , private ThreadHelpBase
                  , public  ::cppu::OWeakObject
{
    //______________________________________
    // member

    private:

        /** the global uno service manager.
            Must be used to create own needed services. */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /** reference to the component window. */
        css::uno::Reference< css::awt::XWindow > m_xWindow;

        /** the owner frame of this component. */
        css::uno::Reference< css::frame::XFrame > m_xFrame;

        /** helper for drag&drop. */
        css::uno::Reference< css::datatransfer::dnd::XDropTargetListener > m_xDropTargetListener;

    //______________________________________
    // interface

    public:

                 BackingComp( const css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR );
        virtual ~BackingComp(                                                                    );

        // XInterface
        virtual css::uno::Any SAL_CALL queryInterface( const css::uno::Type& aType );
        virtual void          SAL_CALL acquire       (                             ) throw(                          );
        virtual void          SAL_CALL release       (                             ) throw(                          );

        // XTypeProvide
        virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes           ();
        virtual css::uno::Sequence< sal_Int8 >       SAL_CALL getImplementationId();

        // XServiceInfo
        virtual ::rtl::OUString                       SAL_CALL getImplementationName   (                                     );
        virtual sal_Bool                              SAL_CALL supportsService         ( const ::rtl::OUString& sServiceName );
        virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(                                     );

        // XInitialization
        virtual void SAL_CALL initialize( const css::uno::Sequence< css::uno::Any >& lArgs );

        // XController
        virtual void                                      SAL_CALL attachFrame    ( const css::uno::Reference< css::frame::XFrame >& xFrame   );
        virtual sal_Bool                                  SAL_CALL attachModel    ( const css::uno::Reference< css::frame::XModel >& xModel   );
        virtual sal_Bool                                  SAL_CALL suspend        (       sal_Bool                                   bSuspend );
        virtual css::uno::Any                             SAL_CALL getViewData    (                                                           );
        virtual void                                      SAL_CALL restoreViewData( const css::uno::Any&                             aData    );
        virtual css::uno::Reference< css::frame::XModel > SAL_CALL getModel       (                                                           );
        virtual css::uno::Reference< css::frame::XFrame > SAL_CALL getFrame       (                                                           );

        // XKeyListener
        virtual void SAL_CALL keyPressed ( const css::awt::KeyEvent& aEvent );
        virtual void SAL_CALL keyReleased( const css::awt::KeyEvent& aEvent );

        // XEventListener
        virtual void SAL_CALL disposing( const css::lang::EventObject& aEvent );

        // XComponent
        virtual void SAL_CALL dispose            (                                                                   );
        virtual void SAL_CALL addEventListener   ( const css::uno::Reference< css::lang::XEventListener >& xListener );
        virtual void SAL_CALL removeEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener );

    //______________________________________
    // helper

    public:

        static css::uno::Sequence< ::rtl::OUString >                   SAL_CALL impl_getStaticSupportedServiceNames(                                                                     );
        static ::rtl::OUString                                         SAL_CALL impl_getStaticImplementationName   (                                                                     );
        static css::uno::Reference< css::uno::XInterface >             SAL_CALL impl_createInstance                ( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR );
        static css::uno::Reference< css::lang::XSingleServiceFactory > SAL_CALL impl_createFactory                 ( const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR );
};

} // namespace framework

#endif // __FRAMEWORK_SERVICES_BACKINGCOMP_HXX_
