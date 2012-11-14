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



#ifndef _XMLOFF_PROPERTYHANDLER_FONTWEIGHTTYPES_HXX
#include <weighhdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif


#include <bf_svtools/bf_solar.h>

#ifndef _INC_LIMITS
#include <limits.h>
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#include "rtl/ustring.hxx"

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_ 
#include <toolkit/helper/vclunohelper.hxx>
#endif

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

struct FontWeightMapper
{
	FontWeight eWeight;
	USHORT nValue;
};

FontWeightMapper const aFontWeightMap[] =	
{
	{ WEIGHT_DONTKNOW,				0 },
	{ WEIGHT_THIN,					100 },
	{ WEIGHT_ULTRALIGHT,			150 },
	{ WEIGHT_LIGHT,					250 },
	{ WEIGHT_SEMILIGHT,				350 },
	{ WEIGHT_NORMAL,				400 },
	{ WEIGHT_MEDIUM,				450 },
	{ WEIGHT_SEMIBOLD,				600 },
	{ WEIGHT_BOLD,					700 },
	{ WEIGHT_ULTRABOLD,				800 },
	{ WEIGHT_BLACK,					900 },
	{ (FontWeight)USHRT_MAX,	   1000 }
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFmtBreakBeforePropHdl
//

XMLFontWeightPropHdl::~XMLFontWeightPropHdl()
{
	// Nothing to do
}

sal_Bool XMLFontWeightPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 nWeight = 0;

	if( IsXMLToken( rStrImpValue, XML_WEIGHT_NORMAL ) )
	{
		nWeight = 400;
		bRet = sal_True;
	}
	else if( IsXMLToken( rStrImpValue, XML_WEIGHT_BOLD ) )
	{
		nWeight = 700;
		bRet = sal_True;
	}
	else
	{
		sal_Int32 nTemp;
		if( ( bRet = rUnitConverter.convertNumber( nTemp, rStrImpValue, 100, 900 ) ) )
			nWeight = nTemp;
	}

	if( bRet )
	{
		bRet = sal_False;

		for( int i = 0; aFontWeightMap[i].eWeight != USHRT_MAX; i++ )
		{
			if( (nWeight >= aFontWeightMap[i].nValue) && (nWeight <= aFontWeightMap[i+1].nValue) )
			{
				sal_uInt16 nDiff1 = nWeight - aFontWeightMap[i].nValue;
				sal_uInt16 nDiff2 = aFontWeightMap[i+1].nValue - nWeight;

				if( nDiff1 < nDiff2 )
					rValue <<= (float)( VCLUnoHelper::ConvertFontWeight( aFontWeightMap[i].eWeight ) );
				else
					rValue <<= (float)( VCLUnoHelper::ConvertFontWeight( aFontWeightMap[i+1].eWeight ) );

				bRet = sal_True;
				break;
			}
		}
	}

	return bRet; 
}

sal_Bool XMLFontWeightPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	FontWeight eWeight;

	float fValue = float();
	if( !( rValue >>= fValue ) )
	{
		sal_Int32 nValue;
		if( rValue >>= nValue )
		{
			fValue = (float)nValue;
			bRet = sal_True;
		}
	}
	else
		bRet = sal_True;

	eWeight = VCLUnoHelper::ConvertFontWeight( fValue );

	if( bRet )
	{
		sal_uInt16 nWeight = 0;

		for( int i = 0; aFontWeightMap[i].eWeight != -1; i++ )
		{
			if( aFontWeightMap[i].eWeight == eWeight )
			{
				 nWeight = aFontWeightMap[i].nValue;
				 break;
			}
		}

		OUStringBuffer aOut;

		if( 400 == nWeight )
			aOut.append( GetXMLToken(XML_WEIGHT_NORMAL) );
		else if( 700 == nWeight )
			aOut.append( GetXMLToken(XML_WEIGHT_BOLD) );
		else
			rUnitConverter.convertNumber( aOut, (sal_Int32)nWeight );

		rStrExpValue = aOut.makeStringAndClear();
	}

	return bRet;
}

}//end of namespace binfilter
