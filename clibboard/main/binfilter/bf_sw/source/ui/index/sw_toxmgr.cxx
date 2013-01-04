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



#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SHELLRES_HXX
#include <shellres.hxx>
#endif

#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif

#include <tox.hxx>

namespace binfilter {

/*-----------------23.01.98 07:41-------------------
	Standard - Verzeichnisnamen herausreichen
--------------------------------------------------*/
/*N*/ const String&	SwTOXBase::GetTOXName(TOXTypes eType)
/*N*/ {
/*N*/ 	const String* pRet;
/*N*/ 	ShellResource* pShellRes = ViewShell::GetShellRes();
/*N*/ 	switch(eType)
/*N*/ 	{
/*N*/ 		case  TOX_CONTENT 	   :	pRet = &pShellRes->aTOXContentName;		  break;
/*N*/ 		case  TOX_INDEX 	   :    pRet = &pShellRes->aTOXIndexName;         break;
/*?*/ 		case  TOX_USER 		   :    pRet = &pShellRes->aTOXUserName;       	  break;
/*?*/ 		case  TOX_ILLUSTRATIONS:    pRet = &pShellRes->aTOXIllustrationsName; break;
/*?*/ 		case  TOX_OBJECTS	   :    pRet = &pShellRes->aTOXObjectsName;       break;
/*?*/ 		case  TOX_TABLES	   :    pRet = &pShellRes->aTOXTablesName;        break;
/*?*/ 		case  TOX_AUTHORITIES :     pRet = &pShellRes->aTOXAuthoritiesName;   break;
/*N*/ 	}
/*N*/ 	return *pRet;
/*N*/ }

}
