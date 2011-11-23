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

//___________________________________________________________________

#ifndef SC_XMLLABRI_HXX
#include "xmllabri.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_XLABELRANGES_HPP_
#include <com/sun/star/sheet/XLabelRanges.hpp>
#endif

#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif
#ifndef SC_UNONAMES_HXX
#include "unonames.hxx"
#endif
#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::rtl;
using namespace xmloff::token;


//___________________________________________________________________

ScXMLLabelRangesContext::ScXMLLabelRangesContext(
		ScXMLImport& rImport,
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList ):
	SvXMLImportContext( rImport, nPrefix, rLName )
{
	rImport.LockSolarMutex();
}

ScXMLLabelRangesContext::~ScXMLLabelRangesContext()
{
	GetScImport().UnlockSolarMutex();
}

SvXMLImportContext* ScXMLLabelRangesContext::CreateChildContext(
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	SvXMLImportContext*		pContext	= NULL;
	const SvXMLTokenMap&	rTokenMap	= GetScImport().GetLabelRangesElemTokenMap();

	switch( rTokenMap.Get( nPrefix, rLName ) )
	{
		case XML_TOK_LABEL_RANGE_ELEM:
			pContext = new ScXMLLabelRangeContext( GetScImport(), nPrefix, rLName, xAttrList );
		break;
	}
	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void ScXMLLabelRangesContext::EndElement()
{
}


//___________________________________________________________________

ScXMLLabelRangeContext::ScXMLLabelRangeContext(
		ScXMLImport& rImport,
		USHORT nPrfx,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	bColumnOrientation( sal_False )
{
	sal_Int16				nAttrCount		= xAttrList.is() ? xAttrList->getLength() : 0;
	const SvXMLTokenMap&	rAttrTokenMap	= GetScImport().GetLabelRangeAttrTokenMap();

	for( sal_Int16 nIndex = 0; nIndex < nAttrCount; nIndex++ )
	{
		OUString	sAttrName	= xAttrList->getNameByIndex( nIndex );
		OUString	sValue		= xAttrList->getValueByIndex( nIndex );
		OUString	aLocalName;
		USHORT		nPrefix		= GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

		switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
		{
			case XML_TOK_LABEL_RANGE_ATTR_LABEL_RANGE:
				sLabelRangeStr = sValue;
			break;
			case XML_TOK_LABEL_RANGE_ATTR_DATA_RANGE:
				sDataRangeStr = sValue;
			break;
			case XML_TOK_LABEL_RANGE_ATTR_ORIENTATION:
				bColumnOrientation = IsXMLToken(sValue, XML_COLUMN );
			break;
		}
	}
}

ScXMLLabelRangeContext::~ScXMLLabelRangeContext()
{
}

SvXMLImportContext* ScXMLLabelRangeContext::CreateChildContext(
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLLabelRangeContext::EndElement()
{
	if (GetScImport().GetModel().is())
	{
		uno::Reference< beans::XPropertySet > xPropSet( GetScImport().GetModel(), uno::UNO_QUERY );
		if( xPropSet.is() )
		{
			uno::Any aAny = xPropSet->getPropertyValue( bColumnOrientation ?
				OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_COLLABELRNG ) ) :
				OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_ROWLABELRNG ) ) );
			uno::Reference< sheet::XLabelRanges > xLabelRanges;
			if( aAny >>= xLabelRanges )
			{
				table::CellRangeAddress aLabelRange;
				table::CellRangeAddress aDataRange;
				sal_Int32 nOffset1(0);
				sal_Int32 nOffset2(0);
				if (ScXMLConverter::GetRangeFromString( aLabelRange, sLabelRangeStr, GetScImport().GetDocument(), nOffset1 ) &&
					ScXMLConverter::GetRangeFromString( aDataRange, sDataRangeStr, GetScImport().GetDocument(), nOffset2 ))
					xLabelRanges->addNew( aLabelRange, aDataRange );
			}
		}
	}
}

}
