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



#ifndef _TOOLKIT_HELPER_UNOMEMORYSTREAM_HXX_
#define _TOOLKIT_HELPER_UNOMEMORYSTREAM_HXX_


#include <com/sun/star/io/XInputStream.hpp>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>

#include <tools/stream.hxx>

//	----------------------------------------------------
//	class UNOMEMORYSTREAM
//	----------------------------------------------------

class UnoMemoryStream :	public SvMemoryStream,
						public ::com::sun::star::io::XInputStream,
						public ::cppu::OWeakObject
{
private:
	::osl::Mutex	maMutex;

protected:
	::osl::Mutex&	GetMutex() { return maMutex; }


public:
	UnoMemoryStream( sal_uInt32 nInitSize = 0x3FFF, sal_uInt32 nResize = 0x3FFFF );

	/*	throw() states what SvStream already promises, and is what a C++11
		compiler would deduce for this destructor anyway.  Under C++03 -- where a
		destructor carries no implicit specification -- it is the only thing that
		says so. */
	~UnoMemoryStream() SAL_THROW( () ) {}	// body is what the compiler generated

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any	SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	void						SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void						SAL_CALL release() throw()	{ OWeakObject::release(); }

	// ::com::sun::star::io::XInputStream
    sal_Int32		SAL_CALL readBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead );
    sal_Int32		SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead );
    void			SAL_CALL skipBytes( sal_Int32 nBytesToSkip );
    sal_Int32		SAL_CALL available(  );
    void			SAL_CALL closeInput(  );
};




#endif // _TOOLKIT_HELPER_UNOMEMORYSTREAM_HXX_
