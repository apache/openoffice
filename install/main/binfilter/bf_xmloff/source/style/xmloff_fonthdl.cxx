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



#ifndef _XMLOFF_PROPERTYHANDLER_FONTTYPES_HXX
#include <fonthdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif


#ifndef _VCL_VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif


#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

static SvXMLEnumMapEntry __READONLY_DATA aFontFamilyGenericMapping[] =
{
	{ XML_DECORATIVE,	    FAMILY_DECORATIVE },

	{ XML_MODERN,		    FAMILY_MODERN	},
	{ XML_ROMAN,	    	FAMILY_ROMAN	},
	{ XML_SCRIPT,		    FAMILY_SCRIPT	},
	{ XML_SWISS,	    	FAMILY_SWISS	},
	{ XML_SYSTEM,   		FAMILY_SYSTEM	},
	{ XML_TOKEN_INVALID,    0 				}
};

static SvXMLEnumMapEntry __READONLY_DATA aFontPitchMapping[] =
{
	{ XML_FIXED,		    PITCH_FIXED		},
	{ XML_VARIABLE,	        PITCH_VARIABLE	},
	{ XML_TOKEN_INVALID,    0 				}
};
///////////////////////////////////////////////////////////////////////////////
//
// class XMLFontFamilyNamePropHdl
//

XMLFontFamilyNamePropHdl::~XMLFontFamilyNamePropHdl()
{
	// Nothing to do
}

sal_Bool XMLFontFamilyNamePropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	String sValue;
	sal_Int32 nPos = 0;

	do
	{
		sal_Int32 nFirst = nPos;
		nPos = SvXMLUnitConverter::indexOfComma( rStrImpValue, nPos );
		sal_Int32 nLast = (-1 == nPos ? rStrImpValue.getLength() : nPos);
		if( nLast > 0 )
			nLast--;

		// skip trailing blanks
		while( sal_Unicode(' ') == rStrImpValue[nLast] && nLast > nFirst )
			nLast--;

		// skip leading blanks
		while( sal_Unicode(' ') == rStrImpValue[nFirst] && nFirst <= nLast )
			nFirst++;

		// remove quotes
		sal_Unicode c = rStrImpValue[nFirst];
		if( nFirst < nLast && (sal_Unicode('\'') == c || sal_Unicode('\"') == c) && rStrImpValue[nLast] == c )
		{
			nFirst++;
			nLast--;
		}

		if( nFirst <= nLast )
		{
			if( sValue.Len() != 0 )
				sValue += sal_Unicode(';');

			OUString sTemp = rStrImpValue.copy( nFirst, nLast-nFirst+1 );
			sValue += sTemp.getStr();
		}

		if( -1 != nPos )
			nPos++;
	}
	while( -1 != nPos );

	if( sValue.Len() )
	{
		rValue <<= OUString(sValue.GetBuffer());
		bRet = sal_True;
	}

	return bRet;
}

sal_Bool XMLFontFamilyNamePropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	OUString aStrFamilyName;

	if( rValue >>= aStrFamilyName )
	{
		OUStringBuffer sValue( aStrFamilyName.getLength() + 2L );
		sal_Int32 nPos = 0L;
		do
		{
			sal_Int32 nFirst = nPos;
			nPos = aStrFamilyName.indexOf( sal_Unicode(';'), nPos );
            sal_Int32 nLast = ((sal_Int32)-1L == nPos ? aStrFamilyName.getLength() : nPos);

			// Set position to the character behind the ';', so we won't
			// forget this.
			if( -1L != nPos )
				nPos++;

			// If the property value was empty, we stop now.
			// If there is a ';' at the first position, the empty name
			// at the start will be removed.
			if( 0L == nLast )
				continue;

			// nFirst and nLast now denote the first and last character of
			// one font name.
			nLast--;

			// skip trailing blanks
			while( sal_Unicode(' ') == aStrFamilyName[nLast] && nLast > nFirst )
				nLast--;

			// skip leading blanks
			while( sal_Unicode(' ') == aStrFamilyName[nFirst] && nFirst <= nLast )
				nFirst++;

			if( nFirst <= nLast )
			{
				if( sValue.getLength() != 0L )
				{
					sValue.append( sal_Unicode( ',' ) );
					sValue.append( sal_Unicode( ' ' ));
				}
                sal_Int32 nLen = nLast-nFirst+(sal_Int32)1L;
				OUString sFamily( aStrFamilyName.copy( nFirst, nLen ) );
				sal_Bool bQuote = sal_False;
				for( sal_Int32 i=0; i < nLen; i++ )
				{
					sal_Unicode c = sFamily[i];
					if( sal_Unicode(' ') == c || sal_Unicode(',') == c )
					{
						bQuote = sal_True;
						break;
					}
				}
				if( bQuote )
					sValue.append( sal_Unicode('\'') );
				sValue.append( sFamily );
				if( bQuote )
					sValue.append( sal_Unicode('\'') );
			}
		}
		while( -1L != nPos );

		rStrExpValue = sValue.makeStringAndClear();

		bRet = sal_True;
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFontFamilyPropHdl
//

XMLFontFamilyPropHdl::~XMLFontFamilyPropHdl()
{
	// Nothing to do
}

sal_Bool XMLFontFamilyPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	sal_uInt16 eNewFamily;

	if( ( bRet = rUnitConverter.convertEnum( eNewFamily, rStrImpValue, aFontFamilyGenericMapping ) ) )
		rValue <<= (sal_Int16)eNewFamily;

	return bRet;
}

sal_Bool XMLFontFamilyPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	OUStringBuffer aOut;

	sal_Int16 nFamily;
	if( rValue >>= nFamily )
	{
		FontFamily eFamily = (FontFamily)nFamily;
		if( eFamily != FAMILY_DONTKNOW )
			bRet = rUnitConverter.convertEnum( aOut, eFamily, aFontFamilyGenericMapping );
	}

	rStrExpValue = aOut.makeStringAndClear();

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFontEncodingPropHdl
//

XMLFontEncodingPropHdl::~XMLFontEncodingPropHdl()
{
	// Nothing to do
}

sal_Bool XMLFontEncodingPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_True;

	if( IsXMLToken( rStrImpValue, XML_X_SYMBOL ) )
		rValue <<= (sal_Int16) RTL_TEXTENCODING_SYMBOL;

	return bRet;
}

sal_Bool XMLFontEncodingPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	OUStringBuffer aOut;
	sal_Int16 nSet;

	if( rValue >>= nSet )
	{
		if( (rtl_TextEncoding)nSet == RTL_TEXTENCODING_SYMBOL )
		{
			aOut.append( GetXMLToken(XML_X_SYMBOL) );
			rStrExpValue = aOut.makeStringAndClear();
			bRet = sal_True;
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFontPitchPropHdl
//

XMLFontPitchPropHdl::~XMLFontPitchPropHdl()
{
	// Nothing to do
}

sal_Bool XMLFontPitchPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	USHORT eNewPitch;

	if( ( bRet = rUnitConverter.convertEnum( eNewPitch, rStrImpValue, aFontPitchMapping ) ) )
		rValue <<= (sal_Int16)eNewPitch;

	return bRet;
}

sal_Bool XMLFontPitchPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	sal_Bool bRet = sal_False;
	sal_Int16 nPitch;
	OUStringBuffer aOut;

	FontPitch ePitch = PITCH_DONTKNOW;
	if( rValue >>= nPitch )
		ePitch =  (FontPitch)nPitch;

	if( PITCH_DONTKNOW != ePitch )
	{
		bRet = rUnitConverter.convertEnum( aOut, ePitch, aFontPitchMapping, XML_FIXED );
		rStrExpValue = aOut.makeStringAndClear();
	}

	return bRet;
}
}//end of namespace binfilter
