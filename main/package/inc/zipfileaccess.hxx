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



#ifndef _ZIPFILEACCESS_HXX_
#define _ZIPFILEACCESS_HXX_

#include <com/sun/star/packages/zip/XZipFileAccess.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNameAccess.hpp>

#include <cppuhelper/interfacecontainer.h>
#include <cppuhelper/implbase5.hxx>

#include <mutexholder.hxx>

#include <ZipFile.hxx>
#include <HashMaps.hxx>

class OZipFileAccess : public ::cppu::WeakImplHelper5<
						::com::sun::star::packages::zip::XZipFileAccess,
						::com::sun::star::container::XNameAccess,
						::com::sun::star::lang::XInitialization,
						::com::sun::star::lang::XComponent,
						::com::sun::star::lang::XServiceInfo >
{
    SotMutexHolderRef m_aMutexHolder;

	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xFactory;

	::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > m_xContentStream;
	ZipFile* m_pZipFile;

	::cppu::OInterfaceContainerHelper* m_pListenersContainer;

	sal_Bool m_bDisposed;

public:
	OZipFileAccess( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

	virtual ~OZipFileAccess();

	::com::sun::star::uno::Sequence< ::rtl::OUString > GetPatternsFromString_Impl( const ::rtl::OUString& aString );

	sal_Bool StringGoodForPattern_Impl( const ::rtl::OUString& aString,
										const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPattern );


	static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL impl_staticGetSupportedServiceNames();

	static ::rtl::OUString SAL_CALL impl_staticGetImplementationName();

	static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL impl_staticCreateSelfInstance(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );


	// XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  );
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	// XZipFileAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getStreamByPattern( const ::rtl::OUString& aPattern );

	// XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

};

#endif
