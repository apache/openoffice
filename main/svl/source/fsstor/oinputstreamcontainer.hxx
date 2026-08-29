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



#ifndef _OINPUTSTREAMCONTAINER_HXX_
#define _OINPUTSTREAMCONTAINER_HXX_

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/embed/XExtendedStorageStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>


#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/interfacecontainer.h>

#include <osl/mutex.hxx>

class OFSInputStreamContainer : public cppu::WeakImplHelper2 < ::com::sun::star::io::XInputStream
															,::com::sun::star::embed::XExtendedStorageStream >
							, public ::com::sun::star::io::XSeekable
{
protected:
	::osl::Mutex m_aMutex;

	::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream > m_xInputStream;
	::com::sun::star::uno::Reference < ::com::sun::star::io::XSeekable > m_xSeekable;

	sal_Bool m_bSeekable;

	sal_Bool m_bDisposed;

	::cppu::OInterfaceContainerHelper* m_pListenersContainer; // list of listeners

public:
	OFSInputStreamContainer( const ::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream >& xStream );

	virtual ~OFSInputStreamContainer();

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XInputStream
    virtual sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead );
    virtual sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead );
    virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip );
    virtual sal_Int32 SAL_CALL available(  );
    virtual void SAL_CALL closeInput(  );

	//XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  );

	//XSeekable
    virtual void SAL_CALL seek( sal_Int64 location );
    virtual sal_Int64 SAL_CALL getPosition();
    virtual sal_Int64 SAL_CALL getLength();

	//XComponent
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

};

#endif
