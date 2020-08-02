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



#ifndef _FRAMEWORK_SCRIPT_SCRIPTHANDLER_HXX
#define _FRAMEWORK_SCRIPT_SCRIPTHANDLER_HXX

#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/script/provider/XScriptProvider.hpp>

namespace rtl
{
class OUString;
}

namespace com { namespace sun { namespace star {

    namespace document {
        class XScriptInvocationContext;
    }
    namespace uno {
        class Any;
        class XComponentContext;
    }
    namespace lang {
        class XMultiServiceFactory;
        class XSingleServiceFactory;
    }
    namespace frame {
        class XFrame;
        class XModel;
        class XDispatch;
        class XNotifyingDispatch;
        class XDispatchResultListener;
        struct DispatchDescriptor;
    }
    namespace beans {
        struct PropertyValue;
    }
    namespace util {
        struct URL;
    }
} } }

namespace scripting_protocolhandler
{

namespace css = ::com::sun::star;

class ScriptProtocolHandler : 
public ::cppu::WeakImplHelper4< css::frame::XDispatchProvider,
    css::frame::XNotifyingDispatch, css::lang::XServiceInfo, css::lang::XInitialization >
{
private:
    bool m_bInitialised;
    css::uno::Reference < css::uno::XComponentContext >             m_xCtx;
    css::uno::Reference < css::frame::XFrame >                      m_xFrame;
    css::uno::Reference < css::script::provider::XScriptProvider >  m_xScriptProvider;
    css::uno::Reference< css::document::XScriptInvocationContext >  m_xScriptInvocation;

    void createScriptProvider();
    bool getScriptInvocation();

public:
    ScriptProtocolHandler( const css::uno::Reference < 
        css::uno::XComponentContext >& xCtx );
    virtual ~ScriptProtocolHandler();

    /* XServiceInfo */
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& sServiceName )
        throw( css::uno::RuntimeException );
    virtual css::uno::Sequence < ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException );

    /* Helper for XServiceInfo */
    static css::uno::Sequence < ::rtl::OUString > impl_getStaticSupportedServiceNames();
    static ::rtl::OUString impl_getStaticImplementationName();

    /* Helper for registry */
    static css::uno::Reference < css::uno::XInterface > SAL_CALL
    impl_createInstance( 
        const css::uno::Reference< css::uno::XComponentContext >& xCtx )
    throw( css::uno::RuntimeException );

    /* Implementation for XDispatchProvider */
    virtual css::uno::Reference < css::frame::XDispatch > SAL_CALL
    queryDispatch( const css::util::URL& aURL, const ::rtl::OUString& sTargetFrameName,
                   sal_Int32 eSearchFlags ) throw( css::uno::RuntimeException ) ;
    virtual css::uno::Sequence< css::uno::Reference < css::frame::XDispatch > > SAL_CALL
    queryDispatches(
        const css::uno::Sequence < css::frame::DispatchDescriptor >& seqDescriptor )
    throw( css::uno::RuntimeException );

    /* Implementation for X(Notifying)Dispatch */
    virtual void SAL_CALL dispatchWithNotification( 
    const css::util::URL& aURL,
    const css::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArgs,
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& Listener )
    throw ( css::uno::RuntimeException );
    virtual void SAL_CALL dispatch(
        const css::util::URL& aURL,
        const css::uno::Sequence< css::beans::PropertyValue >& lArgs )
        throw ( css::uno::RuntimeException );
    virtual void SAL_CALL addStatusListener(
        const css::uno::Reference< css::frame::XStatusListener >& xControl,
        const css::util::URL& aURL )
        throw ( css::uno::RuntimeException );
    virtual void SAL_CALL removeStatusListener(
        const css::uno::Reference< css::frame::XStatusListener >& xControl,
        const css::util::URL& aURL )
        throw ( css::uno::RuntimeException );

    /* Implementation for XInitialization */
    virtual void SAL_CALL initialize(
        const css::uno::Sequence < css::uno::Any >& aArguments )
        throw ( css::uno::Exception );
};

}
#endif
