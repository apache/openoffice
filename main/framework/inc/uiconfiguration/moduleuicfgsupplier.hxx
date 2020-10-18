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



#ifndef __FRAMEWORK_UICONFIGURATION_MODULEUICFGSUPPLIER_HXX_
#define __FRAMEWORK_UICONFIGURATION_MODULEUICFGSUPPLIER_HXX_


/** Attention: stl headers must(!) be included at first. Otherwise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <list>
#include <hash_map>

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
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/weak.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <rtl/ustring.hxx>


namespace framework
{
    class ModuleUIConfigurationManagerSupplier : public com::sun::star::lang::XTypeProvider	,
                                                 public com::sun::star::lang::XServiceInfo	,
                                                 public com::sun::star::lang::XComponent    ,
                                                 public ::com::sun::star::ui::XModuleUIConfigurationManagerSupplier      ,
                                                 private ThreadHelpBase						,	// Struct for right initialization of mutex member! Must be first of baseclasses.
							                     public ::cppu::OWeakObject
    {
        public:
            //  XInterface, XTypeProvider, XServiceInfo
		    FWK_DECLARE_XINTERFACE
		    FWK_DECLARE_XTYPEPROVIDER
		    DECLARE_XSERVICEINFO

            ModuleUIConfigurationManagerSupplier( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );
            virtual ~ModuleUIConfigurationManagerSupplier();

            // XComponent
            virtual void SAL_CALL dispose() 
                throw (::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) 
                throw (::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) 
                throw (::com::sun::star::uno::RuntimeException);

            // XModuleUIConfigurationManagerSupplier
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationManager > SAL_CALL getUIConfigurationManager( const ::rtl::OUString& ModuleIdentifier )
                throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);

        private:
            typedef ::std::hash_map< rtl::OUString, com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationManager >, rtl::OUStringHash, ::std::equal_to< rtl::OUString > > ModuleToModuleCfgMgr;

//TODO_AS            void impl_initStorages();

            // private methods
            ModuleToModuleCfgMgr                                                                m_aModuleToModuleUICfgMgrMap;
            bool                                                                                m_bDisposed;
// TODO_AS            bool                                                                                m_bInit;
            rtl::OUString                                                                       m_aDefaultConfigURL;
            rtl::OUString                                                                       m_aUserConfigURL;
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >                   m_xDefaultCfgRootStorage;
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >                   m_xUserCfgRootStorage;
            com::sun::star::uno::Reference< com::sun::star::embed::XTransactedObject >          m_xUserRootCommit;
            com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager >   m_xModuleMgr;
            com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >        m_xServiceManager;
            ::cppu::OMultiTypeInterfaceContainerHelper                                          m_aListenerContainer;   /// container for ALL Listener
   };
}

#endif // __FRAMEWORK_UICONFIGURATION_MODULEUICFGSUPPLIER_HXX_
