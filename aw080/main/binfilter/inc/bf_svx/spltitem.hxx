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


#ifndef _SVX_SPLTITEM_HXX
#define _SVX_SPLTITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;

// class SvxFmtSplitItem -------------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt eine logische Variable "Absatz trennen ja oder nein".
*/

class SvxFmtSplitItem : public SfxBoolItem
{
public:
	TYPEINFO();
	~SvxFmtSplitItem();

	inline SvxFmtSplitItem( const BOOL bSplit = TRUE,
							const USHORT nWhich = ITEMID_FMTSPLIT );
	inline SvxFmtSplitItem& operator=( const SvxFmtSplitItem& rSplit );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create( SvStream&, USHORT ) const;
	virtual SvStream&		Store( SvStream& , USHORT nItemVersion ) const;

};

inline SvxFmtSplitItem::SvxFmtSplitItem( const BOOL bSplit, const USHORT nWhich ) :
	SfxBoolItem( nWhich, bSplit )
{}

inline SvxFmtSplitItem& SvxFmtSplitItem::operator=(
	const SvxFmtSplitItem& rSplit )
{
	SetValue( rSplit.GetValue() );
	return *this;
}

}//end of namespace binfilter
#endif

