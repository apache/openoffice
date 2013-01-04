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



#ifndef _BUFFER_HXX
#define _BUFFER_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

namespace binfilter {

class SbiParser;

class SbiBuffer { 					// Code/Konstanten-Puffer:
	SbiParser* pParser;				// fuer Fehlermeldungen
	char*	pBuf;					// Puffer-Pointer
	char*	pCur;					// aktueller Puffer-Pointer
	UINT32	nOff;					// aktuelles Offset
	UINT32	nSize;					// aktuelle Groesse
	short	nInc;					// Inkrement
	BOOL    Check( USHORT );		// Buffergroesse testen
public:
	SbiBuffer( SbiParser*, short );	// Inkrement
   ~SbiBuffer();
	BOOL operator += (const String&);// Basic-String speichern
	BOOL operator += (INT8);		// Zeichen speichern
	BOOL operator += (INT16);		// Integer speichern
	BOOL operator += (UINT8);		// Zeichen speichern
	BOOL operator += (UINT16);		// Integer speichern
	BOOL operator += (UINT32);		// Integer speichern
	BOOL operator += (INT32);		// Integer speichern
	char*  GetBuffer();				// Puffer rausgeben (selbst loeschen!)
	char*  GetBufferPtr(){ return pBuf; }
	UINT32 GetSize()				{ return nOff; }
};

}

#endif
