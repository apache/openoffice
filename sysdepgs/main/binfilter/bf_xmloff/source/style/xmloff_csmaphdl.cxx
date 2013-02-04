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



#ifndef _XMLOFF_PROPERTYHANDLER_CASEMAPTYPES_HXX
#include <csmaphdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_CASEMAP_HPP_
#include <com/sun/star/style/CaseMap.hpp>
#endif



namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

static SvXMLEnumMapEntry pXML_Casemap_Enum[] =
{
	{ XML_NONE,				    style::CaseMap::NONE },
	{ XML_CASEMAP_LOWERCASE,	style::CaseMap::LOWERCASE },
	{ XML_CASEMAP_UPPERCASE,	style::CaseMap::UPPERCASE },
	{ XML_CASEMAP_CAPITALIZE,	style::CaseMap::TITLE },
	{ XML_TOKEN_INVALID,        0 }
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLPosturePropHdl
//

XMLCaseMapPropHdl::~XMLCaseMapPropHdl()
{
	// nothing to do
}

sal_Bool XMLCaseMapPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 nVal;

	if( ( bRet = rUnitConverter.convertEnum( nVal, rStrImpValue, pXML_Casemap_Enum ) ) )
		rValue <<= nVal;

	return bRet; 
}

sal_Bool XMLCaseMapPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 nValue;
	OUStringBuffer aOut;

	if( rValue >>= nValue )
	{
		if( ( bRet = rUnitConverter.convertEnum( aOut, nValue, pXML_Casemap_Enum ) ) )
			rStrExpValue = aOut.makeStringAndClear();
	}
	
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLCaseMapVariantHdl
//

XMLCaseMapVariantHdl::~XMLCaseMapVariantHdl()
{
	// nothing to do
}

sal_Bool XMLCaseMapVariantHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;

	if( IsXMLToken( rStrImpValue, XML_CASEMAP_SMALL_CAPS ) )
	{
		rValue <<= (sal_Int16)style::CaseMap::SMALLCAPS;
		bRet = sal_True;
	}
	else if( IsXMLToken( rStrImpValue, XML_CASEMAP_NORMAL ) )
	{
		rValue <<= (sal_Int16)style::CaseMap::NONE;
		bRet = sal_True;
	}

	return bRet; 
}

sal_Bool XMLCaseMapVariantHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_uInt16 nValue;
	OUStringBuffer aOut;

	if( rValue >>= nValue )
	{
		switch( nValue )
		{
		case style::CaseMap::NONE:
			aOut.append( GetXMLToken(XML_CASEMAP_NORMAL) );
			break;
		case style::CaseMap::SMALLCAPS:
			aOut.append( GetXMLToken(XML_CASEMAP_SMALL_CAPS) );
			break;
		}
	}

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength();
}
}//end of namespace binfilter
