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

#ifndef GCC
#endif

#include <tools/string.hxx>

#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>
#include <bf_svtools/svarray.hxx>
#include <bf_svtools/aeitem.hxx>

namespace binfilter
{

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxAllEnumItem)

TYPEINIT1_AUTOFACTORY(SfxAllEnumItem, SfxEnumItem)

// -----------------------------------------------------------------------

struct SfxAllEnumValue_Impl
{
	USHORT nValue;
	XubString aText;
};

SV_DECL_PTRARR_DEL(SfxAllEnumValueArr, SfxAllEnumValue_Impl*, 0, 8)
SV_IMPL_PTRARR(SfxAllEnumValueArr, SfxAllEnumValue_Impl*)

// -----------------------------------------------------------------------

SfxAllEnumItem::SfxAllEnumItem() :
    SfxEnumItem(),
    pValues( 0 ),
    pDisabledValues( 0 )
{
}

// -----------------------------------------------------------------------

SfxAllEnumItem::SfxAllEnumItem( USHORT which, SvStream &rStream ):
	SfxEnumItem(which, rStream),
	pValues( 0 ),
	pDisabledValues( 0 )
{
	DBG_CTOR(SfxAllEnumItem, 0);
	InsertValue( GetValue() );
}

// -----------------------------------------------------------------------


SfxAllEnumItem::SfxAllEnumItem(USHORT which):
	SfxEnumItem(which, 0),
	pValues( 0 ),
	pDisabledValues( 0 )
{
	DBG_CTOR(SfxAllEnumItem, 0);
}


// -----------------------------------------------------------------------

SfxAllEnumItem::SfxAllEnumItem(const SfxAllEnumItem &rCopy):
	SfxEnumItem(rCopy),
	pValues(0),
	pDisabledValues( 0 )
{
	DBG_CTOR(SfxAllEnumItem, 0);
	if ( !rCopy.pValues )
		return;

	pValues = new SfxAllEnumValueArr;

	for ( USHORT nPos = 0; nPos < rCopy.pValues->Count(); ++nPos )
	{
		SfxAllEnumValue_Impl *pVal = new SfxAllEnumValue_Impl;
		pVal->nValue = rCopy.pValues->GetObject(nPos)->nValue;
		pVal->aText = rCopy.pValues->GetObject(nPos)->aText;
		const SfxAllEnumValue_Impl *pTemp = pVal;
		pValues->Insert( pTemp, nPos );
	}

	if( rCopy.pDisabledValues )
	{
		pDisabledValues = new SvUShorts;
		for ( USHORT nPos = 0; nPos < rCopy.pDisabledValues->Count(); ++nPos )
		{
			pDisabledValues->Insert( rCopy.pDisabledValues->GetObject(nPos),
									 nPos );
		}
	}
}

// -----------------------------------------------------------------------

SfxAllEnumItem::~SfxAllEnumItem()
{
	DBG_DTOR(SfxAllEnumItem, 0);
	delete pValues;
	delete pDisabledValues;
}

// -----------------------------------------------------------------------

USHORT SfxAllEnumItem::GetValueCount() const
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	return pValues ? pValues->Count() : 0;
}

// -----------------------------------------------------------------------

XubString SfxAllEnumItem::GetValueTextByPos( USHORT nPos ) const
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	DBG_ASSERT( pValues && nPos < pValues->Count(), "enum overflow" );
	return pValues->GetObject(nPos)->aText;
}

// -----------------------------------------------------------------------

USHORT SfxAllEnumItem::GetValueByPos( USHORT nPos ) const
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	DBG_ASSERT( pValues && nPos < pValues->Count(), "enum overflow" );
	return pValues->GetObject(nPos)->nValue;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxAllEnumItem::Clone( SfxItemPool * ) const
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	return new SfxAllEnumItem(*this);
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxAllEnumItem::Create( SvStream & rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	return new SfxAllEnumItem( Which(), rStream );
}


// -----------------------------------------------------------------------

USHORT SfxAllEnumItem::_GetPosByValue( USHORT nVal ) const

/*	[Beschreibung]

	Im Ggs. zu <SfxEnumItemInterface::GetPosByValue(USHORT)const> liefert
	diese interne Methode bei nicht vorhandenen Values die Position,
	an der der Wert liegen w"urde.
*/

{
	DBG_CHKTHIS(SfxAllEnumItem, 0);

	if ( !pValues )
		return 0;

	//!O: binaere Suche oder SortArray verwenden
	USHORT nPos;
	for ( nPos = 0; nPos < pValues->Count(); ++nPos )
		if ( pValues->GetObject(nPos)->nValue >= nVal )
			return nPos;
	return nPos;
}

// -----------------------------------------------------------------------

USHORT SfxAllEnumItem::GetPosByValue( USHORT nValue ) const

/*  [Beschreibung]

	Liefert im Gegensatz zu <SfxEnumItemInterface::GetPosByValue(USHORT)const>
	immer nValue zur"uck, solange nicht mindestens ein Wert mit einer der
	Methoden <SfxAllEnumItem::InsertValue()> eingef"ugt wurde.
*/

{
	DBG_CHKTHIS(SfxAllEnumItem, 0);

	if ( !pValues || !pValues->Count() )
		return nValue;

	return SfxEnumItem::GetPosByValue( nValue );
}

// -----------------------------------------------------------------------

void SfxAllEnumItem::InsertValue( USHORT nValue )
{
	DBG_CHKTHIS(SfxAllEnumItem, 0);
	SfxAllEnumValue_Impl *pVal = new SfxAllEnumValue_Impl;
	pVal->nValue = nValue;
	pVal->aText = XubString::CreateFromInt32( nValue );
	const SfxAllEnumValue_Impl *pTemp = pVal;
	if ( !pValues )
		pValues = new SfxAllEnumValueArr;

	pValues->Insert( pTemp, _GetPosByValue(nValue) ); //! doppelte?!
}

BOOL SfxAllEnumItem::IsEnabled( USHORT nValue ) const
{
	if ( pDisabledValues )
	{
		for ( USHORT i=0; i<pDisabledValues->Count(); i++ )
			if ( (*pDisabledValues)[i] == nValue )
				return FALSE;
	}

	return TRUE;
}

}
