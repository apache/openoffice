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


#include "XMLChartStyleContext.hxx"

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLNUMFI_HXX 
#include "xmlnumfi.hxx"
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include "families.hxx"
#endif

#include "XMLChartPropertyContext.hxx"
namespace binfilter {

using namespace ::com::sun::star;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_DATA_STYLE_NAME;


TYPEINIT1( XMLChartStyleContext, XMLPropStyleContext );

// protected

void XMLChartStyleContext::SetAttribute(
	sal_uInt16 nPrefixKey,
	const ::rtl::OUString& rLocalName,
	const ::rtl::OUString& rValue )
{
	if( IsXMLToken( rLocalName, XML_DATA_STYLE_NAME ) )
	{
		msDataStyleName = rValue;
	}
	else
	{
		XMLPropStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
	}
}

//public

// CTOR
XMLChartStyleContext::XMLChartStyleContext(
	SvXMLImport& rImport, sal_uInt16 nPrfx,
	const ::rtl::OUString& rLName,
	const uno::Reference< xml::sax::XAttributeList > & xAttrList,
	SvXMLStylesContext& rStyles, sal_uInt16 nFamily ) :

		XMLPropStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, nFamily ),
		mrStyles( rStyles )
{}

// DTOR
XMLChartStyleContext::~XMLChartStyleContext()
{}

void XMLChartStyleContext::FillPropertySet(
	const uno::Reference< beans::XPropertySet > & rPropSet )
{
	XMLPropStyleContext::FillPropertySet( rPropSet );
	if( msDataStyleName.getLength())
	{
		SvXMLNumFormatContext* pStyle = (SvXMLNumFormatContext *)mrStyles.FindStyleChildContext(
			XML_STYLE_FAMILY_DATA_STYLE, msDataStyleName, sal_True );
		if( pStyle )
		{
			uno::Any aNumberFormat;
			sal_Int32 nNumberFormat = pStyle->GetKey();
			aNumberFormat <<= nNumberFormat;
			rPropSet->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "NumberFormat" )),
										aNumberFormat );
		}
	}
}

SvXMLImportContext *XMLChartStyleContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const ::rtl::OUString& rLocalName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext* pContext = NULL;

	if( XML_NAMESPACE_STYLE == nPrefix &&
		IsXMLToken( rLocalName, ::binfilter::xmloff::token::XML_PROPERTIES ) )
	{
		UniReference < SvXMLImportPropertyMapper > xImpPrMap =
			GetStyles()->GetImportPropertyMapper( GetFamily() );
		if( xImpPrMap.is() )
            pContext = new XMLChartPropertyContext(
                GetImport(), nPrefix, rLocalName, xAttrList,
                GetProperties(), xImpPrMap );
	}

	if( !pContext )
		pContext = XMLPropStyleContext::CreateChildContext( nPrefix, rLocalName,
														  xAttrList );

	return pContext;
}
}//end of namespace binfilter
