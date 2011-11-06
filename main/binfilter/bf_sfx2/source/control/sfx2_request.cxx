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



#include <bf_svtools/itempool.hxx>
#include <bf_svtools/itemset.hxx>

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "request.hxx"

namespace binfilter {

/*N*/ const SfxPoolItem* SfxRequest::GetItem
/*N*/ (
/*N*/ 	const SfxItemSet* pArgs,
/*N*/ 	USHORT 			nSlotId, 	// Slot-Id oder Which-Id des Parameters
/*N*/ 	FASTBOOL 		bDeep,	 	// FALSE: nicht in Parent-ItemSets suchen
/*N*/ 	TypeId			aType		// != 0:  RTTI Pruefung mit Assertion
/*N*/ )
/*N*/ {
/*N*/ 	if ( pArgs )
/*N*/ 	{
/*N*/ 		// ggf. in Which-Id umrechnen
/*N*/ 		USHORT nWhich = pArgs->GetPool()->GetWhich(nSlotId);
/*N*/
/*N*/ 		// ist das Item gesetzt oder bei bDeep==TRUE verf"ugbar?
/*N*/ 		const SfxPoolItem *pItem = 0;
/*N*/ 		if ( ( bDeep ? SFX_ITEM_AVAILABLE : SFX_ITEM_SET )
/*N*/ 			 <= pArgs->GetItemState( nWhich, bDeep, &pItem ) )
/*N*/ 		{
/*N*/ 			// stimmt der Typ "uberein?
/*N*/ 			if ( !pItem || pItem->IsA(aType) )
/*N*/ 				return pItem;
/*N*/
/*N*/ 			// Item da aber falsch => Programmierfehler
/*N*/ 			DBG_ERROR(  "invalid argument type" );
/*N*/ 		}
/*N*/ 	}
/*N*/
/*N*/ 	// keine Parameter, nicht gefunden oder falschen Typ gefunden
/*N*/ 	return 0;
/*N*/ }
}