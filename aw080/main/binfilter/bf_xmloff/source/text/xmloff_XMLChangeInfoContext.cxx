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



#ifndef _XMLOFF_XMLCHANGEINFOCONTEXT_HXX
#include "XMLChangeInfoContext.hxx"
#endif

#ifndef _XMLOFF_XMLCHANGEDREGIONIMPORTCONTEXT_HXX
#include "XMLChangedRegionImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLSTRINGBUFFERIMPORTCONTEXT_HXX
#include "XMLStringBufferImportContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif
namespace binfilter {



using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;


TYPEINIT1(XMLChangeInfoContext, SvXMLImportContext);

XMLChangeInfoContext::XMLChangeInfoContext(
	SvXMLImport& rImport,
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	XMLChangedRegionImportContext& rPParent,
	const OUString& rChangeType) :
		SvXMLImportContext(rImport, nPrefix, rLocalName),
		rChangedRegion(rPParent),
		rType(rChangeType)
{
}

XMLChangeInfoContext::~XMLChangeInfoContext()
{
}

void XMLChangeInfoContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// process attributes: chg-author, chg-date-time
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		OUString sValue = xAttrList->getValueByIndex(nAttr);
		if (XML_NAMESPACE_OFFICE == nPrefix)
		{
			if ( IsXMLToken( sLocalName, XML_CHG_AUTHOR ) )
			{
				sAuthor = sValue;
			}
			else if ( IsXMLToken( sLocalName, XML_CHG_DATE_TIME ) )
			{
				sDateTime = sValue;
			}
			// else: unknown attribute
		}
		// else: unknown namespace
	}

}

SvXMLImportContext* XMLChangeInfoContext::CreateChildContext(
	USHORT nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList >& xAttrList )
{
	SvXMLImportContext* pContext = NULL;

	if ( ( XML_NAMESPACE_TEXT == nPrefix ) && 
         IsXMLToken( rLocalName, XML_P )       )
	{
		pContext = new XMLStringBufferImportContext(GetImport(), nPrefix,
												   rLocalName, sCommentBuffer);
	}
	else
	{
		pContext = SvXMLImportContext::CreateChildContext(nPrefix, rLocalName,
														  xAttrList);
	}

	return pContext;
}

void XMLChangeInfoContext::EndElement()
{
	// set values at changed region context
	rChangedRegion.SetChangeInfo(rType, sAuthor, 
								 sCommentBuffer.makeStringAndClear(), 
								 sDateTime);
}
}//end of namespace binfilter
