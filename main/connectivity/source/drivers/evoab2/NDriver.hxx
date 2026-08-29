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



#ifndef _CONNECTIVITY_EVOAB_DRIVER_HXX_
#define _CONNECTIVITY_EVOAB_DRIVER_HXX_

#include <com/sun/star/sdbc/XDriver.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/compbase2.hxx>
#include "connectivity/CommonTools.hxx"
#include <osl/module.h>

#define EVOAB_EVOLUTION_SCHEMA	"evolution"
/*In Future, when separate schema is required for ldap, groupwise*/
#define EVOAB_LDAP_SCHEMA	"ldap"
#define EVOAB_GWISE_SCHEMA	"groupwise"

#define EVOAB_DRIVER_IMPL_NAME	"com.sun.star.comp.sdbc.evoab.OEvoabDriver"

namespace connectivity
{
	namespace evoab
	{

		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL OEvoabDriver_CreateInstance(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory);


		typedef ::cppu::WeakComponentImplHelper2<	::com::sun::star::sdbc::XDriver,
								::com::sun::star::lang::XServiceInfo > ODriver_BASE;


		class OEvoabDriver : public ODriver_BASE
		{

		protected:
			::osl::Mutex										m_aMutex;
			connectivity::OWeakRefArray							m_xConnections;
			::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xFactory;

		public:
			OEvoabDriver(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory);
			virtual ~OEvoabDriver();

			// OComponentHelper
			virtual void SAL_CALL disposing(void);

			// XInterface
			static ::rtl::OUString getImplementationName_Static(  );
			static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(  );

			// XServiceInfo
			virtual ::rtl::OUString SAL_CALL getImplementationName(  );
			virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
			virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );


			// XDriver
			virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL connect( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info );
			virtual sal_Bool SAL_CALL acceptsURL( const ::rtl::OUString& url );
			virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sdbc::DriverPropertyInfo > SAL_CALL getPropertyInfo( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info );
			virtual sal_Int32 SAL_CALL getMajorVersion(  );
			virtual sal_Int32 SAL_CALL getMinorVersion(  );

		public:
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        & getMSFactory(void) const { return m_xFactory; }

			// static methods
			static sal_Bool acceptsURL_Stat( const ::rtl::OUString& url );
		};
	}

}
#endif //_CONNECTIVITY_EVOAB_DRIVER_HXX_
