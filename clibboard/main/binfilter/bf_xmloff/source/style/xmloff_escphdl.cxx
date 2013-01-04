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




#ifndef _XMLOFF_PROPERTYHANDLER_ESCAPETYPES_HXX
#include <escphdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

// this is a copy of defines in svx/inc/escpitem.hxx
#define DFLT_ESC_PROP	 58
#define DFLT_ESC_AUTO_SUPER	101
#define DFLT_ESC_AUTO_SUB  -101

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementPropHdl
//

XMLEscapementPropHdl::~XMLEscapementPropHdl()
{
	// nothing to do
}

sal_Bool XMLEscapementPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Int16 nVal;

	SvXMLTokenEnumerator aTokens( rStrImpValue );

	OUString aToken;
	if( ! aTokens.getNextToken( aToken ) )
		return sal_False;

	if( IsXMLToken( aToken, XML_ESCAPEMENT_SUB ) )
	{
		nVal = DFLT_ESC_AUTO_SUB;
	}
	else if( IsXMLToken( aToken, XML_ESCAPEMENT_SUPER ) )
	{
		nVal = DFLT_ESC_AUTO_SUPER;
	}
	else
	{
		sal_Int32 nNewEsc;
		if( !rUnitConverter.convertPercent( nNewEsc, aToken ) )
			return sal_False;

		nVal = (sal_Int16) nNewEsc;
	}

	rValue <<= nVal;
	return sal_True; 
}

sal_Bool XMLEscapementPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Int32 nValue;
	OUStringBuffer aOut;

	if( rValue >>= nValue )
	{
		if( nValue == DFLT_ESC_AUTO_SUPER )
		{
			aOut.append( GetXMLToken(XML_ESCAPEMENT_SUPER) );
		}
		else if( nValue == DFLT_ESC_AUTO_SUB )
		{
			aOut.append( GetXMLToken(XML_ESCAPEMENT_SUB) );
		}
		else
		{
			rUnitConverter.convertPercent( aOut, nValue );
		}
	}
	
	rStrExpValue = aOut.makeStringAndClear();
	return sal_True;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementHeightPropHdl
//

XMLEscapementHeightPropHdl::~XMLEscapementHeightPropHdl()
{
	// nothing to do
}

sal_Bool XMLEscapementHeightPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	if( IsXMLToken( rStrImpValue, XML_CASEMAP_SMALL_CAPS ) )
		return sal_False;

	SvXMLTokenEnumerator aTokens( rStrImpValue );

	OUString aToken;
	if( ! aTokens.getNextToken( aToken ) )
		return sal_False;

	sal_Int8 nProp;
	if( aTokens.getNextToken( aToken ) )
	{
		sal_Int32 nNewProp;
		if( !rUnitConverter.convertPercent( nNewProp, aToken ) )
			return sal_False;
		nProp = (sal_Int8)nNewProp;	
	}
	else
	{
		nProp = (sal_Int8) DFLT_ESC_PROP;
	}

	rValue <<= nProp;
	return sal_True; 
}

sal_Bool XMLEscapementHeightPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut( rStrExpValue );

	sal_Int32 nValue;
	if( rValue >>= nValue )
	{
		if( rStrExpValue.getLength() )
			aOut.append( sal_Unicode(' '));

		rUnitConverter.convertPercent( aOut, nValue );
	}

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength();
}
}//end of namespace binfilter
