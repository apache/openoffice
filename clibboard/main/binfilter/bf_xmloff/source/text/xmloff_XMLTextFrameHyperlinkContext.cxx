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
#ifndef _XMLTEXTFRAMECONTEXT_HXX
#include "XMLTextFrameContext.hxx"
#endif
#ifndef _XMLTEXTFRAMEHYPERLINKCONTEXT_HXX
#include "XMLTextFrameHyperlinkContext.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::beans;
using namespace ::binfilter::xmloff::token;

TYPEINIT1( XMLTextFrameHyperlinkContext, SvXMLImportContext );

XMLTextFrameHyperlinkContext::XMLTextFrameHyperlinkContext(
		SvXMLImport& rImport,
		sal_uInt16 nPrfx, const OUString& rLName,
		const Reference< XAttributeList > & xAttrList,
		TextContentAnchorType eATyp,
	   	Reference < XTextContent> *pTxtCntnt,
		TextContentAnchorType *pAnchrType ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	eAnchorType( eATyp ),
	pTextContent( pTxtCntnt ),
	pAnchorType( pAnchrType ),
	bMap( sal_False )
{
	OUString sShow;
	const SvXMLTokenMap& rTokenMap =
		GetImport().GetTextImport()->GetTextHyperlinkAttrTokenMap();

	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		const OUString& rAttrName = xAttrList->getNameByIndex( i );
		const OUString& rValue = xAttrList->getValueByIndex( i );

		OUString aLocalName;
		sal_uInt16 nPrefix =
			GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
															&aLocalName );
		switch( rTokenMap.Get( nPrefix, aLocalName ) )
		{
		case XML_TOK_TEXT_HYPERLINK_HREF:
			sHRef = GetImport().GetAbsoluteReference( rValue );
			break;
		case XML_TOK_TEXT_HYPERLINK_NAME:
			sName = rValue;
			break;
		case XML_TOK_TEXT_HYPERLINK_TARGET_FRAME:
			sTargetFrameName = rValue;
			break;
		case XML_TOK_TEXT_HYPERLINK_SHOW:
			sShow = rValue;
			break;
		case XML_TOK_TEXT_HYPERLINK_SERVER_MAP:
			{
				sal_Bool bTmp;
				if( rImport.GetMM100UnitConverter().convertBool( bTmp,
																  rValue ) )
				{
					bMap = bTmp;	
				}
			}
			break;
		}
	}

	if( sShow.getLength() && !sTargetFrameName.getLength() )
	{
		if( IsXMLToken( sShow, XML_NEW ) )
			sTargetFrameName =
					OUString( RTL_CONSTASCII_USTRINGPARAM("_blank" ) );
		else if( IsXMLToken( sShow, XML_REPLACE ) )
			sTargetFrameName =
					OUString( RTL_CONSTASCII_USTRINGPARAM("_self" ) );
	}
}

XMLTextFrameHyperlinkContext::~XMLTextFrameHyperlinkContext()
{
}

void XMLTextFrameHyperlinkContext::EndElement()
{
}

SvXMLImportContext *XMLTextFrameHyperlinkContext::CreateChildContext(
		sal_uInt16 nPrefix,
		const OUString& rLocalName,
		const Reference< XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;
	XMLTextFrameContext *pTextFrameContext = 0;

	if( XML_NAMESPACE_DRAW == nPrefix )
	{
		sal_uInt16 nFrameType = USHRT_MAX;
		if( IsXMLToken( rLocalName, XML_TEXT_BOX ) )
			nFrameType = XML_TEXT_FRAME_TEXTBOX;
		else if( IsXMLToken( rLocalName, XML_IMAGE ) )
			nFrameType = XML_TEXT_FRAME_GRAPHIC;
		else if( IsXMLToken( rLocalName, XML_OBJECT ) )
			nFrameType = XML_TEXT_FRAME_OBJECT;
		else if( IsXMLToken( rLocalName, XML_OBJECT_OLE ) )
			nFrameType = XML_TEXT_FRAME_OBJECT_OLE;
		else if( IsXMLToken( rLocalName, XML_APPLET) )
			nFrameType = XML_TEXT_FRAME_APPLET;
		else if( IsXMLToken( rLocalName, XML_PLUGIN ) )
			nFrameType = XML_TEXT_FRAME_PLUGIN;
		else if( IsXMLToken( rLocalName, XML_FLOATING_FRAME ) )
			nFrameType = XML_TEXT_FRAME_FLOATING_FRAME;

		if( USHRT_MAX != nFrameType )
			pTextFrameContext = new XMLTextFrameContext( GetImport(), nPrefix,
												rLocalName, xAttrList, 
												eAnchorType,
												nFrameType );
	}

	if( pTextFrameContext )
	{
		pTextFrameContext->SetHyperlink( sHRef, sName, sTargetFrameName, bMap );
		if( pAnchorType )
			*pAnchorType = pTextFrameContext->GetAnchorType();
		if( pTextContent )
			*pTextContent = pTextFrameContext->GetTextContent();
		pContext = pTextFrameContext;
	}
	else
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

	return pContext;
}



}//end of namespace binfilter
