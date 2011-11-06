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



#ifndef _XMLOFF_PROPERTYHANDLER_SHADOWTYPES_HXX
#include "shadwhdl.hxx"
#endif


#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

// --

#ifndef _COM_SUN_STAR_TABLE_SHADOWFORMAT_HPP_
#include <com/sun/star/table/ShadowFormat.hpp>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePropHdl
//

XMLShadowPropHdl::~XMLShadowPropHdl()
{
	// nothing to do
}

sal_Bool XMLShadowPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	table::ShadowFormat aShadow;
	aShadow.Location = table::ShadowLocation_BOTTOM_RIGHT;

	sal_Bool bColorFound = sal_False;
	sal_Bool bOffsetFound = sal_False;
	SvXMLTokenEnumerator aTokenEnum( rStrImpValue );
	Color aColor( 128,128, 128 );
	OUString aToken;

	while( aTokenEnum.getNextToken( aToken ) )
	{
		if( IsXMLToken( aToken, XML_NONE ) )
		{
			aShadow.Location = table::ShadowLocation_NONE;
			bRet = sal_True;
			break;
		}
		else if( !bColorFound && aToken.compareToAscii( "#", 1 ) == 0 )
		{
			bRet = rUnitConverter.convertColor( aColor, aToken );
			if( !bRet )
				return sal_False;

			bColorFound = sal_True;
		}
		else if( !bOffsetFound )
		{
			sal_Int32 nX = 0, nY = 0;

			bRet = rUnitConverter.convertMeasure( nX, aToken );
			if( bRet && aTokenEnum.getNextToken( aToken ) )
				bRet = rUnitConverter.convertMeasure( nY, aToken );

			if( bRet )
			{
				if( nX < 0 )
				{
					if( nY < 0 )
						aShadow.Location = table::ShadowLocation_TOP_LEFT;
					else
						aShadow.Location = table::ShadowLocation_BOTTOM_LEFT;
				}
				else
				{
					if( nY < 0 )
						aShadow.Location = table::ShadowLocation_TOP_RIGHT;
					else
						aShadow.Location = table::ShadowLocation_BOTTOM_RIGHT;
				}

				if( nX < 0 ) nX *= -1;
				if( nY < 0 ) nY *= -1;

				aShadow.ShadowWidth = (nX + nY) >> 1;
			}
		}
	}

	if( bRet && ( bColorFound || bOffsetFound ) )
	{
		aShadow.IsTransparent = aColor.GetTransparency() > 0;
		aShadow.Color = aColor.GetColor();
		bRet = sal_True;
	}

	rValue <<= aShadow;

	return bRet; 
}

sal_Bool XMLShadowPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
  	OUStringBuffer aOut;
	table::ShadowFormat aShadow;

	if( rValue >>= aShadow )
	{
		sal_Int32 nX = 1, nY = 1;
		
		switch( aShadow.Location )
		{
			case table::ShadowLocation_TOP_LEFT:
				nX = -1;
				nY = -1;
				break;
			case table::ShadowLocation_TOP_RIGHT:
				nY = -1;
				break;
			case table::ShadowLocation_BOTTOM_LEFT:
				nX = -1;
				break;
			case table::ShadowLocation_BOTTOM_RIGHT:
				break;
			case table::ShadowLocation_NONE:
			default:
				rStrExpValue = GetXMLToken(XML_NONE);
				return sal_True;
		}

		nX *= aShadow.ShadowWidth;
		nY *= aShadow.ShadowWidth;

		rUnitConverter.convertColor( aOut, aShadow.Color );

		aOut.append( sal_Unicode(' ') );
		rUnitConverter.convertMeasure( aOut, nX );
		aOut.append( sal_Unicode(' ') );
		rUnitConverter.convertMeasure( aOut, nY );

		rStrExpValue = aOut.makeStringAndClear();

		bRet = sal_True;
	}

	return bRet;
}
}//end of namespace binfilter
