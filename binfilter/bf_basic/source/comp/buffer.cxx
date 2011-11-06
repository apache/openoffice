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



/*?*/ // #include "sbcomp.hxx"
#include "buffer.hxx"
#include <string.h>

namespace binfilter {

const static UINT32 UP_LIMIT=0xFFFFFF00L;

// Der SbiBuffer wird in Inkrements von mindestens 16 Bytes erweitert.
// Dies ist notwendig, da viele Klassen von einer Pufferlaenge
// von x*16 Bytes ausgehen.

SbiBuffer::SbiBuffer( SbiParser* p, short n )
{
	pParser = p;
	n = ( (n + 15 ) / 16 ) * 16;
	if( !n ) n = 16;
	pBuf  = NULL;
	pCur  = NULL;
	nInc  = n;
	nSize =
	nOff  = 0;
}

SbiBuffer::~SbiBuffer()
{
	delete[] pBuf;
}

// Rausreichen des Puffers
// Dies fuehrt zur Loeschung des Puffers!

char* SbiBuffer::GetBuffer()
{
	char* p = pBuf;
	pBuf = NULL;
	pCur = NULL;
	return p;
}

// Test, ob der Puffer n Bytes aufnehmen kann.
// Im Zweifelsfall wird er vergroessert

BOOL SbiBuffer::Check( USHORT n )
{
	if( !n ) return TRUE;
	if( ( static_cast<UINT32>( nOff )+ n ) >  static_cast<UINT32>( nSize ) )
	{
		if( nInc == 0 )
			return FALSE;
		USHORT nn = 0;
		while( nn < n ) nn = nn + nInc;
		char* p;
		if( ( static_cast<UINT32>( nSize ) + nn ) > UP_LIMIT ) p = NULL;
		else p = new char [nSize + nn];
		if( !p )
		{
/*?*/ // 			pParser->Error( SbERR_PROG_TOO_LARGE );
			nInc = 0;
			delete[] pBuf; pBuf = NULL;
			return FALSE;
		}
		else
		{
			if( nSize ) memcpy( p, pBuf, nSize );
			delete[] pBuf;
			pBuf = p;
			pCur = pBuf + nOff;
			nSize = nSize + nn;
		}
	}
	return TRUE;
}

BOOL SbiBuffer::operator +=( INT8 n )
{
	if( Check( 1 ) )
	{
		*pCur++ = (char) n; nOff++; return TRUE;
	} else return FALSE;
}

BOOL SbiBuffer::operator +=( UINT8 n )
{
	if( Check( 1 ) )
	{
		*pCur++ = (char) n; nOff++; return TRUE;
	} else return FALSE;
}

BOOL SbiBuffer::operator +=( INT16 n )
{
	if( Check( 2 ) )
	{
		*pCur++ = (char) ( n & 0xFF );
		*pCur++ = (char) ( n >> 8 );
		nOff += 2; return TRUE;
	} else return FALSE;
}

BOOL SbiBuffer::operator +=( UINT16 n )
{
	if( Check( 2 ) )
	{
		*pCur++ = (char) ( n & 0xFF );
		*pCur++ = (char) ( n >> 8 );
		nOff += 2; return TRUE;
	} else return FALSE;
}

BOOL SbiBuffer::operator +=( UINT32 n )
{
	if( Check( 4 ) )
	{
		UINT16 n1 = static_cast<UINT16>( n & 0xFFFF );
		UINT16 n2 = static_cast<UINT16>( n >> 16 );
		if ( operator +=( n1 ) && operator +=( n2 ) )
			return TRUE;
		return TRUE;
	} 
	return FALSE;
}

BOOL SbiBuffer::operator +=( INT32 n )
{
	return operator +=( (UINT32) n );
}


BOOL SbiBuffer::operator +=( const String& n )
{
	USHORT l = n.Len() + 1;
	if( Check( l ) )
	{
		ByteString aByteStr( n, gsl_getSystemTextEncoding() );
		memcpy( pCur, aByteStr.GetBuffer(), l );
		pCur += l;
		nOff = nOff + l;
		return TRUE;
	}
	else return FALSE;
}

}
