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

#ifndef _XMLOFF_ENUMPROPERTYHANDLER_HXX
#include <EnumPropertyHdl.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif

#include "rtl/ustring.hxx"

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

/*

#include <bf_svtools/bf_solar.h>

#ifndef _INC_LIMITS
#include <limits.h>
#endif


*/

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEnumPropertyHdl
//

XMLEnumPropertyHdl::~XMLEnumPropertyHdl()
{
	// Nothing to do
}

sal_Bool XMLEnumPropertyHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	USHORT nValue = 0;

	if( SvXMLUnitConverter::convertEnum( nValue, rStrImpValue, mpEnumMap ) )
	{
		switch( mrType.getTypeClass() )
		{
		case TypeClass_ENUM:
			rValue = ::cppu::int2enum( nValue, mrType );
			break;
		case TypeClass_LONG:
			rValue <<= (sal_Int32) nValue;
			break;
		case TypeClass_SHORT:
			rValue <<= (sal_Int16) nValue;
			break;
		case TypeClass_BYTE:
			rValue <<= (sal_Int8) nValue;
			break;
		default:
			DBG_ERROR( "Wrong type for enum property handler!" );
			return sal_False;
		}
		return sal_True;
	}

	return sal_False;
}

sal_Bool XMLEnumPropertyHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Int32 nValue;
	if(!(rValue >>= nValue ))
		if(!::cppu::enum2int(nValue, rValue) )
			return sal_False;

	OUStringBuffer aOut;

	if(!SvXMLUnitConverter::convertEnum( aOut, nValue, mpEnumMap ))
		return sal_False;

	rStrExpValue = aOut.makeStringAndClear();
	return sal_True;
}

}//end of namespace binfilter
