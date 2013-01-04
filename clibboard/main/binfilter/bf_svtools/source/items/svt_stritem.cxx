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


#ifndef _SFXSTRITEM_HXX
#include <bf_svtools/stritem.hxx>
#endif

namespace binfilter
{


//============================================================================
//
//  class SfxStringItem
//
//============================================================================

TYPEINIT1_AUTOFACTORY(SfxStringItem, CntUnencodedStringItem)

//============================================================================
// virtual
SfxStringItem::SfxStringItem(USHORT which, SvStream & rStream):
	CntUnencodedStringItem(which)
{
	UniString aValue;
	readByteString(rStream, aValue);
	SetValue(aValue);
}


//============================================================================
// virtual
SfxPoolItem * SfxStringItem::Create(SvStream & rStream, USHORT) const
{
	return new SfxStringItem(Which(), rStream);
}

//============================================================================
// virtual
SvStream & SfxStringItem::Store(SvStream & rStream, USHORT) const
{
	writeByteString(rStream, GetValue());
	return rStream;
}

//============================================================================
// virtual
SfxPoolItem * SfxStringItem::Clone(SfxItemPool *) const
{
	return new SfxStringItem(*this);
}

}
