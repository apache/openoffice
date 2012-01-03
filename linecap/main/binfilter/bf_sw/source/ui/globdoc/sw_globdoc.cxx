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

#ifndef _SO_CLSIDS_HXX
#include <comphelper/classids.hxx>
#endif

#include "swtypes.hxx"


#include "shellio.hxx"
#include "globdoc.hxx"
#include "globdoc.hrc"
namespace binfilter {

/*--------------------------------------------------------------------
	Beschreibung:	Alle Filter registrieren
 --------------------------------------------------------------------*/

// 4.0: {340AC970-E30D-11d0-A53F-00A0249D57B1}
// 4.0:	0x340ac970, 0xe30d, 0x11d0, 0xa5, 0x3f, 0x0, 0xa0, 0x24, 0x9d, 0x57, 0xb1
// 5.0: 0xc20cf9d3, 0x85ae, 0x11d1, 0xaa, 0xb4, 0x0, 0x60, 0x97, 0xda, 0x56, 0x1a
/*N*/ SFX_IMPL_OBJECTFACTORY_LOD(SwGlobalDocShell, SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU, /*swriter4/GlobalDocument,*/ \
/*N*/ 		SvGlobalName(BF_SO3_SWGLOB_CLASSID) , Sw)

/*N*/ TYPEINIT1(SwGlobalDocShell, SwDocShell);

/*N*/ SwGlobalDocShell::SwGlobalDocShell(SfxObjectCreateMode eMode ) :
/*N*/ 		SwDocShell(eMode)
/*N*/ {
/*N*/ }

/*N*/ SwGlobalDocShell::~SwGlobalDocShell()
/*N*/ {
/*N*/ }

/*N*/ void SwGlobalDocShell::FillClass( SvGlobalName * pClassName,
/*N*/ 								   ULONG * pClipFormat,
/*N*/ 								   String * pAppName,
/*N*/ 								   String * pLongUserName,
/*N*/ 								   String * pUserName,
/*N*/ 								   long nVersion ) const
/*N*/ {
/*N*/ 	SfxInPlaceObject::FillClass(pClassName, pClipFormat, pAppName, pLongUserName,
/*N*/ 								pUserName, nVersion);
/*N*/ 
/*N*/ 	if (nVersion == SOFFICE_FILEFORMAT_40)
/*N*/ 	{
/*?*/ 		*pClassName = SvGlobalName( BF_SO3_SWGLOB_CLASSID_40 );
/*?*/ 		*pClipFormat = SOT_FORMATSTR_ID_STARWRITERGLOB_40;
/*?*/ 		*pAppName = String::CreateFromAscii("StarWriter 4.0/GlobalDocument");
/*?*/ 
/*?*/ 		*pLongUserName = SW_RESSTR(STR_WRITER_GLOBALDOC_FULLTYPE_40);
/*N*/ 	}
/*N*/ 	else if (nVersion == SOFFICE_FILEFORMAT_50)
/*N*/ 	{
/*N*/ 		*pClassName = SvGlobalName( BF_SO3_SWGLOB_CLASSID_50 );
/*N*/ 		*pClipFormat = SOT_FORMATSTR_ID_STARWRITERGLOB_50;
/*N*/ 		*pLongUserName = SW_RESSTR(STR_WRITER_GLOBALDOC_FULLTYPE_50);
/*N*/ 	}
/*N*/ 	else if (nVersion == SOFFICE_FILEFORMAT_60)
/*N*/ 	{
/*N*/ 		*pLongUserName = SW_RESSTR(STR_WRITER_GLOBALDOC_FULLTYPE);

			// for binfilter, we need the FormatIDs to be set. Not setting them
			// has always been an error (!)
			*pClassName = SvGlobalName( BF_SO3_SWGLOB_CLASSID_60 );
			*pClipFormat = SOT_FORMATSTR_ID_STARWRITERGLOB_60;

/*N*/ 	}
/*N*/ 
/*N*/ 	*pUserName = SW_RESSTR(STR_HUMAN_SWGLOBDOC_NAME);
/*N*/ }




}
