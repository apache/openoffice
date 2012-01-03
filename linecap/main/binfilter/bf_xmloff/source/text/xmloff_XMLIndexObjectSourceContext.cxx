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




#ifndef _XMLOFF_XMLINDEXOBJECTSOURCECONTEXT_HXX_
#include "XMLIndexObjectSourceContext.hxx"
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
using ::binfilter::xmloff::token::XML_OBJECT_INDEX_ENTRY_TEMPLATE;
using ::binfilter::xmloff::token::XML_TOKEN_INVALID;

const sal_Char sAPI_CreateFromStarCalc[] = "CreateFromStarCalc";
const sal_Char sAPI_CreateFromStarChart[] = "CreateFromStarChart";
const sal_Char sAPI_CreateFromStarDraw[] = "CreateFromStarDraw";
const sal_Char sAPI_CreateFromStarImage[] = "CreateFromStarImage";
const sal_Char sAPI_CreateFromStarMath[] = "CreateFromStarMath";
const sal_Char sAPI_CreateFromOtherEmbeddedObjects[] = "CreateFromOtherEmbeddedObjects";


TYPEINIT1( XMLIndexObjectSourceContext, XMLIndexSourceBaseContext );

XMLIndexObjectSourceContext::XMLIndexObjectSourceContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	Reference<XPropertySet> & rPropSet) :
		XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName, 
								  rPropSet, sal_False),
		sCreateFromStarCalc(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromStarCalc)),
		sCreateFromStarChart(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromStarChart)),
		sCreateFromStarDraw(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromStarDraw)),
		sCreateFromStarMath(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromStarMath)),
		sCreateFromOtherEmbeddedObjects(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_CreateFromOtherEmbeddedObjects)),
		bUseCalc(sal_False),
		bUseChart(sal_False),
		bUseDraw(sal_False),
		bUseMath(sal_False),
		bUseOtherObjects(sal_False)
{
}

XMLIndexObjectSourceContext::~XMLIndexObjectSourceContext()
{
}

void XMLIndexObjectSourceContext::ProcessAttribute(
	enum IndexSourceParamEnum eParam, 
	const OUString& rValue)
{
	switch (eParam)
	{
		sal_Bool bTmp;

		case XML_TOK_INDEXSOURCE_USE_OTHER_OBJECTS:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseOtherObjects = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_SHEET:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseCalc = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_CHART:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseChart = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_DRAW:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseDraw = bTmp;
			}
			break;

		case XML_TOK_INDEXSOURCE_USE_MATH:
			if (SvXMLUnitConverter::convertBool(bTmp, rValue))
			{
				bUseMath = bTmp;
			}
			break;

		default:
			XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
			break;
	}
}

void XMLIndexObjectSourceContext::EndElement()
{
	Any aAny;

	aAny.setValue(&bUseCalc, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromStarCalc, aAny);

	aAny.setValue(&bUseChart, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromStarChart, aAny);

	aAny.setValue(&bUseDraw, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromStarDraw, aAny);

	aAny.setValue(&bUseMath, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromStarMath, aAny);

	aAny.setValue(&bUseOtherObjects, ::getBooleanCppuType());
	rIndexPropertySet->setPropertyValue(sCreateFromOtherEmbeddedObjects, aAny);

	XMLIndexSourceBaseContext::EndElement();
}

SvXMLImportContext* XMLIndexObjectSourceContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	if ( (XML_NAMESPACE_TEXT == nPrefix) &&
		 (IsXMLToken(rLocalName, XML_OBJECT_INDEX_ENTRY_TEMPLATE)) )
	{
		return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet, 
										   nPrefix, rLocalName,
										   aLevelNameTableMap,
										   XML_TOKEN_INVALID, // no outline-level attr
										   aLevelStylePropNameTableMap,
										   aAllowedTokenTypesTable);
	}
	else 
	{
		return XMLIndexSourceBaseContext::CreateChildContext(nPrefix, 
															 rLocalName,
															 xAttrList);
	}

}
}//end of namespace binfilter
