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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

#include "sdiocmpt.hxx"
namespace binfilter {





/*************************************************************************
|*
|* Konstruktor, schreibt bzw. liest Versionsnummer
|*
\************************************************************************/

/*N*/ SdIOCompat::SdIOCompat(SvStream& rNewStream, USHORT nNewMode, UINT16 nVer)
/*N*/ 		   : SdrDownCompat(rNewStream, nNewMode, TRUE),
/*N*/ 			 nVersion(nVer)
/*N*/ {
/*N*/ 	if (nNewMode == STREAM_WRITE)
/*N*/ 	{
/*N*/ 		DBG_ASSERT(nVer != SDIOCOMPAT_VERSIONDONTKNOW,
/*N*/ 				   "kann unbekannte Version nicht schreiben");
/*N*/ 		rNewStream << nVersion;
/*N*/ 	}
/*N*/ 	else if (nNewMode == STREAM_READ)
/*N*/ 	{
/*N*/ 		DBG_ASSERT(nVer == SDIOCOMPAT_VERSIONDONTKNOW,
/*N*/ 				   "Lesen mit Angabe der Version ist Quatsch!");
/*N*/ 		rNewStream >> nVersion;
/*N*/ 	}
/*N*/ }





}
