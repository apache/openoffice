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



#ifndef _FLBYTES_HXX
#define _FLBYTES_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

namespace binfilter
{

//============================================================================
class SvSyncLockBytes: public SvOpenLockBytes
{
	SvAsyncLockBytesRef m_xAsyncLockBytes;

public:
	TYPEINFO();

	/// Create a synchronous wrapper around existing asynchronous lock bytes.
	///
	/// @param pTheAsyncLockBytes  Must not be null.
	inline SvSyncLockBytes(SvAsyncLockBytes * pTheAsyncLockBytes);

	/// Create a synchronous wrapper around an existing stream.
	///
	/// @descr  This is like first creating asynchronous lock bytes around the
	/// stream and than creating a synchronous wrapper around the asynchronous
	/// lock bytes.
	///
	/// @param pStream  Must not be null.
	///
	/// @param bOwner  True if these lock bytes own the stream (delete it on
	/// destruction).
	SvSyncLockBytes(SvStream * pStream, BOOL bOwner):
		m_xAsyncLockBytes(new SvAsyncLockBytes(pStream, bOwner)) {}

	virtual const SvStream * GetStream() const
	{ return m_xAsyncLockBytes->GetStream(); }

	virtual void SetSynchronMode(BOOL bSync = TRUE)
	{ m_xAsyncLockBytes->SetSynchronMode(bSync); }

	virtual BOOL IsSynchronMode() const
	{ return m_xAsyncLockBytes->IsSynchronMode(); }

	virtual ErrCode ReadAt(ULONG nPos, void * pBuffer, ULONG nCount,
						   ULONG * pRead) const;

	virtual ErrCode WriteAt(ULONG nPos, const void * pBuffer, ULONG nCount,
							ULONG * pWritten);

	virtual ErrCode Flush() const { return m_xAsyncLockBytes->Flush(); }

	virtual ErrCode SetSize(ULONG nSize)
	{ return m_xAsyncLockBytes->SetSize(nSize); }

	virtual ErrCode LockRegion(ULONG nPos, ULONG nCount, LockType eType)
	{ return m_xAsyncLockBytes->LockRegion(nPos, nCount, eType); }

	virtual ErrCode UnlockRegion(ULONG nPos, ULONG nCount, LockType eType)
	{ return m_xAsyncLockBytes->UnlockRegion(nPos, nCount, eType); }

	virtual ErrCode Stat(SvLockBytesStat * pStat, SvLockBytesStatFlag eFlag)
		const
	{ return m_xAsyncLockBytes->Stat(pStat, eFlag); }

	virtual ErrCode FillAppend(const void * pBuffer, ULONG nCount,
							   ULONG * pWritten)
	{ return m_xAsyncLockBytes->FillAppend(pBuffer, nCount, pWritten); }

	virtual ULONG Tell() const { return m_xAsyncLockBytes->Tell(); }

	virtual ULONG Seek(ULONG nPos)
	{ return m_xAsyncLockBytes->Seek(nPos); }

	virtual void Terminate() { m_xAsyncLockBytes->Terminate(); }
};

inline SvSyncLockBytes::SvSyncLockBytes(SvAsyncLockBytes *
										 pTheAsyncLockBytes):
	m_xAsyncLockBytes(pTheAsyncLockBytes)
{
	DBG_ASSERT(m_xAsyncLockBytes.Is(),
			   "SvSyncLockBytes::SvSyncLockBytes(): Null");
}

SV_DECL_IMPL_REF(SvSyncLockBytes);
}

#endif
