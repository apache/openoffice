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

#include "hintids.hxx"

#ifndef _SFXMACITEM_HXX //autogen
#include <bf_svtools/macitem.hxx>
#endif


#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#include "doc.hxx"
#include "inetfld.hxx"
#include "poolfmt.hxx"
#include "hints.hxx"
namespace binfilter {


/*--------------------------------------------------------------------
	Beschreibung: SwINetFieldType by JP
 --------------------------------------------------------------------*/



SwINetFieldType::SwINetFieldType( SwDoc* pD )
	: SwFieldType( RES_INTERNETFLD ), pDoc( pD ),
	aNormalFmt( this, 0 ), aVisitFmt( this, 0 )
{
}



SwFieldType* SwINetFieldType::Copy() const
{
	return new SwINetFieldType( pDoc );
}



SwCharFmt* SwINetFieldType::GetCharFmt( const SwINetField& rFld )
{
	USHORT nId = RES_POOLCHR_INET_NORMAL;
	SwDepend* pDepend = &aNormalFmt;

	if( rFld.sURL.Len() && pDoc->IsVisitedURL( rFld.sURL ) )
	{
		nId = RES_POOLCHR_INET_VISIT;
		pDepend = &aVisitFmt;
	}

	SwCharFmt* pFmt = pDoc->GetCharFmtFromPool( nId );

	// noch nicht registriert ?
	if( !pDepend->GetRegisteredIn() )
		pFmt->Add( pDepend );		// anmelden

	return pFmt;
}

/*--------------------------------------------------------------------
	Beschreibung: SwINetFieldType by JP
 --------------------------------------------------------------------*/



SwINetField::SwINetField( SwINetFieldType* pTyp, USHORT nFmt,
							const String& rURL, const String& rText )
	: SwField( pTyp, nFmt ), sURL( rURL ), sText( rText ),
	pMacroTbl( 0 )
{
}



SwINetField::~SwINetField()
{
	delete pMacroTbl;
}



String SwINetField::Expand() const
{
	return sText;
}



String SwINetField::GetCntnt(BOOL bName) const
{
	if( bName )
	{
		String aStr( sText );
		aStr += ' ';
		aStr += sURL;
		return aStr;
	}
	return Expand();
}



SwField* SwINetField::Copy() const
{
	SwINetField *pTmp = new SwINetField( (SwINetFieldType*)pType, nFormat,
											sURL, sText );
	pTmp->SetLanguage( GetLanguage() );
	pTmp->sTargetFrameName = sTargetFrameName;

	if( pMacroTbl )
		pTmp->pMacroTbl = new SvxMacroTableDtor( *pMacroTbl );

	return pTmp;
}

/*--------------------------------------------------------------------
	Beschreibung: Parameter setzen
 --------------------------------------------------------------------*/


const String& SwINetField::GetPar1() const
{
	return sURL;
}



String SwINetField::GetPar2() const
{
	return sText;
}



void SwINetField::SetPar1(const String& rStr)
{
	sURL = rStr;
}



void SwINetField::SetPar2(const String& rStr)
{
	sText = rStr;
}



SwCharFmt* SwINetField::GetCharFmt()
{
	return ((SwINetFieldType*)pType)->GetCharFmt( *this );
}



void SwINetField::SetMacroTbl( const SvxMacroTableDtor* pNewTbl )
{
	if( pNewTbl )
	{
		if( pMacroTbl )
			*pMacroTbl = *pNewTbl;
		else
			pMacroTbl = new SvxMacroTableDtor( *pNewTbl );
	}
	else if( pMacroTbl )
		delete pMacroTbl, pMacroTbl = 0;
}



void SwINetField::SetMacro( USHORT nEvent, const SvxMacro& rMacro )
{
	if( !pMacroTbl )
		pMacroTbl = new SvxMacroTableDtor;

	SvxMacro *pOldMacro;
	if( 0 != ( pOldMacro = pMacroTbl->Get( nEvent )) )
	{
		delete pOldMacro;
		pMacroTbl->Replace( nEvent, new SvxMacro( rMacro ) );
	}
	else
		pMacroTbl->Insert( nEvent, new SvxMacro( rMacro ) );
}



const SvxMacro* SwINetField::GetMacro( USHORT nEvent ) const
{
	const SvxMacro* pRet = 0;
	if( pMacroTbl && pMacroTbl->IsKeyValid( nEvent ) )
		pRet = pMacroTbl->Get( nEvent );
	return pRet;
}

#ifdef USED


String SwINetField::GetBookmark() const
{
	String sRet;
	USHORT nPos = sURL.Search( '#' );
	if( nPos < sURL.Len() )
		sRet = sURL.Copy( nPos+1 );
	return sRet;
}
#endif



}
