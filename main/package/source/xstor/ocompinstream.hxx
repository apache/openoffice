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



#ifndef _INPUTCOMPSTREAM_HXX_
#define _INPUTCOMPSTREAM_HXX_

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/embed/XExtendedStorageStream.hpp>
#include <com/sun/star/embed/XRelationshipAccess.hpp>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/interfacecontainer.h>

#include "mutexholder.hxx"

struct OWriteStream_Impl;

class OInputCompStream : public cppu::WeakImplHelper4 < ::com::sun::star::io::XInputStream
														,::com::sun::star::embed::XExtendedStorageStream
														,::com::sun::star::embed::XRelationshipAccess
														,::com::sun::star::beans::XPropertySet >
{
protected:
	OWriteStream_Impl* m_pImpl;

	SotMutexHolderRef m_rMutexRef;

	::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream > m_xStream;

	::cppu::OInterfaceContainerHelper* m_pInterfaceContainer;

	::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue > m_aProperties;

	sal_Bool m_bDisposed;

	sal_Int32 m_nStorageType;

public:
	OInputCompStream( OWriteStream_Impl& pImpl,
					  ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > xStream,
					  const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aProps,
					  sal_Int32 nStorageType );

	OInputCompStream( ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > xStream,
					  const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aProps,
					  sal_Int32 nStorageType );

	virtual ~OInputCompStream();

	void InternalDispose();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& rType );

	// XInputStream
    virtual sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead );
    virtual sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead );
    virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip );
    virtual sal_Int32 SAL_CALL available(  );
    virtual void SAL_CALL closeInput(  );

	//XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  );

	//XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	//XRelationshipAccess
    virtual ::sal_Bool SAL_CALL hasByID( const ::rtl::OUString& sID );
    virtual ::rtl::OUString SAL_CALL getTargetByID( const ::rtl::OUString& sID );
    virtual ::rtl::OUString SAL_CALL getTypeByID( const ::rtl::OUString& sID );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair > SAL_CALL getRelationshipByID( const ::rtl::OUString& sID );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair > > SAL_CALL getRelationshipsByType( const ::rtl::OUString& sType );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair > > SAL_CALL getAllRelationships(  );
    virtual void SAL_CALL insertRelationshipByID( const ::rtl::OUString& sID, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair >& aEntry, ::sal_Bool bReplace );
    virtual void SAL_CALL removeRelationshipByID( const ::rtl::OUString& sID );
    virtual void SAL_CALL insertRelationships( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair > >& aEntries, ::sal_Bool bReplace );
    virtual void SAL_CALL clearRelationships(  );

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

};

#endif
