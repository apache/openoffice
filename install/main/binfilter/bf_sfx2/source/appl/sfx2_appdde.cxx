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



#ifndef _SVDDE_HXX //autogen
#include <bf_svtools/svdde.hxx>
#endif
#include <tools/urlobj.hxx>
#include <bf_svtools/pathoptions.hxx>
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "app.hxx"
#include "appdata.hxx"
#include "objsh.hxx"
#include "sfxsids.hrc"

#include <bf_sfx2/sfxuno.hxx>

namespace binfilter {

//========================================================================

/*?*/ long SfxObjectShell::DdeExecute
/*?*/ (
/*?*/ 	const String&	rCmd		// in unserer BASIC-Syntax formuliert
/*?*/ )

/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 
/*?*/ }

//--------------------------------------------------------------------

/*?*/ long SfxObjectShell::DdeGetData
/*?*/ (
/*?*/ 	const String&	rItem,						// das anzusprechende Item
/*?*/ 	const String& rMimeType,					// in: Format
/*?*/ 	::com::sun::star::uno::Any & rValue 	// out: angeforderte Daten
/*?*/ )
{DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 
/*?*/ }

//--------------------------------------------------------------------

/*?*/ long SfxObjectShell::DdeSetData
/*?*/ (
/*?*/ 	const String&	rItem,						// das anzusprechende Item
/*?*/ 	const String& rMimeType,					// in: Format
/*?*/ 	const ::com::sun::star::uno::Any & rValue 	// out: angeforderte Daten
/*?*/ )
/*?*/ 
/*	[Beschreibung]

	Diese Methode kann vom Applikationsentwickler "uberladen werden,
	um an seine SfxObjectShell-Subklasse gerichtete DDE-Daten
	zu empfangen.

	Die Basisimplementierung nimmt keine Daten entgegen und liefert 0 zur"uck.
*/
/*?*/ 
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 
/*?*/ }

//--------------------------------------------------------------------

/*?*/ ::binfilter::SvLinkSource* SfxObjectShell::DdeCreateLinkSource
/*?*/ (
/*?*/ 	const String&	rItem	   // das zu erzeugende Item
/*?*/ )

/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 
/*?*/ }

//--------------------------------------------------------------------
}
