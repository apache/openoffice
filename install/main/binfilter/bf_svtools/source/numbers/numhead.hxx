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



#ifndef NF_NUMHEAD_HXX
#define NF_NUMHEAD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

namespace binfilter
{

// -----------------------------------------------------------------------

		//	"Automatischer" Record-Header mit Groessenangabe

/* 						wird fuer SvNumberFormatter nicht gebraucht
class SvNumReadHeader
{
private:
	SvStream&	rStream;
	ULONG		nDataEnd;

public:
	SvNumReadHeader(SvStream& rNewStream);
	~SvNumReadHeader();

	ULONG	BytesLeft() const;
};

class SvNumWriteHeader
{
private:
	SvStream&	rStream;
	ULONG		nDataPos;
	ULONG		nDataSize;

public:
	SvNumWriteHeader(SvStream& rNewStream, ULONG nDefault = 0);
	~SvNumWriteHeader();
};

*/

		//	Header mit Groessenangaben fuer mehrere Objekte

class ImpSvNumMultipleReadHeader
{
private:
	SvStream&		rStream;
	char*			pBuf;
	SvMemoryStream*	pMemStream;
	ULONG			nEndPos;
	ULONG			nEntryEnd;

public:
	ImpSvNumMultipleReadHeader(SvStream& rNewStream);
	~ImpSvNumMultipleReadHeader();

	void	StartEntry();
	void	EndEntry();
	ULONG	BytesLeft() const;

	static void Skip( SvStream& );		// komplett ueberspringen
};

class ImpSvNumMultipleWriteHeader
{
private:
	SvStream&		rStream;
	SvMemoryStream	aMemStream;
	ULONG			nDataPos;
	sal_uInt32		nDataSize;
	ULONG			nEntryStart;

public:
	ImpSvNumMultipleWriteHeader(SvStream& rNewStream, ULONG nDefault = 0);
	~ImpSvNumMultipleWriteHeader();

	void	StartEntry();
	void	EndEntry();
};

}

#endif


