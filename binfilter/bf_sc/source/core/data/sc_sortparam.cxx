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

#include "sortparam.hxx"

namespace binfilter {


//------------------------------------------------------------------------

/*N*/ ScSortParam::ScSortParam()
/*N*/ {
/*N*/ 	Clear();
/*N*/ }

//------------------------------------------------------------------------

/*N*/ ScSortParam::ScSortParam( const ScSortParam& r ) :
/*N*/ 		nCol1(r.nCol1),nRow1(r.nRow1),nCol2(r.nCol2),nRow2(r.nRow2),
/*N*/ 		bHasHeader(r.bHasHeader),bCaseSens(r.bCaseSens),
/*N*/ 		bByRow(r.bByRow),bUserDef(r.bUserDef),nUserIndex(r.nUserIndex),bIncludePattern(r.bIncludePattern),
/*N*/ 		bInplace(r.bInplace),
/*N*/ 		nDestTab(r.nDestTab),nDestCol(r.nDestCol),nDestRow(r.nDestRow),
/*N*/ 		aCollatorLocale( r.aCollatorLocale ), aCollatorAlgorithm( r.aCollatorAlgorithm )
/*N*/ {
/*N*/ 	for (USHORT i=0; i<MAXSORT; i++)
/*N*/ 	{
/*N*/ 		bDoSort[i]	  = r.bDoSort[i];
/*N*/ 		nField[i]	  = r.nField[i];
/*N*/ 		bAscending[i] = r.bAscending[i];
/*N*/ 	}
/*N*/ }

//------------------------------------------------------------------------

/*N*/ void ScSortParam::Clear()
/*N*/ {
/*N*/ 	nCol1=nRow1=nCol2=nRow2=nDestTab=nDestCol=nDestRow=nUserIndex = 0;
/*N*/ 	bHasHeader=bCaseSens=bUserDef = FALSE;
/*N*/ 	bByRow=bIncludePattern=bInplace	= TRUE;
/*N*/ 	aCollatorLocale = ::com::sun::star::lang::Locale();
/*N*/ 	aCollatorAlgorithm.Erase();
/*N*/ 
/*N*/ 	for (USHORT i=0; i<MAXSORT; i++)
/*N*/ 	{
/*N*/ 		bDoSort[i]	  = FALSE;
/*N*/ 		nField[i]	  = 0;
/*N*/ 		bAscending[i] = TRUE;
/*N*/ 	}
/*N*/ }

//------------------------------------------------------------------------


//------------------------------------------------------------------------


//------------------------------------------------------------------------


//------------------------------------------------------------------------


//------------------------------------------------------------------------


}
