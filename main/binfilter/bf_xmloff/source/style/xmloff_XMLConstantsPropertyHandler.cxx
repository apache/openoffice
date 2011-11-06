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



#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


#ifndef _XMLOFF_XMLCONSTANTSPROPERTYHANDLER_HXX
#include "XMLConstantsPropertyHandler.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::rtl;
using ::binfilter::xmloff::token::XMLTokenEnum;

XMLConstantsPropertyHandler::XMLConstantsPropertyHandler(
	const SvXMLEnumMapEntry *pM,
	enum XMLTokenEnum eDflt ) :
		pMap( pM ),
		eDefault( eDflt )
{
}

XMLConstantsPropertyHandler::~XMLConstantsPropertyHandler() 
{
}

sal_Bool XMLConstantsPropertyHandler::importXML(
	const OUString& rStrImpValue,
	Any& rValue,
	const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_uInt16 nEnum;
	sal_Bool bRet = rUnitConverter.convertEnum( nEnum, rStrImpValue, pMap );

	if( bRet )
		rValue <<= (sal_Int16)nEnum;

	return bRet; 
}

sal_Bool XMLConstantsPropertyHandler::exportXML( 
	OUString& rStrExpValue,
	const Any& rValue,
	const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;
	
	sal_Bool bRet = false;

	sal_Int32 nEnum;

	if( rValue.hasValue() && (rValue.getValueTypeClass() == TypeClass_ENUM))
	{
		nEnum = *((sal_Int32*)rValue.getValue());
		bRet = true;
	}
	else 
	{
		bRet = (rValue >>= nEnum );
	}

	if( bRet )
	{
		if( (nEnum >= 0) && (nEnum <= 0xffff) )
		{
			sal_uInt16 nConst = static_cast<sal_uInt16>( nEnum );

			bRet = rUnitConverter.convertEnum( aOut, nConst, pMap, eDefault ); 

			rStrExpValue = aOut.makeStringAndClear();
		}
		else
		{
			DBG_ERROR("XMLConstantsPropertyHandler::exportXML() constant is out of range for implementation using sal_uInt16");
		}
	}
	else
	{
		DBG_ERROR("XMLConstantsPropertyHandler::exportXML() could not convert any to sal_Int32");
	}

	return bRet;
}

}//end of namespace binfilter
