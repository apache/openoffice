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

#include "xmlcoli.hxx"
#include "xmlstyli.hxx"
#include "document.hxx"
#include "olinetab.hxx"

#ifndef SC_UNONAMES_HXX
#include "unonames.hxx"
#endif

#include <bf_xmloff/nmspmap.hxx>
#include <bf_xmloff/xmlnmspe.hxx>

#ifndef _COM_SUN_STAR_TABLE_XCOLUMNROWRANGE_HPP_
#include <com/sun/star/table/XColumnRowRange.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XPRINTAREAS_HPP_
#include <com/sun/star/sheet/XPrintAreas.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLTableColContext::ScXMLTableColContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	sVisibility(GetXMLToken(XML_VISIBLE))
{
	nColCount = 1;
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetTableColAttrTokenMap();

	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
		::rtl::OUString aLocalName;
		sal_uInt16 nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
											sAttrName, &aLocalName );
		::rtl::OUString sValue = xAttrList->getValueByIndex( i );

		switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
		{
			case XML_TOK_TABLE_COL_ATTR_REPEATED:
				{
					nColCount = sValue.toInt32();
				}
				break;
			case XML_TOK_TABLE_COL_ATTR_STYLE_NAME:
				{
					sStyleName = sValue;
				}
				break;
			case XML_TOK_TABLE_COL_ATTR_VISIBILITY:
				{
					sVisibility = sValue;
				}
				break;
			case XML_TOK_TABLE_COL_ATTR_DEFAULT_CELL_STYLE_NAME:
				{
					sCellStyleName = sValue;
				}
				break;
		}
	}
}

ScXMLTableColContext::~ScXMLTableColContext()
{
}

SvXMLImportContext *ScXMLTableColContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = 0;
/*
	const SvXMLTokenMap& rTokenMap = GetScImport().GetTableRowElemTokenMap();
	sal_Bool bHeader = sal_False;
	switch( rTokenMap.Get( nPrefix, rLName ) )
	{
	case XML_TOK_TABLE_ROW_CELL:
//		if( IsInsertCellPossible() )
			pContext = new ScXMLTableRowCellContext( GetScImport(), nPrefix,
													  rLName, xAttrList//,
													  //this
													  );
		break;
	case XML_TOK_TABLE_ROW_COVERED_CELL:
//		if( IsInsertCellPossible() )
			pContext = new ScXMLTableRowCellContext( GetScImport(), nPrefix,
													  rLName, xAttrList//,
													  //this
													  );
		break;
	}*/

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void ScXMLTableColContext::EndElement()
{
	ScXMLImport& rXMLImport = GetScImport();
	//sal_Int16 nSheet = rXMLImport.GetTables().GetCurrentSheet();
	sal_Int32 nCurrentColumn = rXMLImport.GetTables().GetCurrentColumn();
	uno::Reference<sheet::XSpreadsheet> xSheet = rXMLImport.GetTables().GetCurrentXSheet();
	if(xSheet.is())
	{
		sal_Int32 nLastColumn(nCurrentColumn + nColCount - 1);
		if (nLastColumn > MAXCOL)
			nLastColumn = MAXCOL;
		if (nCurrentColumn > MAXCOL)
			nCurrentColumn = MAXCOL;
		uno::Reference <table::XCellRange> xCellRange = xSheet->getCellRangeByPosition(nCurrentColumn, 0, nLastColumn, 0);
		if (xCellRange.is())
		{
			uno::Reference<table::XColumnRowRange> xColumnRowRange (xCellRange, uno::UNO_QUERY);
			if (xColumnRowRange.is())
			{
				uno::Reference<table::XTableColumns> xTableColumns = xColumnRowRange->getColumns();
				if (xTableColumns.is())
				{
					uno::Reference <beans::XPropertySet> xColumnProperties(xTableColumns, uno::UNO_QUERY);
					if (xColumnProperties.is())
					{
						if (sStyleName.getLength())
						{
							XMLTableStylesContext *pStyles = (XMLTableStylesContext *)rXMLImport.GetAutoStyles();
							XMLTableStyleContext* pStyle = (XMLTableStyleContext *)pStyles->FindStyleChildContext(
								XML_STYLE_FAMILY_TABLE_COLUMN, sStyleName, sal_True);
							if (pStyle)
								pStyle->FillPropertySet(xColumnProperties);
						}
						::rtl::OUString sVisible(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_CELLVIS));
						uno::Any aAny = xColumnProperties->getPropertyValue(sVisible);
						if (IsXMLToken(sVisibility, XML_VISIBLE))
						{
							sal_Bool bValue = sal_True;
							aAny <<= bValue;
						}
						else
						{
							sal_Bool bValue = sal_False;
							aAny <<= bValue;
						}
						xColumnProperties->setPropertyValue(sVisible, aAny);
					}
				}
			}
		}
	}
	GetScImport().GetTables().AddColCount(nColCount);
	GetScImport().GetTables().AddColStyle(nColCount, sCellStyleName);
}

ScXMLTableColsContext::ScXMLTableColsContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
									  const sal_Bool bTempHeader, const sal_Bool bTempGroup) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	nHeaderStartCol(0),
	nHeaderEndCol(0),
	nGroupStartCol(0),
	nGroupEndCol(0),
	bHeader(bTempHeader),
	bGroup(bTempGroup),
	bGroupDisplay(sal_True)
{
	// don't have any attributes
	if (bHeader)
		nHeaderStartCol = rImport.GetTables().GetCurrentColumn();
	else if (bGroup)
	{
		nGroupStartCol = rImport.GetTables().GetCurrentColumn();
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
			::rtl::OUString aLocalName;
			sal_uInt16 nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
												sAttrName, &aLocalName );
			::rtl::OUString sValue = xAttrList->getValueByIndex( i );

			if (nPrfx == XML_NAMESPACE_TABLE && IsXMLToken(aLocalName, XML_DISPLAY))
			{
				if (IsXMLToken(sValue, XML_FALSE))
					bGroupDisplay = sal_False;
			}
		}
	}
}

ScXMLTableColsContext::~ScXMLTableColsContext()
{
}

SvXMLImportContext *ScXMLTableColsContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = 0;

	const SvXMLTokenMap& rTokenMap = GetScImport().GetTableColsElemTokenMap();
	sal_Bool bHeader = sal_False;
	switch( rTokenMap.Get( nPrefix, rLName ) )
	{
	case XML_TOK_TABLE_COLS_COL_GROUP:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_True );
		break;
	case XML_TOK_TABLE_COLS_HEADER_COLS:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_True, sal_False );
		break;
	case XML_TOK_TABLE_COLS_COLS:
		pContext = new ScXMLTableColsContext( GetScImport(), nPrefix,
												   rLName, xAttrList,
												   sal_False, sal_False );
		break;
	case XML_TOK_TABLE_COLS_COL:
			pContext = new ScXMLTableColContext( GetScImport(), nPrefix,
													  rLName, xAttrList//,
													  //this
													  );
		break;
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

	return pContext;
}

void ScXMLTableColsContext::EndElement()
{
	ScXMLImport& rXMLImport = GetScImport();
	if (bHeader)
	{
		nHeaderEndCol = rXMLImport.GetTables().GetCurrentColumn();
		nHeaderEndCol--;
		if (nHeaderStartCol <= nHeaderEndCol)
		{
			uno::Reference<sheet::XSpreadsheet> xSheet = rXMLImport.GetTables().GetCurrentXSheet();
			if(xSheet.is())
			{
				uno::Reference <sheet::XPrintAreas> xPrintAreas (xSheet, uno::UNO_QUERY);
				if (xPrintAreas.is())
				{
					if (!xPrintAreas->getPrintTitleColumns())
					{
						xPrintAreas->setPrintTitleColumns(sal_True);
						table::CellRangeAddress aColumnHeaderRange;
						aColumnHeaderRange.StartColumn = nHeaderStartCol;
						aColumnHeaderRange.EndColumn = nHeaderEndCol;
						xPrintAreas->setTitleColumns(aColumnHeaderRange);
					}
					else
					{
						table::CellRangeAddress aColumnHeaderRange = xPrintAreas->getTitleColumns();
						aColumnHeaderRange.EndColumn = nHeaderEndCol;
						xPrintAreas->setTitleColumns(aColumnHeaderRange);
					}
				}
			}
		}
	}
	else if (bGroup)
	{
		sal_Int16 nSheet = rXMLImport.GetTables().GetCurrentSheet();
		nGroupEndCol = rXMLImport.GetTables().GetCurrentColumn();
		nGroupEndCol--;
		if (nGroupStartCol <= nGroupEndCol)
		{
			ScDocument* pDoc = GetScImport().GetDocument();
			if (pDoc)
			{
				rXMLImport.LockSolarMutex();
				ScOutlineTable* pOutlineTable = pDoc->GetOutlineTable(nSheet, sal_True);
				ScOutlineArray* pColArray = pOutlineTable ? pOutlineTable->GetColArray() : NULL;
				if (pColArray)
				{
					sal_Bool bResized;
					pColArray->Insert(static_cast<USHORT>(nGroupStartCol), static_cast<USHORT>(nGroupEndCol), bResized, !bGroupDisplay, sal_True);
				}
				rXMLImport.UnlockSolarMutex();
			}
		}
	}
}
}
