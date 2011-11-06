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


#ifndef _DOCFUNC_HXX
#define _DOCFUNC_HXX

#include <bf_svtools/bf_solar.h>


// Diese Klasse wird von der Editshell benutz um fuer belibige
// Doc-Aktionen ueber eine definierte Schnittstelle alle Cursor/
// Bookmarks/usw. aus dem Loeschbereich zu verschieben, mit einem
// anschliessenden Join-Node.
// Das wird zur Zeit vom
//		- Delete
//		- Replace
// benotigt.


#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
namespace binfilter {
class SwPaM;

enum DocFNType { DOC_DELETE, DOC_REPLACE };

class SwCallDocFunction
{
	// Parameter fuers Replace	(leider kein union moeglich; Str-Klasse !!)
	const String aRplStr;
	DocFNType eDocFnType;
	BOOL bRplRegExp;

public:
	// fuer parameterlose Methoden
	SwCallDocFunction( DocFNType eDocFn ) : eDocFnType( eDocFn ), aRplStr() {}
	// fuer das Replace
	SwCallDocFunction( const String& rStr, BOOL bFlag );

	BOOL Call( SwPaM& );

	// ALLE Methoden stehen im eddel.cxx !!
};


} //namespace binfilter
#endif
	//_DOCFUNC_HXX


