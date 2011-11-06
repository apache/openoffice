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



#ifndef _COM_SUN_STAR_DRAWING_DIRECTION3D_HPP_
#include <com/sun/star/drawing/Direction3D.hpp>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "e3ditem.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;

// STATIC DATA -----------------------------------------------------------

/*N*/ DBG_NAME(SvxVector3DItem)

// -----------------------------------------------------------------------

/*N*/ TYPEINIT1_AUTOFACTORY(SvxVector3DItem, SfxPoolItem);

// -----------------------------------------------------------------------

/*?*/ SvxVector3DItem::SvxVector3DItem()
/*?*/ {
/*?*/ DBG_BF_ASSERT(0, "STRIP"); //STRIP001 /*?*/ 	DBG_CTOR(SvxVector3DItem, 0);
/*?*/ }

// -----------------------------------------------------------------------

/*N*/ SvxVector3DItem::SvxVector3DItem( USHORT nWhich, const Vector3D& rVal ) :
/*N*/ 	SfxPoolItem( nWhich ),
/*N*/ 	aVal( rVal )
/*N*/ {
/*N*/ 	DBG_CTOR(SvxVector3DItem, 0);
/*N*/ }

// -----------------------------------------------------------------------


// -----------------------------------------------------------------------

/*N*/ SvxVector3DItem::SvxVector3DItem( const SvxVector3DItem& rItem ) :
/*N*/ 	SfxPoolItem( rItem ),
/*N*/ 	aVal( rItem.aVal )
/*N*/ {
/*N*/ 	DBG_CTOR(SvxVector3DItem, 0);
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ int SvxVector3DItem::operator==( const SfxPoolItem &rItem ) const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(SvxVector3DItem, 0);
/*N*/ 	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
/*N*/ 	return ((SvxVector3DItem&)rItem).aVal == aVal;
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SfxPoolItem* SvxVector3DItem::Clone( SfxItemPool *pPool ) const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(SvxVector3DItem, 0);
/*N*/ 	return new SvxVector3DItem( *this );
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SfxPoolItem* SvxVector3DItem::Create(SvStream &rStream, USHORT nVersion) const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(SvxVector3DItem, 0);
/*N*/ 	Vector3D aStr;
/*N*/ 	rStream >> aStr;
/*N*/ 	return new SvxVector3DItem(Which(), aStr);
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ SvStream& SvxVector3DItem::Store(SvStream &rStream, USHORT nItemVersion) const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(SvxVector3DItem, 0);
/*N*/ 
/*N*/ 	// ## if (nItemVersion) 
/*N*/ 	rStream << aVal;
/*N*/ 
/*N*/ 	return rStream;
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ sal_Bool SvxVector3DItem::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
/*N*/ {
/*N*/ 	drawing::Direction3D aDirection;
/*N*/ 
/*N*/ 	// Werte eintragen
/*N*/ 	aDirection.DirectionX = aVal.X();
/*N*/ 	aDirection.DirectionY = aVal.Y();
/*N*/ 	aDirection.DirectionZ = aVal.Z();
/*N*/ 
/*N*/ 	rVal <<= aDirection;
/*N*/ 	return( sal_True );
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ sal_Bool SvxVector3DItem::PutValue( const uno::Any& rVal, BYTE nMemberId )
/*N*/ {
/*N*/ 	drawing::Direction3D aDirection;
/*N*/ 	if(!(rVal >>= aDirection))
/*N*/ 		return sal_False;
/*N*/ 
/*N*/ 	aVal.X() = aDirection.DirectionX;
/*N*/ 	aVal.Y() = aDirection.DirectionY;
/*N*/ 	aVal.Z() = aDirection.DirectionZ;
/*N*/ 	return sal_True;
/*N*/ }

// -----------------------------------------------------------------------

/*N*/ USHORT SvxVector3DItem::GetVersion (USHORT nFileFormatVersion) const
/*N*/ {
/*N*/ 	return (nFileFormatVersion == SOFFICE_FILEFORMAT_31) ? USHRT_MAX : 0;
/*N*/ }


}
