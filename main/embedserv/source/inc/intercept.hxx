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



#ifndef _INTERCEPT_HXX_
#define _INTERCEPT_HXX_

#include <osl/mutex.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/frame/XInterceptorInfo.hpp>
#include <com/sun/star/frame/XDispatch.hpp>

#include <rtl/ref.hxx>
#include "embeddocaccess.hxx"


class StatusChangeListenerContainer;
class EmbedDocument_Impl;
class DocumentHolder;

class Interceptor
	: public ::cppu::WeakImplHelper3<
				   ::com::sun::star::frame::XDispatchProviderInterceptor,
				   ::com::sun::star::frame::XInterceptorInfo,
				   ::com::sun::star::frame::XDispatch>
{
public:

	Interceptor(
		const ::rtl::Reference< EmbeddedDocumentInstanceAccess_Impl >& xOleAccess,
		DocumentHolder* pDocH,
		sal_Bool bLink );

	~Interceptor();

	void DisconnectDocHolder();

	void generateFeatureStateEvent();
	
	// overwritten to release the statuslistener.


	// XComponent
	virtual void SAL_CALL
	addEventListener(
		const com::sun::star::uno::Reference< com::sun::star::lang::XEventListener >& xListener )
		throw( com::sun::star::uno::RuntimeException );
	
	virtual void SAL_CALL
	removeEventListener( const com::sun::star::uno::Reference< com::sun::star::lang::XEventListener >& aListener )
		throw( com::sun::star::uno::RuntimeException );
	
	void SAL_CALL
	dispose() throw(::com::sun::star::uno::RuntimeException);
	
	
	
	//XDispatch
	virtual void SAL_CALL 
	dispatch( 
		const ::com::sun::star::util::URL& URL,
		const ::com::sun::star::uno::Sequence<
		::com::sun::star::beans::PropertyValue >& Arguments )
		throw (::com::sun::star::uno::RuntimeException);
	
    virtual void SAL_CALL
	addStatusListener( 
		const ::com::sun::star::uno::Reference< 
		::com::sun::star::frame::XStatusListener >& Control, 
		const ::com::sun::star::util::URL& URL )
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
    virtual void SAL_CALL
	removeStatusListener( 
		const ::com::sun::star::uno::Reference< 
		::com::sun::star::frame::XStatusListener >& Control, 
		const ::com::sun::star::util::URL& URL ) 
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
	//XInterceptorInfo
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > 
	SAL_CALL getInterceptedURLs(  )
		throw (
			::com::sun::star::uno::RuntimeException
		);
	

	//XDispatchProvider ( inherited by XDispatchProviderInterceptor )
    virtual ::com::sun::star::uno::Reference< 
	::com::sun::star::frame::XDispatch > SAL_CALL
	queryDispatch( 
		const ::com::sun::star::util::URL& URL,
		const ::rtl::OUString& TargetFrameName, 
		sal_Int32 SearchFlags )
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
    virtual ::com::sun::star::uno::Sequence< 
	::com::sun::star::uno::Reference< 
	::com::sun::star::frame::XDispatch > > SAL_CALL
	queryDispatches( 
		const ::com::sun::star::uno::Sequence<
		::com::sun::star::frame::DispatchDescriptor >& Requests )
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
	
	//XDispatchProviderInterceptor
    virtual ::com::sun::star::uno::Reference<
	::com::sun::star::frame::XDispatchProvider > SAL_CALL 
	getSlaveDispatchProvider(  ) 
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
    virtual void SAL_CALL
	setSlaveDispatchProvider( 
		const ::com::sun::star::uno::Reference< 
		::com::sun::star::frame::XDispatchProvider >& NewDispatchProvider )
		throw (
			::com::sun::star::uno::RuntimeException
		);

    virtual ::com::sun::star::uno::Reference< 
	::com::sun::star::frame::XDispatchProvider > SAL_CALL
	getMasterDispatchProvider(  ) 
		throw (
			::com::sun::star::uno::RuntimeException
		);
	
    virtual void SAL_CALL
	setMasterDispatchProvider( 
		const ::com::sun::star::uno::Reference< 
		::com::sun::star::frame::XDispatchProvider >& NewSupplier )
		throw (
			::com::sun::star::uno::RuntimeException
		);	
	
	
private:
	
	osl::Mutex   m_aMutex;
	
	::rtl::Reference< EmbeddedDocumentInstanceAccess_Impl > m_xOleAccess;

	::com::sun::star::uno::WeakReference< ::com::sun::star::uno::XInterface > m_xDocHLocker;
	DocumentHolder*       m_pDocH;

	::com::sun::star::uno::Reference<
	::com::sun::star::frame::XDispatchProvider > m_xSlaveDispatchProvider;
	
	::com::sun::star::uno::Reference< 
	::com::sun::star::frame::XDispatchProvider > m_xMasterDispatchProvider;

	static ::com::sun::star::uno::Sequence< ::rtl::OUString > 
	m_aInterceptedURL;

	cppu::OInterfaceContainerHelper*    m_pDisposeEventListeners;
	StatusChangeListenerContainer*    m_pStatCL;

	sal_Bool m_bLink;
};

#endif
