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



#ifndef _XMLOFF_XMLAUTOMARKFILECONTEXT_HXX_
#include "XMLAutoMarkFileContext.hxx"
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




namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::xml::sax::XAttributeList;
using ::com::sun::star::beans::XPropertySet;

using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_HREF;


TYPEINIT1( XMLAutoMarkFileContext, SvXMLImportContext );

XMLAutoMarkFileContext::XMLAutoMarkFileContext(
	SvXMLImport& rImport, 
	sal_uInt16 nPrefix,
	const OUString& rLocalName) :
		SvXMLImportContext(rImport, nPrefix, rLocalName),
		sIndexAutoMarkFileURL(
			RTL_CONSTASCII_USTRINGPARAM("IndexAutoMarkFileURL"))
{
}

XMLAutoMarkFileContext::~XMLAutoMarkFileContext()
{
}


void XMLAutoMarkFileContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// scan for text:alphabetical-index-auto-mark-file attribute, and if 
	// found set value with the document

	sal_Int16 nLength = xAttrList->getLength();
	for( sal_Int16 i = 0; i < nLength; i++ )
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(i), &sLocalName );

		if ( ( XML_NAMESPACE_XLINK == nPrefix ) && 
             IsXMLToken(sLocalName, XML_HREF) )
		{
			Any aAny;
			aAny <<= GetImport().GetAbsoluteReference( xAttrList->getValueByIndex(i) );
			Reference<XPropertySet> xPropertySet(
				GetImport().GetModel(), UNO_QUERY );
			if (xPropertySet.is())
			{
				xPropertySet->setPropertyValue( sIndexAutoMarkFileURL, aAny );
			}
		}
	}
}
}//end of namespace binfilter
