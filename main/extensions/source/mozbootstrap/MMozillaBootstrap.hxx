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


#ifndef _SMOZILLABOOTSTRAP_HXX
#define _SMOZILLABOOTSTRAP_HXX

#include <com/sun/star/mozilla/XMozillaBootstrap.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/compbase2.hxx>
#include <osl/module.h>


#define MOZAB_MozillaBootstrap_IMPL_NAME "com.sun.star.comp.mozilla.MozillaBootstrap"

namespace connectivity
{
    namespace mozab
    {
        typedef ::cppu::WeakComponentImplHelper2<    ::com::sun::star::mozilla::XMozillaBootstrap,
                                                    ::com::sun::star::lang::XServiceInfo > OMozillaBootstrap_BASE;
        class ProfileAccess;
        class ProfileManager;
        class MozillaBootstrap : public OMozillaBootstrap_BASE
        {
        protected:
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xMSFactory;

            ::osl::Mutex                m_aMutex;        // mutex is need to control member access
            virtual ~MozillaBootstrap();
        protected:
            ProfileAccess * m_ProfileAccess;
            ProfileManager    * m_ProfileManager;
        public:

            void Init();
            MozillaBootstrap(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory);

            // OComponentHelper
            virtual void SAL_CALL disposing(void);
            // XInterface
            static ::rtl::OUString getImplementationName_Static(  );
            static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(  );

            // XServiceInfo
            virtual ::rtl::OUString SAL_CALL getImplementationName(  );
            virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
            virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

            // XMozillaBootstrap

            // XProfileDiscover
            virtual ::sal_Int32 SAL_CALL getProfileCount( ::com::sun::star::mozilla::MozillaProductType product);
            virtual ::sal_Int32 SAL_CALL getProfileList( ::com::sun::star::mozilla::MozillaProductType product, ::com::sun::star::uno::Sequence< ::rtl::OUString >& list );
            virtual ::rtl::OUString SAL_CALL getDefaultProfile( ::com::sun::star::mozilla::MozillaProductType product );
            virtual ::rtl::OUString SAL_CALL getProfilePath( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName );
            virtual ::sal_Bool SAL_CALL isProfileLocked( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName );
            virtual ::sal_Bool SAL_CALL getProfileExists( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName );

            // XProfileManager
            virtual ::sal_Int32 SAL_CALL bootupProfile( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName );
            virtual ::sal_Int32 SAL_CALL shutdownProfile(  );
            virtual ::com::sun::star::mozilla::MozillaProductType SAL_CALL getCurrentProduct(  );
            virtual ::rtl::OUString SAL_CALL getCurrentProfile(  );
            virtual ::sal_Bool SAL_CALL isCurrentProfileLocked(  );
            virtual ::rtl::OUString SAL_CALL setCurrentProfile( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName );

            // XProxyRunner
            virtual ::sal_Int32 SAL_CALL Run( const ::com::sun::star::uno::Reference< ::com::sun::star::mozilla::XCodeProxy >& aCode );

            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        & getMSFactory(void) const { return m_xMSFactory; }

        };
    }

}

#endif // _SMozillaBootstrap_HXX
