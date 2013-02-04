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

#include "asciiopt.hxx"
#include "asciiopt.hrc"

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
namespace binfilter {


// ============================================================================

//! TODO make dynamic
#ifdef WIN
const sal_Int32 ASCIIDLG_MAXROWS                = 10000;
#else
const sal_Int32 ASCIIDLG_MAXROWS                = 32000;
#endif

static const sal_Char __FAR_DATA pStrFix[] = "FIX";
static const sal_Char __FAR_DATA pStrMrg[] = "MRG";


// ============================================================================

/*N*/ ScAsciiOptions::ScAsciiOptions() :
/*N*/ 	bFixedLen		( FALSE ),
/*N*/ 	aFieldSeps		( ';' ),
/*N*/ 	bMergeFieldSeps	( FALSE ),
/*N*/ 	cTextSep		( 34 ),
/*N*/ 	eCharSet		( gsl_getSystemTextEncoding() ),
/*N*/ 	bCharSetSystem	( FALSE ),
/*N*/ 	nStartRow		( 1 ),
/*N*/ 	nInfoCount		( 0 ),
/*N*/     pColStart       ( NULL ),
/*N*/ 	pColFormat		( NULL )
/*N*/ {
/*N*/ }


/*N*/ ScAsciiOptions::ScAsciiOptions(const ScAsciiOptions& rOpt) :
/*N*/ 	bFixedLen		( rOpt.bFixedLen ),
/*N*/ 	aFieldSeps		( rOpt.aFieldSeps ),
/*N*/ 	bMergeFieldSeps	( rOpt.bMergeFieldSeps ),
/*N*/ 	cTextSep		( rOpt.cTextSep ),
/*N*/ 	eCharSet		( rOpt.eCharSet ),
/*N*/ 	bCharSetSystem	( rOpt.bCharSetSystem ),
/*N*/ 	nStartRow		( rOpt.nStartRow ),
/*N*/ 	nInfoCount		( rOpt.nInfoCount )
/*N*/ {
/*N*/ 	if (nInfoCount)
/*N*/ 	{
/*N*/ 		pColStart = new xub_StrLen[nInfoCount];
/*N*/ 		pColFormat = new BYTE[nInfoCount];
/*N*/ 		for (USHORT i=0; i<nInfoCount; i++)
/*N*/ 		{
/*N*/ 			pColStart[i] = rOpt.pColStart[i];
/*N*/ 			pColFormat[i] = rOpt.pColFormat[i];
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		pColStart = NULL;
/*N*/ 		pColFormat = NULL;
/*N*/ 	}
/*N*/ }


/*N*/ ScAsciiOptions::~ScAsciiOptions()
/*N*/ {
/*N*/ 	delete[] pColStart;
/*N*/ 	delete[] pColFormat;
/*N*/ }









//
//	Der Options-String darf kein Semikolon mehr enthalten (wegen Pickliste)
//	darum ab Version 336 Komma stattdessen
//


/*N*/ void ScAsciiOptions::ReadFromString( const String& rString )
/*N*/ {
/*N*/ 	xub_StrLen nCount = rString.GetTokenCount(',');
/*N*/ 	String aToken;
/*N*/ 	xub_StrLen nSub;
/*N*/ 	xub_StrLen i;
/*N*/ 
/*N*/ 		//
/*N*/ 		//	Feld-Trenner
/*N*/ 		//
/*N*/ 
/*N*/ 	if ( nCount >= 1 )
/*N*/ 	{
/*N*/ 		bFixedLen = bMergeFieldSeps = FALSE;
/*N*/ 		aFieldSeps.Erase();
/*N*/ 
/*N*/ 		aToken = rString.GetToken(0,',');
/*N*/ 		if ( aToken.EqualsAscii(pStrFix) )
/*N*/ 			bFixedLen = TRUE;
/*N*/ 		nSub = aToken.GetTokenCount('/');
/*N*/ 		for ( i=0; i<nSub; i++ )
/*N*/ 		{
/*N*/ 			String aCode = aToken.GetToken( i, '/' );
/*N*/ 			if ( aCode.EqualsAscii(pStrMrg) )
/*N*/ 				bMergeFieldSeps = TRUE;
/*N*/ 			else
/*N*/ 			{
/*N*/ 				sal_Int32 nVal = aCode.ToInt32();
/*N*/ 				if ( nVal )
/*N*/ 					aFieldSeps += (sal_Unicode) nVal;
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 		//
/*N*/ 		//	Text-Trenner
/*N*/ 		//
/*N*/ 
/*N*/ 	if ( nCount >= 2 )
/*N*/ 	{
/*N*/ 		aToken = rString.GetToken(1,',');
/*N*/ 		sal_Int32 nVal = aToken.ToInt32();
/*N*/ 		cTextSep = (sal_Unicode) nVal;
/*N*/ 	}
/*N*/ 
/*N*/ 		//
/*N*/ 		//	Zeichensatz
/*N*/ 		//
/*N*/ 
/*N*/ 	if ( nCount >= 3 )
/*N*/ 	{
/*N*/ 		aToken = rString.GetToken(2,',');
/*N*/ 		eCharSet = ScGlobal::GetCharsetValue( aToken );
/*N*/ 	}
/*N*/ 
/*N*/ 		//
/*N*/ 		//	Startzeile
/*N*/ 		//
/*N*/ 
/*N*/ 	if ( nCount >= 4 )
/*N*/ 	{
/*N*/ 		aToken = rString.GetToken(3,',');
/*N*/ 		nStartRow = aToken.ToInt32();
/*N*/ 	}
/*N*/ 
/*N*/ 		//
/*N*/ 		//	Spalten-Infos
/*N*/ 		//
/*N*/ 
/*N*/ 	if ( nCount >= 5 )
/*N*/ 	{
/*N*/ 		delete[] pColStart;
/*N*/ 		delete[] pColFormat;
/*N*/ 
/*N*/ 		aToken = rString.GetToken(4,',');
/*N*/ 		nSub = aToken.GetTokenCount('/');
/*N*/ 		nInfoCount = nSub / 2;
/*N*/ 		if (nInfoCount)
/*N*/ 		{
/*N*/ 			pColStart = new xub_StrLen[nInfoCount];
/*N*/ 			pColFormat = new BYTE[nInfoCount];
/*N*/ 			for (USHORT nInfo=0; nInfo<nInfoCount; nInfo++)
/*N*/ 			{
/*N*/ 				pColStart[nInfo]  = (xub_StrLen) aToken.GetToken( 2*nInfo, '/' ).ToInt32();
/*N*/ 				pColFormat[nInfo] = (BYTE) aToken.GetToken( 2*nInfo+1, '/' ).ToInt32();
/*N*/ 			}
/*N*/ 		}
/*N*/ 		else
/*N*/ 		{
/*N*/ 			pColStart = NULL;
/*N*/ 			pColFormat = NULL;
/*N*/ 		}
/*N*/ 	}
/*N*/ }
}
