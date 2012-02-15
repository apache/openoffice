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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

//------------------------------------------------------------------------

#include <bf_svtools/zformat.hxx>
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#include <tools/debug.hxx>

#include "zforauto.hxx"
#include "global.hxx"
namespace binfilter {

static const sal_Char __FAR_DATA pStandardName[] = "Standard";

//------------------------------------------------------------------------

/*N*/ ScNumFormatAbbrev::ScNumFormatAbbrev() :
/*N*/ 	eLnge			(LANGUAGE_SYSTEM),
/*N*/ 	eSysLnge		(LANGUAGE_GERMAN),		// sonst passt "Standard" nicht
/*N*/ 	sFormatstring	( RTL_CONSTASCII_USTRINGPARAM( pStandardName ) )
/*N*/ {
/*N*/ }

/*N*/ ScNumFormatAbbrev::ScNumFormatAbbrev(const ScNumFormatAbbrev& aFormat) :
/*N*/ 	eLnge			(aFormat.eLnge),
/*N*/ 	eSysLnge		(aFormat.eSysLnge),
/*N*/ 	sFormatstring	(aFormat.sFormatstring)
/*N*/ {
/*N*/ }

/*N*/ void ScNumFormatAbbrev::Load( SvStream& rStream )
/*N*/ {
/*N*/ 	USHORT nSysLang, nLang;
/*N*/ 	rStream.ReadByteString( sFormatstring, rStream.GetStreamCharSet() );
/*N*/ 	rStream >> nSysLang >> nLang;
/*N*/ 	eLnge = (LanguageType) nLang;
/*N*/ 	eSysLnge = (LanguageType) nSysLang;
/*N*/     if ( eSysLnge == LANGUAGE_SYSTEM )          // old versions did write it
/*N*/         eSysLnge = Application::GetSettings().GetLanguage();
/*N*/ }

/*N*/ void ScNumFormatAbbrev::Save( SvStream& rStream ) const
/*N*/ {
/*N*/ 	rStream.WriteByteString( sFormatstring, rStream.GetStreamCharSet() );
/*N*/ 	rStream << (USHORT) eSysLnge << (USHORT) eLnge;
/*N*/ }







}
