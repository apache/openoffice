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


#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _FLBYTES_HXX
#include <flbytes.hxx>
#endif

#ifndef _SVSTDARR_ULONGS_DECL
#define _SVSTDARR_ULONGS
#include <bf_svtools/svstdarr.hxx>
#undef _SVSTDARR_ULONGS
#endif

//============================================================================
namespace binfilter
{

inline ULONG MyMin( long a, long b )
{
	return Max( long( Min(  a ,  b ) ), 0L );
}

//============================================================================
SV_DECL_IMPL_REF_LIST( SvLockBytes, SvLockBytes* )

//============================================================================
//
//  SvSyncLockBytes
//
//============================================================================

TYPEINIT1(SvSyncLockBytes, SvOpenLockBytes);

//============================================================================
// virtual
ErrCode SvSyncLockBytes::ReadAt(ULONG nPos, void * pBuffer, ULONG nCount,
								ULONG * pRead) const
{
	for (ULONG nReadTotal = 0;;)
	{
		ULONG nReadCount = 0;
		ErrCode nError = m_xAsyncLockBytes->ReadAt(nPos, pBuffer, nCount,
												   &nReadCount);
		nReadTotal += nReadCount;
		if (nError != ERRCODE_IO_PENDING || !IsSynchronMode())
		{
			if (pRead)
				*pRead = nReadTotal;
			return nError;
		}
		nPos += nReadCount;
		pBuffer = static_cast< sal_Char * >(pBuffer) + nReadCount;
		nCount -= nReadCount;
		Application::Yield();
	}
}

//============================================================================
// virtual
ErrCode SvSyncLockBytes::WriteAt(ULONG nPos, const void * pBuffer,
								 ULONG nCount, ULONG * pWritten)
{
	for (ULONG nWrittenTotal = 0;;)
	{
		ULONG nWrittenCount = 0;
		ErrCode nError = m_xAsyncLockBytes->WriteAt(nPos, pBuffer, nCount,
													&nWrittenCount);
		nWrittenTotal += nWrittenCount;
		if (nError != ERRCODE_IO_PENDING || !IsSynchronMode())
		{
			if (pWritten)
				*pWritten = nWrittenTotal;
			return nError;
		}
		nPos += nWrittenCount;
		pBuffer = static_cast< sal_Char const * >(pBuffer) + nWrittenCount;
		nCount -= nWrittenCount;
		Application::Yield();
	}
}

}
