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




#ifndef _XMLOFF_PROPERTYHANDLER_CHARLOCALETYPES_HXX
#include <chrlohdl.hxx>
#endif


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif




#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
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

XMLCharLanguageHdl::~XMLCharLanguageHdl()
{
	// nothing to do
}

sal_Bool XMLCharLanguageHdl::equals( const ::com::sun::star::uno::Any& r1, const ::com::sun::star::uno::Any& r2 ) const
{
	sal_Bool bRet = sal_False;
	lang::Locale aLocale1, aLocale2;

	if( ( r1 >>= aLocale1 ) && ( r2 >>= aLocale2 ) )
		bRet = ( aLocale1.Language == aLocale2.Language );

	return bRet;
}

sal_Bool XMLCharLanguageHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	lang::Locale aLocale;

	rValue >>= aLocale;

	if( !IsXMLToken(rStrImpValue, XML_NONE) )
		aLocale.Language = rStrImpValue;

	rValue <<= aLocale;
	return sal_True; 
}

sal_Bool XMLCharLanguageHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	lang::Locale aLocale;
	if(!(rValue >>= aLocale))
		return sal_False;

	rStrExpValue = aLocale.Language;

	if( !rStrExpValue.getLength() )
		rStrExpValue = GetXMLToken( XML_NONE );
	
	return sal_True;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementHeightPropHdl
//

XMLCharCountryHdl::~XMLCharCountryHdl()
{
	// nothing to do
}

sal_Bool XMLCharCountryHdl::equals( const ::com::sun::star::uno::Any& r1, const ::com::sun::star::uno::Any& r2 ) const
{
	sal_Bool bRet = sal_False;
	lang::Locale aLocale1, aLocale2;

	if( ( r1 >>= aLocale1 ) && ( r2 >>= aLocale2 ) )
		bRet = ( aLocale1.Country == aLocale2.Country );

	return bRet;
}

sal_Bool XMLCharCountryHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	lang::Locale aLocale;

	rValue >>= aLocale;

	if( !IsXMLToken( rStrImpValue, XML_NONE ) )
		aLocale.Country = rStrImpValue;

	rValue <<= aLocale;
	return sal_True; 
}

sal_Bool XMLCharCountryHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	lang::Locale aLocale;
	if(!(rValue >>= aLocale))
		return sal_False;

	rStrExpValue = aLocale.Country;
	
	if( !rStrExpValue.getLength() )
		rStrExpValue = GetXMLToken( XML_NONE );

	return sal_True;
}
}//end of namespace binfilter
