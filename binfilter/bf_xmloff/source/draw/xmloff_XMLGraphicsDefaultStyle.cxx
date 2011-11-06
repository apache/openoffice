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


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif



#ifndef _XMLOFF_XMLSHAPEPROPERTYSETCONTEXT_HXX
#include "XMLShapePropertySetContext.hxx"
#endif

#ifndef _XMLOFF_XMLGRAPHICSDEFAULTSTYLE_HXX
#include "XMLGraphicsDefaultStyle.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::xml::sax;

using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_PROPERTIES;

// ---------------------------------------------------------------------

TYPEINIT1( XMLGraphicsDefaultStyle, XMLPropStyleContext );

XMLGraphicsDefaultStyle::XMLGraphicsDefaultStyle( SvXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName, const Reference< XAttributeList >& xAttrList, SvXMLStylesContext& rStyles )
: XMLPropStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, XML_STYLE_FAMILY_SD_GRAPHICS_ID, sal_True )
{
}

XMLGraphicsDefaultStyle::~XMLGraphicsDefaultStyle()
{
}

SvXMLImportContext *XMLGraphicsDefaultStyle::CreateChildContext( sal_uInt16 nPrefix, const OUString& rLocalName, const Reference< XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;

	if( XML_NAMESPACE_STYLE == nPrefix && IsXMLToken( rLocalName, XML_PROPERTIES ) )
	{
		UniReference < SvXMLImportPropertyMapper > xImpPrMap = GetStyles()->GetImportPropertyMapper( GetFamily() );
		if( xImpPrMap.is() )
			pContext = new XMLShapePropertySetContext( GetImport(), nPrefix, rLocalName, xAttrList,	GetProperties(), xImpPrMap );
	}
		
	if( !pContext )
		pContext = XMLPropStyleContext::CreateChildContext( nPrefix, rLocalName, xAttrList );

	return pContext;
}

// This method is called for every default style
void XMLGraphicsDefaultStyle::SetDefaults()
{
	Reference< XMultiServiceFactory > xFact( GetImport().GetModel(), UNO_QUERY );
	if( !xFact.is() )
		return;

	Reference< XPropertySet > xDefaults( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.Defaults") ) ), UNO_QUERY ); 
	if( !xDefaults.is() )
		return;

	FillPropertySet( xDefaults );
}


}//end of namespace binfilter
