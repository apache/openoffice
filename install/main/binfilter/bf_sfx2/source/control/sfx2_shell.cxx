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

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "app.hxx"

namespace binfilter {

//====================================================================

/*N*/ TYPEINIT0(SfxShell);

//====================================================================
/*N*/ SV_IMPL_PTRARR( SfxItemPtrArray, SfxPoolItemPtr);

/*N*/ struct SfxShell_Impl: public SfxBroadcaster
/*N*/ {
/*N*/ 	String                      aObjectName;// Name des Sbx-Objects
/*N*/ 	SfxItemArray_Impl           aItems;     // Datenaustausch auf Item-Basis
/*N*/ };

/*N*/ SfxShell::SfxShell()
/*N*/ :   pPool(0),
/*N*/ 	pImp(0)
/*N*/ {
/*N*/ 	pImp = new SfxShell_Impl;
/*N*/ }

//-------------------------------------------------------------------------

/*N*/ SfxShell::~SfxShell()
/*N*/ {
/*N*/         delete pImp;
/*N*/ }
//--------------------------------------------------------------------

/*N*/ const SfxPoolItem* SfxShell::GetItem
/*N*/ (
/*N*/ 	USHORT  nSlotId         // Slot-Id des zu erfragenden <SfxPoolItem>s
/*N*/ )   const
/*N*/ {
/*N*/ 	for ( USHORT nPos = 0; nPos < pImp->aItems.Count(); ++nPos )
/*N*/ 		if ( pImp->aItems.GetObject(nPos)->Which() == nSlotId )
/*N*/ 			return pImp->aItems.GetObject(nPos);
/*N*/ 	return 0;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxShell::RemoveItem
/*N*/ (
/*N*/ 	USHORT  nSlotId         // Slot-Id des zu l"oschenden <SfxPoolItem>s
/*N*/ )
/*N*/ {
/*N*/ 	for ( USHORT nPos = 0; nPos < pImp->aItems.Count(); ++nPos )
/*?*/ 		if ( pImp->aItems.GetObject(nPos)->Which() == nSlotId )
/*?*/ 		{
/*?*/ 			// Item entfernen und l"oschen
/*?*/ 			SfxPoolItem *pItem = pImp->aItems.GetObject(nPos);
/*?*/ 			delete pItem;
/*?*/ 			pImp->aItems.Remove(nPos);
/*?*/
/*N*/ 		}
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxShell::PutItem
/*N*/ (
/*N*/ 	const SfxPoolItem&  rItem   /*  Instanz, von der eine Kopie erstellt wird,
									die in der SfxShell in einer Liste
									gespeichert wird. */
/*N*/ )
/*N*/ {
/*N*/ 	DBG_ASSERT( !rItem.ISA(SfxSetItem), "SetItems aren't allowed here" );
/*N*/ 	DBG_ASSERT( SfxItemPool::IsSlot( rItem.Which() ),
/*N*/ 				"items with Which-Ids aren't allowed here" );
/*N*/
/*N*/ 	// MSC auf WNT/W95 machte hier Mist, Vorsicht bei Umstellungen
/*N*/ 	const SfxPoolItem *pItem = rItem.Clone();
/*N*/ 	SfxPoolItemHint aItemHint( (SfxPoolItem*) pItem );
/*N*/ 	const USHORT nWhich = rItem.Which();
/*N*/ 	SfxPoolItem **ppLoopItem = (SfxPoolItem**) pImp->aItems.GetData();
/*N*/ 	USHORT nPos;
/*N*/ 	for ( nPos = 0; nPos < pImp->aItems.Count(); ++nPos, ++ppLoopItem )
/*N*/ 	{
/*N*/ 		if ( (*ppLoopItem)->Which() == nWhich )
/*N*/ 		{
/*N*/ 			// Item austauschen
/*N*/ 			delete *ppLoopItem;
/*N*/ 			pImp->aItems.Remove(nPos);
/*N*/ 			pImp->aItems.Insert( (SfxPoolItemPtr) pItem, nPos );
/*N*/
/*N*/ 			return;
/*N*/ 		}
/*N*/ 	}
/*N*/
/*N*/ 	Broadcast( aItemHint );
/*N*/ 	pImp->aItems.Insert((SfxPoolItemPtr)pItem, nPos );
/*N*/ }
}
