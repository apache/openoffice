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

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
#ifndef _FLTGLBLS_HXX
#include <fltglbls.hxx>
#endif
#ifndef _FLTINI_HXX
#include <fltini.hxx>
#endif
#ifndef _LOTPAR_HXX
#include <lotpar.hxx>
#endif
#ifndef _SWFLTOPT_HXX
#include <swfltopt.hxx>
#endif
namespace binfilter {

LotGlob *pLotGlob = NULL;





SwLotusParser::SwLotusParser( SwDoc &rD, const SwPaM & rCrsr,
								SvStream& rInInit, int bReadNewDoc,
								CharSet eQ )
	: bNewDoc( bReadNewDoc )
{
	eQuellChar = eQ;
	pIn = &rInInit;
	pLotGlob = new LotGlob( rD, rCrsr );

	nReadBuffSize = 2048;
	pReadBuff = new char[ nReadBuffSize ];

//	aColRowBuff.SetDefWidth( ( USHORT ) ( 9192 ) );	// 4 Zeichen Standardbreite
}

SwLotusParser::~SwLotusParser()
{
	delete[] pReadBuff;
	delete pLotGlob;
	pLotGlob = NULL;
}


ULONG SwLotusParser::CallParser()
{
	static const sal_Char* aNames[4] = {
		"Excel_Lotus/MinRow", "Excel_Lotus/MaxRow",
		"Excel_Lotus/MinCol", "Excel_Lotus/MaxCol"
	};
	sal_uInt32 aVal[4];
	SwFilterOptions aOpt( 4, aNames, aVal );

	USHORT nMinRow = ( USHORT ) aVal[ 0 ];
	USHORT nMaxRow = ( USHORT ) aVal[ 1 ];
	USHORT nMinCol = ( USHORT ) aVal[ 2 ];
	USHORT nMaxCol = ( USHORT ) aVal[ 3 ];

	USHORT nAnzNodes = 65000U - pLotGlob->pD->GetNodes().Count();

	if( nMaxRow < nMinRow )
	{
		USHORT nTemp = nMinRow;
		nMinRow = nMaxRow;
		nMaxRow = nTemp;
	}

	if( nMaxCol < nMinCol )
	{
		USHORT nTemp = nMinCol;
		nMinCol = nMaxCol;
		nMaxCol = nTemp;
	}

	if( nMaxRow - nMinRow == 0 )
		nMaxRow = nMinRow + 30;		// Default bei 0 Rows

	if( nMaxCol - nMinCol == 0 )
		nMaxCol = nMinCol + 15;		// Default bei 0 Cols

	if( nAnzNodes < ( nMaxRow - nMinRow ) * ( nMaxCol - nMinCol ) * 3 )
		return ERR_EXCLOT_WRONG_RANGE;

	pLotGlob->SetRange( nMinCol, nMaxCol, nMinRow, nMaxRow ); // Default-Einstellung

	Parse();

	return 0;	// kein Fehler
}


ULONG LotusReader::Read( SwDoc &rDoc, SwPaM &rPam,
					const String & /* FileName, falls benoetigt wird */ )
{
	ULONG nRet;
	if( !pStrm )
	{
		ASSERT( FALSE, "Lotus-Read ohne Stream" );
		nRet = ERR_SWG_READ_ERROR;
	}
	else
	{
		SwLotusParser* pParser = new SwLotusParser( rDoc, rPam, *pStrm,
													!bInsertMode, eCodeSet );
		nRet = pParser->CallParser();		// 0 == kein Fehler aufgetreten
		delete pParser;
	}

	return nRet;
}



}
