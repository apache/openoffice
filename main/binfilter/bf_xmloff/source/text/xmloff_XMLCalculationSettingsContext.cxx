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



#ifndef _XMLOFF_XMLCALCULATIONSETTINGSCONTEXT_HXX
#include <XMLCalculationSettingsContext.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
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
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
namespace binfilter {


using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::text;
using namespace ::binfilter::xmloff::token;

XMLCalculationSettingsContext::XMLCalculationSettingsContext( SvXMLImport& rImport, 
									sal_uInt16 nPrefix,
									const ::rtl::OUString& rLocalName,
				 					const ::com::sun::star::uno::Reference<
				 						::com::sun::star::xml::sax::XAttributeList >& xAttrList )
: nYear( 1930 )
, SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
		::rtl::OUString aLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().GetKeyByAttrName(
											sAttrName, &aLocalName );
		::rtl::OUString sValue = xAttrList->getValueByIndex( i );

		if (nPrefix == XML_NAMESPACE_TABLE)
		{
			if ( IsXMLToken( aLocalName, XML_NULL_YEAR ) )
			{
				sal_Int32 nTemp;
				GetImport().GetMM100UnitConverter().convertNumber(nTemp, sValue);
				nYear= static_cast <sal_Int16> (nTemp);
			}
		}
	}
}

XMLCalculationSettingsContext::~XMLCalculationSettingsContext()
{
}
void XMLCalculationSettingsContext::EndElement()
{
	if (nYear != 1930 )
	{
		Reference < XTextDocument > xTextDoc ( GetImport().GetModel(), UNO_QUERY);
		if (xTextDoc.is())
		{
			Reference < XPropertySet > xPropSet ( xTextDoc, UNO_QUERY );
			OUString sTwoDigitYear ( RTL_CONSTASCII_USTRINGPARAM ( "TwoDigitYear" ) );
			Any aAny;
			aAny <<= nYear;
			xPropSet->setPropertyValue ( sTwoDigitYear, aAny );
		}
	}
}
}//end of namespace binfilter
