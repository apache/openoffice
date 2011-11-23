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



#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLMETAI_HXX
#include "xmlmetai.hxx"
#endif
#ifndef _XMLOFF_XMLSTYLE_HXX
#include "xmlstyle.hxx"
#endif

//  #ifndef _XMLOFF_XMLCHARTSTYLECONTEXT_HXX_
//  #include "XMLChartStyleContext.hxx"
//  #endif


#include "contexts.hxx"
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

// ==================================================

SchXMLDocContext::SchXMLDocContext( SchXMLImportHelper& rImpHelper,
									SvXMLImport& rImport,
									USHORT nPrefix,
									const ::rtl::OUString& rLName ) :
		SvXMLImportContext( rImport, nPrefix, rLName ),
		mrImportHelper( rImpHelper )
{
	DBG_ASSERT( XML_NAMESPACE_OFFICE == nPrefix &&
		( IsXMLToken( rLName, XML_DOCUMENT ) ||
		  IsXMLToken( rLName, XML_DOCUMENT_META) ||
		  IsXMLToken( rLName, XML_DOCUMENT_STYLES) ||
		  IsXMLToken( rLName, XML_DOCUMENT_CONTENT) ),
				"SchXMLDocContext instanciated with no <office:document> element" );
}

SchXMLDocContext::~SchXMLDocContext()
{}

SvXMLImportContext* SchXMLDocContext::CreateChildContext(
	sal_uInt16 nPrefix,
	const ::rtl::OUString& rLocalName,
	const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	SvXMLImportContext* pContext = 0;
	const SvXMLTokenMap& rTokenMap = mrImportHelper.GetDocElemTokenMap();
	sal_uInt16 nFlags = GetImport().getImportFlags();

	switch( rTokenMap.Get( nPrefix, rLocalName ))
	{
		case XML_TOK_DOC_AUTOSTYLES:
			if( nFlags & IMPORT_AUTOSTYLES )
                // not nice, but this is safe, as the SchXMLDocContext class can only by
                // instantiated by the chart import class SchXMLImport (header is not exported)
                pContext =
                    static_cast< SchXMLImport& >( GetImport() ).CreateStylesContext( rLocalName, xAttrList );
			break;
		case XML_TOK_DOC_STYLES:
			// for draw styles containing gradients/hatches/markers and dashes
			if( nFlags & IMPORT_STYLES )
				pContext = new SvXMLStylesContext( GetImport(), nPrefix, rLocalName, xAttrList );
			break;
		case XML_TOK_DOC_META:
			if( nFlags & IMPORT_META )
				pContext = new SfxXMLMetaContext( GetImport(), nPrefix, rLocalName, GetImport().GetModel());
			break;
		case XML_TOK_DOC_BODY:
			if( nFlags & IMPORT_CONTENT )
				pContext = new SchXMLBodyContext( mrImportHelper, GetImport(), nPrefix, rLocalName );
			break;
	}

	// call parent when no own context was created
	if( ! pContext )
		pContext = SvXMLImportContext::CreateChildContext( nPrefix, rLocalName, xAttrList );

	return pContext;	
}

// ----------------------------------------

SchXMLBodyContext::SchXMLBodyContext( SchXMLImportHelper& rImpHelper,
									  SvXMLImport& rImport,
									  USHORT nPrefix,
									  const ::rtl::OUString& rLName ) :
		SvXMLImportContext( rImport, nPrefix, rLName ),
		mrImportHelper( rImpHelper )
{
	DBG_ASSERT( XML_NAMESPACE_OFFICE == nPrefix &&
				IsXMLToken( rLName, XML_BODY ),
				"SchXMLBodyContext instanciated with no <office:body> element" );
}

SchXMLBodyContext::~SchXMLBodyContext()
{}

void SchXMLBodyContext::EndElement()
{
}

SvXMLImportContext* SchXMLBodyContext::CreateChildContext(
	sal_uInt16 nPrefix,
	const ::rtl::OUString& rLocalName,
	const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	SvXMLImportContext* pContext = 0;

	// <chart:chart> element
	if( nPrefix == XML_NAMESPACE_CHART &&
		IsXMLToken( rLocalName, XML_CHART ) )
	{
		pContext = mrImportHelper.CreateChartContext( GetImport(),
													  nPrefix, rLocalName,
													  GetImport().GetModel(),
													  xAttrList );
	}
	else
	{
		pContext = SvXMLImportContext::CreateChildContext( nPrefix, rLocalName, xAttrList );
	}

	return pContext;	
}


}//end of namespace binfilter
