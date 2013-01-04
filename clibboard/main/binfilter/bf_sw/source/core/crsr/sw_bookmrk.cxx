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


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif


#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _BOOKMRK_HXX
#include <bookmrk.hxx>
#endif
#ifndef _SWSERV_HXX
#include <swserv.hxx>
#endif

#ifndef _ERRHDL_HXX //autogen
#include <errhdl.hxx>
#endif
namespace binfilter {

/*N*/ SV_IMPL_REF( SwServerObject )

/*N*/ SwBookmark::SwBookmark(const SwPosition& aPos, const KeyCode& rCode,
/*N*/ 						const String& rName, const String& rShortName )
/*N*/ 	: SwModify( 0 ),
/*N*/ 	aStartMacro( aEmptyStr, aEmptyStr ),
/*N*/ 	aEndMacro  ( aEmptyStr, aEmptyStr ),
/*N*/ 	aCode(rCode),
/*N*/ 	aName(rName),
/*N*/ 	aShortName(rShortName),
/*N*/ 	pPos2( 0 ),
/*N*/ 	eMarkType( BOOKMARK )
/*N*/ {
/*N*/ 	pPos1 = new SwPosition(aPos);
/*N*/ }

// Beim Loeschen von Text werden Bookmarks mitgeloescht!


/*N*/ SwBookmark::~SwBookmark()
/*N*/ {
/*N*/ 	// falls wir noch der DDE-Bookmark sind, dann muss der aus dem
/*N*/ 	// Clipboard ausgetragen werden. Wird automatisch ueber DataChanged
/*N*/ 	// ausgeloest.
/*N*/ 	if( refObj.Is() )
/*N*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ 
/*N*/ 	delete pPos1;
/*N*/ 	if( pPos2 )
/*N*/ 		delete pPos2;
/*N*/ }

// Vergleiche auf Basis der Dokumentposition

/*N*/ BOOL SwBookmark::operator<(const SwBookmark &rBM) const
/*N*/ {
/*N*/ 	const SwPosition* pThisPos = ( !pPos2 || *pPos1 <= *pPos2 ) ? pPos1 : pPos2;
/*N*/ 	const SwPosition* pBMPos = ( !rBM.pPos2 || *rBM.pPos1 <= *rBM.pPos2 )
/*N*/ 										? rBM.pPos1 : rBM.pPos2;
/*N*/ 
/*N*/ 	return *pThisPos < *pBMPos;
/*N*/ }

/*N*/ BOOL SwBookmark::operator==(const SwBookmark &rBM) const
/*N*/ {
/*N*/ 	return (this == &rBM);
/*N*/ }

/*N*/ SwUNOMark::SwUNOMark( const SwPosition& aPos,
/*N*/ 				const KeyCode& rCode,
/*N*/ 				const String& rName,
/*N*/ 				const String& rShortName )
/*N*/ 	: SwBookmark( aPos, rCode, rName, rShortName )
/*N*/ {
/*N*/ 	eMarkType = UNO_BOOKMARK;
/*N*/ }

}
