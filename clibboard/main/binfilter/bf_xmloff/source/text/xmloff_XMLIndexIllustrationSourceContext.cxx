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




#ifndef _XMLOFF_XMLINDEXILLUSTRATIONSOURCECONTEXT_HXX_
#include "XMLIndexIllustrationSourceContext.hxx"
#endif


#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
#endif




#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif





namespace binfilter {

using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_ILLUSTRATION_INDEX_ENTRY_TEMPLATE;
using ::binfilter::xmloff::token::XML_TOKEN_INVALID;

TYPEINIT1(XMLIndexIllustrationSourceContext, XMLIndexTableSourceContext);


XMLIndexIllustrationSourceContext::XMLIndexIllustrationSourceContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	Reference<XPropertySet> & rPropSet) :
		XMLIndexTableSourceContext(rImport, nPrfx, rLocalName, rPropSet)
{
}

XMLIndexIllustrationSourceContext::~XMLIndexIllustrationSourceContext()
{
}

SvXMLImportContext* XMLIndexIllustrationSourceContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	if ( ( XML_NAMESPACE_TEXT == nPrefix ) &&
		 ( IsXMLToken( rLocalName, XML_ILLUSTRATION_INDEX_ENTRY_TEMPLATE ) ) )
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
