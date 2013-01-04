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



#ifndef _XMLOFF_PROPERTYHANDLER_UNDERLINETYPES_HXX
#include <undlihdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


#ifndef _COM_SUN_STAR_AWT_FONTUNDERLINE_HPP
#include <com/sun/star/awt/FontUnderline.hpp>
#endif

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::awt;
using namespace ::binfilter::xmloff::token;

SvXMLEnumMapEntry __READONLY_DATA pXML_Underline_Enum[] =
{
	{ XML_UNDERLINE_NONE,				FontUnderline::NONE },
	{ XML_UNDERLINE_SINGLE,		        FontUnderline::SINGLE },
	{ XML_UNDERLINE_DOUBLE,		        FontUnderline::DOUBLE },
	{ XML_UNDERLINE_DOTTED,		        FontUnderline::DOTTED },
	{ XML_UNDERLINE_DASH,				FontUnderline::DASH },
	{ XML_UNDERLINE_LONG_DASH,			FontUnderline::LONGDASH },
	{ XML_UNDERLINE_DOT_DASH,			FontUnderline::DASHDOT },
	{ XML_UNDERLINE_DOT_DOT_DASH,		FontUnderline::DASHDOTDOT },
	{ XML_UNDERLINE_WAVE,				FontUnderline::WAVE },
	{ XML_UNDERLINE_BOLD,				FontUnderline::BOLD },
	{ XML_UNDERLINE_BOLD_DOTTED,		FontUnderline::BOLDDOTTED },
	{ XML_UNDERLINE_BOLD_DASH,			FontUnderline::BOLDDASH },
	{ XML_UNDERLINE_BOLD_LONG_DASH,	    FontUnderline::BOLDLONGDASH },
	{ XML_UNDERLINE_BOLD_DOT_DASH,		FontUnderline::BOLDDASHDOT },
	{ XML_UNDERLINE_BOLD_DOT_DOT_DASH,  FontUnderline::BOLDDASHDOTDOT },
	{ XML_UNDERLINE_BOLD_WAVE,			FontUnderline::BOLDWAVE },
	{ XML_UNDERLINE_DOUBLE_WAVE, 		FontUnderline::DOUBLEWAVE },
	{ XML_UNDERLINE_SMALL_WAVE,		    FontUnderline::SMALLWAVE },
	{ XML_TOKEN_INVALID,				0 }
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLPosturePropHdl
//

XMLUnderlinePropHdl::~XMLUnderlinePropHdl()
{
	// nothing to do
}

sal_Bool XMLUnderlinePropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 eUnderline;

	if( ( bRet = rUnitConverter.convertEnum( eUnderline, rStrImpValue, pXML_Underline_Enum ) ) )
		rValue <<= (sal_Int16)eUnderline;

	return bRet; 
}

sal_Bool XMLUnderlinePropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Int16 nValue;
	OUStringBuffer aOut;

	if( rValue >>= nValue )
	{
		if( ( bRet = rUnitConverter.convertEnum( aOut, (sal_uInt16)nValue,
												 pXML_Underline_Enum ) ) )
			rStrExpValue = aOut.makeStringAndClear();
	}
	
	return bRet;
}

}//end of namespace binfilter
