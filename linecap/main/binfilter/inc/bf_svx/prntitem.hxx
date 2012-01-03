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


#ifndef _SVX_PRNTITEM_HXX
#define _SVX_PRNTITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
namespace binfilter {

// class SvxPrintItem ----------------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt eine logische Variable "Drucken ja oder nein".
*/

class SvxPrintItem : public SfxBoolItem
{
public:
	TYPEINFO();

	SvxPrintItem( const USHORT nId = ITEMID_PRINT, const BOOL bPrt = TRUE );
	inline SvxPrintItem &operator=( const SvxPrintItem &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create(SvStream &, USHORT) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion ) const;

};

inline SvxPrintItem::SvxPrintItem( const USHORT nId, const BOOL bPrt )
	: SfxBoolItem( nId, bPrt )
{}

inline SvxPrintItem &SvxPrintItem::operator=( const SvxPrintItem &rCpy )
{
	SetValue( rCpy.GetValue() );
	return *this;
}

}//end of namespace binfilter
#endif

