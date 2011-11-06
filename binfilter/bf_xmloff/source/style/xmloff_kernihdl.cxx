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



#ifndef _XMLOFF_PROPERTYHANDLER_KERNINGTYPES_HXX
#include "kernihdl.hxx"
#endif


#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

// --


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLKerningPropHdl
//

XMLKerningPropHdl::~XMLKerningPropHdl()
{
	// nothing to do
}

sal_Bool XMLKerningPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_True;
	sal_Int32 nKerning = 0;

	if( ! IsXMLToken( rStrImpValue, XML_KERNING_NORMAL ) )
	{
		bRet = rUnitConverter.convertMeasure( nKerning, rStrImpValue );
	}

	rValue <<= (sal_Int16)nKerning;

	return bRet; 
}

sal_Bool XMLKerningPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Int16 nValue;

	if( rValue >>= nValue )
	{
		OUStringBuffer aOut;

		if( nValue == 0 )
			aOut.append( GetXMLToken(XML_KERNING_NORMAL) );
		else
			rUnitConverter.convertMeasure( aOut, nValue );

		rStrExpValue = aOut.makeStringAndClear();

		bRet = sal_True;
	}

	return bRet;
}
}//end of namespace binfilter
