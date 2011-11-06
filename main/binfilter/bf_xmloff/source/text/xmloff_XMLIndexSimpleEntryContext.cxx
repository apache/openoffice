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




#ifndef _XMLOFF_XMLINDEXSIMPLEENTRYCONTEXT_HXX_
#include "XMLIndexSimpleEntryContext.hxx"
#endif

#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
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


//using namespace ::com::sun::star::text;

using ::rtl::OUString;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::beans::PropertyValues;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_STYLE_NAME;

const sal_Char sAPI_TokenType[] = "TokenType";
const sal_Char sAPI_CharacterStyleName[] = "CharacterStyleName";

TYPEINIT1( XMLIndexSimpleEntryContext, SvXMLImportContext);

XMLIndexSimpleEntryContext::XMLIndexSimpleEntryContext(
	SvXMLImport& rImport, 
	const OUString& rEntry,
	XMLIndexTemplateContext& rTemplate,
	sal_uInt16 nPrfx,
	const OUString& rLocalName ) :
		SvXMLImportContext(rImport, nPrfx, rLocalName),
		rTemplateContext(rTemplate),
		rEntryType(rEntry),
		sCharStyleName(),
		bCharStyleNameOK(sal_False),
		nValues(1)
{
}

XMLIndexSimpleEntryContext::~XMLIndexSimpleEntryContext()
{
}

void XMLIndexSimpleEntryContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// we know only one attribute: style-name
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		if ( (XML_NAMESPACE_TEXT == nPrefix) &&
			 IsXMLToken(sLocalName, XML_STYLE_NAME) )
		{
			sCharStyleName = xAttrList->getValueByIndex(nAttr);
			bCharStyleNameOK = sal_True;
		}
	}

	// if we have a style name, set it!
	if (bCharStyleNameOK)
	{
		nValues++;
	}
		
}

void XMLIndexSimpleEntryContext::EndElement()
{
	Sequence<PropertyValue> aValues(nValues);

	FillPropertyValues(aValues);
	rTemplateContext.addTemplateEntry(aValues);
}

void XMLIndexSimpleEntryContext::FillPropertyValues(
	::com::sun::star::uno::Sequence<
		::com::sun::star::beans::PropertyValue> & rValues)
{
	// due to the limited number of subclasses, we fill the values
	// directly into the slots. Subclasses will have to know they can
	// only use slot so-and-so.

	Any aAny;

	// token type
	rValues[0].Name = rTemplateContext.sTokenType;
	aAny <<= rEntryType;
	rValues[0].Value = aAny;

	// char style
	if (bCharStyleNameOK)
	{
		rValues[1].Name = rTemplateContext.sCharacterStyleName;
		aAny <<= sCharStyleName;
		rValues[1].Value = aAny;
	}

}
}//end of namespace binfilter
