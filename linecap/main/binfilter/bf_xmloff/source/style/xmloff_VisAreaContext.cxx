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

#ifndef _XMLOFF_VISAREACONTEXT_HXX
#include "VisAreaContext.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

//------------------------------------------------------------------

XMLVisAreaContext::XMLVisAreaContext( SvXMLImport& rImport,
										 USHORT nPrfx,
				   	  						  const ::rtl::OUString& rLName,
									  		const uno::Reference<xml::sax::XAttributeList>& xAttrList,
											::com::sun::star::awt::Rectangle& rRect, const sal_Int16 nMeasureUnit ) :
	SvXMLImportContext( rImport, nPrfx, rLName )
{
	process( xAttrList, rRect, nMeasureUnit );
}

XMLVisAreaContext::~XMLVisAreaContext()
{
}

void XMLVisAreaContext::process( const uno::Reference< xml::sax::XAttributeList>& xAttrList, awt::Rectangle& rRect, const sal_Int16 nMeasureUnit )
{
	MapUnit aMapUnit = (MapUnit)nMeasureUnit;

	sal_Int32 nX(0);
	sal_Int32 nY(0);
	sal_Int32 nWidth(0);
	sal_Int32 nHeight(0);
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
		::rtl::OUString aLocalName;
		USHORT nPrefix = GetImport().GetNamespaceMap().GetKeyByAttrName(
											sAttrName, &aLocalName );
		::rtl::OUString sValue = xAttrList->getValueByIndex( i );

		if (nPrefix == XML_NAMESPACE_OFFICE)
		{
			if (IsXMLToken( aLocalName, XML_X ))
			{
				SvXMLUnitConverter::convertMeasure(nX, sValue, aMapUnit);
				rRect.X = nX;
			}
			else if (IsXMLToken( aLocalName, XML_Y ))
			{
				SvXMLUnitConverter::convertMeasure(nY, sValue, aMapUnit);
				rRect.Y = nY;
			}
			else if (IsXMLToken( aLocalName, XML_WIDTH ))
			{
				SvXMLUnitConverter::convertMeasure(nWidth, sValue, aMapUnit);
				rRect.Width = nWidth;
			}
			else if (IsXMLToken( aLocalName, XML_HEIGHT ))
			{
				SvXMLUnitConverter::convertMeasure(nHeight, sValue, aMapUnit);
				rRect.Height = nHeight;
			}
		}
	}
}

SvXMLImportContext *XMLVisAreaContext::CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	// here is no context
	SvXMLImportContext *pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

	return pContext;
}

void XMLVisAreaContext::EndElement()
{
}
}//end of namespace binfilter
