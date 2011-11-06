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


#ifndef SVTOOLS_TRESITEM_HXX
#include <bf_svtools/tresitem.hxx>
#endif

using namespace com::sun::star;

namespace binfilter
{

//============================================================================
//
//  CntTransferResultItem
//
//============================================================================

TYPEINIT1_AUTOFACTORY(CntTransferResultItem, SfxPoolItem)

//============================================================================
// virtual
int CntTransferResultItem::operator ==(SfxPoolItem const & rItem) const
{
	if (CntTransferResultItem * pResultItem = PTR_CAST(CntTransferResultItem,
													   &rItem))
		return m_aResult.Source == pResultItem->m_aResult.Source
			   && m_aResult.Target == pResultItem->m_aResult.Target
			   && m_aResult.Result == pResultItem->m_aResult.Result;
	return false;
}

//============================================================================
// virtual
BOOL CntTransferResultItem::QueryValue(uno::Any & rVal, BYTE) const
{
	rVal <<= m_aResult;
	return true;
}

//============================================================================
// virtual
BOOL CntTransferResultItem::PutValue(uno::Any const & rVal, BYTE)
{
	return rVal >>= m_aResult;
}

//============================================================================
// virtual
SfxPoolItem * CntTransferResultItem::Clone(SfxItemPool *) const
{
	return new CntTransferResultItem(*this);
}

}
