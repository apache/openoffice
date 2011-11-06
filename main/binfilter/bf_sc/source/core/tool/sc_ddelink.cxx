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

// INCLUDE ---------------------------------------------------------------

#include <bf_svtools/zforlist.hxx>

#include "ddelink.hxx"
#include "document.hxx"
#include "scmatrix.hxx"
#include "rechead.hxx"
#include "bf_sc.hrc"
#include "hints.hxx"
namespace binfilter {

/*N*/ TYPEINIT2(ScDdeLink,::binfilter::SvBaseLink,SfxBroadcaster);

#ifdef PM2
#define DDE_TXT_ENCODING    RTL_TEXTENCODING_IBM_850
#else
#define DDE_TXT_ENCODING 	RTL_TEXTENCODING_MS_1252
#endif

/*N*/ BOOL ScDdeLink::bIsInUpdate = FALSE;

//------------------------------------------------------------------------

/*N*/ ScDdeLink::ScDdeLink( ScDocument* pD, const String& rA, const String& rT, const String& rI,
/*N*/ 						BYTE nM ) :
/*N*/ 	::binfilter::SvBaseLink(::binfilter::LINKUPDATE_ALWAYS,FORMAT_STRING),
/*N*/ 	pDoc( pD ),
/*N*/ 	aAppl( rA ),
/*N*/ 	aTopic( rT ),
/*N*/ 	aItem( rI ),
/*N*/ 	nMode( nM ),
/*N*/ 	pResult( NULL ),
/*N*/ 	bNeedUpdate( FALSE )
/*N*/ {
/*N*/ }

/*N*/ __EXPORT ScDdeLink::~ScDdeLink()
/*N*/ {
/*N*/ 	// Verbindung aufheben
/*N*/ 
/*N*/ 	delete pResult;
/*N*/ }

/*N*/ ScDdeLink::ScDdeLink( ScDocument* pD, SvStream& rStream, ScMultipleReadHeader& rHdr ) :
/*N*/ 	::binfilter::SvBaseLink(::binfilter::LINKUPDATE_ALWAYS,FORMAT_STRING),
/*N*/ 	pDoc( pD ),
/*N*/ 	pResult( NULL ),
/*N*/ 	bNeedUpdate( FALSE )
/*N*/ {
/*N*/ 	rHdr.StartEntry();
/*N*/ 
/*N*/ 	rtl_TextEncoding eCharSet = rStream.GetStreamCharSet();
/*N*/ 	rStream.ReadByteString( aAppl, eCharSet );
/*N*/ 	rStream.ReadByteString( aTopic, eCharSet );
/*N*/ 	rStream.ReadByteString( aItem, eCharSet );
/*N*/ 
/*N*/ 	BOOL bHasValue;
/*N*/ 	rStream >> bHasValue;
/*N*/ 	if ( bHasValue )
/*N*/ 		pResult = new ScMatrix( rStream );
/*N*/ 
/*N*/ 	if (rHdr.BytesLeft())		// neu in 388b und der 364w (RealTime-Client) Version
/*N*/ 		rStream >> nMode;
/*N*/ 	else
/*N*/ 		nMode = SC_DDE_DEFAULT;
/*N*/ 
/*N*/ 	rHdr.EndEntry();
/*N*/ }

/*N*/ void ScDdeLink::Store( SvStream& rStream, ScMultipleWriteHeader& rHdr ) const
/*N*/ {
/*N*/ 	rHdr.StartEntry();
/*N*/ 
/*N*/ 	rtl_TextEncoding eCharSet = rStream.GetStreamCharSet();
/*N*/ 	rStream.WriteByteString( aAppl, eCharSet );
/*N*/ 	rStream.WriteByteString( aTopic, eCharSet );
/*N*/ 	rStream.WriteByteString( aItem, eCharSet );
/*N*/ 
/*N*/ 	BOOL bHasValue = ( pResult != NULL );
/*N*/ 	rStream << bHasValue;
/*N*/ 	if (bHasValue)
/*N*/ 		pResult->Store( rStream );
/*N*/ 
/*N*/ 	if( rStream.GetVersion() > SOFFICE_FILEFORMAT_40 )		// nicht bei 4.0 Export
/*N*/ 		rStream << nMode;									// seit 388b
/*N*/ 
/*N*/ 	//	Links mit Mode != SC_DDE_DEFAULT werden bei 4.0 Export komplett weggelassen
/*N*/ 	//	(aus ScDocument::SaveDdeLinks)
/*N*/ 
/*N*/ 	rHdr.EndEntry();
/*N*/ }

/*N*/ void __EXPORT ScDdeLink::DataChanged( const String& rMimeType,
/*N*/ 								const ::com::sun::star::uno::Any & rValue )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 //	wir koennen nur Strings...
/*N*/ }




/*N*/ void ScDdeLink::TryUpdate()
/*N*/ {
/*N*/ 	if (bIsInUpdate)
/*N*/ 		bNeedUpdate = TRUE;			// kann jetzt nicht ausgefuehrt werden
/*N*/ 	else
/*N*/ 	{
/*N*/ 		bIsInUpdate = TRUE;
/*N*/ 		//Application::Reschedule();	//! OS/2-Simulation
/*N*/         pDoc->IncInDdeLinkUpdate();
/*N*/ 		Update();
/*N*/         pDoc->DecInDdeLinkUpdate();
/*N*/ 		bIsInUpdate = FALSE;
/*N*/ 		bNeedUpdate = FALSE;
/*N*/ 	}
/*N*/ }


}
