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

#include "zoomitem.hxx"
namespace binfilter {

// -----------------------------------------------------------------------

/*N*/ TYPEINIT1_AUTOFACTORY(SvxZoomItem,SfxUInt16Item);

// -----------------------------------------------------------------------

/*N*/ SvxZoomItem::SvxZoomItem
/*N*/ (
/*N*/ 	SvxZoomType eZoomType,
/*N*/ 	sal_uInt16		nVal,
/*N*/ 	sal_uInt16		nWhich
/*N*/ )
/*N*/ :	SfxUInt16Item( nWhich, nVal ),
/*N*/ 	nValueSet( SVX_ZOOM_ENABLE_ALL ),
/*N*/ 	eType( eZoomType )
/*N*/ {
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SvxZoomItem::SvxZoomItem( const SvxZoomItem& rOrig )
/*N*/ :	SfxUInt16Item( rOrig.Which(), rOrig.GetValue() ),
/*N*/ 	nValueSet( rOrig.GetValueSet() ),
/*N*/ 	eType( rOrig.GetType() )
/*N*/ {
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SvxZoomItem::~SvxZoomItem()
/*N*/ {
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SfxPoolItem* SvxZoomItem::Clone( SfxItemPool *pPool ) const
/*N*/ {
/*N*/ 	return new SvxZoomItem( *this );
/*N*/ }

// -----------------------------------------------------------------------


// -----------------------------------------------------------------------


// -----------------------------------------------------------------------

/*N*/ int SvxZoomItem::operator==( const SfxPoolItem& rAttr ) const
/*N*/ {
/*N*/ 	DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );
/*N*/ 
/*N*/ 	SvxZoomItem& rItem = (SvxZoomItem&)rAttr;
/*N*/ 
/*N*/ 	return ( GetValue() == rItem.GetValue() 	&&
/*N*/ 			 nValueSet 	== rItem.GetValueSet() 	&&
/*N*/ 			 eType 		== rItem.GetType() 			);
/*N*/ }


}
