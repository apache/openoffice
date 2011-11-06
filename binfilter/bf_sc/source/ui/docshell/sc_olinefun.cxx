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


#include "olinefun.hxx"

#include "docsh.hxx"
#include "globstr.hrc"
#include "bf_sc.hrc"
namespace binfilter {


//========================================================================

/*N*/ BOOL ScOutlineDocFunc::MakeOutline( const ScRange& rRange, BOOL bColumns, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 BOOL bSuccess = FALSE;
/*N*/ }

/*N*/ BOOL ScOutlineDocFunc::RemoveOutline( const ScRange& rRange, BOOL bColumns, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 BOOL bDone = FALSE;
/*N*/ }

/*N*/ BOOL ScOutlineDocFunc::RemoveAllOutlines( USHORT nTab, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 BOOL bSuccess = FALSE;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ BOOL ScOutlineDocFunc::AutoOutline( const ScRange& rRange, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 USHORT nStartCol = rRange.aStart.Col();
/*N*/ 	return TRUE;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ BOOL ScOutlineDocFunc::SelectLevel( USHORT nTab, BOOL bColumns, USHORT nLevel,
/*N*/ 									BOOL bRecord, BOOL bPaint, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 ScDocument* pDoc = rDocShell.GetDocument();
/*N*/ 	return TRUE;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ BOOL ScOutlineDocFunc::ShowMarkedOutlines( const ScRange& rRange, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001 BOOL bDone = FALSE;
/*N*/ }

/*N*/ BOOL ScOutlineDocFunc::HideMarkedOutlines( const ScRange& rRange, BOOL bRecord, BOOL bApi )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 BOOL bDone = FALSE;
/*N*/ }
}
