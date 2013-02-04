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


#ifndef _W4WFLT_HXX
#define _W4WFLT_HXX

#include <bf_svtools/bf_solar.h>
class String; 
namespace binfilter {



// suche fuer das File den enstsprechenden Filter Typ
// und die Version
USHORT AutoDetec( const String& rFileName, USHORT & rVersion );

// W4W speichert beim Laden vom File pFileName die Ausgabe in pTmpFile.
// (das Tempfile wird hier anleget!)
// der Filter wird ueber nFilter bestimmt.
ULONG LoadFile( const String& rFileName, USHORT nFilter,
				const String& rVersion, String & rTmpFile );

enum W4WDLL_TYPE { W4WDLL_EXPORT, W4WDLL_IMPORT, W4WDLL_AUTODETEC };
FASTBOOL W4WDLLExist( W4WDLL_TYPE eType, USHORT nFilter = 0 );


} //namespace binfilter
#endif	// _W4WFLT_HXX
