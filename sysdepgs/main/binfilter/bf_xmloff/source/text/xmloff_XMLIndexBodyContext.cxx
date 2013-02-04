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




#ifndef _XMLOFF_XMLINDEXBODYCONTEXT_HXX_
#include "XMLIndexBodyContext.hxx"
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif



namespace binfilter {

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;


TYPEINIT1( XMLIndexBodyContext, SvXMLImportContext);

XMLIndexBodyContext::XMLIndexBodyContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrfx,
	const ::rtl::OUString& rLocalName ) :
        SvXMLImportContext(rImport, nPrfx, rLocalName),
        bHasContent(sal_False)
{
}

XMLIndexBodyContext::~XMLIndexBodyContext()
{
}

SvXMLImportContext* XMLIndexBodyContext::CreateChildContext( 
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList)
{
	SvXMLImportContext* pContext = NULL;

	// return text content (if possible)
	pContext = GetImport().GetTextImport()->CreateTextChildContext(
		GetImport(), nPrefix, rLocalName, xAttrList, XML_TEXT_TYPE_SECTION );
	if (NULL == pContext)
	{
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
	}
    else
        bHasContent = sal_True;

	return pContext;
}
}//end of namespace binfilter
