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



// MARKER(update_precomp.py): autogen include statement, do not remove

// INCLUDE ---------------------------------------------------------------

#ifndef GCC
#endif

#include <tools/string.hxx>
#include <tools/stream.hxx>

#include <bf_svtools/poolitem.hxx>
#include <bf_svtools/itemset.hxx>

namespace binfilter
{

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxSetItem)

// --------------------------------------------------------------------------

SfxSetItem::SfxSetItem( USHORT which, const SfxItemSet &rSet) :
	SfxPoolItem(which),
	pSet(rSet.Clone(TRUE))
{
	DBG_CTOR(SfxSetItem, 0);
}

// --------------------------------------------------------------------------

SfxSetItem::SfxSetItem( USHORT which, SfxItemSet *pS) :
	SfxPoolItem(which),
	pSet(pS)
{
	DBG_CTOR(SfxSetItem, 0);
	DBG_ASSERT(pS, "SfxSetItem without set constructed" );
}

// --------------------------------------------------------------------------

SfxSetItem::SfxSetItem( const SfxSetItem& rCopy, SfxItemPool *pPool ) :
	SfxPoolItem(rCopy.Which()),
	pSet(rCopy.pSet->Clone(TRUE, pPool))
{
	DBG_CTOR(SfxSetItem, 0);
}

// --------------------------------------------------------------------------

SfxSetItem::~SfxSetItem()
{
	DBG_DTOR(SfxSetItem, 0);
	delete pSet; pSet = 0;
}

// --------------------------------------------------------------------------

int SfxSetItem::operator==( const SfxPoolItem& rCmp) const
{
	DBG_CHKTHIS(SfxSetItem, 0);
	DBG_ASSERT( SfxPoolItem::operator==( rCmp ), "unequal type" );
	return *pSet == *(((const SfxSetItem &)rCmp).pSet);
}

// --------------------------------------------------------------------------

SfxItemPresentation SfxSetItem::GetPresentation
(
	SfxItemPresentation     /*ePresentation*/,
	SfxMapUnit              /*eCoreMetric*/,
	SfxMapUnit              /*ePresentationMetric*/,
	XubString&              /*rText*/,
    const ::IntlWrapper *
)   const
{
	DBG_CHKTHIS(SfxSetItem, 0);
	return SFX_ITEM_PRESENTATION_NONE;
}

// --------------------------------------------------------------------------

SvStream& SfxSetItem::Store(SvStream& rStream, USHORT) const
{
	GetItemSet().Store(rStream);
	return rStream;
}

}
