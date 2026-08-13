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


#ifndef _XTEMPFILE_HXX_
#define _XTEMPFILE_HXX_

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XTempFile.hpp>
#include <com/sun/star/io/XTruncate.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase5.hxx>
#ifndef _CPPUHELPER_PROPERTYSETMIXIN_HXX_
#include <cppuhelper/propertysetmixin.hxx>
#endif
#include <osl/mutex.hxx>

class SvStream;
namespace utl { class TempFile; }

typedef	 ::cppu::WeakImplHelper5<	::com::sun::star::io::XTempFile
									,	::com::sun::star::io::XInputStream
				  								,	::com::sun::star::io::XOutputStream
				  								,	::com::sun::star::io::XTruncate
				  								,	::com::sun::star::lang::XServiceInfo
				  								>
									OTempFileBase;

class OTempFileService :
	public OTempFileBase,
	public ::cppu::PropertySetMixin< ::com::sun::star::io::XTempFile >
{
protected:
	::utl::TempFile* 	mpTempFile;
	::osl::Mutex		maMutex;
	SvStream* 			mpStream;
	sal_Bool			mbRemoveFile;
	sal_Bool			mbInClosed;
	sal_Bool			mbOutClosed;

	sal_Int64			mnCachedPos;
	sal_Bool			mbHasCachedPos;

	void checkError () const;
	void checkConnected ();

public:
	OTempFileService (::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);

	//Methods
	//	XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
	virtual void SAL_CALL acquire(  )
		throw ();
	virtual void SAL_CALL release(  )
		throw ();
	//	XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
	virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getImplementationId(  );

	//	XTempFile
	virtual ::sal_Bool SAL_CALL getRemoveFile();
	virtual void SAL_CALL setRemoveFile( ::sal_Bool _removefile );
	virtual ::rtl::OUString SAL_CALL getUri();
	virtual ::rtl::OUString SAL_CALL getResourceName();

    // XInputStream
    virtual ::sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nBytesToRead );
    virtual ::sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData, ::sal_Int32 nMaxBytesToRead );
    virtual void SAL_CALL skipBytes( ::sal_Int32 nBytesToSkip );
    virtual ::sal_Int32 SAL_CALL available(  );
    virtual void SAL_CALL closeInput(  );
	// XOutputStream
	virtual void SAL_CALL writeBytes( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData );
    virtual void SAL_CALL flush(  );
    virtual void SAL_CALL closeOutput(  );
	// XSeekable
    virtual void SAL_CALL seek( sal_Int64 location );
    virtual sal_Int64 SAL_CALL getPosition(  );
    virtual sal_Int64 SAL_CALL getLength(  );
	// XStream
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  );
	// XTruncate
    virtual void SAL_CALL truncate();
	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	//::com::sun::star::uno::Reference < ::com::sun::star::uno::XInterface > SAL_CALL XTempFile_createInstance( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);
	static ::rtl::OUString getImplementationName_Static ();
	static ::com::sun::star::uno::Sequence < ::rtl::OUString > getSupportedServiceNames_Static();

private:
	OTempFileService( OTempFileService & );
	virtual ~OTempFileService ();

};
#endif
