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


#ifndef _XMLOFF_TEXTPARAI_HXX_
#define _XMLOFF_TEXTPARAI_HXX_


#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif
namespace com { namespace sun { namespace star {
namespace text {  class XTextRange; }
namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace binfilter {

class XMLHints_Impl;

#ifdef CONV_STAR_FONTS
#define CONV_FROM_STAR_BATS 1
#define CONV_FROM_STAR_MATH 2
#define CONV_STAR_FONT_FLAGS_VALID 4
#endif

class XMLParaContext : public SvXMLImportContext
{
	::com::sun::star::uno::Reference <
		::com::sun::star::text::XTextRange > xStart;	// xub_StrLen nStart;
	::rtl::OUString				sStyleName;
	sal_Int8 				nOutlineLevel;
	XMLHints_Impl			*pHints;
	sal_Bool				bIgnoreLeadingSpace;
	sal_Bool				bHeading;
#ifdef CONV_STAR_FONTS
	sal_uInt8				nStarFontsConvFlags;
#endif

public:

	TYPEINFO();

	XMLParaContext( SvXMLImport& rImport,
			sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			sal_Bool bHeading );

	virtual ~XMLParaContext();

	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
			const ::rtl::OUString& rLocalName,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	virtual void Characters( const ::rtl::OUString& rChars );

};


}//end of namespace binfilter
#endif
