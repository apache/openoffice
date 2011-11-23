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



#ifndef _XMLOFF_XMLCLIPPROPERTYHANDLER_HXX
#include "XMLClipPropertyHandler.hxx"
#endif


#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_TEXT_GRAPHICCROP_HPP_
#include <com/sun/star/text/GraphicCrop.hpp>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _XMLOFF_XMLKYWD_HXX
#include <xmlkywd.hxx>
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePropHdl
//

XMLClipPropertyHandler::~XMLClipPropertyHandler()
{
	// nothing to do
}

sal_Bool XMLClipPropertyHandler::equals(
		const Any& r1,
		const Any& r2 ) const
{
	GraphicCrop aCrop1, aCrop2;
	r1 >>= aCrop1;
	r2 >>= aCrop2;
	
	return aCrop1.Top == aCrop2.Top &&
		   aCrop1.Bottom == aCrop2.Bottom &&
		   aCrop1.Left == aCrop2.Left &&
		   aCrop1.Right == aCrop2.Right;
}

sal_Bool XMLClipPropertyHandler::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Int32 nLen = rStrImpValue.getLength();
	if( nLen > 6 &&
		0 == rStrImpValue.compareToAscii( sXML_rect, 4 ) &&
		rStrImpValue[4] == '(' &&
		rStrImpValue[nLen-1] == ')' )
	{
		GraphicCrop aCrop;
		OUString sTmp( rStrImpValue.copy( 5, nLen-6 ) );
		SvXMLTokenEnumerator aTokenEnum( sTmp );

		sal_uInt16 nPos = 0;
		OUString aToken;
		while( aTokenEnum.getNextToken( aToken ) )
		{
			sal_Int32 nVal = 0;
			if( !IsXMLToken(aToken, XML_AUTO) &&
			 	!rUnitConverter.convertMeasure( nVal, aToken ) )
				break;

			switch( nPos )
			{
			case 0: aCrop.Top = nVal;	break;
			case 1: aCrop.Right = nVal;	break;
			case 2: aCrop.Bottom = nVal;	break;
			case 3: aCrop.Left = nVal;	break;
			}
			nPos++;
		}

		bRet = (4 == nPos );
		if( bRet )
			rValue <<= aCrop;
	}

	return bRet; 
}

sal_Bool XMLClipPropertyHandler::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
  	OUStringBuffer aOut(30);
	GraphicCrop aCrop;

	if( rValue >>= aCrop )
	{
		aOut.append( GetXMLToken(XML_RECT) );
		aOut.append( (sal_Unicode)'(' );
		rUnitConverter.convertMeasure( aOut, aCrop.Top );
		aOut.append( (sal_Unicode)' ' );
		rUnitConverter.convertMeasure( aOut, aCrop.Right );
		aOut.append( (sal_Unicode)' ' );
		rUnitConverter.convertMeasure( aOut, aCrop.Bottom );
		aOut.append( (sal_Unicode)' ' );
		rUnitConverter.convertMeasure( aOut, aCrop.Left );
		aOut.append( (sal_Unicode)')' );
		rStrExpValue = aOut.makeStringAndClear();

		bRet = sal_True;
	}

	return bRet;
}
}//end of namespace binfilter
