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




#ifndef _XMLOFF_XMLINDEXBIBLIOGRAPHYSOURCECONTEXT_HXX_
#include "XMLIndexBibliographySourceContext.hxx"
#endif



#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
#endif






#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif





namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;


TYPEINIT1(XMLIndexBibliographySourceContext, XMLIndexSourceBaseContext);


XMLIndexBibliographySourceContext::XMLIndexBibliographySourceContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const OUString& rLocalName,
	Reference<XPropertySet> & rPropSet) :
		XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName, 
								  rPropSet, sal_False)
{
}

XMLIndexBibliographySourceContext::~XMLIndexBibliographySourceContext()
{
}

void XMLIndexBibliographySourceContext::ProcessAttribute(
	enum IndexSourceParamEnum eParam, 
	const OUString& rValue)
{
	// We have no attributes. Who wants attributes, anyway?
}


void XMLIndexBibliographySourceContext::EndElement()
{
	// No attributes, no properties.
}


SvXMLImportContext* XMLIndexBibliographySourceContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList )
{
	if ( ( XML_NAMESPACE_TEXT == nPrefix ) &&
		 ( IsXMLToken( rLocalName, XML_BIBLIOGRAPHY_ENTRY_TEMPLATE ) ) )
	{
		return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet, 
										   nPrefix, rLocalName,
										   aLevelNameBibliographyMap,
										   XML_BIBLIOGRAPHY_TYPE,
										   aLevelStylePropNameBibliographyMap,
										   aAllowedTokenTypesBibliography);
	}
	else 
	{
		return XMLIndexSourceBaseContext::CreateChildContext(nPrefix, 
															 rLocalName,
															 xAttrList);
	}

}
}//end of namespace binfilter
