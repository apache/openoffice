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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"
#include "XMLChartStyleContext.hxx"
#include <xmloff/xmltoken.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmlnumfi.hxx>
#include <xmloff/families.hxx>

#include "XMLChartPropertyContext.hxx"

using namespace com::sun::star;
using ::xmloff::token::IsXMLToken;
using ::xmloff::token::XML_DATA_STYLE_NAME;
using ::xmloff::token::XML_PERCENTAGE_DATA_STYLE_NAME;
using ::xmloff::token::XML_TEXT_PROPERTIES;
using ::xmloff::token::XML_PARAGRAPH_PROPERTIES;
using ::xmloff::token::XML_GRAPHIC_PROPERTIES;
using ::xmloff::token::XML_CHART_PROPERTIES;


TYPEINIT1( XMLChartStyleContext, XMLPropStyleContext );

// protected

void XMLChartStyleContext::SetAttribute(
	sal_uInt16 nPrefixKey,
	const ::rtl::OUString& rLocalName,
	const ::rtl::OUString& rValue )
{
	if( IsXMLToken( rLocalName, XML_DATA_STYLE_NAME ) )
	{
		msDataStyleName =rValue;
	}
    else if( IsXMLToken( rLocalName, XML_PERCENTAGE_DATA_STYLE_NAME ) )
	{
        msPercentageDataStyleName =rValue;
    }
	else
	{
		XMLShapeStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
	}
}

//public

// CTOR
XMLChartStyleContext::XMLChartStyleContext(
	SvXMLImport& rImport, sal_uInt16 nPrfx,
	const ::rtl::OUString& rLName,
	const uno::Reference< xml::sax::XAttributeList > & xAttrList,
	SvXMLStylesContext& rStyles, sal_uInt16 nFamily ) :

        XMLShapeStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, nFamily ),
		mrStyles( rStyles )
{}

// DTOR
XMLChartStyleContext::~XMLChartStyleContext()
{}

namespace
{

    void lcl_NumberFormatStyleToProperty( const ::rtl::OUString& rStyleName, const ::rtl::OUString& rPropertyName,
                                      const SvXMLStylesContext& rStylesContext,
                                      const uno::Reference< beans::XPropertySet >& rPropSet )
{
    if( rStyleName.getLength())
	{
		SvXMLNumFormatContext* pStyle = (SvXMLNumFormatContext *)rStylesContext.FindStyleChildContext(
			XML_STYLE_FAMILY_DATA_STYLE, rStyleName, sal_True );
		if( pStyle )
		{
			uno::Any aNumberFormat;
			sal_Int32 nNumberFormat = pStyle->GetKey();
			aNumberFormat <<= nNumberFormat;
			rPropSet->setPropertyValue( rPropertyName, aNumberFormat );
		}
	}
}

}// anonymous namespace

void XMLChartStyleContext::FillPropertySet(
	const uno::Reference< beans::XPropertySet > & rPropSet )
{
    try
    {
	    XMLShapeStyleContext::FillPropertySet( rPropSet );
    }
    catch( beans::UnknownPropertyException&  )
    {
        DBG_ASSERT( false, "unknown property exception -> shape style not completely imported for chart style" );
    }

    lcl_NumberFormatStyleToProperty( msDataStyleName, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "NumberFormat" )), mrStyles, rPropSet );
    lcl_NumberFormatStyleToProperty( msPercentageDataStyleName, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PercentageNumberFormat" )), mrStyles, rPropSet );
}

SvXMLImportContext *XMLChartStyleContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const ::rtl::OUString& rLocalName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext* pContext = NULL;

	if( XML_NAMESPACE_STYLE == nPrefix )
	{
		sal_uInt32 nFamily = 0;
		if( IsXMLToken( rLocalName, XML_TEXT_PROPERTIES ) )
			nFamily = XML_TYPE_PROP_TEXT;
		else if( IsXMLToken( rLocalName, XML_PARAGRAPH_PROPERTIES ) )
			nFamily = XML_TYPE_PROP_PARAGRAPH;
		else if( IsXMLToken( rLocalName, XML_GRAPHIC_PROPERTIES ) )
			nFamily = XML_TYPE_PROP_GRAPHIC;
		else if( IsXMLToken( rLocalName, XML_CHART_PROPERTIES ) )
			nFamily = XML_TYPE_PROP_CHART;
		if( nFamily )
		{
			UniReference < SvXMLImportPropertyMapper > xImpPrMap =
				GetStyles()->GetImportPropertyMapper( GetFamily() );
			if( xImpPrMap.is() )
				pContext = new XMLChartPropertyContext(
					GetImport(), nPrefix, rLocalName, xAttrList, nFamily,
					GetProperties(), xImpPrMap );
		}
	}

	if( !pContext )
		pContext = XMLShapeStyleContext::CreateChildContext( nPrefix, rLocalName,
                                                             xAttrList );

	return pContext;
}
