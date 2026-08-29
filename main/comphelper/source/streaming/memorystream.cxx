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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_comphelper.hxx"

#include "comphelper_module.hxx"

#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XSeekableInputStream.hpp>
#include <com/sun/star/io/XTruncate.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/implbase4.hxx>

#include <string.h>
#include <vector>

using ::rtl::OUString;
using ::cppu::OWeakObject;
using ::cppu::WeakImplHelper4;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::osl;

namespace comphelper
{

class UNOMemoryStream : public WeakImplHelper4 < XStream, XSeekableInputStream, XOutputStream, XTruncate >
{
public:
	UNOMemoryStream();
	virtual ~UNOMemoryStream();

	// XStream
	virtual Reference< XInputStream > SAL_CALL getInputStream(  );
	virtual Reference< XOutputStream > SAL_CALL getOutputStream(  );

	// XInputStream
	virtual sal_Int32 SAL_CALL readBytes( Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead );
	virtual sal_Int32 SAL_CALL readSomeBytes( Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead );
	virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip );
	virtual sal_Int32 SAL_CALL available();
	virtual void SAL_CALL closeInput();

	// XSeekable
	virtual void SAL_CALL seek( sal_Int64 location );
	virtual sal_Int64 SAL_CALL getPosition();
	virtual sal_Int64 SAL_CALL getLength();

	// XOutputStream
	virtual void SAL_CALL writeBytes( const Sequence< sal_Int8 >& aData );
	virtual void SAL_CALL flush();
	virtual void SAL_CALL closeOutput();

	// XTruncate
	virtual void SAL_CALL truncate();

	// XServiceInfo - static versions (used for component registration)
	static ::rtl::OUString SAL_CALL getImplementationName_static();
	static Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
	static Reference< XInterface > SAL_CALL Create( const Reference< ::com::sun::star::uno::XComponentContext >& );

private:
	std::vector< sal_Int8 > maData;
	sal_Int32 mnCursor;
};

UNOMemoryStream::UNOMemoryStream()
: mnCursor(0)
{
}

UNOMemoryStream::~UNOMemoryStream()
{
}

// XStream
Reference< XInputStream > SAL_CALL UNOMemoryStream::getInputStream(  )
{
	return this;
}

Reference< XOutputStream > SAL_CALL UNOMemoryStream::getOutputStream(  )
{
	return this;
}

// XInputStream
sal_Int32 SAL_CALL UNOMemoryStream::readBytes( Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
{
	if( nBytesToRead < 0 )
		throw IOException();

	nBytesToRead = std::min( nBytesToRead, available() );
	aData.realloc( nBytesToRead );

	if( nBytesToRead )
	{
		sal_Int8* pData = static_cast<sal_Int8*>(&(*maData.begin()));
		sal_Int8* pCursor = &((pData)[mnCursor]);
		memcpy( (void*)aData.getArray(), (void*)pCursor, nBytesToRead );

		mnCursor += nBytesToRead;
	}

	return nBytesToRead;
}

sal_Int32 SAL_CALL UNOMemoryStream::readSomeBytes( Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
{
	return readBytes( aData, nMaxBytesToRead );
}

void SAL_CALL UNOMemoryStream::skipBytes( sal_Int32 nBytesToSkip )
{
	if( nBytesToSkip < 0 )
		throw IOException();

	mnCursor += std::min( nBytesToSkip, available() );
}

sal_Int32 SAL_CALL UNOMemoryStream::available()
{
	return static_cast< sal_Int32 >( maData.size() ) - mnCursor;
}

void SAL_CALL UNOMemoryStream::closeInput()
{
	mnCursor = 0;
}

// XSeekable
void SAL_CALL UNOMemoryStream::seek( sal_Int64 location )
{
	if( (location < 0) || (location > SAL_MAX_INT32) )
		throw IllegalArgumentException( OUString(RTL_CONSTASCII_USTRINGPARAM("this implementation does not support more than 2GB!")), Reference< XInterface >(static_cast<OWeakObject*>(this)), 0 );

	// seek operation should be able to resize the stream
	if ( location > static_cast< sal_Int64 >( maData.size() ) )
		maData.resize( static_cast< sal_Int32 >( location ) );

	if ( location > static_cast< sal_Int64 >( maData.size() ) )
		maData.resize( static_cast< sal_Int32 >( location ) );

	mnCursor = static_cast< sal_Int32 >( location );
}

sal_Int64 SAL_CALL UNOMemoryStream::getPosition()
{
	return static_cast< sal_Int64 >( mnCursor );
}

sal_Int64 SAL_CALL UNOMemoryStream::getLength()
{
	return static_cast< sal_Int64 >( maData.size() );
}

// XOutputStream
void SAL_CALL UNOMemoryStream::writeBytes( const Sequence< sal_Int8 >& aData )
{
	const sal_Int32 nBytesToWrite( aData.getLength() );
	if( nBytesToWrite )
	{
		sal_Int64 nNewSize = static_cast< sal_Int64 >( mnCursor + nBytesToWrite );
		if( nNewSize > SAL_MAX_INT32 )
		{
			OSL_ASSERT(false);
			throw IOException( OUString(RTL_CONSTASCII_USTRINGPARAM("this implementation does not support more than 2GB!")), Reference< XInterface >(static_cast<OWeakObject*>(this)) );
		}

		if( static_cast< sal_Int32 >( nNewSize ) > static_cast< sal_Int32 >( maData.size() ) )
			maData.resize( static_cast< sal_Int32 >( nNewSize ) );

		sal_Int8* pData = static_cast<sal_Int8*>(&(*maData.begin()));
		sal_Int8* pCursor = &(pData[mnCursor]);
		memcpy( (void*)pCursor, (void*)aData.getConstArray(), nBytesToWrite );

		mnCursor += nBytesToWrite;
	}
}

void SAL_CALL UNOMemoryStream::flush()
{
}

void SAL_CALL UNOMemoryStream::closeOutput()
{
	mnCursor = 0;
}

//XTruncate
void SAL_CALL UNOMemoryStream::truncate()
{
	maData.resize( 0 );
	mnCursor = 0;
}

::rtl::OUString SAL_CALL UNOMemoryStream::getImplementationName_static()
{
	static const OUString sImplName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.MemoryStream" ) );
	return sImplName;
}

Sequence< ::rtl::OUString > SAL_CALL UNOMemoryStream::getSupportedServiceNames_static()
{
	Sequence< OUString > aSeq(1);
	aSeq[0] = getImplementationName_static();
	return aSeq;
}

Reference< XInterface > SAL_CALL UNOMemoryStream::Create(
	const Reference< XComponentContext >& )
{
	return static_cast<OWeakObject*>(new UNOMemoryStream());
}

} // namespace comphelper

void createRegistryInfo_UNOMemoryStream()
{
	static ::comphelper::module::OAutoRegistration< ::comphelper::UNOMemoryStream > aAutoRegistration;
}
