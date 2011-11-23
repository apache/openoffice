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
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTSSUPPLIER_HPP
#include <com/sun/star/document/XEventsSupplier.hpp>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLEVENTSIMPORTCONTEXT_HXX
#include "XMLEventsImportContext.hxx"
#endif
#ifndef _XMLOFF_XMLSHAPEPROPERTYSETCONTEXT_HXX
#include "XMLShapePropertySetContext.hxx"
#endif
#ifndef _XMLTEXTCOLUMNSCONTEXT_HXX
#include "XMLTextColumnsContext.hxx"
#endif
#ifndef _XMLBACKGROUNDIMAGECONTEXT_HXX
#include "XMLBackgroundImageContext.hxx"
#endif
#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif

#ifndef _XMLOFF_XMLTEXTSHAPESTYLECONTEXT_HXX
#include "XMLTextShapeStyleContext.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::style;
using namespace ::com::sun::star::beans;
using namespace ::binfilter::xmloff::token;

class XMLTextShapePropertySetContext_Impl : public XMLShapePropertySetContext
{
public:
	XMLTextShapePropertySetContext_Impl( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const OUString& rLName,
		const Reference< XAttributeList >& xAttrList,
		::std::vector< XMLPropertyState > &rProps,
		const UniReference < SvXMLImportPropertyMapper > &rMap );

	virtual ~XMLTextShapePropertySetContext_Impl();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
		const OUString& rLocalName,
		const Reference< XAttributeList >& xAttrList,
	    ::std::vector< XMLPropertyState > &rProperties,
	    const XMLPropertyState& rProp);
};

XMLTextShapePropertySetContext_Impl::XMLTextShapePropertySetContext_Impl(
				 SvXMLImport& rImport, sal_uInt16 nPrfx,
				 const OUString& rLName,
				 const Reference< XAttributeList > & xAttrList,
				 ::std::vector< XMLPropertyState > &rProps,
				 const UniReference < SvXMLImportPropertyMapper > &rMap ) :
	XMLShapePropertySetContext( rImport, nPrfx, rLName, xAttrList, rProps,
								rMap )
{
}

XMLTextShapePropertySetContext_Impl::~XMLTextShapePropertySetContext_Impl()
{
}

SvXMLImportContext *XMLTextShapePropertySetContext_Impl::CreateChildContext(
				   sal_uInt16 nPrefix,
				   const OUString& rLocalName,
				   const Reference< XAttributeList > & xAttrList,
				   ::std::vector< XMLPropertyState > &rProperties,
				   const XMLPropertyState& rProp )
{
	SvXMLImportContext *pContext = 0;

	switch( xMapper->getPropertySetMapper()
					->GetEntryContextId( rProp.mnIndex ) )
	{
	case CTF_TEXTCOLUMNS:
		pContext = new XMLTextColumnsContext( GetImport(), nPrefix,
												   rLocalName, xAttrList, rProp,
												   rProperties );
		break;

	case CTF_BACKGROUND_URL:
		DBG_ASSERT( rProp.mnIndex >= 3 &&
					CTF_BACKGROUND_TRANSPARENCY == 
                        xMapper->getPropertySetMapper()
						->GetEntryContextId( rProp.mnIndex-3 ) &&
					CTF_BACKGROUND_POS  == xMapper->getPropertySetMapper()
						->GetEntryContextId( rProp.mnIndex-2 ) &&
					CTF_BACKGROUND_FILTER  == xMapper->getPropertySetMapper()
						->GetEntryContextId( rProp.mnIndex-1 ),
					"invalid property map!");
		pContext = 
			new XMLBackgroundImageContext( GetImport(), nPrefix,
										   rLocalName, xAttrList,
										   rProp,
										   rProp.mnIndex-2,
										   rProp.mnIndex-1,
                                           rProp.mnIndex-3,
										   rProperties );
		break;
	}
	
	if( !pContext )
		pContext = XMLShapePropertySetContext::CreateChildContext(
						nPrefix, rLocalName, xAttrList, rProperties, rProp );
	
	return pContext;
}

//----------------------------------------------------------------------------- 

void XMLTextShapeStyleContext::SetAttribute( sal_uInt16 nPrefixKey,
										const OUString& rLocalName,
										const OUString& rValue )
{
	if( XML_NAMESPACE_STYLE == nPrefixKey &&
		IsXMLToken( rLocalName, XML_AUTO_UPDATE ) )
	{
	  	if( IsXMLToken( rValue, XML_TRUE ) )
			bAutoUpdate = sal_True;
	}
	else
	{
		XMLShapeStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
	}
}

TYPEINIT1( XMLTextShapeStyleContext, XMLShapeStyleContext );

XMLTextShapeStyleContext::XMLTextShapeStyleContext( SvXMLImport& rImport,
		sal_uInt16 nPrfx, const OUString& rLName,
		const Reference< XAttributeList > & xAttrList,
		SvXMLStylesContext& rStyles, sal_uInt16 nFamily,
		sal_Bool bDefaultStyle ) :
	XMLShapeStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles,
					      nFamily ),
	bAutoUpdate( sal_False ),
	sIsAutoUpdate( RTL_CONSTASCII_USTRINGPARAM( "IsAutoUpdate" ) )
{
}

XMLTextShapeStyleContext::~XMLTextShapeStyleContext()
{
}

SvXMLImportContext *XMLTextShapeStyleContext::CreateChildContext(
		sal_uInt16 nPrefix,
		const OUString& rLocalName,
		const Reference< XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;

	if( XML_NAMESPACE_STYLE == nPrefix &&
		IsXMLToken( rLocalName, XML_PROPERTIES ) )
	{
		UniReference < SvXMLImportPropertyMapper > xImpPrMap =
			GetStyles()->GetImportPropertyMapper( GetFamily() );
		if( xImpPrMap.is() )
		{		
			pContext = new XMLTextShapePropertySetContext_Impl(
					GetImport(), nPrefix, rLocalName, xAttrList,
					GetProperties(), xImpPrMap );
		}
	}
	else if ( (XML_NAMESPACE_OFFICE == nPrefix) &&
			  IsXMLToken( rLocalName, XML_EVENTS ) )
	{
		// create and remember events import context 
		// (for delayed processing of events)
		pContext = new XMLEventsImportContext( GetImport(), nPrefix,
												   rLocalName);
		xEventContext = pContext;
	}
		
	if( !pContext )
		pContext = XMLShapeStyleContext::CreateChildContext( nPrefix, rLocalName,
														  xAttrList );

	return pContext;
}

void XMLTextShapeStyleContext::CreateAndInsert( sal_Bool bOverwrite )
{
	XMLShapeStyleContext::CreateAndInsert( bOverwrite );
	Reference < XStyle > xStyle = GetStyle();
	if( !xStyle.is() || !(bOverwrite || IsNew()) )
		return;
	
	Reference < XPropertySet > xPropSet( xStyle, UNO_QUERY );
	Reference< XPropertySetInfo > xPropSetInfo =
				xPropSet->getPropertySetInfo();
	if( xPropSetInfo->hasPropertyByName( sIsAutoUpdate ) )
	{
		Any aAny;
		sal_Bool bTmp = bAutoUpdate;
		aAny.setValue( &bTmp, ::getBooleanCppuType() );
		xPropSet->setPropertyValue( sIsAutoUpdate, aAny );
	}

	// tell the style about it's events (if applicable)
	if( xEventContext.Is() )
	{
		// set event suppplier and release reference to context
		Reference<XEventsSupplier> xEventsSupplier(xStyle, UNO_QUERY);
		((XMLEventsImportContext *)&xEventContext)->SetEvents(xEventsSupplier);
		xEventContext = 0;
	}
}


void XMLTextShapeStyleContext::Finish( sal_Bool bOverwrite )
{
    XMLPropStyleContext::Finish( bOverwrite );
}
}//end of namespace binfilter
