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

#ifndef _SC_XMLCELLRANGESOURCECONTEXT_HXX
#include "XMLCellRangeSourceContext.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif

#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;


//___________________________________________________________________

ScMyImpCellRangeSource::ScMyImpCellRangeSource() :
	nColumns( 0 ),
	nRows( 0 ),
	nRefresh( 0 )
{
}


//___________________________________________________________________

ScXMLCellRangeSourceContext::ScXMLCellRangeSourceContext(
		ScXMLImport& rImport,
		USHORT nPrfx,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList,
		ScMyImpCellRangeSource* pCellRangeSource ) :
	SvXMLImportContext( rImport, nPrfx, rLName )
{
	if( !xAttrList.is() ) return;

	sal_Int16				nAttrCount		= xAttrList->getLength();
	const SvXMLTokenMap&	rAttrTokenMap	= GetScImport().GetTableCellRangeSourceAttrTokenMap();

	for( sal_Int16 nIndex = 0; nIndex < nAttrCount; nIndex++ )
	{
		OUString sAttrName	= xAttrList->getNameByIndex( nIndex );
		OUString sValue		= xAttrList->getValueByIndex( nIndex );
		OUString aLocalName;
		USHORT nPrefix		= GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

		switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
		{
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_NAME:
				pCellRangeSource->sSourceStr = sValue;
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_FILTER_NAME:
				pCellRangeSource->sFilterName = sValue;
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_FILTER_OPTIONS:
				pCellRangeSource->sFilterOptions = sValue;
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_HREF:
				pCellRangeSource->sURL = GetScImport().GetAbsoluteReference(sValue);
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_LAST_COLUMN:
			{
				sal_Int32 nValue;
				if( SvXMLUnitConverter::convertNumber( nValue, sValue, 1 ) )
					pCellRangeSource->nColumns = nValue;
				else
					pCellRangeSource->nColumns = 1;
			}
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_LAST_ROW:
			{
				sal_Int32 nValue;
				if( SvXMLUnitConverter::convertNumber( nValue, sValue, 1 ) )
					pCellRangeSource->nRows = nValue;
				else
					pCellRangeSource->nRows = 1;
			}
			break;
			case XML_TOK_TABLE_CELL_RANGE_SOURCE_ATTR_REFRESH_DELAY:
			{
				double fTime;
				if( SvXMLUnitConverter::convertTime( fTime, sValue ) )
					pCellRangeSource->nRefresh = Max( (sal_Int32)(fTime * 86400.0), (sal_Int32)0 );
			}
			break;
		}
	}
}

ScXMLCellRangeSourceContext::~ScXMLCellRangeSourceContext()
{
}

SvXMLImportContext *ScXMLCellRangeSourceContext::CreateChildContext(
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList>& xAttrList )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLCellRangeSourceContext::EndElement()
{
}

}
