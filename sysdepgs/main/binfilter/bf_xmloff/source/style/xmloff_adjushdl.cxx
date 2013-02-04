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



#ifndef _XMLOFF_PROPERTYHANDLER_ADJUSTTYPES_HXX
#include <adjushdl.hxx>
#endif

#include <bf_svtools/bf_solar.h>


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHADJUST_HPP_
#include <com/sun/star/style/ParagraphAdjust.hpp>
#endif


namespace binfilter {

using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::binfilter::xmloff::token;

SvXMLEnumMapEntry __READONLY_DATA pXML_Para_Adjust_Enum[] =
{
	{ XML_START,		style::ParagraphAdjust_LEFT },
	{ XML_END,			style::ParagraphAdjust_RIGHT },
	{ XML_CENTER,		style::ParagraphAdjust_CENTER },
	{ XML_JUSTIFY,		style::ParagraphAdjust_BLOCK },
	{ XML_JUSTIFIED,	style::ParagraphAdjust_BLOCK },	// obsolete
	{ XML_TOKEN_INVALID, 0 }
};

SvXMLEnumMapEntry __READONLY_DATA pXML_Para_Align_Last_Enum[] =
{
	{ XML_START,		style::ParagraphAdjust_LEFT },
	{ XML_CENTER,		style::ParagraphAdjust_CENTER },
	{ XML_JUSTIFY,		style::ParagraphAdjust_BLOCK },
	{ XML_JUSTIFIED,	style::ParagraphAdjust_BLOCK },	// obsolete
	{ XML_TOKEN_INVALID, 0 }
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLParaAdjustPropHdl
//

XMLParaAdjustPropHdl::~XMLParaAdjustPropHdl()
{
	// nothing to do
}

sal_Bool XMLParaAdjustPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 eAdjust;

	if( ( bRet = rUnitConverter.convertEnum( eAdjust, rStrImpValue, pXML_Para_Adjust_Enum ) ) )
		rValue <<= (sal_Int16)eAdjust;

	return bRet; 
}

sal_Bool XMLParaAdjustPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	if(!rValue.hasValue())
		return sal_False;     //added by BerryJia for fixing Bug102407 2002-11-5
	OUStringBuffer aOut;
	sal_Int16 nVal;

	rValue >>= nVal;

	sal_Bool bRet = rUnitConverter.convertEnum( aOut, nVal, pXML_Para_Adjust_Enum, XML_START ); 

	rStrExpValue = aOut.makeStringAndClear();

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLLastLineAdjustPropHdl
//

XMLLastLineAdjustPropHdl::~XMLLastLineAdjustPropHdl()
{
	// nothing to do
}

sal_Bool XMLLastLineAdjustPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 eAdjust;

	if( ( bRet = rUnitConverter.convertEnum( eAdjust, rStrImpValue, pXML_Para_Align_Last_Enum ) ) )
		rValue <<= (sal_Int16)eAdjust;

	return bRet; 
}

sal_Bool XMLLastLineAdjustPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;
	sal_Int16 nVal;
	sal_Bool bRet = sal_False;

	rValue >>= nVal;

	if( nVal != style::ParagraphAdjust_LEFT )
		bRet = rUnitConverter.convertEnum( aOut, nVal, pXML_Para_Align_Last_Enum, XML_START ); 

	rStrExpValue = aOut.makeStringAndClear();

	return bRet;
}

}//end of namespace binfilter
