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


#include "XMLChartPropertyContext.hxx"
#include "PropertyMap.hxx"

#ifndef _XMLOFF_SYMBOLIMAGECONTEXT_HXX_
#include "XMLSymbolImageContext.hxx"
#endif
namespace binfilter {

TYPEINIT1( XMLChartPropertyContext, SvXMLImportContext );

using namespace ::com::sun::star;

XMLChartPropertyContext::XMLChartPropertyContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrfx,
    const ::rtl::OUString& rLName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList,
    ::std::vector< XMLPropertyState >& rProps,
    const UniReference< SvXMLImportPropertyMapper >& rMapper ) :
        SvXMLPropertySetContext( rImport, nPrfx, rLName, xAttrList, rProps, rMapper )
{
}

XMLChartPropertyContext::~XMLChartPropertyContext()
{}

SvXMLImportContext* XMLChartPropertyContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const ::rtl::OUString& rLocalName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList,
    ::std::vector< XMLPropertyState > &rProperties,
    const XMLPropertyState& rProp )
{
    SvXMLImportContext *pContext = 0;

	switch( xMapper->getPropertySetMapper()->GetEntryContextId( rProp.mnIndex ) )
	{
        case XML_SCH_CONTEXT_SPECIAL_SYMBOL_IMAGE:
            pContext = new XMLSymbolImageContext( GetImport(), nPrefix, rLocalName, rProp, rProperties );
            break;
    }

    // default / no context yet: create child context by base class
	if( !pContext )
    {
        pContext = SvXMLPropertySetContext::CreateChildContext(
            nPrefix, rLocalName, xAttrList, rProperties, rProp );
    }
	
	return pContext;
}
}//end of namespace binfilter
