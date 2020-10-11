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



#ifndef __FRAMEWORK_UICONFIGURATION_UICONFIGMANAGERIMPL_HXX_
#define __FRAMEWORK_UICONFIGURATION_UICONFIGMANAGERIMPL_HXX_


/** Attention: stl headers must(!) be included at first. Otherwise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <list>
#include <hash_map>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <accelerators/presethandler.hxx>
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>
#include <uiconfiguration/moduleimagemanager.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>
#include <com/sun/star/ui/XUIConfiguration.hpp>
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#include <com/sun/star/ui/UIElementType.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ui/ConfigurationEvent.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/weak.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <rtl/ustring.hxx>


namespace framework
{
    class UIConfigurationManagerImpl :   public ThreadHelpBase		// Struct for right initialization of mutex member! Must be first of baseclasses.
    {
        public:
            //  XInterface, XTypeProvider, XServiceInfo
            UIConfigurationManagerImpl( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& xServiceManager
                                        , const com::sun::star::uno::Reference< com::sun::star::uno::XInterface >& _xOwner
                                        , bool _bUseDefault);
            ~UIConfigurationManagerImpl();

            // XComponent
            void dispose() throw (::com::sun::star::uno::RuntimeException);
            void addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
            void removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

            // XInitialization
            void initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

            // XUIConfiguration
            void addConfigurationListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationListener >& Listener ) throw (::com::sun::star::uno::RuntimeException);
            void removeConfigurationListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationListener >& Listener ) throw (::com::sun::star::uno::RuntimeException);

            // XUIConfigurationManager
            void reset() throw (::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > > getUIElementsInfo( sal_Int16 ElementType ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > createSettings(  ) throw (::com::sun::star::uno::RuntimeException);
            sal_Bool hasSettings( const ::rtl::OUString& ResourceURL ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > getSettings( const ::rtl::OUString& ResourceURL, sal_Bool bWriteable ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            void replaceSettings( const ::rtl::OUString& ResourceURL, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& aNewData ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);
            void removeSettings( const ::rtl::OUString& ResourceURL ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);
            void insertSettings( const ::rtl::OUString& NewResourceURL, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& aNewData ) throw (::com::sun::star::container::ElementExistException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IllegalAccessException, ::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getImageManager() throw (::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getShortCutManager() throw (::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getEventsManager() throw (::com::sun::star::uno::RuntimeException);

            // XUIConfigurationManagerImpl
            sal_Bool isDefaultSettings( const ::rtl::OUString& ResourceURL ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > getDefaultSettings( const ::rtl::OUString& ResourceURL ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

            // XUIConfigurationPersistence
            void reload() throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            void store() throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            void storeToStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
            sal_Bool isModified() throw (::com::sun::star::uno::RuntimeException);
            sal_Bool isReadOnly() throw (::com::sun::star::uno::RuntimeException);

            // XUIConfigurationStorage
            void setStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage ) throw (::com::sun::star::uno::RuntimeException);
            sal_Bool hasStorage() throw (::com::sun::star::uno::RuntimeException);

            enum Layer
            {
                LAYER_DEFAULT,
                LAYER_USERDEFINED,
                LAYER_COUNT
            };
            
            enum NotifyOp
            {
                NotifyOp_Remove,
                NotifyOp_Insert,
                NotifyOp_Replace
            };

            struct UIElementInfo
            {
                UIElementInfo( const rtl::OUString& rResourceURL, const rtl::OUString& rUIName ) :
                    aResourceURL( rResourceURL), aUIName( rUIName ) {}
                rtl::OUString   aResourceURL;
                rtl::OUString   aUIName;
            };
            
            struct UIElementData
            {
                UIElementData() : bModified( false ), bDefault( true ), bDefaultNode( true ) {};

                rtl::OUString aResourceURL;
                rtl::OUString aName;
                bool          bModified;        // has been changed since last storing
                bool          bDefault;         // default settings
                bool          bDefaultNode;     // this is a default layer element data
                com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess > xSettings;
            };

		    struct UIElementType;
            friend struct UIElementType;
		    typedef ::std::hash_map< rtl::OUString, UIElementData, rtl::OUStringHash, ::std::equal_to< rtl::OUString > > UIElementDataHashMap;
            
            struct UIElementType
            {
                UIElementType() : bModified( false ),
                                  bLoaded( false ), 
                                  bDefaultLayer( false ),
                                  nElementType( ::com::sun::star::ui::UIElementType::UNKNOWN ) {}
                

                bool                                                              bModified;
                bool                                                              bLoaded;
                bool                                                              bDefaultLayer;
                sal_Int16                                                         nElementType;
                UIElementDataHashMap                                              aElementsHashMap;
                com::sun::star::uno::Reference< com::sun::star::embed::XStorage > xStorage;
            };

            typedef ::std::vector< UIElementType > UIElementTypesVector;
            typedef ::std::vector< ::com::sun::star::ui::ConfigurationEvent > ConfigEventNotifyContainer;
            typedef ::std::hash_map< rtl::OUString, UIElementInfo, rtl::OUStringHash, ::std::equal_to< rtl::OUString > > UIElementInfoHashMap;

            // private methods
            void            impl_Initialize();
            void            implts_notifyContainerListener( const ::com::sun::star::ui::ConfigurationEvent& aEvent, NotifyOp eOp );
            void            impl_fillSequenceWithElementTypeInfo( UIElementInfoHashMap& aUIElementInfoCollection, sal_Int16 nElementType );
            void            impl_preloadUIElementTypeList( Layer eLayer, sal_Int16 nElementType );
            UIElementData*  impl_findUIElementData( const rtl::OUString& aResourceURL, sal_Int16 nElementType, bool bLoad = true );
            void            impl_requestUIElementData( sal_Int16 nElementType, Layer eLayer, UIElementData& aUIElementData );
            void            impl_storeElementTypeData( com::sun::star::uno::Reference< com::sun::star::embed::XStorage > xStorage, UIElementType& rElementType, bool bResetModifyState = true );
            void            impl_resetElementTypeData( UIElementType& rUserElementType, UIElementType& rDefaultElementType, ConfigEventNotifyContainer& rRemoveNotifyContainer, ConfigEventNotifyContainer& rReplaceNotifyContainer );
            void            impl_reloadElementTypeData( UIElementType& rUserElementType, UIElementType& rDefaultElementType, ConfigEventNotifyContainer& rRemoveNotifyContainer, ConfigEventNotifyContainer& rReplaceNotifyContainer );

            UIElementTypesVector                                                            m_aUIElements[LAYER_COUNT];
            PresetHandler*                                                                  m_pStorageHandler[::com::sun::star::ui::UIElementType::COUNT];
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >               m_xDefaultConfigStorage;
            com::sun::star::uno::Reference< com::sun::star::embed::XStorage >               m_xUserConfigStorage;
            com::sun::star::uno::Reference< com::sun::star::uno::XInterface >               m_xOwner;
            bool                                                                            m_bUseDefault;
            bool                                                                            m_bReadOnly;
            bool                                                                            m_bInitialized;
            bool                                                                            m_bModified;
            bool                                                                            m_bConfigRead;
            bool                                                                            m_bDisposed;
            rtl::OUString                                                                   m_aXMLPostfix;
            rtl::OUString                                                                   m_aPropUIName;
            rtl::OUString                                                                   m_aPropResourceURL;
            rtl::OUString                                                                   m_aModuleIdentifier;
            rtl::OUString                                                                   m_aModuleShortName;
            com::sun::star::uno::Reference< com::sun::star::embed::XTransactedObject >      m_xUserRootCommit;
            com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
            ::cppu::OMultiTypeInterfaceContainerHelper                                      m_aListenerContainer;   /// container for ALL Listener
            com::sun::star::uno::Reference< com::sun::star::uno::XInterface >               m_xAccConfig;
            com::sun::star::uno::Reference< com::sun::star::lang::XComponent >              m_xModuleImageManager;
   };
}

#endif // __FRAMEWORK_UICONFIGURATION_UICONFIGMANAGERIMPL_HXX_
