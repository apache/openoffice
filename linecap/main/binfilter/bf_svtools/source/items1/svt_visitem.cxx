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


#ifndef _SFXVISIBILITYITEM_HXX
#include <bf_svtools/visitem.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

namespace binfilter
{

//============================================================================
//
//  class SfxVisibilityItem
//
//============================================================================

DBG_NAME(SfxVisibilityItem)

//============================================================================
TYPEINIT1_AUTOFACTORY(SfxVisibilityItem, SfxPoolItem);

//============================================================================
SfxVisibilityItem::SfxVisibilityItem(USHORT which, SvStream & rStream):
	SfxPoolItem(which)
{
	DBG_CTOR(SfxVisibilityItem, 0);
	sal_Bool bValue = 0;
	rStream >> bValue;
	m_nValue.bVisible = bValue;
}

//============================================================================
// virtual
int SfxVisibilityItem::operator ==(const SfxPoolItem & rItem) const
{
	DBG_CHKTHIS(SfxVisibilityItem, 0);
	DBG_ASSERT(SfxPoolItem::operator ==(rItem), "unequal type");
	return m_nValue.bVisible == SAL_STATIC_CAST(const SfxVisibilityItem *, &rItem)->
	                    m_nValue.bVisible;
}

//============================================================================
// virtual
int SfxVisibilityItem::Compare(const SfxPoolItem & rWith) const
{
	DBG_ASSERT(rWith.ISA(SfxVisibilityItem), "SfxVisibilityItem::Compare(): Bad type");
	return m_nValue.bVisible == static_cast< SfxVisibilityItem const * >(&rWith)->m_nValue.bVisible ?
		       0 : m_nValue.bVisible ? -1 : 1;
}

//============================================================================
// virtual
SfxItemPresentation SfxVisibilityItem::GetPresentation(SfxItemPresentation,
												  SfxMapUnit, SfxMapUnit,
												  XubString & rText,
                                                  const ::IntlWrapper *) const
{
	rText = GetValueTextByVal(m_nValue.bVisible);
	return SFX_ITEM_PRESENTATION_NAMELESS;
}


//============================================================================
// virtual
BOOL SfxVisibilityItem::QueryValue(com::sun::star::uno::Any& rVal,BYTE) const
{
	rVal <<= m_nValue;
	return TRUE;
}

//============================================================================
// virtual
BOOL SfxVisibilityItem::PutValue(const com::sun::star::uno::Any& rVal,BYTE)
{
	if (rVal >>= m_nValue)
		return TRUE;

	DBG_ERROR( "SfxInt16Item::PutValue - Wrong type!" );
	return FALSE;
}

//============================================================================
// virtual
SfxPoolItem * SfxVisibilityItem::Create(SvStream & rStream, USHORT) const
{
	DBG_CHKTHIS(SfxVisibilityItem, 0);
	return new SfxVisibilityItem(Which(), rStream);
}

//============================================================================
// virtual
SvStream & SfxVisibilityItem::Store(SvStream & rStream, USHORT) const
{
	DBG_CHKTHIS(SfxVisibilityItem, 0);
	rStream << m_nValue.bVisible;
	return rStream;
}

//============================================================================
// virtual
SfxPoolItem * SfxVisibilityItem::Clone(SfxItemPool *) const
{
	DBG_CHKTHIS(SfxVisibilityItem, 0);
	return new SfxVisibilityItem(*this);
}

//============================================================================
// virtual
USHORT SfxVisibilityItem::GetValueCount() const
{
	return 2;
}

//============================================================================
// virtual
UniString SfxVisibilityItem::GetValueTextByVal(BOOL bTheValue) const
{
	return
		bTheValue ?
		    UniString::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("TRUE")) :
		    UniString::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("FALSE"));
}
}
