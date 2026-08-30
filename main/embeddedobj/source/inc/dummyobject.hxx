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



#ifndef _INC_DUMMYOBJECT_HXX_
#define _INC_DUMMYOBJECT_HXX_

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#include <com/sun/star/embed/XEmbedPersist.hpp>
#include <cppuhelper/implbase2.hxx>

namespace com { namespace sun { namespace star {
	namespace embed {
		class XStorage;
	}
	namespace frame {
		class XModel;
		class XFrame;
	}
	namespace lang {
		class XMultiServiceFactory;
	}
	namespace util {
		class XCloseListener;
	}
	namespace beans {
		struct PropertyValue;
		struct NamedValue;
	}
}}}

namespace cppu {
	class OMultiTypeInterfaceContainerHelper;
}

class ODummyEmbeddedObject : public ::cppu::WeakImplHelper2
						< ::com::sun::star::embed::XEmbeddedObject
						, ::com::sun::star::embed::XEmbedPersist >
{
	::osl::Mutex	m_aMutex;
	::cppu::OMultiTypeInterfaceContainerHelper* m_pInterfaceContainer;
	sal_Bool m_bDisposed;

	::rtl::OUString m_aEntryName;
	::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > m_xParentStorage;
	sal_Int32 m_nObjectState;

	::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient > m_xClientSite;

	sal_Int64 m_nCachedAspect;
	::com::sun::star::awt::Size m_aCachedSize;
	sal_Bool m_bHasCachedSize;

	// following information will be used between SaveAs and SaveCompleted
	sal_Bool m_bWaitSaveCompleted;
	::rtl::OUString m_aNewEntryName;
	::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > m_xNewParentStorage;

protected:
	void CheckInit();
	void PostEvent_Impl( const ::rtl::OUString& aEventName,
						const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xSource );

public:

	ODummyEmbeddedObject()
	: m_pInterfaceContainer( NULL )
	, m_bDisposed( sal_False )
	, m_nObjectState( -1 )
	, m_nCachedAspect( 0 )
	, m_bHasCachedSize( sal_False )
	, m_bWaitSaveCompleted( sal_False )
	{}

	~ODummyEmbeddedObject();

// XEmbeddedObject

    virtual void SAL_CALL changeState( sal_Int32 nNewState );

    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getReachableStates();

    virtual sal_Int32 SAL_CALL getCurrentState();

    virtual void SAL_CALL doVerb( sal_Int32 nVerbID );

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::embed::VerbDescriptor > SAL_CALL getSupportedVerbs();

    virtual void SAL_CALL setClientSite(
				const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient >& xClient );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient > SAL_CALL getClientSite();

    virtual void SAL_CALL update();

    virtual void SAL_CALL setUpdateMode( sal_Int32 nMode );

    virtual sal_Int64 SAL_CALL getStatus( sal_Int64 nAspect );

    virtual void SAL_CALL setContainerName( const ::rtl::OUString& sName );


// XVisualObject

    virtual void SAL_CALL setVisualAreaSize( sal_Int64 nAspect, const ::com::sun::star::awt::Size& aSize );

    virtual ::com::sun::star::awt::Size SAL_CALL getVisualAreaSize( sal_Int64 nAspect );

    virtual ::com::sun::star::embed::VisualRepresentation SAL_CALL getPreferredVisualRepresentation( ::sal_Int64 nAspect );

    virtual sal_Int32 SAL_CALL getMapUnit( sal_Int64 nAspect );

// XEmbedPersist

    virtual void SAL_CALL setPersistentEntry(
					const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
					const ::rtl::OUString& sEntName,
					sal_Int32 nEntryConnectionMode,
					const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
					const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs );

    virtual void SAL_CALL storeToEntry( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::rtl::OUString& sEntName, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs );

    virtual void SAL_CALL storeAsEntry(
				const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
				const ::rtl::OUString& sEntName,
				const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
				const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs );

    virtual void SAL_CALL saveCompleted( sal_Bool bUseNew );

    virtual sal_Bool SAL_CALL hasEntry();

    virtual ::rtl::OUString SAL_CALL getEntryName();


// XCommonEmbedPersist

    virtual void SAL_CALL storeOwn();

    virtual sal_Bool SAL_CALL isReadonly();

    virtual void SAL_CALL reload(
				const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
				const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs );


// XClassifiedObject

	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getClassID();

    virtual ::rtl::OUString SAL_CALL getClassName();

    virtual void SAL_CALL setClassInfo(
				const ::com::sun::star::uno::Sequence< sal_Int8 >& aClassID, const ::rtl::OUString& aClassName );


// XComponentSupplier

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseable > SAL_CALL getComponent();

// XStateChangeBroadcaster
    virtual void SAL_CALL addStateChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStateChangeListener >& xListener );
    virtual void SAL_CALL removeStateChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStateChangeListener >& xListener );

// XCloseable

    virtual void SAL_CALL close( sal_Bool DeliverOwnership );

    virtual void SAL_CALL addCloseListener(
				const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener );

    virtual void SAL_CALL removeCloseListener(
				const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener );

// XEventBroadcaster
    virtual void SAL_CALL addEventListener(
				const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& Listener );

    virtual void SAL_CALL removeEventListener(
				const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& Listener );

};

#endif
