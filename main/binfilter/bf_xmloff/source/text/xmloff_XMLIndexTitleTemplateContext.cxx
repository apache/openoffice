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



#ifndef _XMLOFF_XMLINDEXTITLETEMPLATECONTEXT_HXX_
#include "XMLIndexTitleTemplateContext.hxx"
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX 
#include "nmspmap.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

namespace binfilter {


using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_STYLE_NAME;


const sal_Char sAPI_Title[] = "Title";
const sal_Char sAPI_ParaStyleHeading[] = "ParaStyleHeading";


TYPEINIT1( XMLIndexTitleTemplateContext, SvXMLImportContext );

XMLIndexTitleTemplateContext::XMLIndexTitleTemplateContext(
	SvXMLImport& rImport, 
	Reference<XPropertySet> & rPropSet,
	sal_uInt16 nPrfx,
	const OUString& rLocalName) :
		SvXMLImportContext(rImport, nPrfx, rLocalName),
		sStyleName(),
		bStyleNameOK(sal_False),
		sContent(),
		rTOCPropertySet(rPropSet),
		sTitle(RTL_CONSTASCII_USTRINGPARAM(sAPI_Title)),
		sParaStyleHeading(RTL_CONSTASCII_USTRINGPARAM(sAPI_ParaStyleHeading))
{
}


XMLIndexTitleTemplateContext::~XMLIndexTitleTemplateContext()
{
}

void XMLIndexTitleTemplateContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// there's only one attribute: style-name
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		if ( (XML_NAMESPACE_TEXT == nPrefix) &&
			 (IsXMLToken(sLocalName, XML_STYLE_NAME)) )
		{
			sStyleName = xAttrList->getValueByIndex(nAttr);
			bStyleNameOK = sal_True;
		}
	}
}

void XMLIndexTitleTemplateContext::EndElement()
{
	Any aAny;

	aAny <<= sContent.makeStringAndClear();
	rTOCPropertySet->setPropertyValue(sTitle, aAny);

	if (bStyleNameOK)
	{
		aAny <<= sStyleName;
		rTOCPropertySet->setPropertyValue(sParaStyleHeading, aAny);
	}
}

void XMLIndexTitleTemplateContext::Characters(
	const OUString& sString)
{
	sContent.append(sString);
}
}//end of namespace binfilter
