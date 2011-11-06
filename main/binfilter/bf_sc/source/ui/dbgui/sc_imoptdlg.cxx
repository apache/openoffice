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

#include "imoptdlg.hxx"
#include "imoptdlg.hrc"

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

static const sal_Char pStrFix[] = "FIX";

namespace binfilter {

class ScDelimiterTable
{
public:
		ScDelimiterTable( const String& rDelTab )
			:	theDelTab ( rDelTab ),
				cSep	  ( '\t' ),
				nCount	  ( rDelTab.GetTokenCount('\t') ),
				nIter	  ( 0 )
			{}

	String	FirstDel()	{ nIter = 0; return theDelTab.GetToken( nIter, cSep ); }
	String	NextDel()	{ nIter +=2; return theDelTab.GetToken( nIter, cSep ); }

private:
	const String		theDelTab;
	const sal_Unicode	cSep;
	const xub_StrLen	nCount;
	xub_StrLen			nIter;
};

//------------------------------------------------------------------------

/*N*/ void ScImportOptions::SetTextEncoding( rtl_TextEncoding nEnc )
/*N*/ {
/*N*/ 	eCharSet = (nEnc == RTL_TEXTENCODING_DONTKNOW ?
/*N*/ 		gsl_getSystemTextEncoding() : nEnc);
/*N*/ 	aStrFont = ScGlobal::GetCharsetString( nEnc );
/*N*/ }
}
