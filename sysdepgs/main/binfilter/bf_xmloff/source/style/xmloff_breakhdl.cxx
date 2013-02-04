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



#ifndef _XMLOFF_PROPERTYHANDLER_BREAKTYPES_HXX
#include <breakhdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_BREAKTYPE_HPP_
#include <com/sun/star/style/BreakType.hpp>
#endif


namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

SvXMLEnumMapEntry pXML_BreakTypes[] =
{
	{ XML_AUTO,	        0 },
	{ XML_COLUMN,       1 },
	{ XML_PAGE,         2 },
	{ XML_EVEN_PAGE,    2 },
	{ XML_ODD_PAGE,     2 },
	{ XML_TOKEN_INVALID, 0}
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFmtBreakBeforePropHdl
//

XMLFmtBreakBeforePropHdl::~XMLFmtBreakBeforePropHdl()
{
	// Nothing to do
}

sal_Bool XMLFmtBreakBeforePropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	style::BreakType eBreak = style::BreakType_NONE;
	sal_uInt16 nEnum;

	if( ( bRet = rUnitConverter.convertEnum( nEnum, rStrImpValue, pXML_BreakTypes ) ) )
	{
		if( nEnum != 0 )
			eBreak = ( nEnum == 1 ) ? style::BreakType_COLUMN_BEFORE : style::BreakType_PAGE_BEFORE;

		rValue <<= eBreak;
	}

	return bRet; 
}

sal_Bool XMLFmtBreakBeforePropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	style::BreakType eBreak;

	if( !( rValue >>= eBreak ) )
	{
		sal_Int32 nValue;
		if( !( rValue >>= nValue ) )
			return sal_False;

		eBreak = (style::BreakType) nValue;
	}
	
	sal_uInt16 nEnum = 0;
	switch( eBreak )
	{
		case style::BreakType_COLUMN_BEFORE:
			nEnum = 1;
			break;
		case style::BreakType_PAGE_BEFORE:
			nEnum = 2;
			break;
		case style::BreakType_NONE:
			nEnum = 0;
			break;
		default:
			return sal_False;
	}
	
	OUStringBuffer aOut;
	sal_Bool bOk = rUnitConverter.convertEnum( aOut, nEnum, pXML_BreakTypes );
	rStrExpValue = aOut.makeStringAndClear();

	return sal_True;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFmtBreakBeforePropHdl
//

XMLFmtBreakAfterPropHdl::~XMLFmtBreakAfterPropHdl()
{
	// Nothing to do
}

sal_Bool XMLFmtBreakAfterPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	style::BreakType eBreak = style::BreakType_NONE;
	sal_uInt16 nEnum;

	if( ( bRet = rUnitConverter.convertEnum( nEnum, rStrImpValue, pXML_BreakTypes ) ) )
	{
		if( nEnum != 0 )
			eBreak = ( nEnum == 1 ) ? style::BreakType_COLUMN_AFTER : style::BreakType_PAGE_AFTER;

		rValue <<= eBreak;
	}

	return bRet; 
}

sal_Bool XMLFmtBreakAfterPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	style::BreakType eBreak;

	if( !( rValue >>= eBreak ) )
	{
		sal_Int32 nValue;
		if( !( rValue >>= nValue ) )
			return sal_False;

		eBreak = (style::BreakType) nValue;
	}
	
	sal_uInt16 nEnum = 0;
	switch( eBreak )
	{
		case style::BreakType_COLUMN_AFTER:
			nEnum = 1;
			break;
		case style::BreakType_PAGE_AFTER:
			nEnum = 2;
			break;
		case style::BreakType_NONE:
			nEnum = 0;
			break;
		default:
			return sal_False;
	}
	
	OUStringBuffer aOut;
	sal_Bool bOk = rUnitConverter.convertEnum( aOut, nEnum, pXML_BreakTypes );
	rStrExpValue = aOut.makeStringAndClear();

	return sal_True;
}
}//end of namespace binfilter
