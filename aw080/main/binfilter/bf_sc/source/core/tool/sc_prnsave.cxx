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

#include <tools/debug.hxx>

#include "prnsave.hxx"
#include "global.hxx"
namespace binfilter {

// STATIC DATA -----------------------------------------------------------

//------------------------------------------------------------------

//
//		Daten pro Tabelle
//

/*N*/ ScPrintSaverTab::ScPrintSaverTab() :
/*N*/ 	nPrintCount(0),
/*N*/ 	pPrintRanges(NULL),
/*N*/ 	pRepeatCol(NULL),
/*N*/ 	pRepeatRow(NULL)
/*N*/ {
/*N*/ }

/*N*/ ScPrintSaverTab::~ScPrintSaverTab()
/*N*/ {
/*N*/ 	delete[] pPrintRanges;
/*N*/ 	delete pRepeatCol;
/*N*/ 	delete pRepeatRow;
/*N*/ }

/*N*/ void ScPrintSaverTab::SetAreas( USHORT nCount, const ScRange* pRanges )
/*N*/ {
/*N*/ 	delete[] pPrintRanges;
/*N*/ 	if (nCount && pRanges)
/*N*/ 	{
/*N*/ 		nPrintCount = nCount;
/*N*/ 		pPrintRanges = new ScRange[nCount];
/*N*/ 		for (USHORT i=0; i<nCount; i++)
/*N*/ 			pPrintRanges[i] = pRanges[i];
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		nPrintCount = 0;
/*N*/ 		pPrintRanges = NULL;
/*N*/ 	}
/*N*/ }

/*N*/ void ScPrintSaverTab::SetRepeat( const ScRange* pCol, const ScRange* pRow )
/*N*/ {
/*N*/ 	delete pRepeatCol;
/*N*/ 	pRepeatCol = pCol ? new ScRange(*pCol) : NULL;
/*N*/ 	delete pRepeatRow;
/*N*/ 	pRepeatRow = pRow ? new ScRange(*pRow) : NULL;
/*N*/ }



//
//		Daten fuer das ganze Dokument
//

/*N*/ ScPrintRangeSaver::ScPrintRangeSaver( USHORT nCount ) :
/*N*/ 	nTabCount( nCount )
/*N*/ {
/*N*/ 	if (nCount)
/*N*/ 		pData = new ScPrintSaverTab[nCount];
/*N*/ 	else
/*N*/ 		pData = NULL;
/*N*/ }

/*N*/ ScPrintRangeSaver::~ScPrintRangeSaver()
/*N*/ {
/*N*/ 	delete[] pData;
/*N*/ }

/*N*/ ScPrintSaverTab& ScPrintRangeSaver::GetTabData(USHORT nTab)
/*N*/ {
/*N*/ 	DBG_ASSERT(nTab<nTabCount,"ScPrintRangeSaver Tab zu gross");
/*N*/ 	return pData[nTab];
/*N*/ }






}
