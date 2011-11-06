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



#ifndef _DECODE_HXX
#define _DECODE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif

namespace binfilter
{

struct GIFLZWTableEntry;

class GIFLZWDecompressor
{
	GIFLZWTableEntry*	pTable;
	HPBYTE				pOutBuf;
	HPBYTE				pOutBufData;
	HPBYTE				pBlockBuf;
	ULONG				nInputBitsBuf;
	USHORT				nTableSize;
	USHORT				nClearCode;
	USHORT				nEOICode;
	USHORT				nCodeSize;
	USHORT				nOldCode;
	USHORT				nOutBufDataLen;
	USHORT				nInputBitsBufSize;
	BOOL				bEOIFound;
	BYTE				nDataSize;
	BYTE				nBlockBufSize;
	BYTE				nBlockBufPos;

	void				AddToTable(USHORT nPrevCode, USHORT nCodeFirstData);
	BOOL				ProcessOneCode();


public:

						GIFLZWDecompressor( BYTE cDataSize );
						~GIFLZWDecompressor();

	HPBYTE				DecompressBlock( HPBYTE pSrc, BYTE cBufSize, ULONG& rCount, BOOL& rEOI );
};

}

#endif
