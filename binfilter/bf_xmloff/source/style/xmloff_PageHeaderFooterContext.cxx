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

#ifndef _XMLOFF_PAGEPHEADERFOOTERCONTEXT_HXX
#include "PageHeaderFooterContext.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_PAGEPROPERTYSETCONTEXT_HXX
#include "PagePropertySetContext.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_PROPERTIES;

//------------------------------------------------------------------

PageHeaderFooterContext::PageHeaderFooterContext( SvXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
									  ::std::vector< XMLPropertyState > & rTempProperties,
									  const UniReference < SvXMLImportPropertyMapper > &rTempMap,
									  sal_Int32 nStart, sal_Int32 nEnd,
									  const sal_Bool bTempHeader ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	rProperties(rTempProperties),
	rMap(rTempMap),
	nStartIndex(nStart),
	nEndIndex(nEnd)
{
	bHeader = bTempHeader;
}

PageHeaderFooterContext::~PageHeaderFooterContext()
{
}

SvXMLImportContext *PageHeaderFooterContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = 0;

	if( XML_NAMESPACE_STYLE == nPrefix && IsXMLToken( rLName, XML_PROPERTIES ) )
	{
		PageContextType aType = Header;
		if (!bHeader)
			aType = Footer;
		pContext = new PagePropertySetContext( GetImport(), nPrefix,
												rLName, xAttrList,
												rProperties,
												rMap,  nStartIndex, nEndIndex, aType);
	}


	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void PageHeaderFooterContext::EndElement()
{
}

}//end of namespace binfilter
