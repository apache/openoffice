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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _SFXITEMPOOL_HXX
#include <bf_svtools/itempool.hxx>
#endif
#ifndef _TXATBASE_HXX
#include <txatbase.hxx>
#endif
namespace binfilter {

/*N*/ SwTxtAttr::SwTxtAttr( const SfxPoolItem& rAttr, xub_StrLen nStt )
/*N*/ 	: pAttr( &rAttr ), nStart( nStt )
/*N*/ {
/*N*/ 	bDontExpand = bLockExpandFlag =	bDontMergeAttr = bDontMoveAttr =
/*N*/         bCharFmtAttr = bOverlapAllowedAttr = bPriorityAttr =
/*N*/ 		bDontExpandStart = FALSE;
/*N*/ }

/*N*/ SwTxtAttr::~SwTxtAttr( )
/*N*/ {
/*N*/ }

/*N*/ xub_StrLen* SwTxtAttr::GetEnd()
/*N*/ {
/*N*/ 	return 0;
/*N*/ }

	// RemoveFromPool muss immer vorm DTOR Aufruf erfolgen!!
	// Meldet sein Attribut beim Pool ab
/*N*/ void SwTxtAttr::RemoveFromPool( SfxItemPool& rPool )
/*N*/ {
/*N*/ 	rPool.Remove( GetAttr() );
/*N*/ 	pAttr = 0;
/*N*/ }


/*N*/ SwTxtAttrEnd::SwTxtAttrEnd( const SfxPoolItem& rAttr, xub_StrLen nS,
/*N*/ 							xub_StrLen nE )
/*N*/ 	: SwTxtAttr( rAttr, nS ), nEnd( nE )
/*N*/ {
/*N*/ }

/*N*/ xub_StrLen* SwTxtAttrEnd::GetEnd()
/*N*/ {
/*N*/ 	return &nEnd;
/*N*/ }
}
