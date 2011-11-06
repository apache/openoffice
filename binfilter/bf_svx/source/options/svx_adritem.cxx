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



// include ---------------------------------------------------------------

#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <bf_svtools/useroptions.hxx>
#endif
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "adritem.hxx"
#include "svxids.hrc"
namespace binfilter {

#define INVALID_TOKEN ((USHORT)0xFFFF)

// -----------------------------------------------------------------------

/*?*/ String ConvertToStore_Impl( const String& rText )
/*?*/ {
/*?*/ 	String sRet;
/*?*/ 	USHORT i = 0;
/*?*/ 
/*?*/ 	while ( i < rText.Len() )
/*?*/ 	{
/*?*/ 		if ( rText.GetChar(i) == '\\' || rText.GetChar(i) == '#' )
/*?*/ 			sRet += '\\';
/*?*/ 		sRet += rText.GetChar(i++);
/*?*/ 	}
/*?*/ 	return sRet;
/*?*/ }

// -----------------------------------------------------------------------

/*N*/ TYPEINIT1(SvxAddressItem, SfxStringItem);

// -----------------------------------------------------------------------

/*N*/ SvxAddressItem::SvxAddressItem( USHORT nWhich ) :
/*N*/ 
/*N*/ 	SfxStringItem( nWhich, String() )
/*N*/ 
/*N*/ {
/*N*/ 	SvtUserOptions aUserOpt;
/*N*/ 
/*N*/ 	aName = aUserOpt.GetLastName();
/*N*/ 	aFirstName = aUserOpt.GetFirstName();
/*N*/ 	aShortName = aUserOpt.GetID();
/*N*/ 
/*N*/ 	SetToken( POS_COMPANY, aUserOpt.GetCompany() );
/*N*/ 	SetToken( POS_STREET, aUserOpt.GetStreet() );
/*N*/ 	SetToken( POS_CITY, aUserOpt.GetCity() );
/*N*/ 	SetToken( POS_STATE, aUserOpt.GetState() );
/*N*/ 	SetToken( POS_PLZ, aUserOpt.GetZip() );
/*N*/ 	SetToken( POS_COUNTRY, aUserOpt.GetCountry() );
/*N*/ 	SetToken( POS_POSITION, aUserOpt.GetPosition() );
/*N*/ 	SetToken( POS_TITLE, aUserOpt.GetTitle() );
/*N*/ 	SetToken( POS_TEL_PRIVATE, aUserOpt.GetTelephoneHome() );
/*N*/ 	SetToken( POS_TEL_COMPANY, aUserOpt.GetTelephoneWork() );
/*N*/ 	SetToken( POS_FAX, aUserOpt.GetFax() );
/*N*/ 	SetToken( POS_EMAIL, aUserOpt.GetEmail() );
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SvxAddressItem::SvxAddressItem( const SvxAddressItem& rItem ) :
/*N*/ 
/*N*/ 	SfxStringItem( rItem.Which(), rItem.GetValue() ),
/*N*/ 
/*N*/ 	aName		( rItem.aName ),
/*N*/ 	aFirstName	( rItem.aFirstName ),
/*N*/ 	aShortName	( rItem.aShortName )
/*N*/ 
/*N*/ {
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SvxAddressItem::SvxAddressItem( const String &rAdress,
/*N*/ 								const String &rShortName,
/*N*/ 								const String &rFirstName,
/*N*/ 								const String &rName,
/*N*/ 								USHORT nWhich ) :
/*N*/ 
/*N*/ 	SfxStringItem( nWhich, rAdress ),
/*N*/ 
/*N*/ 	aName		( rName ),
/*N*/ 	aFirstName	( rFirstName ),
/*N*/ 	aShortName	( rShortName )
/*N*/ 
/*N*/ {
/*N*/ }

// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------

/*N*/ BOOL SvxAddressItem::SetToken( USHORT nPos, const String &rVal )
/*N*/ {
/*N*/ 	// nach dem 'nPos'-ten Token suchen
/*N*/ 	String aAdress( GetValue() );
/*N*/ 	USHORT nStart, nEnd = 0;
/*N*/ 	for ( USHORT nNo = 0; nNo <= nPos; ++nNo )
/*N*/ 	{
/*N*/ 		// End-'#' finden
/*N*/ 		for ( nStart = nEnd;
/*N*/ 			  nEnd < aAdress.Len() && aAdress.GetChar(nEnd) != '#';
/*N*/ 			  ++nEnd )
/*N*/ 			if ( aAdress.GetChar(nEnd) == '\\' )
/*N*/ 				++nEnd;
/*N*/ 
/*N*/ 		// aAdress[nEnd] == '#' oder am Ende, also eine Position weiter gehen
/*N*/ 		++nEnd;
/*N*/ 
/*N*/ 		// ggf. fehlende '#' auff"ullen
/*N*/ 		if ( nNo < nPos && nEnd >= aAdress.Len() )
/*N*/ 			aAdress += '#';
/*N*/ 	}
/*N*/ 
/*N*/ 	// gefunden
/*N*/ 	aAdress.Erase( nStart, nEnd-nStart-1 );
/*N*/ 	aAdress.Insert( ::binfilter::ConvertToStore_Impl( rVal ), nStart );
/*N*/ 	SetValue( aAdress );
/*N*/ 	return TRUE;
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ String SvxAddressItem::GetToken( USHORT nPos ) const
/*N*/ {
/*N*/ 	String sRet, sEmpty;
/*N*/ 	const String &rAdress = GetValue();
/*N*/ 	USHORT i = 0, nTxt = 0;
/*N*/ 
/*N*/ 	while ( i < rAdress.Len() )
/*N*/ 	{
/*N*/ 		while ( i < rAdress.Len() && rAdress.GetChar(i) != '#' )
/*N*/ 		{
/*N*/ 			if ( rAdress.GetChar(i) == '\\' )
/*N*/ 				i++;
/*N*/ 			sRet += rAdress.GetChar(i++);
/*N*/ 		}
/*N*/ 
/*N*/ 		// rAdress[i] == '#' oder am Ende, also eine Position weiter gehen
/*N*/ 		i++;
/*N*/ 
/*N*/ 		if ( nPos == nTxt )
/*N*/ 			return sRet;
/*N*/ 		else if ( i >= rAdress.Len() )
/*N*/ 			return sEmpty;
/*N*/ 		else
/*N*/ 		{
/*N*/ 			sRet.Erase();
/*N*/ 			nTxt++;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return sEmpty;
/*N*/ }

// -----------------------------------------------------------------------


}
