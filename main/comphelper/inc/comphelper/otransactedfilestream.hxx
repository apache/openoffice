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


#ifndef _COMPHELPER_OTRANSACTEDFILESTREAM_HXX
#define _COMPHELPER_OTRANSACTEDFILESTREAM_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <com/sun/star/io/XTruncate.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <osl/mutex.hxx>
#include <cppuhelper/implbase7.hxx>
#include "comphelper/comphelperdllapi.h"

namespace comphelper
{


//==================================================================
// OTransactedFileStream
// allows to get transacted stream access to a file
// the file contents are automatically truncated, but the file
// itself is changed only on commit
// after the first commit the stream switches to direct mode
//==================================================================

struct TTFileStreamData_Impl;
class COMPHELPER_DLLPUBLIC OTruncatedTransactedFileStream
		: public ::cppu::WeakImplHelper7 <
										::com::sun::star::io::XStream,
										::com::sun::star::io::XInputStream,
										::com::sun::star::io::XOutputStream,
										::com::sun::star::io::XTruncate,
										::com::sun::star::io::XSeekable,
										::com::sun::star::beans::XPropertySetInfo,
										::com::sun::star::beans::XPropertySet >
{
	::osl::Mutex	m_aMutex;
	TTFileStreamData_Impl* m_pStreamData;

	void CommonInit_Impl(
			const ::rtl::OUString& aURL,
			const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess,
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory,
			sal_Bool bDeleteOptionIsProvided );

	void CloseAll_Impl();

public:

	OTruncatedTransactedFileStream( const ::rtl::OUString& aURL, const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

	OTruncatedTransactedFileStream( const ::rtl::OUString& aURL, const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess, const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

	OTruncatedTransactedFileStream( const ::rtl::OUString& aURL, const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess, const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory, sal_Bool bDeleteIfNotCommited );

	~OTruncatedTransactedFileStream();

	void Commit_Impl();
	void Revert_Impl();

// com::sun::star::io::XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  );

// com::sun::star::io::XInputStream
    virtual ::sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nBytesToRead );
    virtual ::sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nMaxBytesToRead );
    virtual void SAL_CALL skipBytes( ::sal_Int32 nBytesToSkip );
    virtual ::sal_Int32 SAL_CALL available(  );
    virtual void SAL_CALL closeInput(  );

// com::sun::star::io::XOutputStream
    virtual void SAL_CALL writeBytes( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData );
    virtual void SAL_CALL flush(  );
    virtual void SAL_CALL closeOutput(  );

// com::sun::star::io::XTruncate
    virtual void SAL_CALL truncate(  );

// com::sun::star::io::XSeekable
    virtual void SAL_CALL seek( ::sal_Int64 location );
    virtual ::sal_Int64 SAL_CALL getPosition(  );
    virtual ::sal_Int64 SAL_CALL getLength(  );

// ::com::sun::star::beans::XPropertySetInfo
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL getProperties(  );
    virtual ::com::sun::star::beans::Property SAL_CALL getPropertyByName( const ::rtl::OUString& aName );
    virtual ::sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& Name );

// com::sun::star::beans::XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

};

} // namespace comphelper

#endif //_COMPHELPER_OTRANSACTEDFILESTREAM_HXX
