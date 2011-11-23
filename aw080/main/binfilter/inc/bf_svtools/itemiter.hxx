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


#ifndef _SFXITEMITER_HXX
#define _SFXITEMITER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif

namespace binfilter
{

class SfxPoolItem;
class SfxItemSet;
class SfxItemPool;

class  SfxItemIter
{
	// Item-Feld - Start & Ende
	const SfxItemSet&	_rSet;
	USHORT				_nStt, _nEnd, _nAkt;

public:
	SfxItemIter( const SfxItemSet& rSet );
	~SfxItemIter();

	// falls es diese gibt, returne sie, sonst 0
	const SfxPoolItem* FirstItem()
					   { _nAkt = _nStt;
						 return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
	const SfxPoolItem* LastItem()
					   { _nAkt = _nEnd;
						 return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
	const SfxPoolItem* GetCurItem()
					   { return _rSet._nCount ? *(_rSet._aItems+_nAkt) : 0; }
	const SfxPoolItem* NextItem();

	BOOL			   IsAtStart() const { return _nAkt == _nStt; }
	BOOL               IsAtEnd() const   { return _nAkt == _nEnd; }

	USHORT             GetCurPos() const { return _nAkt; }
	USHORT             GetFirstPos() const { return _nStt; }
	USHORT             GetLastPos() const { return _nEnd; }
};

}

#endif
