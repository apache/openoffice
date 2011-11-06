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

#include <unotools/textsearch.hxx>

#include "chgviset.hxx"
#include "rechead.hxx"
namespace binfilter {
// -----------------------------------------------------------------------
/*N*/ ScChangeViewSettings::~ScChangeViewSettings()
/*N*/ {
/*N*/ 	if(pCommentSearcher!=NULL)
/*?*/ 		delete pCommentSearcher;
/*N*/ }


// #i49161# this is needed to save documents with change tracking
ScChangeViewSettings& ScChangeViewSettings::operator=( const ScChangeViewSettings& r )
{
	SetTheComment(r.aComment);

	aFirstDateTime	=r.aFirstDateTime;
	aLastDateTime	=r.aLastDateTime;
	aAuthorToShow	=r.aAuthorToShow;
	aRangeList		=r.aRangeList;
	eDateMode		=r.eDateMode;
	bShowIt			=r.bShowIt;
	bIsDate			=r.bIsDate;
	bIsAuthor		=r.bIsAuthor;
	bIsComment		=r.bIsComment;
	bIsRange		=r.bIsRange;
	bEveryoneButMe	=r.bEveryoneButMe;
	bShowAccepted	=r.bShowAccepted;
	bShowRejected	=r.bShowRejected;

	return *this;
}


/*N*/ void ScChangeViewSettings::SetTheComment(const String& rString)
/*N*/ {
/*N*/ 	aComment=rString;
/*N*/ 	if(pCommentSearcher!=NULL)
/*N*/ 	{
/*N*/ 		delete pCommentSearcher;
/*N*/ 		pCommentSearcher=NULL;
/*N*/ 	}
/*N*/ 
/*N*/ 	if(rString.Len()>0)
/*N*/ 	{
/*N*/ 		utl::SearchParam aSearchParam( rString,
/*N*/ 			utl::SearchParam::SRCH_REGEXP,FALSE,FALSE,FALSE );
/*N*/ 
/*N*/ 		pCommentSearcher = new utl::TextSearch( aSearchParam, *ScGlobal::pCharClass );
/*N*/ 	}
/*N*/ }

/*N*/ void ScChangeViewSettings::Load( SvStream& rStream, USHORT nVer )
/*N*/ {
/*N*/ 	ScReadHeader aHdr( rStream );
/*N*/ 
/*N*/ 	BYTE nByte;
/*N*/ 	UINT32 nDT;
/*N*/ 	rStream >> bShowIt;
/*N*/ 	rStream >> bIsDate;
/*N*/ 	rStream >> nByte; eDateMode = (ScChgsDateMode)nByte;
/*N*/ 	rStream >> nDT; aFirstDateTime.SetDate( nDT );
/*N*/ 	rStream >> nDT; aFirstDateTime.SetTime( nDT );
/*N*/ 	rStream >> nDT; aLastDateTime.SetDate( nDT );
/*N*/ 	rStream >> nDT; aLastDateTime.SetTime( nDT );
/*N*/ 	rStream >> bIsAuthor;
/*N*/ 	rStream >> bEveryoneButMe;
/*N*/ 	rStream.ReadByteString( aAuthorToShow, rStream.GetStreamCharSet() );
/*N*/ 	rStream >> bIsRange;
/*N*/ 	aRangeList.Load( rStream, nVer );
/*N*/ 	if ( aHdr.BytesLeft() )
/*N*/ 	{
/*N*/ 		rStream >> bShowAccepted;
/*N*/ 		rStream >> bShowRejected;
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		bShowAccepted = FALSE;
/*N*/ 		bShowRejected = FALSE;
/*N*/ 	}
/*N*/ 
/*N*/ 	// Zusaetzlich Kommentar-Informationen lesen (src509)
/*N*/ 	if ( aHdr.BytesLeft() )	 //#59103#
/*N*/ 	{
/*N*/ 		rStream >> bIsComment;
/*N*/ 		rStream.ReadByteString( aComment, rStream.GetStreamCharSet() );
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		bIsComment = FALSE;
/*N*/ 		aComment.Erase();
/*N*/ 	}
/*N*/ 	SetTheComment(aComment);
/*N*/ }

/*N*/ void ScChangeViewSettings::Store( SvStream& rStream ) const
/*N*/ {
/*N*/ 	ScWriteHeader aHdr( rStream, 42 );		// Groesse, wenn String und RangeList leer sind
/*N*/ 
/*N*/ 	rStream << bShowIt;
/*N*/ 	rStream << bIsDate;
/*N*/ 	rStream << (BYTE) eDateMode;
/*N*/ 	rStream << (UINT32) aFirstDateTime.GetDate();
/*N*/ 	rStream << (UINT32) aFirstDateTime.GetTime();
/*N*/ 	rStream << (UINT32) aLastDateTime.GetDate();
/*N*/ 	rStream << (UINT32) aLastDateTime.GetTime();
/*N*/ 	rStream << bIsAuthor;
/*N*/ 	rStream << bEveryoneButMe;
/*N*/ 	rStream.WriteByteString( aAuthorToShow, rStream.GetStreamCharSet() );
/*N*/ 	rStream << bIsRange;
/*N*/ 	aRangeList.Store( rStream );
/*N*/ 	rStream << bShowAccepted;
/*N*/ 	rStream << bShowRejected;
/*N*/ 
/*N*/ 	// Zusaetzlich Kommentar-Informationen schreiben (src509)
/*N*/ 	if(bIsComment || aComment.Len()>0) //#59103#
/*N*/ 	{
/*N*/ 		rStream << bIsComment;
/*N*/ 		rStream.WriteByteString( aComment, rStream.GetStreamCharSet() );
/*N*/ 	}
/*N*/ }



}
