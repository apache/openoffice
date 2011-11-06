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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

// INCLUDE ---------------------------------------------------------------

#include "xmlconti.hxx"
#include "xmlimprt.hxx"

#include <bf_xmloff/nmspmap.hxx>
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
namespace binfilter {

using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLContentContext::ScXMLContentContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
									  ::rtl::OUStringBuffer& sTempValue) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	sOUText(),
	sValue(sTempValue)
{
}

ScXMLContentContext::~ScXMLContentContext()
{
}

SvXMLImportContext *ScXMLContentContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = 0;

	if ((nPrefix == XML_NAMESPACE_TEXT) && IsXMLToken(rLName, XML_S))
	{
		sal_Int32 nRepeat(0);
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
			::rtl::OUString sValue = xAttrList->getValueByIndex( i );
			::rtl::OUString aLocalName;
			USHORT nPrfx = GetScImport().GetNamespaceMap().GetKeyByAttrName(
												sAttrName, &aLocalName );
			if ((nPrfx == XML_NAMESPACE_TEXT) && IsXMLToken(aLocalName, XML_C))
				nRepeat = sValue.toInt32();
		}
		if (nRepeat)
			for (sal_Int32 j = 0; j < nRepeat; j++)
				sOUText.append(static_cast<sal_Unicode>(' '));
		else
			sOUText.append(static_cast<sal_Unicode>(' '));
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void ScXMLContentContext::Characters( const ::rtl::OUString& rChars )
{
	sOUText.append(rChars);
}

void ScXMLContentContext::EndElement()
{
	sValue.append(sOUText);
}
}
