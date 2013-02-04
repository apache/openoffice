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



#ifndef _XMLTEXTFRAMEHYPERLINKCONTEXT_HXX
#define _XMLTEXTFRAMEHYPERLINKCONTEXT_HXX

#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFRAME_HPP
#include <com/sun/star/text/XTextFrame.hpp>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif

namespace com { namespace sun { namespace star {
	namespace text { class XTextCursor; class XTextFrame; }
	namespace beans { class XPropertySet; }
} } }

namespace binfilter {

class XMLTextFrameHyperlinkContext : public SvXMLImportContext
{
	::rtl::OUString				 sHRef;
	::rtl::OUString				 sName;
	::rtl::OUString				 sTargetFrameName;
	::com::sun::star::text::TextContentAnchorType eAnchorType;
	::com::sun::star::uno::Reference < 
		::com::sun::star::text::XTextContent> *pTextContent;
	::com::sun::star::text::TextContentAnchorType *pAnchorType;
	sal_Bool					bMap;

public:

	TYPEINFO();

	XMLTextFrameHyperlinkContext( SvXMLImport& rImport,
			sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			::com::sun::star::text::TextContentAnchorType eAnchorType,
			::com::sun::star::uno::Reference < 
				::com::sun::star::text::XTextContent> *pTxtCntnt = 0 ,
			::com::sun::star::text::TextContentAnchorType *pAnchrType = 0 );
	virtual ~XMLTextFrameHyperlinkContext();

	virtual void EndElement();

	SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
				const ::rtl::OUString& rLocalName,
			 	const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList );

};


}//end of namespace binfilter
#endif
