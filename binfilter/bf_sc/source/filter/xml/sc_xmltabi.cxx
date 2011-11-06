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

#include "xmltabi.hxx"
#include "xmlimprt.hxx"
#include "xmlrowi.hxx"
#include "xmlcoli.hxx"
#include "xmlsceni.hxx"
#include "document.hxx"
#include "olinetab.hxx"

#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif
#ifndef _SC_XMLTABLESHAPESCONTEXT_HXX
#include "XMLTableShapesContext.hxx"
#endif
#ifndef _SC_XMLTABLESOURCECONTEXT_HXX
#include "XMLTableSourceContext.hxx"
#endif
#ifndef _SC_XMLSTYLESIMPORTHELPER_HXX
#include "XMLStylesImportHelper.hxx"
#endif

#include <bf_xmloff/nmspmap.hxx>

#include <com/sun/star/sheet/XPrintAreas.hpp>
namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLTableContext::ScXMLTableContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
									  const sal_Bool bTempIsSubTable,
									  const sal_Int32 nSpannedCols) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	bStartFormPage(sal_False)
{
	if (!bTempIsSubTable)
	{
		sal_Bool bProtection(sal_False);
		::rtl::OUString sName;
		::rtl::OUString sStyleName;
		::rtl::OUString sPassword;
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetTableAttrTokenMap();
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
			::rtl::OUString aLocalName;
			USHORT nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
												sAttrName, &aLocalName );
			::rtl::OUString sValue = xAttrList->getValueByIndex( i );

			switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
			{
				case XML_TOK_TABLE_NAME:
						sName = sValue;
					break;
				case XML_TOK_TABLE_STYLE_NAME:
						sStyleName = sValue;
					break;
				case XML_TOK_TABLE_PROTECTION:
						bProtection = IsXMLToken(sValue, XML_TRUE);
					break;
				case XML_TOK_TABLE_PRINT_RANGES:
						sPrintRanges = sValue;
					break;
				case XML_TOK_TABLE_PASSWORD:
						sPassword = sValue;
					break;
			}
		}
		GetScImport().GetTables().NewSheet(sName, sStyleName, bProtection, sPassword);
	}
	else
	{
		GetScImport().GetTables().NewTable(nSpannedCols);
	}
}

ScXMLTableContext::~ScXMLTableContext()
{
}

SvXMLImportContext *ScXMLTableContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = 0;

	const SvXMLTokenMap& rTokenMap = GetScImport().GetTableElemTokenMap();
	switch( rTokenMap.Get( nPrefix, rLName ) )
	{
	case XML_TOK_TABLE_COL_GROUP:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_True );
		break;
	case XML_TOK_TABLE_HEADER_COLS:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_True, sal_False );
		break;
	case XML_TOK_TABLE_COLS:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_False );
		break;
	case XML_TOK_TABLE_COL:
			pContext = new ScXMLTableColContext( GetScImport(), nPrefix,
													  rLName, xAttrList );
		break;
	case XML_TOK_TABLE_ROW_GROUP:
		pContext = new ScXMLTableRowsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_True );
		break;
	case XML_TOK_TABLE_HEADER_ROWS:
		pContext = new ScXMLTableRowsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_True, sal_False );
		break;
	case XML_TOK_TABLE_ROWS:
		pContext = new ScXMLTableRowsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_False );
		break;
	case XML_TOK_TABLE_ROW:
			pContext = new ScXMLTableRowContext( GetScImport(), nPrefix,
													  rLName, xAttrList//,
													  //this
													  );
		break;
	case XML_TOK_TABLE_SOURCE:
		pContext = new ScXMLTableSourceContext( GetScImport(), nPrefix, rLName, xAttrList);
		break;
	case XML_TOK_TABLE_SCENARIO:
		pContext = new ScXMLTableScenarioContext( GetScImport(), nPrefix, rLName, xAttrList);
		break;
	case XML_TOK_TABLE_SHAPES:
		pContext = new ScXMLTableShapesContext( GetScImport(), nPrefix, rLName, xAttrList);
		break;
	case XML_TOK_TABLE_FORMS:
		{
			GetScImport().GetFormImport()->startPage(GetScImport().GetTables().GetCurrentXDrawPage());
			bStartFormPage = sal_True;
			pContext = GetScImport().GetFormImport()->createOfficeFormsContext( GetScImport(), nPrefix, rLName );
		}
		break;
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void ScXMLTableContext::EndElement()
{
	GetScImport().LockSolarMutex();
	GetScImport().GetStylesImportHelper()->EndTable();
	ScDocument* pDoc = GetScImport().GetDocument();
	if (pDoc)
	{
		if (sPrintRanges.getLength())
		{
			uno::Reference< sheet::XSpreadsheet > xTable = GetScImport().GetTables().GetCurrentXSheet();
			if( xTable.is() )
			{
				uno::Reference< sheet::XPrintAreas > xPrintAreas( xTable, uno::UNO_QUERY );
				if( xPrintAreas.is() )
				{
					uno::Sequence< table::CellRangeAddress > aRangeList;
					ScXMLConverter::GetRangeListFromString( aRangeList, sPrintRanges, pDoc );
					xPrintAreas->setPrintAreas( aRangeList );
				}
			}
		}

		ScOutlineTable* pOutlineTable = pDoc->GetOutlineTable(GetScImport().GetTables().GetCurrentSheet(), sal_False);
		if (pOutlineTable)
		{
			ScOutlineArray* pColArray = pOutlineTable->GetColArray();
			sal_Int32 nDepth = pColArray->GetDepth();
			sal_Int32 i;
			for (i = 0; i < nDepth; i++)
			{
				sal_Int32 nCount = pColArray->GetCount(static_cast<USHORT>(i));
				sal_Bool bChanged(sal_False);
				for (sal_Int32 j = 0; j < nCount && !bChanged; j++)
				{
					ScOutlineEntry* pEntry = pColArray->GetEntry(static_cast<USHORT>(i), static_cast<USHORT>(j));
					if (pEntry->IsHidden())
					{
						pColArray->SetVisibleBelow(static_cast<USHORT>(i), static_cast<USHORT>(j), sal_False);
						bChanged = sal_True;
					}
				}
			}
			ScOutlineArray* pRowArray = pOutlineTable->GetRowArray();
			nDepth = pRowArray->GetDepth();
			for (i = 0; i < nDepth; i++)
			{
				sal_Int32 nCount = pRowArray->GetCount(static_cast<USHORT>(i));
				sal_Bool bChanged(sal_False);
				for (sal_Int32 j = 0; j < nCount && !bChanged; j++)
				{
					ScOutlineEntry* pEntry = pRowArray->GetEntry(static_cast<USHORT>(i), static_cast<USHORT>(j));
					if (pEntry->IsHidden())
					{
						pRowArray->SetVisibleBelow(static_cast<USHORT>(i), static_cast<USHORT>(j), sal_False);
						bChanged = sal_True;
					}
				}
			}
		}
		if (GetScImport().GetTables().HasDrawPage())
		{
			if (GetScImport().GetTables().HasXShapes())
			{
				GetScImport().GetShapeImport()->popGroupAndSort();
                uno::Reference<drawing::XShapes> xXShapes(GetScImport().GetTables().GetCurrentXShapes());
				GetScImport().GetShapeImport()->endPage(xXShapes);
			}
			if (bStartFormPage)
				GetScImport().GetFormImport()->endPage();
		}

		GetScImport().GetTables().DeleteTable();
		GetScImport().GetProgressBarHelper()->Increment();
	}
	GetScImport().UnlockSolarMutex();
}

}
