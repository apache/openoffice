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



#ifndef __FRAMEWORK_UIFACTORY_UIELEMENTFACTORYMANAGER_HXX_
#define __FRAMEWORK_UIFACTORY_UIELEMENTFACTORYMANAGER_HXX_

/** Attention: stl headers must(!) be included at first. Otherwise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <list>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/XUIElementFactory.hpp>
#include <com/sun/star/ui/XUIElementFactoryRegistration.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include "com/sun/star/frame/XModuleManager.hpp"

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <rtl/ustring.hxx>

namespace framework
{

    class ConfigurationAccess_FactoryManager : // interfaces
                                                    // baseclasses
							                        // Order is necessary for right initialization!
                                                    private ThreadHelpBase                           ,
                                                    public  ::cppu::WeakImplHelper1< ::com::sun::star::container::XContainerListener>
{
    public:
                      ConfigurationAccess_FactoryManager( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager,const ::rtl::OUString& _sRoot );
        virtual       ~ConfigurationAccess_FactoryManager();

        void          readConfigurationData();

        rtl::OUString                           getFactorySpecifierFromTypeNameModule( const rtl::OUString& rType, const rtl::OUString& rName, const rtl::OUString& rModule ) const;
        void                                    addFactorySpecifierToTypeNameModule( const rtl::OUString& rType, const rtl::OUString& rName, const rtl::OUString& rModule, const rtl::OUString& aServiceSpecifier );
        void                                    removeFactorySpecifierFromTypeNameModule( const rtl::OUString& rType, const rtl::OUString& rName, const rtl::OUString& rModule );
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > >   getFactoriesDescription() const;

        // container.XContainerListener
    virtual void SAL_CALL elementInserted( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL elementRemoved( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL elementReplaced( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);

    // lang.XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);
 
    private:
        class FactoryManagerMap : public std::hash_map< rtl::OUString,
                                                     rtl::OUString,
                                                     rtl::OUStringHash,
                                                     ::std::equal_to< ::rtl::OUString > >
        {
            inline void free()
            {
                FactoryManagerMap().swap( *this );
            }
        };
    
        sal_Bool impl_getElementProps( const ::com::sun::star::uno::Any& rElement, rtl::OUString& rType, rtl::OUString& rName, rtl::OUString& rModule, rtl::OUString& rServiceSpecifier ) const;

        rtl::OUString                     m_aPropType;
        rtl::OUString                     m_aPropName;
        rtl::OUString                     m_aPropModule;
        rtl::OUString                     m_aPropFactory;
        ::rtl::OUString                   m_sRoot;
        FactoryManagerMap                 m_aFactoryManagerMap;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xConfigProvider;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >     m_xConfigAccess;
        sal_Bool                          m_bConfigAccessInitialized;
        bool							  m_bConfigDirty;
};


class UIElementFactoryManager :  private ThreadHelpBase							                    ,	// Struct for right initialization of mutex member! Must be first of baseclasses.
                                 public ::cppu::WeakImplHelper3< ::com::sun::star::lang::XServiceInfo,
                                                                 ::com::sun::star::ui::XUIElementFactory,
                                                                 ::com::sun::star::ui::XUIElementFactoryRegistration>
{
	public:
		UIElementFactoryManager( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
		virtual ~UIElementFactoryManager();

        //  XInterface, XTypeProvider, XServiceInfo
		DECLARE_XSERVICEINFO

        // XUIElementFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIElement > SAL_CALL createUIElement( const ::rtl::OUString& ResourceURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Args ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

        // XUIElementFactoryRegistration
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > > SAL_CALL getRegisteredFactories(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIElementFactory > SAL_CALL getFactory( const ::rtl::OUString& ResourceURL, const ::rtl::OUString& ModuleIdentifier ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL registerFactory( const ::rtl::OUString& aType, const ::rtl::OUString& aName, const ::rtl::OUString& aModuleIdentifier, const ::rtl::OUString& aFactoryImplementationName ) throw (::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL deregisterFactory( const ::rtl::OUString& aType, const ::rtl::OUString& aName, const ::rtl::OUString& aModuleIdentifier ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);
        
    private:

        sal_Bool                                                                            m_bConfigRead;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager > m_xModuleManager;
        ConfigurationAccess_FactoryManager*                                        m_pConfigAccess;
};

} // namespace framework

#endif // __FRAMEWORK_UIFACTORY_UIELEMENTFACTORYMANAGER_HXX_
