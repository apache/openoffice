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





#ifndef _XMLOFF_XMLINDEXTOCSTYLESCONTEXT_HXX_
#include "XMLIndexTOCStylesContext.hxx"
#endif


#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX 
#include "nmspmap.hxx"
#endif


#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif



namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::container::XIndexReplace;
using ::com::sun::star::xml::sax::XAttributeList;


const sal_Char sAPI_LevelParagraphStyles[] = "LevelParagraphStyles";

TYPEINIT1( XMLIndexTOCStylesContext, SvXMLImportContext );


XMLIndexTOCStylesContext::XMLIndexTOCStylesContext(
	SvXMLImport& rImport, 
	Reference<XPropertySet> & rPropSet,
	sal_uInt16 nPrfx,
	const OUString& rLocalName ) :
		SvXMLImportContext(rImport, nPrfx, rLocalName),
		rTOCPropertySet(rPropSet), 
		aStyleNames(),
		nOutlineLevel(-1),
		sLevelParagraphStyles(RTL_CONSTASCII_USTRINGPARAM(
			sAPI_LevelParagraphStyles))
{
}

XMLIndexTOCStylesContext::~XMLIndexTOCStylesContext()
{
}

void XMLIndexTOCStylesContext::StartElement(
	const Reference<XAttributeList> & xAttrList )
{
	// find text:outline-level attribute
	sal_Int16 nCount = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		if ( (XML_NAMESPACE_TEXT == nPrefix) &&
			 (IsXMLToken(sLocalName, XML_OUTLINE_LEVEL)) )
		{
			sal_Int32 nTmp;
			if (SvXMLUnitConverter::convertNumber(
					nTmp, xAttrList->getValueByIndex(nAttr), 1,
					GetImport().GetTextImport()->GetChapterNumbering()->
																getCount()))
			{
				// API numbers 0..9, we number 1..10
				nOutlineLevel = nTmp-1;
			}
		}
	}
}

void XMLIndexTOCStylesContext::EndElement()
{
	// if valid...
	if (nOutlineLevel >= 0)
	{
		// copy vector into sequence
		sal_Int32 nCount = aStyleNames.size();
		Sequence<OUString> aStyleNamesSequence(nCount);
		for(sal_Int32 i = 0; i < nCount; i++)
		{
			aStyleNamesSequence[i] = aStyleNames[i];
		}

		// get index replace
		Any aAny = rTOCPropertySet->getPropertyValue(sLevelParagraphStyles);
		Reference<XIndexReplace> xIndexReplace;
		aAny >>= xIndexReplace;

		// set style names
		aAny <<= aStyleNamesSequence;
		xIndexReplace->replaceByIndex(nOutlineLevel, aAny);
	}
}

SvXMLImportContext *XMLIndexTOCStylesContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	// check for index-source-style
	if ( (XML_NAMESPACE_TEXT == nPrefix) && 
		 IsXMLToken( rLocalName, XML_INDEX_SOURCE_STYLE ) )
	{
		// find text:style-name attribute and record in aStyleNames
		sal_Int16 nCount = xAttrList->getLength();
		for(sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
		{
			OUString sLocalName;
			sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
				GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
								  &sLocalName );
			if ( (XML_NAMESPACE_TEXT == nPrefix) &&
				 IsXMLToken( sLocalName, XML_STYLE_NAME ) )
			{
				aStyleNames.push_back(xAttrList->getValueByIndex(nAttr));
			}
		}
	}

	// always return default context; we already got the interesting info
	return SvXMLImportContext::CreateChildContext(nPrefix, rLocalName, 
												  xAttrList);
}
}//end of namespace binfilter
