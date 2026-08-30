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


#ifndef CONNECTIVITY_POOLCOLLECTION_HXX
#define CONNECTIVITY_POOLCOLLECTION_HXX

#include <cppuhelper/implbase5.hxx>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/sdbc/XDriverManager.hpp>
#include <com/sun/star/sdbc/XDriver.hpp>
#include <com/sun/star/sdbc/XDriverAccess.hpp>
#include <com/sun/star/sdbc/XPooledConnection.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XTerminateListener.hpp>
#include <com/sun/star/reflection/XProxyFactory.hpp>
#include <comphelper/stl_types.hxx>
#include <osl/mutex.hxx>

namespace connectivity
{
	class OConnectionPool;
	//==========================================================================
	//= OPoolCollection - the one-instance service for PooledConnections
	//= manages the active connections and the connections in the pool
	//==========================================================================
	typedef	::cppu::WeakImplHelper5<	::com::sun::star::sdbc::XDriverManager,
										::com::sun::star::sdbc::XDriverAccess,
										::com::sun::star::lang::XServiceInfo,
										::com::sun::star::frame::XTerminateListener,
										::com::sun::star::beans::XPropertyChangeListener
										>	OPoolCollection_Base;

	/// OPoolCollection: control the whole connection pooling for oo
	class OPoolCollection : public OPoolCollection_Base
	{

		//==========================================================================
		typedef ::comphelper::OInterfaceCompare< ::com::sun::star::sdbc::XDriver >	ODriverCompare;
		DECLARE_STL_USTRINGACCESS_MAP(OConnectionPool*,	OConnectionPools);

		DECLARE_STL_MAP(
				::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver >,
				::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XDriver >,
				ODriverCompare,
				MapDriver2DriverRef );

		MapDriver2DriverRef																	m_aDriverProxies;
		::osl::Mutex																		m_aMutex;
		OConnectionPools																	m_aPools;		   // the driver pools
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >	m_xServiceFactory;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriverManager >			m_xManager;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriverAccess >			m_xDriverAccess;
		::com::sun::star::uno::Reference< ::com::sun::star::reflection::XProxyFactory >		m_xProxyFactory;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >				m_xConfigNode;		// config node for generel connection pooling
		::com::sun::star::uno::Reference< ::com::sun::star::frame::XDesktop>				m_xDesktop;

	private:
		OPoolCollection();							// never implemented
		OPoolCollection(const OPoolCollection&);	// never implemented
		int operator= (const OPoolCollection&);			// never implemented

		OPoolCollection(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&	_rxFactory);

		// some configuration helper methods
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createWithServiceFactory(const ::rtl::OUString& _rPath) const;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getConfigPoolRoot();
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createWithProvider(	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxConfProvider,
																									const ::rtl::OUString& _rPath) const;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > openNode(	const ::rtl::OUString& _rPath,
																						const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _xTreeNode) const throw();
		sal_Bool isPoolingEnabled();
		sal_Bool isDriverPoolingEnabled(const ::rtl::OUString& _sDriverImplName,
										::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxDriverNode);
		sal_Bool isPoolingEnabledByUrl(	const ::rtl::OUString& _sUrl,
										::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver >& _rxDriver,
										::rtl::OUString& _rsImplName,
										::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxDriverNode);

		OConnectionPool* getConnectionPool(	const ::rtl::OUString& _sImplName,
											const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver >& _xDriver,
											const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxDriverNode);
		void clearConnectionPools(sal_Bool _bDispose);
		void clearDesktop();
	protected:
		virtual ~OPoolCollection();
	public:

		static ::com::sun::star::uno::Any getNodeValue(	const ::rtl::OUString& _rPath,
														const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& _xTreeNode)throw();

	// XDriverManager
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection( const ::rtl::OUString& url );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnectionWithInfo( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info );
		virtual void SAL_CALL setLoginTimeout( sal_Int32 seconds );
		virtual sal_Int32 SAL_CALL getLoginTimeout(  );

	//XDriverAccess
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver > SAL_CALL getDriverByURL( const ::rtl::OUString& url );
	// XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// XServiceInfo - static methods
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL CreateInstance(const::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);
		static ::rtl::OUString SAL_CALL getImplementationName_Static(  );
		static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static(  );

		// XEventListener
		virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );
		// XPropertyChangeListener
		virtual void SAL_CALL propertyChange( const ::com::sun::star::beans::PropertyChangeEvent& evt );

		// XTerminateListener
		virtual void SAL_CALL queryTermination( const ::com::sun::star::lang::EventObject& Event );
		virtual void SAL_CALL notifyTermination( const ::com::sun::star::lang::EventObject& Event );
	};
}
#endif // CONNECTIVITY_POOLCOLLECTION_HXX
