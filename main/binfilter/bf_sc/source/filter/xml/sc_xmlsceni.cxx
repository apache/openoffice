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

#include "document.hxx"
#include "xmlimprt.hxx"
#include "xmlsceni.hxx"
#include "attrib.hxx"

#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif

#include <bf_xmloff/nmspmap.hxx>
#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLTableScenarioContext::ScXMLTableScenarioContext(
		ScXMLImport& rImport,
		USHORT nPrfx,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList ):
	SvXMLImportContext( rImport, nPrfx, rLName ),
	aBorderColor( COL_BLACK ),
	bDisplayBorder( sal_True ),
	bCopyBack( sal_True ),
	bCopyStyles( sal_True ),
	bCopyFormulas( sal_True ),
	bIsActive( sal_False )
{
	rImport.LockSolarMutex();
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetTableScenarioAttrTokenMap();
	for( sal_Int16 i = 0; i < nAttrCount; i++ )
	{
		OUString sAttrName = xAttrList->getNameByIndex( i );
		OUString aLocalName;
		USHORT nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
											sAttrName, &aLocalName );
		OUString sValue = xAttrList->getValueByIndex( i );

		switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
		{
			case XML_TOK_TABLE_SCENARIO_ATTR_DISPLAY_BORDER:
			{
				bDisplayBorder = IsXMLToken(sValue, XML_TRUE);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_BORDER_COLOR:
			{
				SvXMLUnitConverter::convertColor(aBorderColor, sValue);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_COPY_BACK:
			{
				bCopyBack = IsXMLToken(sValue, XML_TRUE);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_COPY_STYLES:
			{
				bCopyStyles = IsXMLToken(sValue, XML_TRUE);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_COPY_FORMULAS:
			{
				bCopyFormulas = IsXMLToken(sValue, XML_TRUE);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_IS_ACTIVE:
			{
				bIsActive = IsXMLToken(sValue, XML_TRUE);
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_SCENARIO_RANGES:
			{
				ScXMLConverter::GetRangeListFromString(
					aScenarioRanges, sValue, GetScImport().GetDocument() );
			}
			break;
			case XML_TOK_TABLE_SCENARIO_ATTR_COMMENT:
			{
				sComment = sValue;
			}
			break;
		}
	}
}

ScXMLTableScenarioContext::~ScXMLTableScenarioContext()
{
	GetScImport().UnlockSolarMutex();
}

SvXMLImportContext *ScXMLTableScenarioContext::CreateChildContext(
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLTableScenarioContext::EndElement()
{
	sal_Int16	nCurrTable( GetScImport().GetTables().GetCurrentSheet() );
	ScDocument*	pDoc = GetScImport().GetDocument();
	if (pDoc)
	{
		pDoc->SetScenario( nCurrTable, TRUE );
		USHORT nFlags( 0 );
		if( bDisplayBorder )
			nFlags |= SC_SCENARIO_SHOWFRAME;
		if( bCopyBack )
			nFlags |= SC_SCENARIO_TWOWAY;
		if( bCopyStyles )
			nFlags |= SC_SCENARIO_ATTRIB;
		if( !bCopyFormulas )
			nFlags |= SC_SCENARIO_VALUE;
		pDoc->SetScenarioData( nCurrTable, String( sComment ), aBorderColor, nFlags );
		for( sal_Int32 i = 0; i < static_cast<sal_Int32>(aScenarioRanges.Count()); i++ )
		{
			ScRange* pRange = aScenarioRanges.GetObject( i );
			if( pRange )
				pDoc->ApplyFlagsTab( pRange->aStart.Col(), pRange->aStart.Row(),
					pRange->aEnd.Col(), pRange->aEnd.Row(), nCurrTable, SC_MF_SCENARIO );
		}
		pDoc->SetActiveScenario( nCurrTable, bIsActive );
	}
}

}
