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



#ifndef _DBTOKEN_HXX
#define _DBTOKEN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_COMPILER_HXX
#include "compiler.hxx"
#endif
namespace binfilter {


// SET DBG_TOKEN=string
// +  - make new data
// -  - do not make old data
// aA - dump old/new token array
// uU - dump old/new UPN array

BOOL DbgToken( char );
ScTokenArray* DbgMakeTokenArray
	( ScDocument* pDoc, USHORT nTab, const String& r, USHORT& rErr );
void DbgDumpTokenArray( const BYTE* pArr, USHORT nLen, const char* pMsg );
void DbgDelTokenArray( ScTokenArray* );
StackVar DbgInterpret( ScDocument* pDok, const ScTokenArray* pToken,
					   USHORT nCOL, USHORT nROW, USHORT nTAB,
								  StackVar eStackVar,
								  char* &rStringErgPtr,
								  double& rDoubleErg,
								  USHORT& rError,
								  ScMatrix** ppMat);

} //namespace binfilter
#endif

