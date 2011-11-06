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



#ifndef _XMLOFF_XMLSECTIONSOURCEIMPORTCONTEXT_HXX_
#include "XMLSectionSourceImportContext.hxx"
#endif


#ifndef _COM_SUN_STAR_TEXT_SECTIONFILELINK_HPP_
#include <com/sun/star/text/SectionFileLink.hpp>
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


#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::binfilter::xmloff::token;


TYPEINIT1(XMLSectionSourceImportContext, SvXMLImportContext);

XMLSectionSourceImportContext::XMLSectionSourceImportContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	Reference<XPropertySet> & rSectPropSet) :
		SvXMLImportContext(rImport, nPrfx, rLocalName),
		rSectionPropertySet(rSectPropSet)
{
}

XMLSectionSourceImportContext::~XMLSectionSourceImportContext()
{
}

enum XMLSectionSourceToken 
{
	XML_TOK_SECTION_XLINK_HREF,
	XML_TOK_SECTION_TEXT_FILTER_NAME,
	XML_TOK_SECTION_TEXT_SECTION_NAME
};

static __FAR_DATA SvXMLTokenMapEntry aSectionSourceTokenMap[] =
{
	{ XML_NAMESPACE_XLINK, XML_HREF, XML_TOK_SECTION_XLINK_HREF },
	{ XML_NAMESPACE_TEXT, XML_FILTER_NAME, XML_TOK_SECTION_TEXT_FILTER_NAME },
	{ XML_NAMESPACE_TEXT, XML_SECTION_NAME, 
										XML_TOK_SECTION_TEXT_SECTION_NAME },
	XML_TOKEN_MAP_END
};


void XMLSectionSourceImportContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	SvXMLTokenMap aTokenMap(aSectionSourceTokenMap);
	OUString sURL;
	OUString sFilterName;
	OUString sSectionName;
	
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );

		switch (aTokenMap.Get(nPrefix, sLocalName))
		{
			case XML_TOK_SECTION_XLINK_HREF:
				sURL = xAttrList->getValueByIndex(nAttr);
				break;

			case XML_TOK_SECTION_TEXT_FILTER_NAME:
				sFilterName = xAttrList->getValueByIndex(nAttr);
				break;

			case XML_TOK_SECTION_TEXT_SECTION_NAME:
				sSectionName = xAttrList->getValueByIndex(nAttr);
				break;

			default:
				; // ignore
				break;
		}
	}

	// we only need them once
	const OUString sFileLink(RTL_CONSTASCII_USTRINGPARAM("FileLink"));
	const OUString sLinkRegion(RTL_CONSTASCII_USTRINGPARAM("LinkRegion"));

	Any aAny;
	if ((sURL.getLength() > 0) || (sFilterName.getLength() > 0))
	{
		SectionFileLink aFileLink;
		aFileLink.FileURL = GetImport().GetAbsoluteReference( sURL );
		aFileLink.FilterName = sFilterName;

		aAny <<= aFileLink;
		rSectionPropertySet->setPropertyValue(sFileLink, aAny);
	}

	if (sSectionName.getLength() > 0)
	{
		aAny <<= sSectionName;
		rSectionPropertySet->setPropertyValue(sLinkRegion, aAny);
	}
}

void XMLSectionSourceImportContext::EndElement()
{
	// this space intentionally left blank.
}

SvXMLImportContext* XMLSectionSourceImportContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	// ignore -> default context
	return new SvXMLImportContext(GetImport(), nPrefix, rLocalName);
}
}//end of namespace binfilter
