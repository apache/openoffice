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



// include ---------------------------------------------------------------
#ifdef _MSC_VER
#pragma hdrstop
#endif

#define _SVX_HLNKITEM_CXX

#ifndef _SVX_SVXIDS_HRC
#include <svxids.hrc>
#endif

#ifndef _MEMBERID_HRC
#include <memberid.hrc>
#endif


#include "hlnkitem.hxx"
namespace binfilter {

// -----------------------------------------------------------------------

/*N*/ TYPEINIT1_AUTOFACTORY(SvxHyperlinkItem, SfxPoolItem);

// class SvxHyperlinkItem ------------------------------------------------

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

#define HYPERLINKFF_MARKER	0x599401FE


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*?*/ SfxPoolItem* SvxHyperlinkItem::Clone( SfxItemPool* ) const
/*?*/ {
/*?*/ DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001 	return new SvxHyperlinkItem( *this );
/*?*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*?*/ int SvxHyperlinkItem::operator==( const SfxPoolItem& rAttr ) const
/*?*/ {
/*?*/ DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 //STRIP001 	DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unterschiedliche Typen" );
/*?*/ }


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/




}
