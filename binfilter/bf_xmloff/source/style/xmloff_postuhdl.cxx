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



#ifndef _XMLOFF_PROPERTYHANDLER_POSTURETYPES_HXX
#include <postuhdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif

#ifndef _VCL_VCLENUM_HXX 
#include <vcl/vclenum.hxx>
#endif

namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

SvXMLEnumMapEntry __READONLY_DATA aPostureGenericMapping[] =
{
	{ XML_POSTURE_NORMAL,		ITALIC_NONE		},
	{ XML_POSTURE_ITALIC,		ITALIC_NORMAL	},
	{ XML_POSTURE_OBLIQUE,		ITALIC_OBLIQUE	},
	{ XML_TOKEN_INVALID,		0 				}
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLPosturePropHdl
//

XMLPosturePropHdl::~XMLPosturePropHdl()
{
	// nothing to do
}

sal_Bool XMLPosturePropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_uInt16 ePosture;

	if( ( bRet = rUnitConverter.convertEnum( ePosture, rStrImpValue, aPostureGenericMapping ) ) )
		rValue <<= (awt::FontSlant)ePosture;

	return bRet; 
}

sal_Bool XMLPosturePropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	OUStringBuffer aOut;
	awt::FontSlant eSlant;

	if( !( rValue >>= eSlant ) )
	{
		sal_Int32 nValue;
		
		if( !( rValue >>= nValue ) )
			return sal_False;

		eSlant = (awt::FontSlant)nValue;
	}

	if( ( bRet = rUnitConverter.convertEnum( aOut, (sal_Int32)eSlant, aPostureGenericMapping ) ) )
		rStrExpValue = aOut.makeStringAndClear();

	return bRet;
}

}//end of namespace binfilter
