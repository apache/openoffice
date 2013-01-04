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




#ifndef _XMLOFF_XMLINDEXCHAPTERINFOENTRYCONTEXT_HXX_
#include "XMLIndexChapterInfoEntryContext.hxx"
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


#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _COM_SUN_STAR_TEXT_CHAPTERFORMAT_HPP_
#include <com/sun/star/text/ChapterFormat.hpp>
#endif
namespace binfilter {


using namespace ::com::sun::star::text;
using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::beans::PropertyValues;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;



TYPEINIT1( XMLIndexChapterInfoEntryContext, XMLIndexSimpleEntryContext);

XMLIndexChapterInfoEntryContext::XMLIndexChapterInfoEntryContext(
	SvXMLImport& rImport, 
	XMLIndexTemplateContext& rTemplate,
	sal_uInt16 nPrfx,
	const OUString& rLocalName ) :
		XMLIndexSimpleEntryContext(rImport, rTemplate.sTokenChapterInfo,
								   rTemplate, nPrfx, rLocalName),
		nChapterInfo(ChapterFormat::NAME_NUMBER),
		bChapterInfoOK(sal_False)
{
}

XMLIndexChapterInfoEntryContext::~XMLIndexChapterInfoEntryContext()
{
}

static const SvXMLEnumMapEntry aChapterDisplayMap[] =
{
	{ XML_NAME,				    	ChapterFormat::NAME },
	{ XML_NUMBER,					ChapterFormat::NUMBER },
	{ XML_NUMBER_AND_NAME,			ChapterFormat::NAME_NUMBER },
// not supported by the template:
//	{ XML_PLAIN_NUMBER_AND_NAME,	ChapterFormat::NO_PREFIX_SUFFIX },
//	{ XML_PLAIN_NUMBER,	    		ChapterFormat::DIGIT },
	{ XML_TOKEN_INVALID,			0 }
};

void XMLIndexChapterInfoEntryContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// handle both, style name and bibliography info
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		if (XML_NAMESPACE_TEXT == nPrefix)
		{
			if ( IsXMLToken( sLocalName, XML_STYLE_NAME ) )
			{
				sCharStyleName = xAttrList->getValueByIndex(nAttr);
				bCharStyleNameOK = sal_True;
			}
			else if ( IsXMLToken( sLocalName, XML_DISPLAY ) )
			{
				sal_uInt16 nTmp;
				if (SvXMLUnitConverter::convertEnum(
					nTmp, xAttrList->getValueByIndex(nAttr), 
					aChapterDisplayMap))
				{
					nChapterInfo = nTmp;
					bChapterInfoOK = sal_True;
				}
			}
		}
	}

	// if we have a style name, set it!
	if (bCharStyleNameOK)
	{
		nValues++;
	}

	// if we have chaper info, set it!
	if (bChapterInfoOK)
	{
		nValues++;
	}
}

void XMLIndexChapterInfoEntryContext::FillPropertyValues(
	::com::sun::star::uno::Sequence<
		::com::sun::star::beans::PropertyValue> & rValues)
{
	// entry name and (optionally) style name in parent class
	XMLIndexSimpleEntryContext::FillPropertyValues(rValues);

	// chapter info field
	sal_Int32 nIndex = bCharStyleNameOK ? 2 : 1;
	rValues[nIndex].Name = rTemplateContext.sChapterFormat;
	Any aAny;	
	aAny <<= nChapterInfo;
	rValues[nIndex].Value = aAny;
}
}//end of namespace binfilter
