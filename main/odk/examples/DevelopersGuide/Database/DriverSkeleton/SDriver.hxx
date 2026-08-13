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



#ifndef CONNECTIVITY_SDRIVER_HXX
#define CONNECTIVITY_SDRIVER_HXX

#include <com/sun/star/sdbc/XDriver.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/compbase2.hxx>
#include "SConnection.hxx"

namespace connectivity
{
	namespace skeleton
	{
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL SkeletonDriver_CreateInstance(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory);

		typedef ::cppu::WeakComponentImplHelper2<	::com::sun::star::sdbc::XDriver,
													::com::sun::star::lang::XServiceInfo > ODriver_BASE;

		class SkeletonDriver : public ODriver_BASE
		{
		protected:
			::osl::Mutex				m_aMutex;		// mutex is need to control member access
			OWeakRefArray				m_xConnections;	//	vector containing a list
														//  of all the Connection objects
														//  for this Driver
		public:

			SkeletonDriver();

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
		};
	}

}

#endif // CONNECTIVITY_SDRIVER_HXX
