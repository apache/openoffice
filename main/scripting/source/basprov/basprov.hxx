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



#ifndef SCRIPTING_BASPROV_HXX
#define SCRIPTING_BASPROV_HXX

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/script/XLibraryContainer.hpp>
#include <com/sun/star/script/browse/XBrowseNode.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/document/XScriptInvocationContext.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/implbase4.hxx>

class BasicManager;


//.........................................................................
namespace basprov
{
//.........................................................................

    //	----------------------------------------------------
    //	class BasicProviderImpl
    //	----------------------------------------------------

    typedef ::cppu::WeakImplHelper4<
        ::com::sun::star::lang::XServiceInfo,
        ::com::sun::star::lang::XInitialization,
        ::com::sun::star::script::provider::XScriptProvider,
        ::com::sun::star::script::browse::XBrowseNode > BasicProviderImpl_BASE;


    class BasicProviderImpl : public BasicProviderImpl_BASE
    {
    private:
        BasicManager*   m_pAppBasicManager;
        BasicManager*   m_pDocBasicManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XLibraryContainer >             m_xLibContainerApp;
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XLibraryContainer >             m_xLibContainerDoc;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >                m_xContext;
        ::com::sun::star::uno::Reference< ::com::sun::star::document::XScriptInvocationContext >    m_xInvocationContext;
        ::rtl::OUString  m_sScriptingContext;
        bool m_bIsAppScriptCtx;
        bool m_bIsUserCtx;
        ::rtl::OUString m_sCtxLocation;

        bool isLibraryShared(
            const ::com::sun::star::uno::Reference< ::com::sun::star::script::XLibraryContainer >& rxLibContainer,
            const ::rtl::OUString& rLibName );

    public:
        BasicProviderImpl(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext );
        virtual ~BasicProviderImpl();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  );
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

        // XScriptProvider
        virtual ::com::sun::star::uno::Reference < ::com::sun::star::script::provider::XScript > SAL_CALL getScript(
            const ::rtl::OUString& scriptURI );

        // XBrowseNode
        virtual ::rtl::OUString SAL_CALL getName(  );
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::script::browse::XBrowseNode > > SAL_CALL getChildNodes(  );
        virtual sal_Bool SAL_CALL hasChildNodes(  );
        virtual sal_Int16 SAL_CALL getType(  );
    };

//.........................................................................
}	// namespace basprov
//.........................................................................

#endif // SCRIPTING_BASPROV_HXX
