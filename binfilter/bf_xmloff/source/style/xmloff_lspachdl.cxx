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




#ifndef _XMLOFF_PROPERTYHANDLER_LINESPACETYPES_HXX
#include <lspachdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


#ifndef _COM_SUN_STAR_STYLE_LINESPACING_HPP_
#include <com/sun/star/style/LineSpacing.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_LINESPACINGMODE_HPP_
#include <com/sun/star/style/LineSpacingMode.hpp>
#endif

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_CASEMAP_NORMAL;

// this is a copy of defines in svx/inc/escpitem.hxx
#define DFLT_ESC_PROP	 58
#define DFLT_ESC_AUTO_SUPER	101
#define DFLT_ESC_AUTO_SUB  -101

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementPropHdl
//

XMLLineHeightHdl::~XMLLineHeightHdl()
{
	// nothing to do
}

sal_Bool XMLLineHeightHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	style::LineSpacing aLSp;
	sal_Int32 nTemp = 0;

	if( -1 != rStrImpValue.indexOf( sal_Unicode( '%' ) ) )
	{
		aLSp.Mode = style::LineSpacingMode::PROP;
		if(!rUnitConverter.convertPercent( nTemp, rStrImpValue ))
			return sal_False;
		aLSp.Height = nTemp;
	}
	else if( IsXMLToken( rStrImpValue, XML_CASEMAP_NORMAL) )
	{
		aLSp.Mode = style::LineSpacingMode::PROP;
		aLSp.Height = 100;
	}
	else
	{
		aLSp.Mode = style::LineSpacingMode::FIX;
		if(!rUnitConverter.convertMeasure( nTemp, rStrImpValue, 0x0000, 0xffff ))
			return sal_False;
		aLSp.Height = nTemp;
	}

	rValue <<= aLSp;
	return sal_True;
}

sal_Bool XMLLineHeightHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;

	style::LineSpacing aLSp;
	if(!(rValue >>= aLSp))
		return sal_False;

	if( style::LineSpacingMode::PROP != aLSp.Mode && style::LineSpacingMode::FIX  != aLSp.Mode )
		return sal_False;

	if( style::LineSpacingMode::PROP == aLSp.Mode )
	{
		rUnitConverter.convertPercent( aOut, aLSp.Height );
	}
	else
	{
		rUnitConverter.convertMeasure( aOut, aLSp.Height );
	}

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength() != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLLineHeightAtLeastHdl
//

XMLLineHeightAtLeastHdl::~XMLLineHeightAtLeastHdl()
{
	// nothing to do
}

sal_Bool XMLLineHeightAtLeastHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	style::LineSpacing aLSp;

	sal_Int32 nTemp;
	aLSp.Mode = style::LineSpacingMode::MINIMUM;
	if(!rUnitConverter.convertMeasure( nTemp, rStrImpValue, 0x0000, 0xffff ))
		return sal_False;
	aLSp.Height = nTemp;

	rValue <<= aLSp;
	return sal_True;
}

sal_Bool XMLLineHeightAtLeastHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;

	style::LineSpacing aLSp;
	if(!(rValue >>= aLSp))
		return sal_False;

	if( style::LineSpacingMode::MINIMUM != aLSp.Mode )
		return sal_False;

	rUnitConverter.convertMeasure( aOut, aLSp.Height );

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength() != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLLineSpacingHdl
//

XMLLineSpacingHdl::~XMLLineSpacingHdl()
{
	// nothing to do
}

sal_Bool XMLLineSpacingHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	style::LineSpacing aLSp;
	sal_Int32 nTemp;

	aLSp.Mode = style::LineSpacingMode::LEADING;
	if(!rUnitConverter.convertMeasure( nTemp, rStrImpValue, 0x0000, 0xffff ))
		return sal_False;
	aLSp.Height = nTemp;

	rValue <<= aLSp;
	return sal_True;
}

sal_Bool XMLLineSpacingHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;

	style::LineSpacing aLSp;
	if(!(rValue >>= aLSp))
		return sal_False;

	if( style::LineSpacingMode::LEADING != aLSp.Mode )
		return sal_False;

	rUnitConverter.convertMeasure( aOut, aLSp.Height );

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength() != 0;
}
}//end of namespace binfilter
