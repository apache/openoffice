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



#ifndef __FRAMEWORK_UIELEMENT_MENUBARWRAPPER_HXX_
#define __FRAMEWORK_UIELEMENT_MENUBARWRAPPER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <helper/uiconfigelementwrapperbase.hxx>
#include <uielement/menubarmanager.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#ifndef __COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________

#include <hash_map>

namespace framework
{

class MenuBarWrapper : public UIConfigElementWrapperBase,
                       public ::com::sun::star::container::XNameAccess

{
    public:
        // #110897#
		MenuBarWrapper(
			const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
        virtual ~MenuBarWrapper();

        //---------------------------------------------------------------------------------------------------------
        //	XInterface, XTypeProvider
        //---------------------------------------------------------------------------------------------------------
        FWK_DECLARE_XINTERFACE
        FWK_DECLARE_XTYPEPROVIDER

		MenuBarManager* GetMenuBarManager() const { return static_cast< MenuBarManager* >( m_xMenuBarManager.get() ); }
        void            InvalidatePopupControllerCache() { m_bRefreshPopupControllerCache = sal_True; }

        // XComponent
        virtual void SAL_CALL dispose();

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

        // XUIElement
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getRealInterface();

        // XUIElementSettings
        virtual void SAL_CALL updateSettings(  );

        // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
        virtual ::sal_Bool SAL_CALL hasElements();

        // XNameAccess
        virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
        virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

    private:
        virtual void impl_fillNewData();
        void fillPopupControllerCache();

        sal_Bool                                                                                m_bRefreshPopupControllerCache : 1;
        com::sun::star::uno::Reference< com::sun::star::lang::XComponent >                      m_xMenuBarManager;
        PopupControllerCache                                                                    m_aPopupControllerCache;
};

} // namespace framework

#endif // __FRAMEWORK_UIELEMENT_MENUBARWRAPPER_HXX_
