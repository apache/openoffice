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




#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif


#ifndef _XMLOFF_DRAWASPECTHDL_HXX
#include "DrawAspectHdl.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePropHdl
//

SvXMLEnumMapEntry __READONLY_DATA pXML_DrawAspect_Enum[] =
{
	{ XML_CONTENT,			1	},
	{ XML_THUMBNAIL,		2	},
	{ XML_ICON,			    4	},
	{ XML_PRINT,			8	},
	{ XML_TOKEN_INVALID, 0 }
};

DrawAspectHdl::~DrawAspectHdl()
{
	// nothing to do
}

sal_Bool DrawAspectHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Int32 nAspect = 0;

	SvXMLTokenEnumerator aTokenEnum( rStrImpValue );
	OUString aToken;
	while( aTokenEnum.getNextToken( aToken ) )
	{
		sal_uInt16 nVal;
		if( rUnitConverter.convertEnum( nVal, aToken, pXML_DrawAspect_Enum ) )
		{
			nAspect = nAspect | (sal_Int32)nVal;
		}
	}

	rValue <<= nAspect;

	return nAspect != 0; 
}

sal_Bool DrawAspectHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
  	OUStringBuffer aOut;

	sal_Int32 nAspect;
	if( rValue >>= nAspect )
	{
		if( (nAspect & 1) != 0 )
			aOut.append( GetXMLToken(XML_CONTENT) );

		if( (nAspect & 2) != 0 )
		{
			if( aOut.getLength() )
				aOut.append( sal_Unicode(' ') );
			aOut.append( GetXMLToken(XML_THUMBNAIL) );
		}

		if( (nAspect & 4) != 0 )
		{
			if( aOut.getLength() )
				aOut.append( sal_Unicode(' ') );
			aOut.append( GetXMLToken(XML_ICON) );
		}

		if( (nAspect & 8) != 0 )
		{
			if( aOut.getLength() )
				aOut.append( sal_Unicode(' ') );
			aOut.append( GetXMLToken(XML_PRINT) );
		}

		rStrExpValue = aOut.makeStringAndClear();

		bRet = sal_True;
	}

	return bRet;
}
}//end of namespace binfilter
