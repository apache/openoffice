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



#ifndef _UCB_HXX
#define _UCB_HXX

#include <com/sun/star/ucb/XCommandProcessor.hpp>
#include <com/sun/star/ucb/XContentProvider.hpp>
#include <com/sun/star/ucb/XContentIdentifierFactory.hpp>
#include <com/sun/star/ucb/XContentProviderManager.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/util/XChangesListener.hpp>
#include <com/sun/star/util/XChangesNotifier.hpp>
#include <com/sun/star/container/XContainer.hpp>

#include <rtl/ustrbuf.hxx>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>
#include <osl/interlck.h>
#include <ucbhelper/macros.hxx>
#include "providermap.hxx"
#include <ucbhelper/registerucb.hxx>

#include <vector>
//=========================================================================

#define UCB_SERVICE_NAME "com.sun.star.ucb.UniversalContentBroker"

//=========================================================================

namespace cppu { class OInterfaceContainerHelper; }

namespace com { namespace sun { namespace star { namespace ucb {
	class XCommandInfo;
	struct GlobalTransferCommandArgument;
} } } }

class UniversalContentBroker :
				public cppu::OWeakObject,
				public com::sun::star::lang::XTypeProvider,
				public com::sun::star::lang::XComponent,
				public com::sun::star::lang::XServiceInfo,
				public com::sun::star::lang::XInitialization,
				public com::sun::star::ucb::XContentProviderManager,
				public com::sun::star::ucb::XContentProvider,
				public com::sun::star::ucb::XContentIdentifierFactory,
				public com::sun::star::ucb::XCommandProcessor,
				public com::sun::star::util::XChangesListener
{
public:
	UniversalContentBroker( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rXSMgr );
	virtual ~UniversalContentBroker();

	// XInterface
	XINTERFACE_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XServiceInfo
	XSERVICEINFO_DECL()

	// XComponent
    virtual void SAL_CALL
	dispose();
    virtual void SAL_CALL
	addEventListener( const com::sun::star::uno::Reference<
						com::sun::star::lang::XEventListener >& Listener );
    virtual void SAL_CALL
	removeEventListener( const com::sun::star::uno::Reference<
							com::sun::star::lang::XEventListener >& Listener );

	// XInitialization
    virtual void SAL_CALL
	initialize( const com::sun::star::uno::Sequence<
						com::sun::star::uno::Any >& aArguments );

	// XContentProviderManager
    virtual com::sun::star::uno::Reference<
		com::sun::star::ucb::XContentProvider > SAL_CALL
	registerContentProvider( const com::sun::star::uno::Reference<
								com::sun::star::ucb::XContentProvider >&
									Provider,
							 const rtl::OUString& Scheme,
							 sal_Bool ReplaceExisting );
    virtual void SAL_CALL
	deregisterContentProvider( const com::sun::star::uno::Reference<
									com::sun::star::ucb::XContentProvider >&
										Provider,
							   const rtl::OUString& Scheme );
    virtual com::sun::star::uno::Sequence<
		com::sun::star::ucb::ContentProviderInfo > SAL_CALL
	queryContentProviders();
    virtual com::sun::star::uno::Reference<
		com::sun::star::ucb::XContentProvider >	SAL_CALL
	queryContentProvider( const rtl::OUString& Identifier );

	// XContentProvider
	virtual com::sun::star::uno::Reference<
		com::sun::star::ucb::XContent > SAL_CALL
	queryContent( const com::sun::star::uno::Reference<
					com::sun::star::ucb::XContentIdentifier >& Identifier );
    virtual sal_Int32 SAL_CALL
	compareContentIds( const com::sun::star::uno::Reference<
						com::sun::star::ucb::XContentIdentifier >& Id1,
					   const com::sun::star::uno::Reference<
					   	com::sun::star::ucb::XContentIdentifier >& Id2 );

	// XContentIdentifierFactory
    virtual com::sun::star::uno::Reference<
		com::sun::star::ucb::XContentIdentifier > SAL_CALL
	createContentIdentifier( const rtl::OUString& ContentId );

	// XCommandProcessor
    virtual sal_Int32 SAL_CALL
	createCommandIdentifier();
    virtual com::sun::star::uno::Any SAL_CALL
	execute( const com::sun::star::ucb::Command& aCommand,
	         sal_Int32 CommandId,
			 const com::sun::star::uno::Reference<
			 	com::sun::star::ucb::XCommandEnvironment >& Environment );
    virtual void SAL_CALL
	abort( sal_Int32 CommandId );

	// XChangesListener
    virtual void SAL_CALL changesOccurred( const com::sun::star::util::ChangesEvent& Event );

	 // XEventListener ( base of XChangesLisetenr )
    virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source );

private:
	com::sun::star::uno::Reference< com::sun::star::ucb::XContentProvider >
	queryContentProvider( const rtl::OUString& Identifier,
                          sal_Bool bResolved );

    com::sun::star::uno::Reference< com::sun::star::ucb::XCommandInfo >
	getCommandInfo();

	void
	globalTransfer(
			const com::sun::star::ucb::GlobalTransferCommandArgument & rArg,
			const com::sun::star::uno::Reference<
			 	com::sun::star::ucb::XCommandEnvironment >& xEnv );


	bool configureUcb();

	bool getContentProviderData(
			const rtl::OUString & rKey1,
			const rtl::OUString & rKey2,
            ucbhelper::ContentProviderDataList & rListToFill);

	void prepareAndRegister( const ucbhelper::ContentProviderDataList& rData);

	com::sun::star::uno::Reference<
		com::sun::star::lang::XMultiServiceFactory > m_xSMgr;

	com::sun::star::uno::Reference<
		com::sun::star::util::XChangesNotifier > m_xNotifier;

	com::sun::star::uno::Sequence< com::sun::star::uno::Any > m_aArguments;
	ProviderMap_Impl m_aProviders;
	osl::Mutex m_aMutex;
	cppu::OInterfaceContainerHelper* m_pDisposeEventListeners;
	oslInterlockedCount m_nInitCount; //@@@ see initialize() method
	sal_Int32 m_nCommandId;
};

#endif /* !_UCB_HXX */
