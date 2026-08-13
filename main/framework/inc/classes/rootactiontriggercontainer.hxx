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



#ifndef __FRAMEWORK_CLASSES_ROOTACTIONTRIGGERCONTAINER_HXX_
#define __FRAMEWORK_CLASSES_ROOTACTIONTRIGGERCONTAINER_HXX_

#include <helper/propertysetcontainer.hxx>
#include <vcl/menu.hxx>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <framework/fwedllapi.h>

#define IMPLEMENTATIONNAME_ROOTACTIONTRIGGERCONTAINER   "com.sun.star.comp.ui.RootActionTriggerContainer"


namespace framework
{

class FWE_DLLPUBLIC RootActionTriggerContainer :  public PropertySetContainer,
                                    public com::sun::star::lang::XMultiServiceFactory,
                                    public com::sun::star::lang::XServiceInfo,
                                    public com::sun::star::lang::XUnoTunnel,
                                    public com::sun::star::lang::XTypeProvider,
                                    public com::sun::star::container::XNamed
{
    public:
        RootActionTriggerContainer( const Menu* pMenu, const ::rtl::OUString* pMenuIdentifier, const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );
        virtual ~RootActionTriggerContainer();

        const Menu* GetMenu();

        ::com::sun::star::uno::Sequence< sal_Int8 > GetUnoTunnelId() const;

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
        virtual void SAL_CALL acquire() throw ();
        virtual void SAL_CALL release() throw ();

        // XMultiServiceFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstance( const ::rtl::OUString& aServiceSpecifier );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArguments( const ::rtl::OUString& ServiceSpecifier, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Arguments );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableServiceNames();

        // XIndexContainer
        virtual void SAL_CALL insertByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element );

        virtual void SAL_CALL removeByIndex( sal_Int32 Index );

        // XIndexReplace
        virtual void SAL_CALL replaceByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element );

        // XIndexAccess
        virtual sal_Int32 SAL_CALL getCount();

        virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

        // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType();

        virtual sal_Bool SAL_CALL hasElements();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  );
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

        // XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

        // XTypeProvider
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

        // XNamed
        virtual ::rtl::OUString SAL_CALL getName(  );
        virtual void SAL_CALL setName( const ::rtl::OUString& aName );

    private:
        void FillContainer();

        sal_Bool    m_bContainerCreated;
        sal_Bool    m_bContainerChanged;
        sal_Bool    m_bInContainerCreation;
        const Menu* m_pMenu;
        const ::rtl::OUString* m_pMenuIdentifier;
};

}

#endif // __FRAMEWORK_CLASSES_ROOTACTIONTRIGGERCONTAINER_HXX_
