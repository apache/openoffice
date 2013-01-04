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



#ifndef SVTOOLS_STRMADPT_HXX
#define SVTOOLS_STRMADPT_HXX

#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
namespace binfilter
{


//============================================================================
class  SvOutputStreamOpenLockBytes: public SvOpenLockBytes
{
	com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >
	    m_xOutputStream;
	sal_uInt32 m_nPosition;

public:
	TYPEINFO();

	SvOutputStreamOpenLockBytes(
		    const com::sun::star::uno::Reference<
			          com::sun::star::io::XOutputStream > &
			    rTheOutputStream):
		m_xOutputStream(rTheOutputStream), m_nPosition(0) {}

	virtual ErrCode ReadAt(ULONG, void *, ULONG, ULONG *) const;

	virtual ErrCode WriteAt(ULONG nPos, const void * pBuffer, ULONG nCount,
							ULONG * pWritten);

	virtual ErrCode Flush() const;

	virtual ErrCode SetSize(ULONG);

	virtual ErrCode Stat(SvLockBytesStat * pStat, SvLockBytesStatFlag) const;

	virtual ErrCode FillAppend(const void * pBuffer, ULONG nCount,
							   ULONG * pWritten);

	virtual ULONG Tell() const;

	virtual ULONG Seek(ULONG);

	virtual void Terminate();
};

//============================================================================
class  SvLockBytesInputStream: public cppu::OWeakObject,
							  public com::sun::star::io::XInputStream,
							  public com::sun::star::io::XSeekable
{
	SvLockBytesRef m_xLockBytes;
	sal_Int64 m_nPosition;
	bool m_bDone;

public:
	SvLockBytesInputStream(SvLockBytes * pTheLockBytes):
		m_xLockBytes(pTheLockBytes), m_nPosition(0), m_bDone(false) {}

	virtual com::sun::star::uno::Any SAL_CALL
	queryInterface(const com::sun::star::uno::Type & rType)
		throw (com::sun::star::uno::RuntimeException);

	virtual void SAL_CALL acquire() throw();

	virtual void SAL_CALL release() throw();

    virtual sal_Int32 SAL_CALL
	readBytes(com::sun::star::uno::Sequence< sal_Int8 > & rData,
			  sal_Int32 nBytesToRead)
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL
	readSomeBytes(com::sun::star::uno::Sequence< sal_Int8 > & rData,
				  sal_Int32 nMaxBytesToRead)
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL skipBytes(sal_Int32 nBytesToSkip)
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL available()
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL closeInput()
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL seek(sal_Int64 nLocation)
		throw (com::sun::star::lang::IllegalArgumentException,
			   com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual sal_Int64 SAL_CALL getPosition()
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);

    virtual sal_Int64 SAL_CALL getLength()
		throw (com::sun::star::io::IOException,
			   com::sun::star::uno::RuntimeException);
};

}

#endif // SVTOOLS_STRMADPT_HXX

