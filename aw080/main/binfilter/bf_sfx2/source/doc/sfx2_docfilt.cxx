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



// INCLUDE ---------------------------------------------------------------

#ifdef SOLARIS
// HACK: prevent conflict between STLPORT and Workshop headers on Solaris 8
#include <ctime>
#endif

#include <string> // HACK: prevent conflict between STLPORT and Workshop headers


#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "fltfnc.hxx"
#include "sfxuno.hxx"

namespace binfilter {

// STATIC DATA -----------------------------------------------------------

/*N*/ DBG_NAME(SfxFilter)
/*N*/ 
/*N*/ SfxFilter::SfxFilter(  const String &rName,
/*N*/ 					   const String &rWildCard,
/*N*/ 					   SfxFilterFlags nType,
/*N*/ 					   ULONG lFmt,
/*N*/ 					   const String &rMacTyp,
/*N*/ 					   const String &rTypNm,
/*N*/ 					   sal_uInt16 nIcon,
/*N*/ 					   const String &rMimeType,
/*N*/ 					   const SfxFilterContainer* pContainerP,
/*N*/ 					   const String &rUsrDat ):
/*N*/ 	lFormat(lFmt),
/*N*/ 	nFormatType(nType),
/*N*/ 	aWildCard(rWildCard, ';'),
/*N*/ 	aMacType(rMacTyp),
/*N*/ 	aTypeName(rTypNm),
/*N*/ 	nDocIcon(nIcon),
/*N*/ 	aUserData(rUsrDat),
/*N*/ 	aName( rName ),
/*N*/ 	aMimeType( rMimeType ),
/*N*/ 	aFilterName( rName ),
/*N*/ 	pContainer( pContainerP )
/*N*/ {
/*N*/ 	InitMembers_Impl();
/*N*/ 	pContainer = pContainerP;
/*N*/ 	aFilterName = rName;
/*N*/ 	aMimeType = rMimeType;
/*N*/ }

/*N*/ void SfxFilter::InitMembers_Impl()
/*N*/ {
/*N*/ 	String aExts = GetWildcard()();
/*N*/ 	String aShort, aLong;
/*N*/ 	String aRet;
/*N*/ 	sal_uInt16 nMaxLength =
/*N*/ #if defined( WIN ) || defined( OS2 )
/*N*/ 		3
/*N*/ #else
/*N*/ 		USHRT_MAX
/*N*/ #endif
/*N*/ 		;
/*N*/ 	String aTest;
/*N*/ 	sal_uInt16 nPos = 0;
/*N*/ 	while( ( aRet = aExts.GetToken( nPos++, ';' ) ).Len() )
/*N*/ 	{
/*N*/ 		aTest = aRet;
/*N*/ 		aTest.SearchAndReplace( DEFINE_CONST_UNICODE( "*." ), String() );
/*N*/ 		if( aTest.Len() <= nMaxLength )
/*N*/ 		{
/*N*/ 			if( aShort.Len() ) aShort += ';';
/*N*/ 			aShort += aRet;
/*N*/ 		}
/*N*/ 		else
/*N*/ 		{
/*N*/ 			if( aLong.Len() ) aLong += ';';
/*N*/ 			aLong += aRet;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	if( aShort.Len() && aLong.Len() )
/*N*/ 	{
/*N*/ 		aShort += ';';
/*N*/ 		aShort += aLong;
/*N*/ 	}
/*N*/ 	aWildCard = aShort;
/*N*/ 
/*N*/     nVersion = SOFFICE_FILEFORMAT_50;
/*N*/ 	bPlugDataSearched = 0;
/*N*/ 	pPlugData = 0;
/*N*/ 	aUIName = aFilterName;
/*N*/ }

/*N*/ SfxFilter::~SfxFilter()
/*N*/ {
/*N*/ 	delete pPlugData;
/*N*/ }

/*N*/ const String&	SfxFilter::GetTypeName() const
/*N*/ {
/*N*/ 	return
/*N*/ #ifdef MAC
/*N*/ 	aMacType;
/*N*/ #else
/*N*/ 	aTypeName;
/*N*/ #endif
/*N*/ }

/*N*/ String SfxFilter::GetFilterNameWithPrefix() const
/*N*/ {
/*N*/     String aName = pContainer->GetName();
/*N*/ 	aName += DEFINE_CONST_UNICODE( ": " );
/*N*/ 	aName += aFilterName;
/*N*/     return aName;
/*N*/ }
}
