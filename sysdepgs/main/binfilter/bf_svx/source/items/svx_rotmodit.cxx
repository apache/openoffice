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



#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _COM_SUN_STAR_TABLE_CELLVERTJUSTIFY_HPP_
#include <com/sun/star/table/CellVertJustify.hpp>
#endif

#include "rotmodit.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;

// STATIC DATA -----------------------------------------------------------

/*N*/ TYPEINIT1_AUTOFACTORY(SvxRotateModeItem,        SfxEnumItem);


//-----------------------------------------------------------------------
//	SvxRotateModeItem - Ausrichtung bei gedrehtem Text
//-----------------------------------------------------------------------

/*N*/ SvxRotateModeItem::SvxRotateModeItem( SvxRotateMode eMode, USHORT nWhich )
/*N*/ 	: SfxEnumItem( nWhich, eMode )
/*N*/ {
/*N*/ }

/*N*/ SvxRotateModeItem::SvxRotateModeItem( const SvxRotateModeItem& rItem )
/*N*/ 	: SfxEnumItem( rItem )
/*N*/ {
/*N*/ }

/*N*/ __EXPORT SvxRotateModeItem::~SvxRotateModeItem()
/*N*/ {
/*N*/ }

/*N*/ SfxPoolItem* __EXPORT SvxRotateModeItem::Create( SvStream& rStream, USHORT ) const
/*N*/ {
/*N*/ 	USHORT nVal;
/*N*/ 	rStream >> nVal;
/*N*/ 	return new SvxRotateModeItem( (SvxRotateMode) nVal,Which() );
/*N*/ }



/*?*/ USHORT __EXPORT SvxRotateModeItem::GetValueCount() const
/*?*/ {
/*?*/ 	return 4;		// STANDARD, TOP, CENTER, BOTTOM
/*?*/ }

/*N*/ SfxPoolItem* __EXPORT SvxRotateModeItem::Clone( SfxItemPool* ) const
/*N*/ {
/*N*/ 	return new SvxRotateModeItem( *this );
/*N*/ }

/*N*/ USHORT __EXPORT SvxRotateModeItem::GetVersion( USHORT nFileVersion ) const
/*N*/ {
/*N*/ 	return 0;
/*N*/ }

//	QueryValue/PutValue: Der ::com::sun::star::table::CellVertJustify enum wird mitbenutzt...

/*N*/ sal_Bool SvxRotateModeItem::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
/*N*/ {
/*N*/ 	table::CellVertJustify eUno = table::CellVertJustify_STANDARD;
/*N*/ 	switch ( (SvxRotateMode)GetValue() )
/*N*/ 	{
/*N*/ 		case SVX_ROTATE_MODE_STANDARD: eUno = table::CellVertJustify_STANDARD; break;
/*N*/ 		case SVX_ROTATE_MODE_TOP:	   eUno = table::CellVertJustify_TOP;	  	break;
/*N*/ 		case SVX_ROTATE_MODE_CENTER:   eUno = table::CellVertJustify_CENTER;	break;
/*N*/ 		case SVX_ROTATE_MODE_BOTTOM:   eUno = table::CellVertJustify_BOTTOM;	break;
/*N*/ 	}
/*N*/ 	rVal <<= eUno;
/*N*/ 	return sal_True;
/*N*/ }

/*N*/ sal_Bool SvxRotateModeItem::PutValue( const uno::Any& rVal, BYTE nMemberId )
/*N*/ {
/*N*/ 	table::CellVertJustify eUno;
/*N*/ 	if(!(rVal >>= eUno))
/*N*/ 	{
/*N*/ 		sal_Int32 nValue;
/*?*/ 		if(!(rVal >>= nValue))
/*?*/ 			return sal_False;
/*?*/ 		eUno = (table::CellVertJustify)nValue;
/*N*/ 	}
/*N*/ 
/*N*/ 	SvxRotateMode eSvx = SVX_ROTATE_MODE_STANDARD;
/*N*/ 	switch (eUno)
/*N*/ 	{
/*N*/ 		case table::CellVertJustify_STANDARD: eSvx = SVX_ROTATE_MODE_STANDARD; break;
/*?*/ 		case table::CellVertJustify_TOP:	   eSvx = SVX_ROTATE_MODE_TOP;	  	break;
/*?*/ 		case table::CellVertJustify_CENTER:   eSvx = SVX_ROTATE_MODE_CENTER;	break;
/*N*/ 		case table::CellVertJustify_BOTTOM:   eSvx = SVX_ROTATE_MODE_BOTTOM;	break;
/*N*/ 	}
/*N*/ 	SetValue( eSvx );
/*N*/ 	return sal_True;
/*N*/ }



}
