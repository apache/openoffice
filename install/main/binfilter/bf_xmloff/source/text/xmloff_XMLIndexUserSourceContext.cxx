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




#ifndef _XMLOFF_XMLINDEXUSERSOURCECONTEXT_HXX_
#include "XMLIndexUserSourceContext.hxx"
#endif



#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
#endif






#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif



#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif


namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_USER_INDEX_ENTRY_TEMPLATE;
using ::binfilter::xmloff::token::XML_OUTLINE_LEVEL;


const sal_Char sAPI_CreateFromEmbeddedObjects[] = "CreateFromEmbeddedObjects";
const sal_Char sAPI_CreateFromGraphicObjects[] = "CreateFromGraphicObjects";
const sal_Char sAPI_CreateFromMarks[] = "CreateFromMarks";
const sal_Char sAPI_CreateFromTables[] = "CreateFromTables";
const sal_Char sAPI_CreateFromTextFrames[] = "CreateFromTextFrames";
const sal_Char sAPI_UseLevelFromSource[] = "UseLevelFromSource";
const sal_Char sAPI_CreateFromLevelParagraphStyles[] = "CreateFromLevelParagraphStyles";
const sal_Char sAPI_UserIndexName[] = "UserIndexName";


TYPEINIT1(XMLIndexUserSourceContext, XMLIndexSourceBaseContext);


XMLIndexUserSourceContext::XMLIndexUserSourceContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	Reference<XPropertySet> & rPropSet) :
		XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName, 
								  rPropSet, sal_True),
		sCreateFromEmbeddedObjects(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromEmbeddedObjects)),
		sCreateFromGraphicObjects(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromGraphicObjects)),
		sCreateFromMarks(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromMarks)),
		sCreateFromTables(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromTables)),
		sCreateFromTextFrames(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromTextFrames)),
		sUseLevelFromSource(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_UseLevelFromSource)),
		sCreateFromLevelParagraphStyles(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromLevelParagraphStyles)),
        sUserIndexName(RTL_CONSTASCII_USTRINGPARAM(sAPI_UserIndexName)),
		bUseObjects(sal_False),
		bUseGraphic(sal_False),
		bUseMarks(sal_False),
		bUseTables(sal_False),
		bUseFrames(sal_False),
		bUseLevelFromSource(sal_False),
		bUseLevelParagraphStyles(sal_False)
{
}

XMLIndexUserSourceContext::~XMLIndexUserSourceContext()
{
}

void XMLIndexUserSourceContext::ProcessAttribute(
	enum IndexSourceParamEnum eParam, 
	const OUString& rValue)
{
	sal_Bool bTmp;

	switch (eParam)
	{
		case XML_TOK_INDEXSOURCE_USE_INDEX_MARKS:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseMarks = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_OBJECTS:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseObjects = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_GRAPHICS:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseGraphic = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_TABLES:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseTables = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_FRAMES:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseFrames = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_COPY_OUTLINE_LEVELS:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseLevelFromSource = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_INDEX_SOURCE_STYLES:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseLevelParagraphStyles = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USER_INDEX_NAME:
            sIndexName = rValue;
			break;

		default:
			XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
			break;
	}
}


void XMLIndexUserSourceContext::EndElement()
{
	Any aAny;

	aAny.setValue(&bUseObjects, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromEmbeddedObjects, aAny);

	aAny.setValue(&bUseGraphic, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromGraphicObjects, aAny);

	aAny.setValue(&bUseLevelFromSource, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sUseLevelFromSource, aAny);

	aAny.setValue(&bUseMarks, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromMarks, aAny);

	aAny.setValue(&bUseTables, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromTables, aAny);

	aAny.setValue(&bUseFrames, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromTextFrames, aAny);

	aAny.setValue(&bUseLevelParagraphStyles, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromLevelParagraphStyles, aAny);

    if( sIndexName.getLength() > 0 )
    {
        aAny <<= sIndexName;
        rIndexPropertySet->setPropertyValue(sUserIndexName, aAny);
    }

	XMLIndexSourceBaseContext::EndElement();
}


SvXMLImportContext* XMLIndexUserSourceContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	if ( (XML_NAMESPACE_TEXT == nPrefix) &&
		 (IsXMLToken(rLocalName, XML_USER_INDEX_ENTRY_TEMPLATE)) )
	{
		return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet, 
										   nPrefix, rLocalName,
										   aLevelNameTOCMap,
										   XML_OUTLINE_LEVEL,
										   aLevelStylePropNameTOCMap,
										   aAllowedTokenTypesUser);
	}
	else 
	{
		return XMLIndexSourceBaseContext::CreateChildContext(nPrefix, 
															 rLocalName,
															 xAttrList);
	}

}
}//end of namespace binfilter
