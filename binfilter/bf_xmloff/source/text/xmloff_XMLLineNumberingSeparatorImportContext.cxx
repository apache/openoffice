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



#ifndef _XMLOFF_XMLLINENUMBERINGSEPARATORIMPORTCONTEXT_HXX_
#include "XMLLineNumberingSeparatorImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLLINENUMBERINGIMPORTCONTEXT_HXX_
#include "XMLLineNumberingImportContext.hxx"
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
namespace binfilter {


using namespace ::com::sun::star::uno;

using ::com::sun::star::xml::sax::XAttributeList;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_INCREMENT;

TYPEINIT1( XMLLineNumberingSeparatorImportContext, SvXMLImportContext );

XMLLineNumberingSeparatorImportContext::XMLLineNumberingSeparatorImportContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	XMLLineNumberingImportContext& rLineNumbering) :
		SvXMLImportContext(rImport, nPrfx, rLocalName),
		rLineNumberingContext(rLineNumbering)
{
}

XMLLineNumberingSeparatorImportContext::~XMLLineNumberingSeparatorImportContext()
{
}

void XMLLineNumberingSeparatorImportContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 i=0; i<nLength; i++) 
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(i), &sLocalName );

		if ( (nPrefix == XML_NAMESPACE_TEXT) &&
			 IsXMLToken(sLocalName, XML_INCREMENT) )
		{
			sal_Int32 nTmp;
			if (SvXMLUnitConverter::convertNumber(
				nTmp, xAttrList->getValueByIndex(i), 0))
			{
				rLineNumberingContext.SetSeparatorIncrement((sal_Int16)nTmp);
			}
			// else: invalid number -> ignore
		}
		// else: unknown attribute -> ignore
	}
}

void XMLLineNumberingSeparatorImportContext::Characters(
	const OUString& rChars )
{
	sSeparatorBuf.append(rChars);
}

void XMLLineNumberingSeparatorImportContext::EndElement()
{
	rLineNumberingContext.SetSeparatorText(sSeparatorBuf.makeStringAndClear());
}
}//end of namespace binfilter
