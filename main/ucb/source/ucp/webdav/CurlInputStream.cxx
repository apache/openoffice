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
#include "precompiled_webdav.hxx"
#include "CurlInputStream.hxx"
#include <rtl/memory.h>

using namespace cppu;
using namespace rtl;
using namespace com::sun::star::io;
using namespace com::sun::star::uno;
using namespace http_dav_ucp;


// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
CurlInputStream::CurlInputStream( void )
: mLen( 0 ),
  mPos( 0 ),
  mCapacity( 0 )
{
}

// -------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------
CurlInputStream::~CurlInputStream( void )
{
}

// -------------------------------------------------------------------
// AddToStream
// Allows the caller to add some data to the "end" of the stream
// -------------------------------------------------------------------
void CurlInputStream::AddToStream( const char * inBuf, sal_Int32 inLen )
{
    if ( mLen + inLen > mCapacity )
    {
        if ( 2*mCapacity >= ( mLen + inLen ) )
            mCapacity *= 2;
        else
            mCapacity = mLen + inLen;
        mInputBuffer.realloc( mCapacity );
    }
    rtl_copyMemory( mInputBuffer.getArray() + mLen, inBuf, inLen );
    mLen += inLen;
}

// -------------------------------------------------------------------
// queryInterface
// -------------------------------------------------------------------
Any CurlInputStream::queryInterface( const Type &type )
						throw( RuntimeException )
{
	Any aRet = ::cppu::queryInterface( type,
									   static_cast< XInputStream * >( this ),
									   static_cast< XSeekable * >( this ) );
	return aRet.hasValue() ? aRet : OWeakObject::queryInterface( type );
}

// -------------------------------------------------------------------
// readBytes
// "Reads" the specified number of bytes from the stream
// -------------------------------------------------------------------
sal_Int32 SAL_CALL CurlInputStream::readBytes(
  ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
		throw( ::com::sun::star::io::NotConnectedException,
			   ::com::sun::star::io::BufferSizeExceededException,
			   ::com::sun::star::io::IOException,
			   ::com::sun::star::uno::RuntimeException )
{
	// Work out how much we're actually going to write
	sal_Int32 theBytes2Read = nBytesToRead;
	sal_Int32 theBytesLeft  = sal::static_int_cast<sal_Int32>(mLen - mPos);
	if ( theBytes2Read > theBytesLeft )
		theBytes2Read = theBytesLeft;

	// Realloc buffer.
	aData.realloc( theBytes2Read );

	// Write the data
	rtl_copyMemory(
		aData.getArray(), mInputBuffer.getConstArray() + mPos, theBytes2Read );

	// Update our stream position for next time
	mPos += theBytes2Read;

    return theBytes2Read;
}

// -------------------------------------------------------------------
// readSomeBytes
// -------------------------------------------------------------------
sal_Int32 SAL_CALL CurlInputStream::readSomeBytes(
 ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
        throw( ::com::sun::star::io::NotConnectedException,
               ::com::sun::star::io::BufferSizeExceededException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException )
{
	// Warning: What should this be doing ?
	return readBytes( aData, nMaxBytesToRead );
}

// -------------------------------------------------------------------
// skipBytes
// Moves the current stream position forward
// -------------------------------------------------------------------
void SAL_CALL CurlInputStream::skipBytes( sal_Int32 nBytesToSkip )
        throw( ::com::sun::star::io::NotConnectedException,
               ::com::sun::star::io::BufferSizeExceededException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException )
{
	mPos += nBytesToSkip;
	if ( mPos >= mLen )
		mPos = mLen;
}

// -------------------------------------------------------------------
// available
// Returns the number of unread bytes currently remaining on the stream
// -------------------------------------------------------------------
sal_Int32 SAL_CALL CurlInputStream::available(  )
        throw( ::com::sun::star::io::NotConnectedException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException )
{
	return sal::static_int_cast<sal_Int32>(mLen - mPos);
}

// -------------------------------------------------------------------
// closeInput
// -------------------------------------------------------------------
void SAL_CALL CurlInputStream::closeInput( void )
 		throw( ::com::sun::star::io::NotConnectedException,
       		   ::com::sun::star::io::IOException,
       		   ::com::sun::star::uno::RuntimeException )
{
}

// -------------------------------------------------------------------
// seek
// -------------------------------------------------------------------
void SAL_CALL CurlInputStream::seek( sal_Int64 location )
		throw( ::com::sun::star::lang::IllegalArgumentException,
			   ::com::sun::star::io::IOException,
			   ::com::sun::star::uno::RuntimeException )
{
	if ( location < 0 )
		throw ::com::sun::star::lang::IllegalArgumentException();

    if ( location <= mLen )
		mPos = location;
	else
        throw ::com::sun::star::lang::IllegalArgumentException();
}

// -------------------------------------------------------------------
// getPosition
// -------------------------------------------------------------------
sal_Int64 SAL_CALL CurlInputStream::getPosition()
		throw( ::com::sun::star::io::IOException,
			   ::com::sun::star::uno::RuntimeException )
{
	return mPos;
}

// -------------------------------------------------------------------
// getLength
// -------------------------------------------------------------------
sal_Int64 SAL_CALL CurlInputStream::getLength()
		throw( ::com::sun::star::io::IOException,
			   ::com::sun::star::uno::RuntimeException )
{
	return mLen;
}
