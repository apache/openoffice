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



#ifndef SVTOOLS_TRESITEM_HXX
#define SVTOOLS_TRESITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_UCB_TRANSFERRESULT_HPP_
#include <com/sun/star/ucb/TransferResult.hpp>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

namespace binfilter
{

//============================================================================
class CntTransferResultItem: public SfxPoolItem
{
	com::sun::star::ucb::TransferResult m_aResult;

public:
	TYPEINFO();

	CntTransferResultItem(USHORT which = 0): SfxPoolItem(which) {}

	CntTransferResultItem(USHORT which,
						  com::sun::star::ucb::TransferResult const &
						      rTheResult):
		SfxPoolItem(which), m_aResult(rTheResult) {}

	virtual int operator ==(SfxPoolItem const & rItem) const;

	virtual BOOL QueryValue(com::sun::star::uno::Any & rVal, BYTE = 0) const;

	virtual BOOL PutValue(const com::sun::star::uno::Any & rVal, BYTE = 0);

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;

	com::sun::star::ucb::TransferResult const & GetValue() const
	{ return m_aResult; }
};

}

#endif // SVTOOLS_TRESITEM_HXX

