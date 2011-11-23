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

// INCLUDE --------------------------------------------------------------

#include <tools/debug.hxx>
#include <bf_svtools/zforlist.hxx>

#include "dpshttab.hxx"
#include "document.hxx"
#include "collect.hxx"
#include "globstr.hrc"
namespace binfilter {

// -----------------------------------------------------------------------

/*N*/ class ScSheetDPData_Impl
/*N*/ {
/*N*/ public:
/*N*/ 	ScDocument*		pDoc;
/*N*/ 	ScRange			aRange;
/*N*/ 	ScQueryParam	aQuery;
/*N*/ 	long			nColCount;
/*N*/ 	BOOL			bIgnoreEmptyRows;
/*N*/ 	BOOL			bRepeatIfEmpty;
/*N*/ 	TypedStrCollection**	ppStrings;
/*N*/ 	BOOL*			pDateDim;
/*N*/ 	USHORT			nNextRow;		// for iterator, within range
/*N*/ 
/*N*/ 	ScSheetDPData_Impl() {}
/*N*/ };

// -----------------------------------------------------------------------

/*M*/ ScSheetDPData::ScSheetDPData( ScDocument* pD, const ScSheetSourceDesc& rDesc )
/*M*/     : pSpecial(NULL)
/*M*/ {
/*M*/ 	long nCount = rDesc.aSourceRange.aEnd.Col() - rDesc.aSourceRange.aStart.Col() + 1;
/*M*/ 	pImpl = new ScSheetDPData_Impl;
/*M*/ 	pImpl->pDoc = pD;
/*M*/ 	pImpl->aRange = rDesc.aSourceRange;
/*M*/ 	pImpl->aQuery = rDesc.aQueryParam;
/*M*/ 	pImpl->bIgnoreEmptyRows = FALSE;
/*M*/ 	pImpl->bRepeatIfEmpty = FALSE;
/*M*/ 	pImpl->nColCount = nCount;
/*M*/ 	pImpl->ppStrings = new TypedStrCollection*[nCount];
/*M*/ 	pImpl->pDateDim = NULL;
/*M*/ 	for (long i=0; i<nCount; i++)
/*M*/ 		pImpl->ppStrings[i] = NULL;
/*M*/ 
/*M*/ 	pImpl->nNextRow = pImpl->aRange.aStart.Row() + 1;
/*M*/ 
/*M*/     long nEntryCount(pImpl->aQuery.GetEntryCount());
/*M*/     pSpecial = new BOOL[nEntryCount];
/*M*/     for (long j = 0; j < nEntryCount; ++j )
/*M*/     {
/*M*/         ScQueryEntry& rEntry = pImpl->aQuery.GetEntry(j);
/*M*/         if (rEntry.bDoQuery)
/*M*/         {
/*M*/             pSpecial[j] = false;
/*M*/             if (!rEntry.bQueryByString)
/*M*/             {
/*M*/                 if (*rEntry.pStr == EMPTY_STRING &&
/*M*/                    ((rEntry.nVal == SC_EMPTYFIELDS) || (rEntry.nVal == SC_NONEMPTYFIELDS)))
/*M*/                     pSpecial[j] = true;
/*M*/             }
/*M*/             else
/*M*/             {
/*M*/ 		        sal_uInt32 nIndex = 0;
/*M*/ 		        rEntry.bQueryByString =
/*M*/ 					        !(pD->GetFormatTable()->
/*M*/ 						        IsNumberFormat(*rEntry.pStr, nIndex, rEntry.nVal));
/*M*/             }
/*M*/     	}
/*M*/     }
/*M*/ }

/*N*/ ScSheetDPData::~ScSheetDPData()
/*N*/ {
/*N*/ 	for (long i=0; i<pImpl->nColCount; i++)
/*N*/ 		delete pImpl->ppStrings[i];
/*N*/ 	delete[] pImpl->ppStrings;
/*N*/ 	delete[] pImpl->pDateDim;
/*N*/ 	delete pImpl;
/*M*/     delete[] pSpecial;
/*N*/ }

/*N*/ void ScSheetDPData::DisposeData()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*N*/ long ScSheetDPData::GetColumnCount()
/*N*/ {
/*N*/ 	return pImpl->nColCount;
/*N*/ }


/*N*/ const TypedStrCollection& ScSheetDPData::GetColumnEntries(long nColumn)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001 
/*N*/ }

/*N*/ String ScSheetDPData::getDimensionName(long nColumn)
/*N*/ {
/*N*/ 	if (getIsDataLayoutDimension(nColumn))
/*N*/ 	{
/*N*/ 		//!	different internal and display names?
/*N*/ 		//return "Data";
/*N*/ 		return ScGlobal::GetRscString(STR_PIVOT_DATA);
/*N*/ 	}
/*N*/ 	else if ( nColumn >= pImpl->nColCount )
/*N*/ 	{
/*?*/ 		DBG_ERROR("getDimensionName: invalid dimension");
/*?*/ 		return String();
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		USHORT nDocCol = (USHORT)(pImpl->aRange.aStart.Col() + nColumn);
/*N*/ 		USHORT nDocRow = pImpl->aRange.aStart.Row();
/*N*/ 		USHORT nDocTab = pImpl->aRange.aStart.Tab();
/*N*/ 		String aDocStr;
/*N*/ 		pImpl->pDoc->GetString( nDocCol, nDocRow, nDocTab, aDocStr );
/*N*/ 		return aDocStr;
/*N*/ 	}
/*N*/ }

/*N*/ BOOL lcl_HasDateFormat( ScDocument* pDoc, const ScRange& rRange )
/*N*/ {
/*N*/ 	//!	iterate formats in range?
/*N*/ 
/*N*/ 	ScAddress aPos = rRange.aStart;
/*N*/ 	aPos.SetRow( aPos.Row() + 1 );		// below title
/*N*/ 	ULONG nFormat = pDoc->GetNumberFormat( aPos );
/*N*/ 	SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
/*N*/ 	return ( pFormatter->GetType(nFormat) & NUMBERFORMAT_DATE ) != 0;
/*N*/ }

/*N*/ BOOL ScSheetDPData::IsDateDimension(long nDim)
/*N*/ {
/*N*/ 	if (getIsDataLayoutDimension(nDim))
/*N*/ 	{
/*N*/ 		return FALSE;
/*N*/ 	}
/*N*/ 	else if ( nDim >= pImpl->nColCount )
/*N*/ 	{
/*?*/ 		DBG_ERROR("IsDateDimension: invalid dimension");
/*?*/ 		return FALSE;
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		if (!pImpl->pDateDim)
/*N*/ 		{
/*N*/ 			pImpl->pDateDim = new BOOL[pImpl->nColCount];
/*N*/ 			ScRange aTestRange = pImpl->aRange;
/*N*/ 			for (long i=0; i<pImpl->nColCount; i++)
/*N*/ 			{
/*N*/ 				USHORT nCol = (USHORT)( pImpl->aRange.aStart.Col() + i );
/*N*/ 				aTestRange.aStart.SetCol(nCol);
/*N*/ 				aTestRange.aEnd.SetCol(nCol);
/*N*/ 				pImpl->pDateDim[i] = lcl_HasDateFormat( pImpl->pDoc, aTestRange );
/*N*/ 			}
/*N*/ 		}
/*N*/ 		return pImpl->pDateDim[nDim];
/*N*/ 	}
/*N*/ }

/*N*/ UINT32 ScSheetDPData::GetNumberFormat(long nDim)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return 0; //STRIP001 
/*N*/ }

/*N*/ BOOL ScSheetDPData::getIsDataLayoutDimension(long nColumn)
/*N*/ {
/*N*/ 	return ( nColumn == pImpl->nColCount );
/*N*/ }

/*N*/ void ScSheetDPData::SetEmptyFlags( BOOL bIgnoreEmptyRows, BOOL bRepeatIfEmpty )
/*N*/ {
/*N*/ 	pImpl->bIgnoreEmptyRows = bIgnoreEmptyRows;
/*N*/ 	pImpl->bRepeatIfEmpty   = bRepeatIfEmpty;
/*N*/ }

/*N*/ void ScSheetDPData::ResetIterator()
/*N*/ {
/*N*/ 	pImpl->nNextRow = pImpl->aRange.aStart.Row() + 1;
/*N*/ }


/*N*/ BOOL ScSheetDPData::GetNextRow( const ScDPTableIteratorParam& rParam )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 
/*N*/ }

// -----------------------------------------------------------------------







}
