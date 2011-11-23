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

#ifndef _XMLOFF_XMLIMP_HXX 
#include "xmlimp.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX 
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX 
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_TXTPARAI_HXX 
#include "txtparai.hxx"
#endif
#ifndef _XMLTEXTLISTBLOCKCONTEXT_HXX
#include "XMLTextListBlockContext.hxx"
#endif


#ifndef _XMLTEXTLISTITEMCONTEXT_HXX
#include "XMLTextListItemContext.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

TYPEINIT1( XMLTextListItemContext, SvXMLImportContext );

XMLTextListItemContext::XMLTextListItemContext(
		SvXMLImport& rImport,
		XMLTextImportHelper& rTxtImp, sal_uInt16 nPrfx,
		const OUString& rLName,
		const Reference< xml::sax::XAttributeList > & xAttrList,
		sal_Bool bIsHeader ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	rTxtImport( rTxtImp ),
	nStartValue( -1 )
{
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		const OUString& rAttrName = xAttrList->getNameByIndex( i );
		const OUString& rValue = xAttrList->getValueByIndex( i );

		OUString aLocalName;
		sal_uInt16 nPrefix =
			GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
															&aLocalName );
		if( !bIsHeader && XML_NAMESPACE_TEXT == nPrefix &&
			IsXMLToken( aLocalName, XML_START_VALUE ) )
		{
			sal_Int32 nTmp = rValue.toInt32();
			if( nTmp >= 0 && nTmp <= SHRT_MAX )
				nStartValue = (sal_Int16)nTmp;
		}
	}

	DBG_ASSERT( !rTxtImport.GetListItem(),
		"SwXMLListItemContext::SwXMLListItemContext: list item is existing" );

	// If this is a <text:list-item> element, then remember it as a sign
	// that a bullet has to be generated.
	if( !bIsHeader )
		rTxtImport.SetListItem( this );
}

XMLTextListItemContext::~XMLTextListItemContext()
{
}

void XMLTextListItemContext::EndElement()
{
	// finish current list item
	rTxtImport.SetListItem( 0 );
}

SvXMLImportContext *XMLTextListItemContext::CreateChildContext(
		sal_uInt16 nPrefix,
		const OUString& rLocalName,
		const Reference< xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;

	const SvXMLTokenMap& rTokenMap = rTxtImport.GetTextElemTokenMap();
	sal_Bool bOrdered = sal_False;
	sal_Bool bHeading = sal_False;
	switch( rTokenMap.Get( nPrefix, rLocalName ) )
	{
	case XML_TOK_TEXT_H:
		bHeading = sal_True;
	case XML_TOK_TEXT_P:
		pContext = new XMLParaContext( GetImport(),
									   nPrefix, rLocalName,
									   xAttrList, bHeading );
        if (rTxtImport.IsProgress())
            GetImport().GetProgressBarHelper()->Increment();

		break;
	case XML_TOK_TEXT_ORDERED_LIST:
		bOrdered = sal_True;
	case XML_TOK_TEXT_UNORDERED_LIST:
		pContext = new XMLTextListBlockContext( GetImport(), rTxtImport,
										    nPrefix, rLocalName,
											xAttrList, bOrdered );
		break;
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

	return pContext;
}


}//end of namespace binfilter
