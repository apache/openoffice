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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;

// ---------------------------------------------------------------------

class SwXMLBodyContext_Impl : public SvXMLImportContext
{
	SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }

public:

	SwXMLBodyContext_Impl( SwXMLImport& rImport, sal_uInt16 nPrfx,
					   	  const OUString& rLName );
	virtual ~SwXMLBodyContext_Impl();

	virtual SvXMLImportContext *CreateChildContext(
			sal_uInt16 nPrefix, const OUString& rLocalName,
			const Reference< xml::sax::XAttributeList > & xAttrList );

	virtual void EndElement();
};

SwXMLBodyContext_Impl::SwXMLBodyContext_Impl( SwXMLImport& rImport,
											  sal_uInt16 nPrfx,
				   	  						  const OUString& rLName ) :
	SvXMLImportContext( rImport, nPrfx, rLName )
{
}

SwXMLBodyContext_Impl::~SwXMLBodyContext_Impl()
{
}

SvXMLImportContext *SwXMLBodyContext_Impl::CreateChildContext(
		sal_uInt16 nPrefix, const OUString& rLocalName,
		const Reference< xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;

	pContext = GetSwImport().GetTextImport()->CreateTextChildContext(
			GetImport(), nPrefix, rLocalName, xAttrList,
		   	XML_TEXT_TYPE_BODY );
	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

	return pContext;
}

void SwXMLBodyContext_Impl::EndElement()
{
    /* #108146# Code moved to SwXMLOmport::endDocument */
	GetImport().GetTextImport()->SetOutlineStyles();
}

SvXMLImportContext *SwXMLImport::CreateBodyContext(
									   const OUString& rLocalName )
{
	SvXMLImportContext *pContext = 0;

	if( !IsStylesOnlyMode() )
 		pContext = new SwXMLBodyContext_Impl( *this, XML_NAMESPACE_OFFICE,
											  rLocalName );
	else
		pContext = new SvXMLImportContext( *this, XML_NAMESPACE_OFFICE,
										   rLocalName );

	return pContext;
}
}
