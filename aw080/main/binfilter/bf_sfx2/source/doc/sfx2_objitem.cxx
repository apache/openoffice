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

#include "objsh.hxx"
//#include "objshimp.hxx"
#include "objitem.hxx"
namespace binfilter {

//====================================================================

/*N*/ TYPEINIT1(SfxObjectShellItem,SfxPoolItem)
/*N*/ TYPEINIT1_AUTOFACTORY(SfxObjectItem,SfxPoolItem)

//=========================================================================

/*N*/ int SfxObjectShellItem::operator==( const SfxPoolItem &rItem ) const
/*N*/ {
/*N*/ 	 return PTR_CAST(SfxObjectShellItem, &rItem)->pObjSh == pObjSh;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ String SfxObjectShellItem::GetValueText() const
/*N*/ {
/*N*/ 	return String();
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxPoolItem* SfxObjectShellItem::Clone( SfxItemPool *) const
/*N*/ {
/*N*/ 	return new SfxObjectShellItem( Which(), pObjSh );
/*N*/ }

//=========================================================================

/*N*/ SfxObjectItem::SfxObjectItem( USHORT nWhich, SfxShell *pSh )
/*N*/ :	SfxPoolItem( nWhich ),
/*N*/     _pSh( pSh )
/*N*/ {}

//--------------------------------------------------------------------

/*N*/ int SfxObjectItem::operator==( const SfxPoolItem &rItem ) const
/*N*/ {
/*N*/ 	 SfxObjectItem *pOther = PTR_CAST(SfxObjectItem, &rItem);
/*N*/      return pOther->_pSh == _pSh;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxPoolItem* SfxObjectItem::Clone( SfxItemPool *) const
/*N*/ {
/*N*/     return new SfxObjectItem( Which(), _pSh );
/*N*/ }


}
