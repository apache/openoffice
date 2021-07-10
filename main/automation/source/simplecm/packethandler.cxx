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
#include "precompiled_automation.hxx"

/*************************************************************************
 *
 *	  ATTENTION
 *	  This file is intended to work inside and outside the StarOffice environment.
 *	  Only adaption of file commtypes.hxx should be necessary. Else it is a bug!
 *
 ************************************************************************/

#include "packethandler.hxx"
#include <automation/commtypes.hxx>
#include <automation/commdefines.hxx>
#include "communiio.hxx"

/**
Forces switch to multichannel headers even for old communication Method
**/
#define FORCE_MULTI_CHANNEL_HEADERS


PacketHandler::PacketHandler( ITransmiter* pTransmitter_, IReceiver* pReceiver_, comm_BOOL bMC )
: pTransmitter( pTransmitter_ )
, pReceiver( pReceiver_ )
, bMultiChannel( bMC )
{
}

unsigned char PacketHandler::CalcCheckByte( comm_UINT32 nBytes )
{
	comm_UINT16 nRes = 0;
	nRes += HIBYTE( HIWORD( nBytes ) ) ^ 0xf0;
	nRes += LOBYTE( HIWORD( nBytes ) ) ^ 0x0f;
	nRes += HIBYTE( LOWORD( nBytes ) ) ^ 0xf0;
	nRes += LOBYTE( LOWORD( nBytes ) ) ^ 0x0f;

	nRes ^= HIBYTE( nRes );

	return LOBYTE( nRes );
}


#define READ_SOCKET( pBuffer, nLength )\
	if ( !bWasError )\
	{\
		bWasError |= pReceiver->ReceiveBytes( pBuffer, nLength ) != C_ERROR_NONE;\
	}

#define READ_SOCKET_LEN( pBuffer, nLength, nTotal )\
	READ_SOCKET( pBuffer, nLength );\
	if ( !bWasError )\
		{nTotal += nLength;}

comm_BOOL PacketHandler::ReceiveData( void* &pData, comm_UINT32 &nLen )
{
	DBG_ASSERT( !pData, "pData should be NULL -> memory leak" );

	nLen = 0;
	pData = NULL;
	comm_BOOL bWasError = sal_False;
	comm_BOOL bForceMultiChannelThisPacket = sal_False;
	if ( pReceiver )
	{
		comm_UINT32 nBytes = 0;
		nReceiveProtocol = CM_PROTOCOL_OLDSTYLE;
		nReceiveHeaderType = CH_NoHeader;

		READ_SOCKET( &nBytes, sizeof(nBytes) )
		if ( bWasError )
			return sal_False;

		if ( 0xFFFFFFFF == nBytes )		// Expliziter Request f�r dieses Datenpaket auf MultiChannel umzuschalten
		{
			READ_SOCKET( &nBytes, sizeof(nBytes) )
			if ( bWasError )
				return sal_False;
			bForceMultiChannelThisPacket = sal_True;
		}

		nBytes = NETDWORD( nBytes );

		if ( bMultiChannel || bForceMultiChannelThisPacket )
		{
			comm_ULONG nReadSoFar = 0;
			comm_ULONG nHeaderReadSoFar = 0;

			// Pr�fbyte f�r L�ngenangabe
			unsigned char nLenCheck = 0;
			READ_SOCKET_LEN( &nLenCheck, 1, nReadSoFar );
			// Stimmt das Pr�fbyte?
			bWasError |= nLenCheck != CalcCheckByte( nBytes );


			comm_UINT16 nHeaderBytes = 0;
			READ_SOCKET_LEN( &nHeaderBytes, 2, nReadSoFar );
			nHeaderBytes = NETWORD( nHeaderBytes );
			// reicht der Header �ber das Ende hinaus?
			bWasError |= !(nBytes >= nReadSoFar + nHeaderBytes);

			READ_SOCKET_LEN( &nReceiveHeaderType, 2, nHeaderReadSoFar );
			nReceiveHeaderType = NETWORD( nReceiveHeaderType );

			switch ( nReceiveHeaderType )
			{
			case CH_SimpleMultiChannel:
				{
					READ_SOCKET_LEN( &nReceiveProtocol, 2, nHeaderReadSoFar );
					nReceiveProtocol = NETWORD( nReceiveProtocol );
				}
				break;
			case CH_Handshake:
				{
				}
				break;
			default:
				{
					DBG_ERROR("Unbekannter Headertyp in der Kommunikation");
					bWasError = sal_True;
				}

			}

			if ( bWasError )
				return sal_False;

			/// L�ngen anpassen und ggf restheader �berlesen.
			while ( nHeaderBytes > nHeaderReadSoFar )
			{
				unsigned char nDummy;
				READ_SOCKET_LEN( &nDummy, 1, nHeaderReadSoFar );
			}

			nReadSoFar += nHeaderReadSoFar;
			nBytes -= nReadSoFar;

		}

		/* @@@ Notes @@@
		 *
		 * 1) a 'void*' allocated via 'new char[]' is always deallocated
		 * via plain 'delete()', not via array 'delete[]()'; it's just
		 * raw memory.
		 *
		 * 2) as the caller of this routine later-on changes ownership
		 * of 'pData' via 'SvMemoryStream::SetBuffer()' (in 'simplecm.cxx',
		 * 'SimpleCommunicationLinkViaSocket::DoReceiveDataStream()'),
		 * the allocator used here for 'void* pData' must match the
		 * deallocator used in 'SvMemoryStream::FreeMemory()', i.e.
		 * '::operator delete()'.
		 */
		pData = ::operator new(nBytes);
		READ_SOCKET( pData, nBytes )
		if ( bWasError )
		{
			::operator delete(pData), pData = 0;
			return sal_False;
		}
		nLen = nBytes;
	}
	else
		bWasError = sal_True;

	return !bWasError;
}

/*#define WRITE_SOCKET( pBuffer, nLength )\
	if ( !bWasError )\
		bWasError |= !pStreamSocket || (pStreamSocket->write( pBuffer, nLength ) != nLength)*/

#define WRITE_SOCKET( pBuffer, nLength )\
	if ( !bWasError )\
		{bWasError |= pTransmitter->TransferBytes( pBuffer, nLength ) != C_ERROR_NONE;}



comm_BOOL PacketHandler::TransferData( const void* pData, comm_UINT32 nLen, CMProtocol nProtocol )
{
	comm_UINT32 nBuffer = nLen;
	comm_BOOL bWasError = sal_False;

#ifndef FORCE_MULTI_CHANNEL_HEADERS
	if ( bMultiChannel )
#endif
		nBuffer += 1+2+2+2;	// f�r einen CH_SimpleMultiChannel

#ifdef FORCE_MULTI_CHANNEL_HEADERS
	if ( !bMultiChannel )
	{
		comm_UINT32 n32;
		n32 = 0xffffffff;	// Umschalten auf MultiChannel
		n32 = NETDWORD( n32 );
		WRITE_SOCKET( &n32, 4 );
	}
#endif


	comm_UINT32 nNetworkBuffer = NETDWORD( nBuffer );
	WRITE_SOCKET( &nNetworkBuffer, sizeof(nNetworkBuffer) );


#ifndef FORCE_MULTI_CHANNEL_HEADERS
	if ( bMultiChannel )
#endif
	{
		comm_UINT16 n16;
		unsigned char c;

		c = CalcCheckByte( nBuffer );
		WRITE_SOCKET( &c, 1 );

		n16 = 4;	// L�nge des Headers f�r einen CH_SimpleMultiChannel
		n16 = NETWORD( n16 );
		WRITE_SOCKET( &n16, 2 );

		n16 = CH_SimpleMultiChannel;	// Typ des Headers
		n16 = NETWORD( n16 );
		WRITE_SOCKET( &n16, 2 );

		nProtocol = NETWORD( nProtocol );
		WRITE_SOCKET( &nProtocol, 2 );
	}

	WRITE_SOCKET( pData, nLen );
	return !bWasError;
}

comm_BOOL PacketHandler::SendHandshake( HandshakeType aHandshakeType, const void* pData, comm_UINT32 nLen )
{
	comm_BOOL bWasError = sal_False;

	comm_UINT32 nBuffer = 0;

//	if ( pMyManager->IsMultiChannel() )		Wir senden immer FFFFFFFF vorweg -> immer MultiChannel (Oder GPF bei �lteren)
		nBuffer += 1+2+2;	// f�r einen CH_Handshake

	nBuffer += 2;	// f�r den Typ des Handshakes

	switch ( aHandshakeType )
	{
		case CH_REQUEST_HandshakeAlive:
			nBuffer += 0;	// Keine extra Daten
			break;
		case CH_RESPONSE_HandshakeAlive:
			nBuffer += 0;	// Keine extra Daten
			break;
		case CH_REQUEST_ShutdownLink:
			nBuffer += 0;	// Keine extra Daten
			break;
		case CH_ShutdownLink:
			nBuffer += 0;	// Keine extra Daten
			break;
		case CH_SUPPORT_OPTIONS:
			nBuffer += 2 ;	// one word extradata for options
			break;
		case CH_SetApplication:
			nBuffer += 0 ;	// one word extradata for options
			break;
		default:
			DBG_ERROR("Unknown HandshakeType");
	}

	if ( pData )
		nBuffer += nLen;	// Extra data in Buffer

	comm_UINT32 n32;
	n32 = 0xffffffff;	// Umschalten auf MultiChannel
	n32 = NETDWORD( n32 );
	WRITE_SOCKET( &n32, 4 );

	comm_UINT32 nNetworkBuffer = NETDWORD( nBuffer );
	WRITE_SOCKET( &nNetworkBuffer, sizeof(nNetworkBuffer) );


	comm_UINT16 n16;
	unsigned char c;

	c = CalcCheckByte( nBuffer );
	WRITE_SOCKET( &c, 1 );

	n16 = 2;	// L�nge des Headers f�r einen CH_Handshake
	n16 = NETWORD( n16 );
	WRITE_SOCKET( &n16, 2 );

	n16 = CH_Handshake;	// Typ des Headers
	n16 = NETWORD( n16 );
	WRITE_SOCKET( &n16, 2 );

	n16 = aHandshakeType;	// Typ des Handshakes
	n16 = NETWORD( n16 );
	WRITE_SOCKET( &n16, 2 );


	switch ( aHandshakeType )
	{
		case CH_SUPPORT_OPTIONS:
			n16 = OPT_USE_SHUTDOWN_PROTOCOL;
			n16 = NETWORD( n16 );
			WRITE_SOCKET( &n16, 2 );
			break;
	}

	if ( pData )
		WRITE_SOCKET( pData, nLen );

	return !bWasError;
}
