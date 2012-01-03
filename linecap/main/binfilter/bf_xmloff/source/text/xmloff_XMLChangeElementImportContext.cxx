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



#ifndef _XMLOFF_XMLCHANGEELEMENTIMPORTCONTEXT_HXX
#include "XMLChangeElementImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLCHANGEDREGIONIMPORTCONTEXT_HXX
#include "XMLChangedRegionImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLCHANGEINFOCONTEXT_HXX
#include "XMLChangeInfoContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

namespace binfilter {



using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_P;
using ::binfilter::xmloff::token::XML_CHANGE_INFO;

TYPEINIT1( XMLChangeElementImportContext, SvXMLImportContext );

XMLChangeElementImportContext::XMLChangeElementImportContext(
	SvXMLImport& rImport,
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	sal_Bool bAccContent,
	XMLChangedRegionImportContext& rParent) :
		SvXMLImportContext(rImport, nPrefix, rLocalName),
		bAcceptContent(bAccContent),
		rChangedRegion(rParent)
{
}

SvXMLImportContext* XMLChangeElementImportContext::CreateChildContext(
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList)
{
	SvXMLImportContext* pContext = NULL;

	if ( (XML_NAMESPACE_OFFICE == nPrefix) &&
         IsXMLToken( rLocalName, XML_CHANGE_INFO) )
	{
		pContext = new XMLChangeInfoContext(GetImport(), nPrefix, rLocalName,
											rChangedRegion, GetLocalName());
	}
	else 
	{
        // import into redline -> create XText
        rChangedRegion.UseRedlineText();

		pContext = GetImport().GetTextImport()->CreateTextChildContext(
			GetImport(), nPrefix, rLocalName, xAttrList,
			XML_TEXT_TYPE_CHANGED_REGION);

        if (NULL == pContext)
        {
            // no text element -> use default
            pContext = SvXMLImportContext::CreateChildContext(
                nPrefix, rLocalName, xAttrList);

            // illegal element content! TODO: discard this redline!
        }
	}


	return pContext;
}

// #107848#
void XMLChangeElementImportContext::StartElement( const Reference< XAttributeList >& xAttrList )
{
	if(bAcceptContent)
	{
		GetImport().GetTextImport()->SetInsideDeleteContext(sal_True);
	}
}

// #107848#
void XMLChangeElementImportContext::EndElement()
{
	if(bAcceptContent)
	{
		GetImport().GetTextImport()->SetInsideDeleteContext(sal_False);
	}
}
}//end of namespace binfilter
