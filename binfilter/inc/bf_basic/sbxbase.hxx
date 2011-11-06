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



#ifndef _SBXBASE_HXX
#define _SBXBASE_HXX

#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif

#ifndef _LIST_HXX
#include "tools/list.hxx"
#endif

#ifndef _SVARRAY_HXX
#include "bf_svtools/svarray.hxx"
#endif

#ifndef _SBXDEF_HXX
#include "sbxdef.hxx"
#endif

namespace binfilter {

class SbxFactory;
class SbxVariable;
class SbxBasicFormater;

SV_DECL_PTRARR_DEL(SbxFacs,SbxFactory*,5,5)
DECLARE_LIST(SbxVarList_Impl, SbxVariable*)

// AppData-Struktur fuer SBX:
struct SbxAppData
{
	SbxError			eSbxError;	// Fehlercode
	SbxFacs 			aFacs;		// Factories
	SbxVarList_Impl		aVars;		// fuer Dump
											// Format()-Befehl
    LanguageType        eBasicFormaterLangType;
		// Bem.: es ist sinnvoll diese Klasse 'global' zu speichern, da
		// 		 einige Resourcen (Strings) in der Klasse gehalten werden.

	SbxAppData() : eSbxError( SbxERR_OK ), aFacs() {}
};

SbxAppData* GetSbxData_Impl();

}

#endif
